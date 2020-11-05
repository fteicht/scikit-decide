/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_MARTDP_HH
#define SKDECIDE_MARTDP_HH

#include <functional>
#include <memory>
#include <unordered_set>
#include <stack>
#include <list>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>

#include <boost/range/irange.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "utils/associative_container_deducer.hh"
#include "utils/string_converter.hh"
#include "utils/execution.hh"

namespace skdecide {

template <typename Tdomain,
          typename Texecution_policy = SequentialExecution>
class MARTDPSolver {
public :
    typedef Tdomain Domain;
    typedef typename Domain::State State;
    typedef typename Domain::State::Agent Agent;
    typedef typename Domain::State::Data AgentState;
    typedef typename Domain::Event Action;
    typedef typename Domain::Action::Data AgentAction;
    typedef typename Domain::Value Value;
    typedef typename Domain::Predicate Predicate;
    typedef typename Domain::EnvironmentOutcome EnvironmentOutcome;
    typedef Texecution_policy ExecutionPolicy;

    typedef std::function<Predicate (Domain&, const State&)> GoalCheckerFunctor;
    typedef std::function<std::pair<Value, Action> (Domain&, const State&)> HeuristicFunctor;
    typedef std::function<bool (const std::size_t&, const std::size_t&, const double&)> WatchdogFunctor;

    MARTDPSolver(Domain& domain,
                 const GoalCheckerFunctor& goal_checker,
                 const HeuristicFunctor& heuristic,
                 std::size_t time_budget = 3600000,
                 std::size_t rollout_budget = 100000,
                 std::size_t max_depth = 1000,
                 std::size_t max_feasibility_trials = 0, // will then choose nb_agents
                 std::size_t nb_transition_samples = 0, // will then choose 10*nb_agents
                 std::size_t epsilon_moving_average_window = 100,
                 double epsilon = 0.0, // not a stopping criterion by default
                 double discount = 1.0,
                 double action_choice_noise = 0.1,
                 const double& dead_end_cost = 10e4,
                 bool debug_logs = false,
                 const WatchdogFunctor& watchdog = [](const std::size_t&, const std::size_t&, const double&){ return true; })
        : _domain(domain), _goal_checker(goal_checker), _heuristic(heuristic),
          _time_budget(time_budget), _rollout_budget(rollout_budget), _max_depth(max_depth),
          _max_feasibility_trials(max_feasibility_trials), _nb_transition_samples(nb_transition_samples),
          _epsilon_moving_average_window(epsilon_moving_average_window), _epsilon(epsilon),
          _discount(discount), _dead_end_cost(dead_end_cost), _debug_logs(debug_logs), _watchdog(watchdog),
          _epsilon_moving_average(0), _current_state(nullptr), _nb_rollouts(0), _nb_agents(0) {
              if (debug_logs && (spdlog::get_level() > spdlog::level::debug)) {
                  std::string msg = "Debug logs requested for algorithm MARTDP but global log level is higher than debug";
                  if (spdlog::get_level() <= spdlog::level::warn) {
                      spdlog::warn(msg);
                  } else {
                      msg = "\033[1;33mbold " + msg + "\033[0m";
                      std::cerr << msg << std::endl;
                  }
              }

            std::random_device rd;
            _gen = std::make_unique<std::mt19937>(rd());
            _action_choice_noise_dist = std::bernoulli_distribution(action_choice_noise);
        }

    // clears the solver (clears the search graph, thus preventing from reusing
    // previous search results)
    void clear() {
        _graph.clear();
    }

    // solves from state s using heuristic function h
    void solve(const State& s) {
        try {
            spdlog::info("Running " + ExecutionPolicy::print_type() + " MARTDP solver from state " + s.print());
            auto start_time = std::chrono::high_resolution_clock::now();

            if (_nb_agents != s.size()) {
                // We are solving a new problem.
                _graph.clear();
                _nb_agents = s.size();
                _agents_orders.resize(_nb_agents);

                for (std::size_t a = 0 ; a < _nb_agents ; a++) {
                    _agents_orders[a].resize(_nb_agents - 1);
                    for (std::size_t aa = 0 ; aa < _nb_agents - 1 ; aa++) {
                        if (aa < a) {
                            _agents_orders[a][aa] = aa;
                        } else {
                            _agents_orders[a][aa] = aa + 1;
                        }
                    }
                }

                _agents.clear();
                _agents.reserve(_nb_agents);

                for (auto a : s) {
                    _agents.push_back(a.agent());
                }

                if (_max_feasibility_trials == 0) {
                    _max_feasibility_trials = _nb_agents;
                }

                if (_nb_transition_samples == 0) {
                    _nb_transition_samples = 10 * _nb_agents;
                }
            }
            
            auto si = _graph.emplace(s);
            Node& root_node = const_cast<Node&>(*(si.first)); // we won't change the real key (StateNode::state) so we are safe

            if (si.second) {
                Predicate termination;
                for (auto a: _agents) {
                    termination[a] = false;
                }
                initialize_node(root_node, termination);
            }

            if (root_node.all_goal) { // problem already solved from this state (was present in _graph and already solved)
                spdlog::info("MARTDP finished to solve from state " + s.print() + " [goal state]");
                return;
            }

            _nb_rollouts = 0;
            _epsilon_moving_average = 0.0;
            _epsilons.clear();
            std::size_t etime = 0;
                
            do {
                if (_debug_logs) spdlog::debug("Starting rollout " + StringConverter::from(_nb_rollouts));

                _nb_rollouts++;
                double root_node_record_value = root_node.all_value;
                trial(&root_node, start_time);
                update_epsilon_moving_average(root_node, root_node_record_value);
            } while(_watchdog(etime = elapsed_time(start_time), _nb_rollouts,
                              (_epsilons.size() >= _epsilon_moving_average_window) ?
                                    _epsilon_moving_average :
                                    std::numeric_limits<double>::infinity()) &&
                    (etime < _time_budget) &&
                    (_nb_rollouts < _rollout_budget) &&
                    (_epsilon_moving_average > _epsilon));

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
            spdlog::info("MARTDP finished to solve from state " + s.print() +
                         " in " + StringConverter::from((double) duration / (double) 1e9) + " seconds with " +
                         StringConverter::from(_nb_rollouts) + " rollouts and visited " +
                         StringConverter::from(_graph.size()) + " states. ");
        } catch (const std::exception& e) {
            spdlog::error("MARTDP failed solving from state " + s.print() + ". Reason: " + e.what());
            throw;
        }
    }

    bool is_solution_defined_for(const State& s) const {
        auto si = _graph.find(s);
        if ((si == _graph.end()) || !(si->action)) {
            return false;
        } else {
            return true;
        }
    }

    const Action& get_best_action(const State& s) {
        auto si = _graph.find(s);
        if ((si == _graph.end()) || !(si->action)) {
            throw std::runtime_error("SKDECIDE exception: no best action found in state " + s.print());
        } else {
            return *(si->action);
        }
    }

    double get_best_value(const State& s) const {
        auto si = _graph.find(s);
        if (si == _graph.end()) {
            throw std::runtime_error("SKDECIDE exception: no best action found in state " + s.print());
        }
        return si->all_value;
    }

    std::size_t get_nb_of_explored_states() const {
        return _graph.size();
    }

    std::size_t get_nb_rollouts() const {
        return _nb_rollouts;
    }

    typename MapTypeDeducer<State, std::pair<Action, double>>::Map policy() const {
        typename MapTypeDeducer<State, std::pair<Action, double>>::Map p;
        for (auto& n : _graph) {
            if (n.action) {
                p.insert(std::make_pair(n.state,
                                        std::make_pair(*n.action,
                                        (double) n.all_value)));
            }
        }
        return p;
    }

private :
    Domain& _domain;
    GoalCheckerFunctor _goal_checker;
    HeuristicFunctor _heuristic;
    std::size_t _time_budget;
    std::size_t _rollout_budget;
    std::size_t _max_depth;
    std::size_t _max_feasibility_trials;
    std::size_t _nb_transition_samples;
    std::size_t _epsilon_moving_average_window;
    double _epsilon;
    double _discount;
    double _dead_end_cost;
    bool _debug_logs;
    WatchdogFunctor _watchdog;
    ExecutionPolicy _execution_policy;
    std::unique_ptr<std::mt19937> _gen;
    typename ExecutionPolicy::Mutex _gen_mutex;
    typename ExecutionPolicy::Mutex _time_mutex;

    double _epsilon_moving_average;
    std::list<double> _epsilons;

    struct Node {
        State state;
        std::unique_ptr<Action> action;
        std::vector<double> value;
        double all_value;
        std::vector<bool> goal;
        bool all_goal;
        std::vector<bool> termination;
        bool all_termination;

        Node(const State& s)
        : state(s),
          all_value(std::numeric_limits<double>::infinity()),
          all_goal(false),
          all_termination(false) {}
        
        struct Key {
            const State& operator()(const Node& sn) const { return sn.state; }
        };
    };

    typedef typename SetTypeDeducer<Node, State>::Set Graph;
    Graph _graph;
    Node* _current_state;
    std::size_t _nb_rollouts;
    std::size_t _nb_agents;
    std::vector<Agent> _agents;
    std::vector<std::vector<std::size_t>> _agents_orders;
    std::bernoulli_distribution _action_choice_noise_dist;

    std::size_t elapsed_time(const std::chrono::time_point<std::chrono::high_resolution_clock>& start_time) {
        std::size_t milliseconds_duration;
        _execution_policy.protect([&milliseconds_duration, &start_time](){
            milliseconds_duration = static_cast<std::size_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - start_time
                ).count()
            );
        }, _time_mutex);
        return milliseconds_duration;
    }

    std::unique_ptr<Action>& greedy_action(Node* s) {
        if (_debug_logs) spdlog::debug("Updating state " + s->state.print());
        
        double best_value = std::numeric_limits<double>::infinity();
        std::unique_ptr<Action> best_action;
        
        for (std::size_t agent = 0 ; agent < _nb_agents ; agent++) {
            if (_debug_logs) spdlog::debug("Trying agent " + _agents[agent].print() + " actions");
            auto agent_applicable_actions = _domain.get_agent_applicable_actions(s->state, Action(), _agents[agent]).get_elements();

            if (agent_applicable_actions.empty()) {
                if (_debug_logs) spdlog::debug("No agent applicable actions");
                continue;
            } else {
                for (auto action : agent_applicable_actions) {
                    if (_debug_logs) spdlog::debug("Trying agent action " + action.print());
                    std::unique_ptr<Action> agent_actions = std::make_unique<Action>();
                    (*agent_actions)[_agents[agent]] = action;

                    // try to find a feasible assignment
                    bool feasible = false;
                    std::size_t feasibility_trial = 0;
                    
                    while (!feasible && feasibility_trial < _max_feasibility_trials) {
                        std::shuffle(_agents_orders[agent].begin(), _agents_orders[agent].end(), *_gen);
                        feasibility_trial++;
                        feasible = true;

                        // construct the joint action
                        for (auto other_agent : _agents_orders[agent]) {
                            auto other_agent_aa = _domain.get_agent_applicable_actions(s->state, *agent_actions, _agents[other_agent]);

                            if (other_agent_aa.empty()) {
                                feasible = false;
                                break;
                            }
                            
                            // Is the agent's optimal action applicable ?
                            if ((s->action) &&
                                other_agent_aa.contains((*(s->action))[_agents[other_agent]]) &&
                                !(_action_choice_noise_dist(*_gen))) { // choose it with high probability
                                    (*agent_actions)[_agents[other_agent]] = (*s->action)[_agents[other_agent]];
                            } else {
                                (*agent_actions)[_agents[other_agent]] = other_agent_aa.sample();
                            }
                        }
                    }

                    if (feasible) {
                        // make the transition and compute Q-value
                        double qval = q_value(s->state, *agent_actions).first;
                        if (_debug_logs) spdlog::debug("Found joint applicable action " + agent_actions->print() +
                                                       " with Q-value=" + StringConverter::from(qval));

                        if (qval < best_value) {
                            best_value = qval;
                            best_action = std::move(agent_actions);
                        }
                    } else if (_debug_logs) spdlog::debug("Failed finding a joint applicable action");
                }
            }
        }

        if (_debug_logs) {
            if (best_action) {
                spdlog::debug("Greedy action of state " + s->state.print() + ": " +
                              best_action->print() +
                              " with value " + StringConverter::from(best_value));
            } else {
                spdlog::debug("State " + s->state.print() +
                              " is a dead-end (no feasible actions found)");
            }
        }

        s->action = std::move(best_action);
        s->all_value = best_value;
        return s->action; // action is nullptr if state is a dead-end
    }

    // void depth_first_search_applicable_action(const State& state, const Agent& agent) {

    // }

    std::pair<double, std::vector<double>> q_value(const State& state, const Action& action) {
        if (_debug_logs) spdlog::debug("Computing Q-value of (" + state.print() + ", " + action.print() + ")");

        std::pair<double, std::vector<double>> qval;
        qval.first= 0.0;
        qval.second.resize(_nb_agents, 0.0);

        for (std::size_t transition_sample = 0 ; transition_sample < _nb_transition_samples ; transition_sample++) {
            EnvironmentOutcome outcome = _domain.sample(state, action);
            Value tval = outcome.transition_value();
            auto i = _graph.emplace(outcome.observation());
            Node& next_node = const_cast<Node&>(*(i.first)); // we won't change the real key (StateNode::state) so we are safe

            if (i.second) { // new node
                initialize_node(next_node, outcome.termination());
            }
            
            for (std::size_t a = 0 ; a < _nb_agents ; a++) {
                double aval = (tval[_agents[a]].cost() + (_discount * next_node.value[a])) / ((double) _nb_transition_samples);
                qval.second[a] += aval;
                qval.first += aval;
            }
        }

        if (_debug_logs) spdlog::debug("Updated Q-value of action " + action.print() +
                                        " with value " + StringConverter::from(qval.first));

        return qval;
    }

    Node* pick_next_state(const Node* s, const Action& a) {
        EnvironmentOutcome outcome = _domain.sample(s->state, a);
        auto i = _graph.emplace(outcome.observation());
        Node& next_node = const_cast<Node&>(*(i.first)); // we won't change the real key (StateNode::state) so we are safe
        if (i.second) { // new node
            initialize_node(next_node, outcome.termination());
        }
        if (_debug_logs) spdlog::debug("Picked next state " + next_node.state.print() +
                                       " from state " + s->state.print() +
                                        " and action " + a.print());
        if (next_node.all_termination) {
            if (_debug_logs) spdlog::debug(std::string("Next state is a ") +
                                           ((next_node.all_goal)?("goal"):("dead-end")) + ".");
            return nullptr;
        } else {
            return &next_node;
        }
    }

    void initialize_node(Node& n, const Predicate& termination) {
        if (_debug_logs) {
            spdlog::debug("Initializing new state node " + n.state.print());
        }

        n.value.resize(_nb_agents, 0.0);
        n.all_value = 0.0;
        n.goal.resize(_nb_agents, false);
        n.all_goal = true;
        n.termination.resize(_nb_agents, false);
        n.all_termination = true;

        Predicate g = _goal_checker(_domain, n.state);
        auto h = _heuristic(_domain, n.state);
        
        n.action = std::make_unique<Action>();

        for (std::size_t a = 0 ; a < _nb_agents ; a++) {
            n.goal[a] = g[_agents[a]];
            n.all_goal = n.all_goal && n.goal[a];
            n.termination[a] = termination[_agents[a]];
            n.all_termination = n.all_termination && n.termination[a];

            if (n.goal[a]) {
                n.value[a] = 0.0;
            } else if (n.termination[a]) { // dead-end state
                    n.value[a] = _dead_end_cost / ((double) _nb_agents);
            } else {
                n.value[a] = h.first[_agents[a]].cost();
                (*n.action)[_agents[a]] = h.second[_agents[a]];
            }

            n.all_value += n.value[a];
        }
    }

    void trial(Node* s,
               const std::chrono::time_point<std::chrono::high_resolution_clock>& start_time) {
        Node* cs = s;
        std::size_t depth = 0;
        bool found_goal = false;

        while (!found_goal && cs &&
               (elapsed_time(start_time) < _time_budget) &&
               (depth < _max_depth)) {
            depth++;

            if (cs->all_goal) {
                if (_debug_logs) spdlog::debug("Found goal state " + cs->state.print() +
                                                ExecutionPolicy::print_thread());
                found_goal = true;
            }
            
            std::unique_ptr<Action>& action = greedy_action(cs);

            if (action) { // next state is neither a goal nor a dead-end
                cs = pick_next_state(cs, *action);
            }
        }
    }

    void update_epsilon_moving_average(const Node& node, const double& node_record_value) {
        if (_epsilon_moving_average_window > 0) {
            double current_epsilon = std::fabs(node_record_value - node.all_value);
            if (_epsilons.size() < _epsilon_moving_average_window) {
                _epsilon_moving_average += current_epsilon / ((double) _epsilon_moving_average_window);
            } else {
                _epsilon_moving_average += (current_epsilon - _epsilons.front()) / ((double) _epsilon_moving_average_window);
                _epsilons.pop_front();
            }
            _epsilons.push_back(current_epsilon);
        }
    }
};

} // namespace skdecide

#endif // SKDECIDE_MA_RTDP_HH
