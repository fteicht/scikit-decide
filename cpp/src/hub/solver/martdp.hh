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
    typedef std::function<bool (const std::size_t&, const std::size_t&, const double&, const double&)> WatchdogFunctor;

    MARTDPSolver(Domain& domain,
                 const GoalCheckerFunctor& goal_checker,
                 const HeuristicFunctor& heuristic,
                 std::size_t time_budget = 3600000,
                 std::size_t rollout_budget = 100000,
                 std::size_t max_depth = 1000,
                 std::size_t max_feasibility_trials = 0, // will then choose nb_agents
                 double graph_expansion_rate = 0.1,
                 std::size_t epsilon_moving_average_window = 100,
                 double epsilon = 0.0, // not a stopping criterion by default
                 double discount = 1.0,
                 double action_choice_noise = 0.1,
                 const double& dead_end_cost = 10e4,
                 bool online_node_garbage = false,
                 bool debug_logs = false,
                 const WatchdogFunctor& watchdog = [](const std::size_t&, const std::size_t&, const double&, const double&){ return true; })
        : _domain(domain), _goal_checker(goal_checker), _heuristic(heuristic),
          _time_budget(time_budget), _rollout_budget(rollout_budget), _max_depth(max_depth),
          _max_feasibility_trials(max_feasibility_trials), _graph_expansion_rate(graph_expansion_rate),
          _epsilon_moving_average_window(epsilon_moving_average_window), _epsilon(epsilon),
          _discount(discount), _dead_end_cost(dead_end_cost), _online_node_garbage(online_node_garbage),
          _debug_logs(debug_logs), _watchdog(watchdog), _epsilon_moving_average(0),
          _current_state(nullptr), _nb_rollouts(0), _nb_agents(0) {
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
            }
            
            auto si = _graph.emplace(s);
            StateNode& root_node = const_cast<StateNode&>(*(si.first)); // we won't change the real key (StateNode::state) so we are safe

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
                              root_node.all_value,
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
            if (_debug_logs) {
                    std::string str = "(";
                    for (const auto& o : si->action->outcomes) {
                        str += "\n    " + o.first->state.print();
                    }
                    str += "\n)";
                    spdlog::debug("Best action's outcomes:\n" + str);
                }
            if (_online_node_garbage && _current_state) {
                std::unordered_set<StateNode*> root_subgraph, child_subgraph;
                compute_reachable_subgraph(_current_state, root_subgraph);
                compute_reachable_subgraph(const_cast<StateNode*>(&(*si)), child_subgraph); // we won't change the real key (StateNode::state) so we are safe
                remove_subgraph(root_subgraph, child_subgraph);
            }
            _current_state = const_cast<StateNode*>(&(*si)); // we won't change the real key (StateNode::state) so we are safe
            return si->action->action;
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
                                        std::make_pair(n.action->action,
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
    double _graph_expansion_rate;
    std::size_t _epsilon_moving_average_window;
    double _epsilon;
    double _discount;
    double _dead_end_cost;
    bool _online_node_garbage;
    bool _debug_logs;
    WatchdogFunctor _watchdog;
    ExecutionPolicy _execution_policy;
    std::unique_ptr<std::mt19937> _gen;
    typename ExecutionPolicy::Mutex _gen_mutex;
    typename ExecutionPolicy::Mutex _time_mutex;

    double _epsilon_moving_average;
    std::list<double> _epsilons;

    struct StateNode;

    struct ActionNode {
        typedef typename SetTypeDeducer<ActionNode, Action>::Set ActionSet;
        Action action;
        typedef std::unordered_map<StateNode*, std::pair<double, std::size_t>> OutcomeMap; // next state nodes owned by _graph
        OutcomeMap outcomes;
        std::vector<typename OutcomeMap::iterator> dist_to_outcome;
        std::discrete_distribution<> dist;
        std::size_t expansions_count;
        std::vector<double> value;
        double all_value;
        StateNode* parent;

        ActionNode(const Action& a)
            : action(a), expansions_count(0), all_value(0.0),
              parent(nullptr) {}
        
        ActionNode(const ActionNode& a)
            : action(a.action), outcomes(a.outcomes),
              dist_to_outcome(a.dist_to_outcome), dist(a.dist),
              expansions_count(a.expansions_count),
              value(a.value), all_value(a.all_value),
              parent(a.parent) {}
        
        struct Key {
            const Action& operator()(const ActionNode& an) const { return an.action; }
        };
    };

    struct StateNode {
        typedef typename SetTypeDeducer<ActionNode, Action>::Set ActionSet;
        State state;
        ActionNode* action; // best action in the graph
        std::unique_ptr<Action> best_action; // can be an action in the graph or the heuristic one
        std::size_t expansions_count;
        ActionSet actions;
        std::vector<double> value;
        double all_value;
        std::vector<bool> goal;
        bool all_goal;
        std::vector<bool> termination;
        bool all_termination;
        std::unordered_set<ActionNode*> parents;

        StateNode(const State& s)
        : state(s), action(nullptr), best_action(nullptr), expansions_count(0),
          all_value(std::numeric_limits<double>::infinity()),
          all_goal(false), all_termination(false) {}
        
        StateNode(const StateNode& s) {
            state(s.state), action(s.action), best_action(s.best_action),
            expansions_count(s.expansions_count), actions(s.actions),
            value(s.value), all_value(s.all_value), goal(s.goal),
            all_goal(s.all_goal), termination(s.termination),
            all_termination(s.all_termination), parents(s.parents) {}
        }
        
        struct Key {
            const State& operator()(const StateNode& sn) const { return sn.state; }
        };
    };

    typedef typename SetTypeDeducer<StateNode, State>::Set Graph;
    Graph _graph;
    StateNode* _current_state;
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

    void expand_state(StateNode* s) {
        if (_debug_logs) spdlog::debug("Trying to expand state " + s->state.print());
        
        if (s->actions.empty()) {
            Predicate termination;
            for (auto a: _agents) {
                termination[a] = false;
            }
            initialize_node(*s, termination);
        }

        if (s->actions.empty()) {
            s->expansions_count += generate_more_actions(s);
        } else {
            std::bernoulli_distribution dist_state_expansion(std::exp(- _graph_expansion_rate * (s->expansions_count)));
            if(dist_state_expansion(*_gen)) {
                s->expansions_count += generate_more_actions(s);
            }
        }
    }

    StateNode* expand_action(ActionNode* a) {
        if (_debug_logs) spdlog::debug("Trying to expand action " + a->action.print());
        EnvironmentOutcome outcome = _domain.sample(a->parent->state, a->action);
        auto i = _graph.emplace(outcome.observation());
        StateNode& next_node = const_cast<StateNode&>(*(i.first)); // we won't change the real key (StateNode::state) so we are safe

        auto tv = outcome.transition_value();
        double transition_cost = 0.0;
        for (const auto& agent : _agents) {
            transition_cost += tv[agent].cost();
        }

        if (i.second) { // new node
            initialize_node(next_node, outcome.termination());
        }
        auto ins = a->outcomes.emplace(std::make_pair(&next_node, std::make_pair(transition_cost, 1)));

        // Update the outcome's reward and visits count
        if (ins.second) { // new outcome
            if (_debug_logs) spdlog::debug("Discovered new outcome " + next_node.state.print());
            a->dist_to_outcome.push_back(ins.first);
            a->expansions_count += 1;
            next_node.parents.insert(a);
        } else { // known outcome
            if (_debug_logs) spdlog::debug("Discovered known outcome " + next_node.state.print());
            std::pair<double, std::size_t>& mp = ins.first->second;
            mp.first = ((double) (mp.second * mp.first) + transition_cost) / ((double) (mp.second + 1));
            mp.second += 1;
        }

        // Reconstruct the probability distribution
        std::vector<double> weights(a->dist_to_outcome.size());
        for (unsigned int oid = 0 ; oid < weights.size() ; oid++) {
            weights[oid] = (double) a->dist_to_outcome[oid]->second.second;
        }
        a->dist = std::discrete_distribution<>(weights.begin(), weights.end());

        return &next_node;
    }

    bool generate_more_actions(StateNode* s) {
        if (_debug_logs) spdlog::debug("Generating (more) actions for state " + s->state.print());
        bool new_actions = false;

        for (std::size_t agent = 0 ; agent < _nb_agents ; agent++) {
            if (_debug_logs) spdlog::debug("Trying agent " + _agents[agent].print() + " actions");
            auto agent_applicable_actions = _domain.get_agent_applicable_actions(s->state, Action(), _agents[agent]).get_elements();

            if (agent_applicable_actions.empty()) {
                if (_debug_logs) spdlog::debug("No agent applicable actions");
                continue;
            } else {
                for (auto action : agent_applicable_actions) {
                    if (_debug_logs) spdlog::debug("Trying agent action " + action.print());
                    Action agent_actions;
                    agent_actions[_agents[agent]] = action;

                    // try to find a feasible assignment
                    bool feasible = false;
                    std::size_t feasibility_trial = 0;
                    
                    while (!feasible && feasibility_trial < _max_feasibility_trials) {
                        std::shuffle(_agents_orders[agent].begin(), _agents_orders[agent].end(), *_gen);
                        feasibility_trial++;
                        feasible = true;

                        // construct the joint action
                        for (auto other_agent : _agents_orders[agent]) {
                            auto other_agent_aa = _domain.get_agent_applicable_actions(s->state, agent_actions, _agents[other_agent]);

                            if (other_agent_aa.empty()) {
                                feasible = false;
                                break;
                            }
                            
                            // Is the agent's optimal action applicable ?
                            if ((s->best_action) &&
                                other_agent_aa.contains((*(s->best_action))[_agents[other_agent]]) &&
                                !(_action_choice_noise_dist(*_gen))) { // choose it with high probability
                                    agent_actions[_agents[other_agent]] = (*(s->best_action))[_agents[other_agent]];
                            } else {
                                agent_actions[_agents[other_agent]] = other_agent_aa.sample();
                            }
                        }
                    }

                    if (feasible) {
                        // Add the action
                        auto a = s->actions.emplace(agent_actions);
                        ActionNode* action_node = &const_cast<ActionNode&>(*(a.first)); // we won't change the real key (ActionNode::action) so we are safe
                        if (a.second) { // new action
                            new_actions = true;
                            action_node->parent = s;
                            action_node->value.resize(_nb_agents, std::numeric_limits<double>::infinity());
                            action_node->all_value = std::numeric_limits<double>::infinity();

                            // add one sampled outcome
                            expand_action(action_node);
                        }
                    } else if (_debug_logs) spdlog::debug("Failed finding a joint applicable action");
                }
            }
        }

        return new_actions;
    }

    ActionNode* greedy_action(StateNode* s) {
        if (_debug_logs) spdlog::debug("Updating state " + s->state.print());
        
        double best_value = std::numeric_limits<double>::infinity();
        ActionNode* best_action = nullptr;

        for (const ActionNode& act : s->actions) {
            if (_debug_logs) spdlog::debug("Computing Q-value of (" + s->state.print() + ", " + act.action.print() + ")");

            ActionNode& action = const_cast<ActionNode&>(act); // we won't change the real key (ActionNode::action) so we are safe
            action.all_value = 0.0;
            std::for_each(action.value.begin(), action.value.end(), [](double& v){ v = 0.0; });

            for (const auto& outcome : action.outcomes) {
                double outcome_cost = outcome.second.first;
                double outcome_probability = ((double) outcome.second.second) / ((double) action.expansions_count);
                for (std::size_t a = 0 ; a < _nb_agents ; a++) {
                    action.value[a] = outcome_probability * (outcome_cost + (_discount * outcome.first->value[a]));
                    action.all_value += action.value[a];
                }
            }

            if (action.all_value < best_value) {
                best_value = action.all_value;
                best_action = &action;
            }

            if (_debug_logs) spdlog::debug("Updated Q-value of action " + action.action.print() +
                                           " with value " + StringConverter::from(action.all_value));
        }

        if (_debug_logs) {
            if (best_action) {
                spdlog::debug("Greedy action of state " + s->state.print() + ": " +
                              best_action->action.print() +
                              " with value " + StringConverter::from(best_value));
            } else {
                spdlog::debug("State " + s->state.print() +
                              " is a dead-end or a goal (no feasible actions found)");
            }
        }

        s->action = best_action;
        s->best_action = std::make_unique<Action>(best_action->action);
        s->all_value = best_value;
        return s->action; // action is nullptr if state is a dead-end or a goal
    }

    // void depth_first_search_applicable_action(const State& state, const Agent& agent) {

    // }

    StateNode* pick_next_state(ActionNode* a) {
        if (_debug_logs) spdlog::debug("Picking next state from State " + a->parent->state.print() +
                                       " with action " + a->action.print());

        StateNode* next_node = expand_action(a);
        
        if (_debug_logs) spdlog::debug("Picked next state " + next_node->state.print() +
                                       " from state " + a->parent->state.print() +
                                       " and action " + a->action.print());
        return next_node;
    }

    void backtrack_values(StateNode* s) {
        std::unordered_set<StateNode*> frontier;
        std::unordered_set<StateNode*> visited;
        frontier.insert(s);
        visited.insert(s);

        while (!frontier.empty()) {
            std::unordered_set<StateNode*> new_frontier;
            for (StateNode* f : frontier) {
                for (ActionNode* a : f->parents) {
                    if (visited.find(a->parent) == visited.end()) {
                        greedy_action(a->parent);
                        visited.insert(a->parent);
                        new_frontier.insert(a->parent);
                    }
                }
            }
            frontier = new_frontier;
        }
    }

    void initialize_node(StateNode& n, const Predicate& termination) {
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
        
        n.action = nullptr;
        n.best_action = std::make_unique<Action>();

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
                (*n.best_action)[_agents[a]] = h.second[_agents[a]];
            }

            n.all_value += n.value[a];
        }
    }

    void trial(StateNode* s,
               const std::chrono::time_point<std::chrono::high_resolution_clock>& start_time) {
        StateNode* cs = s;
        std::size_t depth = 0;

        while ((elapsed_time(start_time) < _time_budget) &&
               (depth < _max_depth)) {
            depth++;

            if (cs->all_goal) {
                if (_debug_logs) spdlog::debug("Found goal state " + cs->state.print());
                break;
            } else if (cs->all_termination) {
                if (_debug_logs) spdlog::debug("Found dead-end state " + cs->state.print());
                break;
            }
            
            expand_state(cs);
            ActionNode* action = greedy_action(cs);

            if (action) {
                cs = pick_next_state(action);
            } else { // current state is a dead-end
                cs->all_value = _dead_end_cost;
                std::for_each(cs->value.begin(), cs->value.end(),
                              [this](auto& v){ v = _dead_end_cost / ((double) _nb_agents); });
                break;
            }
        }

        backtrack_values(cs);
    }

    void compute_reachable_subgraph(StateNode* node, std::unordered_set<StateNode*>& subgraph) {
        std::unordered_set<StateNode*> frontier;
        frontier.insert(node);
        subgraph.insert(node);
        while(!frontier.empty()) {
            std::unordered_set<StateNode*> new_frontier;
            for (auto& n : frontier) {
                for (auto& action : n->actions) {
                    for (auto& outcome : action.outcomes) {
                        if (subgraph.find(outcome.first) == subgraph.end()) {
                            new_frontier.insert(outcome.first);
                            subgraph.insert(outcome.first);
                        }
                    }
                }
            }
            frontier = new_frontier;
        }
    }

    void remove_subgraph(std::unordered_set<StateNode*>& root_subgraph, std::unordered_set<StateNode*>& child_subgraph) {
        for (auto& n : root_subgraph) {
            if (child_subgraph.find(n) == child_subgraph.end()) {
                _graph.erase(StateNode(n->state));
            }
        }
    }

    void update_epsilon_moving_average(const StateNode& node, const double& node_record_value) {
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
