/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 * This is the scikit-decide implementation of MCTS and UCT from
 * "A Survey of Monte Carlo Tree Search Methods" by Browne et al
 * (IEEE Transactions on Computational Intelligence  and AI in games,
 * 2012). We additionnally implement a heuristic value estimate as in
 * "Monte-Carlo tree search and rapid action value estimation in 
 * computer Go" by Gelly and Silver (Artificial Intelligence, 2011)
 * except that the heuristic estimate is called on states but not
 * on state-action pairs to be more in line with heuristic search
 * algorithms in the literature and other implementations of
 * heuristic search algorithms in scikit-decide.
 */
#ifndef SKDECIDE_MCTS_IMPL_HH
#define SKDECIDE_MCTS_IMPL_HH

#include <boost/range/irange.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "utils/string_converter.hh"
#include "utils/execution.hh"

namespace skdecide {

// === StepTransitionMode implementation ===

#define SK_MCTS_STEP_TRANSITION_MODE_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_STEP_TRANSITION_MODE_CLASS \
StepTransitionMode<Tsolver>

SK_MCTS_STEP_TRANSITION_MODE_TEMPLATE_DECL
void SK_MCTS_STEP_TRANSITION_MODE_CLASS::init_rollout(Tsolver& solver,
                                                      const std::size_t* thread_id) const {
    solver.domain().reset(thread_id);
    std::for_each(solver.action_prefix().begin(), solver.action_prefix().end(),
                    [&solver, &thread_id](const typename Tsolver::Domain::Action& a){solver.domain().step(a, thread_id);});
}

SK_MCTS_STEP_TRANSITION_MODE_TEMPLATE_DECL
typename Tsolver::Domain::EnvironmentOutcome
SK_MCTS_STEP_TRANSITION_MODE_CLASS::random_next_outcome(Tsolver& solver,
                                                        const std::size_t* thread_id,
                                                        const typename Tsolver::Domain::State& state,
                                                        const typename Tsolver::Domain::Action& action) const {
    return solver.domain().step(action, thread_id);
}

SK_MCTS_STEP_TRANSITION_MODE_TEMPLATE_DECL
typename Tsolver::StateNode*
SK_MCTS_STEP_TRANSITION_MODE_CLASS::random_next_node(Tsolver& solver,
                                                     const std::size_t* thread_id,
                                                     typename Tsolver::ActionNode& action) const {
    auto outcome = solver.domain().step(action.action, thread_id);
    typename Tsolver::StateNode* n = nullptr;

    solver.execution_policy().protect([&n, &solver, &outcome](){
        auto si = solver.graph().find(typename Tsolver::StateNode(outcome.observation()));
        if (si != solver.graph().end()) {
            // we won't change the real key (ActionNode::action) so we are safe
            n = &const_cast<typename Tsolver::StateNode&>(*si);
        }
    });

    return n;
}

// === SampleTransitionMode implementation ===

#define SK_MCTS_SAMPLE_TRANSITION_MODE_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_SAMPLE_TRANSITION_MODE_CLASS \
SampleTransitionMode<Tsolver>

SK_MCTS_SAMPLE_TRANSITION_MODE_TEMPLATE_DECL
void SK_MCTS_SAMPLE_TRANSITION_MODE_CLASS::init_rollout(Tsolver& solver,
                                                        const std::size_t* thread_id) const {}

SK_MCTS_SAMPLE_TRANSITION_MODE_TEMPLATE_DECL
typename Tsolver::Domain::EnvironmentOutcome
SK_MCTS_SAMPLE_TRANSITION_MODE_CLASS::random_next_outcome(Tsolver& solver,
                                                          const std::size_t* thread_id,
                                                          const typename Tsolver::Domain::State& state,
                                                          const typename Tsolver::Domain::Action& action) const {
    return solver.domain().sample(state, action, thread_id);
}

SK_MCTS_SAMPLE_TRANSITION_MODE_TEMPLATE_DECL
typename Tsolver::StateNode*
SK_MCTS_SAMPLE_TRANSITION_MODE_CLASS::random_next_node(Tsolver& solver,
                                                       const std::size_t* thread_id,
                                                       typename Tsolver::ActionNode& action) const {
    typename Tsolver::StateNode* n = nullptr;

    solver.execution_policy().protect([&n, &action, &solver](){
        solver.execution_policy().protect([&n, &action, &solver](){
            n = action.dist_to_outcome[action.dist(solver.gen())]->first;
        }, solver.gen_mutex());
    }, action.parent->mutex);

    return n;
}

// === DistributionTransitionMode implementation ===

#define SK_MCTS_DISTRIBUTION_TRANSITION_MODE_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_DISTRIBUTION_TRANSITION_MODE_CLASS \
DistributionTransitionMode<Tsolver>

SK_MCTS_DISTRIBUTION_TRANSITION_MODE_TEMPLATE_DECL
void SK_MCTS_DISTRIBUTION_TRANSITION_MODE_CLASS::init_rollout(Tsolver& solver,
                                                              const std::size_t* thread_id) const {}

SK_MCTS_DISTRIBUTION_TRANSITION_MODE_TEMPLATE_DECL
typename Tsolver::Domain::EnvironmentOutcome random_next_outcome(Tsolver& solver,
                                                                 const std::size_t* thread_id,
                                                                 const typename Tsolver::Domain::State& state,
                                                                 const typename Tsolver::Domain::Action& action) const {
    return solver.domain().sample(state, action, thread_id);
}

SK_MCTS_DISTRIBUTION_TRANSITION_MODE_TEMPLATE_DECL
typename Tsolver::StateNode* random_next_node(Tsolver& solver,
                                              const std::size_t* thread_id,
                                              typename Tsolver::ActionNode& action) const {
    typename Tsolver::StateNode* n = nullptr;

    solver.execution_policy().protect([&n, &action, &solver](){
        solver.execution_policy().protect([&n, &action, &solver](){
            n = action.dist_to_outcome[action.dist(solver.gen())]->first;
        }, solver.gen_mutex());
    }, action.parent->mutex);

    return n;
}

// === DefaultTreePolicy implementation ===

#define SK_MCTS_DEFAULT_TREE_POLICY_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_DEFAULT_TREE_POLICY_CLASS \
DefaultTreePolicy<Tsolver>

SK_MCTS_DEFAULT_TREE_POLICY_TEMPLATE_DECL
typename Tsolver::StateNode*
SK_MCTS_DEFAULT_TREE_POLICY_CLASS::operator()(Tsolver& solver,
                                              const std::size_t* thread_id, // for parallelisation
                                              const typename Tsolver::Expander& expander,
                                              const typename Tsolver::ActionSelectorOptimization& action_selector,
                                              typename Tsolver::StateNode& n,
                                              std::size_t& d) const {
    try {
        if (solver.debug_logs()) {
            solver.execution_policy().protect([&n](){
                spdlog::debug("Launching default tree policy from state " + n.state.print() +
                              Tsolver::ExecutionPolicy::print_thread());
            }, n.mutex);
        }

        solver.transition_mode().init_rollout(solver, thread_id);
        typename Tsolver::StateNode* current_node = &n;

        while(!(current_node->terminal) && d < solver.max_depth()) {
            typename Tsolver::StateNode* next_node = expander(solver, thread_id, *current_node);
            d++;

            if (next_node == nullptr) { // node fully expanded
                typename Tsolver::ActionNode* action = action_selector(solver, thread_id, *current_node);
                
                if (action == nullptr) {
                    // It might happen in parallel execution mode when the current node's actions are all being
                    // expanded by concurrent threads that claim the node is expanded but not yet backpropagated
                    // and a new thread meantime comes and sees the node as expanded, thus all action visits counts
                    // are still equal to zero (implying action_selector to return nullptr).
                    // This shall NOT happen in sequential execution mode.
                    break;
                } else {
                    next_node = solver.transition_mode().random_next_node(solver, thread_id, *action);

                    if (next_node == nullptr) { // might happen with step transition mode and stochastic environments
                        break;
                    } else {
                        current_node = next_node;
                    }
                }
            } else {
                current_node = next_node;
                break;
            }
        }

        return current_node;
    } catch (const std::exception& e) {
        solver.execution_policy().protect([&n, &e](){
            spdlog::error("SKDECIDE exception in MCTS when simulating the tree policy from state " + n.state.print() + ": " + e.what() +
                          Tsolver::ExecutionPolicy::print_thread());
        }, n.mutex);
        throw;
    }
}

// === FullExpand implementation ===

template <typename Tsolver>
template <typename Ttransition_mode>
struct FullExpand<Tsolver>::ExpandActionImplementation<
            Ttransition_mode,
            typename std::enable_if<std::is_same<Ttransition_mode, DistributionTransitionMode<Tsolver>>::value>::type> {
    
    const HeuristicFunctor& _heuristic;

    ExpandActionImplementation(const HeuristicFunctor& heuristic)
    : _heuristic(heuristic) {}

    typename Tsolver::StateNode* expand_action(Tsolver& solver,
                                               const std::size_t* thread_id,
                                               typename Tsolver::StateNode& state,
                                               typename Tsolver::ActionNode& action) const {
        try {
            // Generate the next states of this action
            auto next_states = solver.domain().get_next_state_distribution(state.state, action.action, thread_id).get_values();
            std::vector<typename Tsolver::StateNode*> untried_outcomes;
            std::vector<double> weights;
            std::vector<double> outcome_weights;

            for (auto ns : next_states) {
                std::pair<typename Tsolver::Graph::iterator, bool> i;

                solver.execution_policy().protect([&i, &solver, &ns](){
                    i = solver.graph().emplace(ns.state());
                });

                typename Tsolver::StateNode& next_node = const_cast<typename Tsolver::StateNode&>(*(i.first)); // we won't change the real key (StateNode::state) so we are safe
                double reward = 0.0;

                solver.execution_policy().protect([&reward, &solver, &state, &action, &next_node, &thread_id](){
                    reward = solver.domain().get_transition_value(state.state, action.action, next_node.state, thread_id).reward();
                }, next_node.mutex);

                solver.execution_policy().protect([&action, &next_node, &outcome_weights, &reward, &ns](){
                    auto ii = action.outcomes.insert(std::make_pair(&next_node, std::make_pair(reward, 1)));

                    if (ii.second) { // new outcome
                        action.dist_to_outcome.push_back(ii.first);
                        outcome_weights.push_back(ns.probability());
                    } else { // existing outcome (following code not efficient but hopefully very rare case if domain is well defined)
                        for (unsigned int oid = 0 ; oid < outcome_weights.size() ; oid++) {
                            if (action.dist_to_outcome[oid]->first == ii.first->first) { // found my outcome!
                                std::pair<double, std::size_t>& mp = ii.first->second;
                                mp.first = ((double) (outcome_weights[oid] * mp.first) + (reward * ns.probability())) / ((double) (outcome_weights[oid] + ns.probability()));
                                outcome_weights[oid] += ns.probability();
                                mp.second += 1; // useless in this mode a priori, but just keep track for coherency
                                break;
                            }
                        }
                    }
                }, action.parent->mutex);

                solver.execution_policy().protect([this, &next_node, &action, &i, &solver, &thread_id, &untried_outcomes, &weights, &ns](){
                    next_node.parents.insert(&action);

                    if (i.second) { // new node
                        next_node.terminal = solver.domain().is_terminal(next_node.state, thread_id);
                        std::pair<typename Tsolver::Domain::Value, std::size_t> h = _heuristic(solver.domain(), next_node.state, thread_id);
                        next_node.value = h.first.reward();
                        next_node.visits_count = h.second;
                    }

                    if (next_node.actions.empty()) {
                        if (solver.debug_logs()) spdlog::debug("Candidate next state: " + next_node.state.print() +
                                                               Tsolver::ExecutionPolicy::print_thread());
                        untried_outcomes.push_back(&next_node);
                        weights.push_back(ns.probability());
                    }
                }, next_node.mutex);
            }

            // Record the action's outcomes distribution
            solver.execution_policy().protect([&action, &outcome_weights](){
                action.dist = std::discrete_distribution<>(outcome_weights.begin(), outcome_weights.end());
            }, action.parent->mutex);

            // Pick a random next state
            if (untried_outcomes.empty()) {
                // All next states already visited => pick a random next state using action.dist
                std::size_t outcome_id = 0;

                solver.execution_policy().protect([&outcome_id, &action, &solver](){
                    outcome_id = action.dist(solver.gen());
                }, solver.gen_mutex());

                typename Tsolver::StateNode* outcome = nullptr;

                solver.execution_policy().protect([&action, &outcome, &outcome_id](){
                    outcome = action.dist_to_outcome[outcome_id]->first;
                }, action.parent->mutex);

                return outcome;
            } else {
                // Pick a random next state among untried ones
                std::discrete_distribution<> odist(weights.begin(), weights.end());
                std::size_t outcome_id = 0;

                solver.execution_policy().protect([&outcome_id, &odist, &solver](){
                    outcome_id = odist(solver.gen());
                }, solver.gen_mutex());

                return untried_outcomes[outcome_id];
            }
        } catch (const std::exception& e) {
            solver.execution_policy().protect([&action, &e](){
                spdlog::error("SKDECIDE exception in MCTS when expanding action " + action.action.print() + ": " + e.what() +
                              Tsolver::ExecutionPolicy::print_thread());
            }, action.parent->mutex);
            throw;
        }
    }
};

template <typename Tsolver>
template <typename Ttransition_mode>
struct FullExpand<Tsolver>::ExpandActionImplementation<
            Ttransition_mode,
            typename std::enable_if<std::is_same<Ttransition_mode, StepTransitionMode<Tsolver>>::value ||
                                    std::is_same<Ttransition_mode, SampleTransitionMode<Tsolver>>::value>::type> {
    
    const HeuristicFunctor& _heuristic;
    mutable typename Tsolver::ExecutionPolicy::template atomic<bool> _checked_transition_mode;

    ExpandActionImplementation(const HeuristicFunctor& heuristic)
    : _heuristic(heuristic), _checked_transition_mode(false) {}

    typename Tsolver::StateNode* expand_action(Tsolver& solver,
                                               const std::size_t* thread_id,
                                               typename Tsolver::StateNode& state,
                                               typename Tsolver::ActionNode& action) const {
        try {
            if (!_checked_transition_mode) {
                spdlog::warn("Using MCTS full expansion mode with step() or sample() domain's transition mode assumes the domain is deterministic (unpredictable result otherwise).");
                _checked_transition_mode = true;
            }
            // Generate the next state of this action
            typename Tsolver::Domain::EnvironmentOutcome to = solver.transition_mode().random_next_outcome(solver, thread_id, state.state, action.action);
            std::pair<typename Tsolver::Graph::iterator, bool> i;

            solver.execution_policy().protect([&i, &solver, &to](){
                i = solver.graph().emplace(to.observation());
            });
            
            typename Tsolver::StateNode& next_node = const_cast<typename Tsolver::StateNode&>(*(i.first)); // we won't change the real key (StateNode::state) so we are safe
            
            solver.execution_policy().protect([&action, &next_node, &to](){
                auto ii = action.outcomes.insert(std::make_pair(&next_node, std::make_pair(to.transition_value().reward(), 1)));
                action.dist_to_outcome.push_back(ii.first);
            }, action.parent->mutex);

            solver.execution_policy().protect([this, &next_node, &action, &i, &to, &solver, &thread_id](){
                next_node.parents.insert(&action);

                if (i.second) { // new node
                    next_node.terminal = to.termination();
                    std::pair<typename Tsolver::Domain::Value, std::size_t> h = expander._heuristic(solver.domain(), next_node.state, thread_id);
                    next_node.value = h.first.reward();
                    next_node.visits_count = h.second;
                }
            }, next_node.mutex);

            // Record the action's outcomes distribution
            solver.execution_policy().protect([&action](){
                action.dist = std::discrete_distribution<>({1.0});
            }, action.parent->mutex);

            if (solver.debug_logs()) {
                solver.execution_policy().protect([&next_node](){
                    spdlog::debug("Candidate next state: " + next_node.state.print() +
                                  Tsolver::ExecutionPolicy::print_thread());
                }, next_node.mutex);
            }
            
            return &next_node;
        } catch (const std::exception& e) {
            solver.execution_policy().protect([&action, &e](){
                spdlog::error("SKDECIDE exception in MCTS when expanding action " + action.action.print() + ": " + e.what() +
                              Tsolver::ExecutionPolicy::print_thread());
            }, action.parent->mutex);
            throw;
        }
    }
};

#define SK_MCTS_FULL_EXPAND_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_FULL_EXPAND_CLASS \
FullExpand<Tsolver>

SK_MCTS_FULL_EXPAND_TEMPLATE_DECL
SK_MCTS_FULL_EXPAND_CLASS::FullExpand(const HeuristicFunctor& heuristic)
    : _heuristic(heuristic) {
    _action_expander = std::make_unique<ExpandActionImplementation>(_heuristic);
}

SK_MCTS_FULL_EXPAND_TEMPLATE_DECL
SK_MCTS_FULL_EXPAND_CLASS::FullExpand(const FullExpand& other)
    : _heuristic(other._heuristic) {
    _action_expander = std::make_unique<ExpandActionImplementation>(_heuristic);
}

SK_MCTS_FULL_EXPAND_TEMPLATE_DECL
typename Tsolver::StateNode*
SK_MCTS_FULL_EXPAND_CLASS::operator()(Tsolver& solver,
                                      const std::size_t* thread_id,
                                      typename Tsolver::StateNode& n) const {
    try {
        if (solver.debug_logs()) {
            solver.execution_policy().protect([&n](){
                spdlog::debug("Testing expansion of state " + n.state.print() +
                              Tsolver::ExecutionPolicy::print_thread());
            }, n.mutex);
        }

        if (n.expanded) {
            if (solver.debug_logs()) { spdlog::debug("State already fully expanded" +
                                                     Tsolver::ExecutionPolicy::print_thread()); }
            return nullptr;
        }

        // Generate applicable actions if not already done
        solver.execution_policy().protect([&n, &solver, &thread_id](){
            if (n.actions.empty()) {
                if (solver.debug_logs()) { spdlog::debug("State never expanded, generating all next actions" +
                                                         Tsolver::ExecutionPolicy::print_thread()); }
                auto applicable_actions = solver.domain().get_applicable_actions(n.state, thread_id).get_elements();

                for (const auto& a : applicable_actions) {
                    auto i = n.actions.emplace(typename Tsolver::ActionNode(a));

                    if (i.second) {
                        // we won't change the real key (ActionNode::action) so we are safe
                        const_cast<typename Tsolver::ActionNode&>(*i.first).parent = &n;
                    }
                }
            }
        }, n.mutex);

        // Check for untried outcomes
        if (solver.debug_logs()) { spdlog::debug("Checking for untried outcomes" +
                                                 Tsolver::ExecutionPolicy::print_thread()); }
        std::vector<std::pair<typename Tsolver::ActionNode*, typename Tsolver::StateNode*>> untried_outcomes;
        std::vector<double> weights;

        solver.execution_policy().protect([&n, &untried_outcomes, &weights](){
            for (auto& a : n.actions) {
                // we won't change the real key (ActionNode::action) so we are safe
                typename Tsolver::ActionNode& ca = const_cast<typename Tsolver::ActionNode&>(a);

                if (a.outcomes.empty()) {
                    // we won't change the real key (ActionNode::action) so we are safe
                    untried_outcomes.push_back(std::make_pair(&ca, nullptr));
                    weights.push_back(1.0);
                } else {
                    // Check if there are next states that have been never visited
                    std::vector<double> probs = a.dist.probabilities();

                    for (std::size_t p = 0 ; p < probs.size() ; p++) {
                        typename Tsolver::StateNode* on = ca.dist_to_outcome[p]->first;

                        if (on->visits_count == 0) {
                            untried_outcomes.push_back(std::make_pair(&ca, on));
                            weights.push_back(probs[p]);
                        }
                    }
                }
            }
        }, n.mutex);

        if (untried_outcomes.empty()) { // nothing to expand
            if (solver.debug_logs()) { spdlog::debug("All outcomes already tried" +
                                                     Tsolver::ExecutionPolicy::print_thread()); }
            n.expanded = true;
            return nullptr;
        } else {
            std::discrete_distribution<> odist(weights.begin(), weights.end());
            std::size_t outcome_id = 0;
            solver.execution_policy().protect([&outcome_id, &odist, &solver](){
                outcome_id = odist(solver.gen());
            }, solver.gen_mutex());
            auto& uo = untried_outcomes[outcome_id];

            if (uo.second == nullptr) { // unexpanded action
                if (solver.debug_logs()) { spdlog::debug("Found one unexpanded action: " + uo.first->action.print() +
                                                         Tsolver::ExecutionPolicy::print_thread()); }
                return expand_action(solver, thread_id, n, *(uo.first));
            } else { // expanded action, just return the selected next state
                if (solver.debug_logs()) {
                    solver.execution_policy().protect([&uo](){
                        spdlog::debug("Found one untried outcome: action " + uo.first->action.print() +
                                      " and next state " + uo.second->state.print() +
                                      Tsolver::ExecutionPolicy::print_thread());
                    }, uo.second->mutex);
                }
                return uo.second;
            }
        }
    } catch (const std::exception& e) {
        solver.execution_policy().protect([&n, &e](){
            spdlog::error("SKDECIDE exception in MCTS when expanding state " + n.state.print() + ": " + e.what() +
                          Tsolver::ExecutionPolicy::print_thread());
        }, n.mutex);
        throw;
    }
}

SK_MCTS_FULL_EXPAND_TEMPLATE_DECL
typename Tsolver::StateNode*
SK_MCTS_FULL_EXPAND_CLASS::expand_action(Tsolver& solver,
                                         const std::size_t* thread_id,
                                         typename Tsolver::StateNode& state,
                                         typename Tsolver::ActionNode& action) const {
    return _action_expander->expand_action(solver, thread_id, state, action);
}

// === PartialExpand implementation ===

#define SK_MCTS_PARTIAL_EXPAND_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_PARTIAL_EXPAND_CLASS \
PartialExpand<Tsolver>

SK_MCTS_PARTIAL_EXPAND_TEMPLATE_DECL
SK_MCTS_PARTIAL_EXPAND_CLASS::PartialExpand(const double& state_expansion_rate,
                                            const double& action_expansion_rate,
                                            const HeuristicFunctor& heuristic)
    : _heuristic(heuristic),
    _state_expansion_rate(state_expansion_rate),
    _action_expansion_rate(action_expansion_rate) {}

SK_MCTS_PARTIAL_EXPAND_TEMPLATE_DECL
SK_MCTS_PARTIAL_EXPAND_CLASS::PartialExpand(const PartialExpand& other)
    : _heuristic(other._heuristic),
      _state_expansion_rate(other._state_expansion_rate),
      _action_expansion_rate(other._action_expansion_rate) {}

SK_MCTS_PARTIAL_EXPAND_TEMPLATE_DECL
typename Tsolver::StateNode*
SK_MCTS_PARTIAL_EXPAND_CLASS::operator()(Tsolver& solver,
                                         const std::size_t* thread_id,
                                         typename Tsolver::StateNode& n) const {
    try {
        if (solver.debug_logs()) {
            solver.execution_policy().protect([&n](){
                spdlog::debug("Testing expansion of state " + n.state.print() +
                              Tsolver::ExecutionPolicy::print_thread());
            }, n.mutex);
        }

        // Sample an action
        std::bernoulli_distribution dist_state_expansion(std::exp(- _state_expansion_rate * n.expansions_count));
        typename Tsolver::ActionNode* action_node = nullptr;
        bool dist_res = false;

        solver.execution_policy().protect([&dist_res, &solver, &dist_state_expansion](){
            dist_res = dist_state_expansion(solver.gen());
        }, solver.gen_mutex());

        if (dist_res) {
            typename Tsolver::Domain::Action action = solver.domain().get_applicable_actions(n.state, thread_id).sample();
            solver.execution_policy().protect([&n, &action, &action_node, &solver](){
                auto a = n.actions.emplace(typename Tsolver::ActionNode(action));
                action_node = &const_cast<typename Tsolver::ActionNode&>(*(a.first)); // we won't change the real key (ActionNode::action) so we are safe

                if (a.second) { // new action
                    n.expansions_count += 1;
                    action_node->parent = &n;
                }

                if (solver.debug_logs()) { spdlog::debug("Sampled a new action: " + action_node->action.print() +
                                                         Tsolver::ExecutionPolicy::print_thread()); }
            }, n.mutex);
        } else {
            std::vector<typename Tsolver::ActionNode*> actions;

            solver.execution_policy().protect([&n, &actions](){
                for (auto& a : n.actions) {
                    actions.push_back(&const_cast<typename Tsolver::ActionNode&>(a)); // we won't change the real key (ActionNode::action) so we are safe
                }
            }, n.mutex);

            std::uniform_int_distribution<std::size_t> dist_known_actions(0, actions.size()-1);
            std::size_t action_id = 0;

            solver.execution_policy().protect([&action_id, &solver, &dist_known_actions](){
                action_id = dist_known_actions(solver.gen());
            }, solver.gen_mutex());

            action_node = actions[action_id];
            if (solver.debug_logs()) {
                solver.execution_policy().protect([&action_node](){
                    spdlog::debug("Sampled among known actions: " + action_node->action.print() +
                                  Tsolver::ExecutionPolicy::print_thread());
                }, action_node->parent->mutex);
            }
        }

        // Sample an outcome
        std::bernoulli_distribution dist_action_expansion(std::exp(- _action_expansion_rate * (action_node->expansions_count)));
        typename Tsolver::StateNode* ns = nullptr;
        
        solver.execution_policy().protect([&dist_res, &solver, &dist_action_expansion](){
            dist_res = dist_action_expansion(solver.gen());
        }, solver.gen_mutex());

        if (dist_res) {
            typename Tsolver::Domain::EnvironmentOutcome to = solver.transition_mode().random_next_outcome(solver, thread_id, n.state, action_node->action);
            std::pair<typename Tsolver::Graph::iterator, bool> s;

            solver.execution_policy().protect([&s, &solver, &to](){
                s = solver.graph().emplace(to.observation());
            });
            
            ns = &const_cast<typename Tsolver::StateNode&>(*(s.first)); // we won't change the real key (StateNode::state) so we are safe

            if (s.second) { // new state
                solver.execution_policy().protect([this, &ns, &to, &solver, &thread_id](){
                    ns->terminal = to.termination();
                    std::pair<typename Tsolver::Domain::Value, std::size_t> h = _heuristic(solver.domain(), ns->state, thread_id);
                    ns->value = h.first.reward();
                    ns->visits_count = h.second;
                }, ns->mutex);
            }

            std::pair<typename Tsolver::ActionNode::OutcomeMap::iterator, bool> ins;
            solver.execution_policy().protect([&action_node, &ns, &to, &ins](){
                ins = action_node->outcomes.emplace(std::make_pair(ns, std::make_pair(to.transition_value().reward(), 1)));
            }, action_node->parent->mutex);

            // Update the outcome's reward and visits count
            if (ins.second) { // new outcome
                solver.execution_policy().protect([&action_node, &ins](){
                    action_node->dist_to_outcome.push_back(ins.first);
                    action_node->expansions_count += 1;
                }, action_node->parent->mutex);

                solver.execution_policy().protect([&ns, &action_node](){
                    ns->parents.insert(action_node);
                }, ns->mutex);
            } else { // known outcome
                solver.execution_policy().protect([&ins, &to, &ns](){
                    std::pair<double, std::size_t>& mp = ins.first->second;
                    mp.first = ((double) (mp.second * mp.first) + to.transition_value().reward()) / ((double) (mp.second + 1));
                    mp.second += 1;
                    ns = nullptr; // we have not discovered anything new
                }, action_node->parent->mutex);
            }

            // Reconstruct the probability distribution
            solver.execution_policy().protect([&action_node](){
                std::vector<double> weights(action_node->dist_to_outcome.size());

                for (unsigned int oid = 0 ; oid < weights.size() ; oid++) {
                    weights[oid] = (double) action_node->dist_to_outcome[oid]->second.second;
                }

                action_node->dist = std::discrete_distribution<>(weights.begin(), weights.end());
            }, action_node->parent->mutex);
        } else {
            ns = nullptr; // we have not discovered anything new
        }

        if (solver.debug_logs()) {
            if (ns) {
                solver.execution_policy().protect([&ns](){
                        spdlog::debug("Sampled a new outcome: " + ns->state.print() +
                                      Tsolver::ExecutionPolicy::print_thread());
                }, ns->mutex);
            } else {
                solver.execution_policy().protect([&n](){
                    spdlog::debug("Not expanding state: " + n.state.print() +
                                  Tsolver::ExecutionPolicy::print_thread());
                }, n.mutex);
            }
        }

        return ns;
    } catch (const std::exception& e) {
        solver.execution_policy().protect([&n, &e](){
            spdlog::error("SKDECIDE exception in MCTS when expanding state " + n.state.print() + ": " + e.what() +
                          Tsolver::ExecutionPolicy::print_thread());
        }, n.mutex);
        throw;
    }
}

// === UCB1ActionSelector implementation ===

#define SK_MCTS_UCB1_ACTION_SELECTOR_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_UCB1_ACTION_SELECTOR_CLASS \
UCB1ActionSelector<Tsolver>

SK_MCTS_UCB1_ACTION_SELECTOR_TEMPLATE_DECL
SK_MCTS_UCB1_ACTION_SELECTOR_CLASS::UCB1ActionSelector(double ucb_constant)
    : _ucb_constant(ucb_constant) {}

SK_MCTS_UCB1_ACTION_SELECTOR_TEMPLATE_DECL
SK_MCTS_UCB1_ACTION_SELECTOR_CLASS::UCB1ActionSelector(const UCB1ActionSelector& other)
    : _ucb_constant((double) other._ucb_constant) {}

SK_MCTS_UCB1_ACTION_SELECTOR_TEMPLATE_DECL
typename Tsolver::ActionNode*
SK_MCTS_UCB1_ACTION_SELECTOR_CLASS::operator()(Tsolver& solver,
                                               const std::size_t* thread_id,
                                               const typename Tsolver::StateNode& n) const {
    double best_value = -std::numeric_limits<double>::max();
    typename Tsolver::ActionNode* best_action = nullptr;

    solver.execution_policy().protect([this, &n, &best_value, &best_action, &solver](){
        for (const auto& a : n.actions) {
            if (a.visits_count > 0) {
                double tentative_value = a.value + (2.0 * _ucb_constant * std::sqrt((2.0 * std::log((double) n.visits_count)) / ((double) a.visits_count)));

                if (tentative_value > best_value) {
                    best_value = tentative_value;
                    best_action = &const_cast<typename Tsolver::ActionNode&>(a); // we won't change the real key (ActionNode::action) so we are safe
                }
            }
        }

        if (solver.debug_logs()) { spdlog::debug("UCB1 selection from state " + n.state.print() +
                                                 ": value=" + StringConverter::from(best_value) +
                                                 ", action=" + ((best_action != nullptr)?(best_action->action.print()):("nullptr")) +
                                                 Tsolver::ExecutionPolicy::print_thread()); }
    }, n.mutex);
    
    return best_action;
}

// === BestQValueActionSelector implementation ===

#define SK_MCTS_BEST_QVALUE_ACTION_SELECTOR_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_BEST_QVALUE_ACTION_SELECTOR_CLASS \
BestQValueActionSelector<Tsolver>

SK_MCTS_BEST_QVALUE_ACTION_SELECTOR_TEMPLATE_DECL
typename Tsolver::ActionNode*
SK_MCTS_BEST_QVALUE_ACTION_SELECTOR_CLASS::operator()(Tsolver& solver,
                                                      const std::size_t* thread_id,
                                                      const typename Tsolver::StateNode& n) const {
    double best_value = -std::numeric_limits<double>::max();
    typename Tsolver::ActionNode* best_action = nullptr;

    solver.execution_policy().protect([&n, &best_value, &best_action, &solver](){
        for (const auto& a : n.actions) {
            if (a.visits_count > 0) {
                if (a.value > best_value) {
                    best_value = a.value;
                    best_action = &const_cast<typename Tsolver::ActionNode&>(a); // we won't change the real key (ActionNode::action) so we are safe
                }
            }
        }

        if (solver.debug_logs()) { spdlog::debug("Best Q-value selection from state " + n.state.print() +
                                                 ": value=" + StringConverter::from(best_value) +
                                                 ", action=" + ((best_action != nullptr)?(best_action->action.print()):("nullptr")) +
                                                 Tsolver::ExecutionPolicy::print_thread()); }
    }, n.mutex);
    
    return best_action;
}

// === DefaultRolloutPolicy implementation ===

#define SK_MCTS_DEFAULT_ROLLOUT_POLICY_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_DEFAULT_ROLLOUT_POLICY_CLASS \
DefaultRolloutPolicy<Tsolver>

SK_MCTS_DEFAULT_ROLLOUT_POLICY_TEMPLATE_DECL
SK_MCTS_DEFAULT_ROLLOUT_POLICY_CLASS::DefaultRolloutPolicy(const PolicyFunctor& policy)
    : _policy(policy) {}

SK_MCTS_DEFAULT_ROLLOUT_POLICY_TEMPLATE_DECL
void SK_MCTS_DEFAULT_ROLLOUT_POLICY_CLASS::operator()(Tsolver& solver,
                                                      const std::size_t* thread_id,
                                                      typename Tsolver::StateNode& n,
                                                      std::size_t d) const {
    try {
        typename Tsolver::Domain::State current_state;

        solver.execution_policy().protect([&solver, &n, &current_state](){
            if (solver.debug_logs()) { spdlog::debug("Launching default rollout policy from state " + n.state.print() +
                                                     Tsolver::ExecutionPolicy::print_thread()); }
            current_state = n.state;
        }, n.mutex);

        bool termination = false;
        std::size_t current_depth = d;
        double reward = 0.0;
        double gamma_n = 1.0;

        while(!termination && current_depth < solver.max_depth()) {
            typename Tsolver::Domain::Action action = _policy(solver.domain(), current_state, thread_id);
            typename Tsolver::Domain::EnvironmentOutcome o = solver.transition_mode().random_next_outcome(solver, thread_id, current_state, action);
            reward += gamma_n * (o.transition_value().reward());
            gamma_n *= solver.discount();
            current_state = o.observation();
            termination = o.termination();
            current_depth++;
            if (solver.debug_logs()) { spdlog::debug("Sampled transition: action=" + action.print() +
                                                     ", next state=" + current_state.print() +
                                                     ", reward=" + StringConverter::from(o.transition_value().reward()) +
                                                     Tsolver::ExecutionPolicy::print_thread()); }
        }

        // since we can come to state n after exhausting the depth, n might be already visited
        // so don't erase its value but rather update it
        solver.execution_policy().protect([&n, &reward](){
            n.value = ((n.visits_count * n.value)  + reward) / ((double) (n.visits_count + 1));
            n.visits_count += 1;
        }, n.mutex);
    } catch (const std::exception& e) {
        solver.execution_policy().protect([&n, &e](){
            spdlog::error("SKDECIDE exception in MCTS when simulating the random default policy from state " + n.state.print() + ": " + e.what() +
                          Tsolver::ExecutionPolicy::print_thread());
        }, n.mutex);
        throw;
    }
}

// === GraphBackup implementation ===

template <typename Tsolver>
template <typename Texecution_policy>
struct GraphBackup<Tsolver>::UpdateFrontierImplementation<
            Texecution_policy,
            typename std::enable_if<std::is_same<Texecution_policy, SequentialExecution>::value>::type> {

    static void update_frontier(Tsolver& solver,
                                std::unordered_set<typename Tsolver::StateNode*>& new_frontier,
                                typename Tsolver::StateNode* f) {
        for (auto& a : f->parents) {
            double q_value = a->outcomes[f].first + (solver.discount() * (f->value));
            a->value = (((a->visits_count) * (a->value))  + q_value) / ((double) (a->visits_count + 1));
            a->visits_count += 1;
            typename Tsolver::StateNode* parent_node = a->parent;
            parent_node->value = (((parent_node->visits_count) * (parent_node->value))  + (a->value)) / ((double) (parent_node->visits_count + 1));
            parent_node->visits_count += 1;
            new_frontier.insert(parent_node);
            if (solver.debug_logs()) { spdlog::debug("Updating state " + parent_node->state.print() +
                                                    ": value=" + StringConverter::from(parent_node->value) +
                                                    ", visits=" + StringConverter::from(parent_node->visits_count) +
                                                    Tsolver::ExecutionPolicy::print_thread()); }
        }
    }
};

template <typename Tsolver>
template <typename Texecution_policy>
struct GraphBackup<Tsolver>::UpdateFrontierImplementation<
            Texecution_policy,
            typename std::enable_if<std::is_same<Texecution_policy, ParallelExecution>::value>::type> {

    static void update_frontier(Tsolver& solver,
                                std::unordered_set<typename Tsolver::StateNode*>& new_frontier,
                                typename Tsolver::StateNode* f) {
        std::list<typename Tsolver::ActionNode*> parents;
        solver.execution_policy().protect([&f, &parents](){
            std::copy(f->parents.begin(), f->parents.end(), std::inserter(parents, parents.end()));
        }, f->mutex);
        for (auto& a : parents) {
            solver.execution_policy().protect([&a, &solver, &f, &new_frontier](){
                double q_value = a->outcomes[f].first + (solver.discount() * (f->value));
                a->value = (((a->visits_count) * (a->value))  + q_value) / ((double) (a->visits_count + 1));
                a->visits_count += 1;
                typename Tsolver::StateNode* parent_node = a->parent;
                parent_node->value = (((parent_node->visits_count) * (parent_node->value))  + (a->value)) / ((double) (parent_node->visits_count + 1));
                parent_node->visits_count += 1;
                new_frontier.insert(parent_node);
                if (solver.debug_logs()) { spdlog::debug("Updating state " + parent_node->state.print() +
                                                        ": value=" + StringConverter::from(parent_node->value) +
                                                        ", visits=" + StringConverter::from(parent_node->visits_count) +
                                                        Tsolver::ExecutionPolicy::print_thread()); }
            }, a->parent->mutex);
        }
    }
};

#define SK_MCTS_GRAPH_BACKUP_TEMPLATE_DECL \
template <typename Tsolver>

#define SK_MCTS_GRAPH_BACKUP_CLASS \
GraphBackup<Tsolver>

SK_MCTS_GRAPH_BACKUP_TEMPLATE_DECL
void SK_MCTS_GRAPH_BACKUP_CLASS::operator()(Tsolver& solver,
                                            const std::size_t* thread_id,
                                            typename Tsolver::StateNode& n) const {
    if (solver.debug_logs()) {
        solver.execution_policy().protect([&n](){
            spdlog::debug("Back-propagating values from state " + n.state.print() +
                          Tsolver::ExecutionPolicy::print_thread());
        }, n.mutex);
    }

    std::size_t depth = 0; // used to prevent infinite loop in case of cycles
    std::unordered_set<typename Tsolver::StateNode*> frontier;
    frontier.insert(&n);

    while (!frontier.empty() && depth <= solver.max_depth()) {
        depth++;
        std::unordered_set<typename Tsolver::StateNode*> new_frontier;
        
        for (auto& f : frontier) {
            update_frontier(solver, new_frontier, f, &solver.execution_policy());
        }

        frontier = new_frontier;
    }
}

SK_MCTS_GRAPH_BACKUP_TEMPLATE_DECL
void SK_MCTS_GRAPH_BACKUP_CLASS::update_frontier(Tsolver& solver,
                                std::unordered_set<typename Tsolver::StateNode*>& new_frontier,
                                typename Tsolver::StateNode* f) {
    UpdateFrontierImplementation::update_frontier(solver, new_frontier, f);
}

// === MCTSSolver implementation ===

#define SK_MCTS_SOLVER_TEMPLATE_DECL \
template <typename Tdomain, \
          typename TexecutionPolicy, \
          template <typename Tsolver> class TtransitionMode, \
          template <typename Tsolver> class TtreePolicy, \
          template <typename Tsolver> class Texpander, \
          template <typename Tsolver> class TactionSelectorOptimization, \
          template <typename Tsolver> class TactionSelectorExecution, \
          template <typename Tsolver> class TrolloutPolicy, \
          template <typename Tsolver> class TbackPropagator>

#define SK_MCTS_SOLVER_CLASS \
MCTSSolver<Tdomain, \
           TexecutionPolicy, \
           TtransitionMode, \
           TtreePolicy, \
           Texpander, \
           TactionSelectorOptimization, \
           TactionSelectorExecution, \
           TrolloutPolicy, \
           TbackPropagator>

SK_MCTS_SOLVER_TEMPLATE_DECL
SK_MCTS_SOLVER_CLASS::ActionNode::ActionNode(const Action& a)
    : action(a), expansions_count(0), value(0.0),
      visits_count(0), parent(nullptr) {}
        
SK_MCTS_SOLVER_TEMPLATE_DECL
SK_MCTS_SOLVER_CLASS::ActionNode::ActionNode(const ActionNode& a)
    : action(a.action), outcomes(a.outcomes), dist_to_outcome(a.dist_to_outcome),
      dist(a.dist), expansions_count((std::size_t) a.expansions_count),
      value((double) a.value), visits_count((std::size_t) a.visits_count),
      parent(a.parent) {}

SK_MCTS_SOLVER_TEMPLATE_DECL
const typename SK_MCTS_SOLVER_CLASS::Action&
SK_MCTS_SOLVER_CLASS::ActionNode::Key::operator()(const ActionNode& an) const {
    return an.action;
}

SK_MCTS_SOLVER_TEMPLATE_DECL
SK_MCTS_SOLVER_CLASS::StateNode::StateNode(const State& s)
    : state(s), terminal(false), expanded(false),
      expansions_count(0), value(0.0), visits_count(0) {}
        
SK_MCTS_SOLVER_TEMPLATE_DECL
SK_MCTS_SOLVER_CLASS::StateNode::StateNode(const StateNode& s)
    : state(s.state), terminal((bool) s.terminal), expanded((bool) s.expanded),
      expansions_count((std::size_t) s.expansions_count), actions(s.actions),
      value((double) s.value), visits_count((std::size_t) s.visits_count),
      parents(s.parents) {}

SK_MCTS_SOLVER_TEMPLATE_DECL
const typename SK_MCTS_SOLVER_CLASS::State&
SK_MCTS_SOLVER_CLASS::StateNode::Key::operator()(const StateNode& sn) const {
    return sn.state;
}

SK_MCTS_SOLVER_TEMPLATE_DECL
SK_MCTS_SOLVER_CLASS::MCTSSolver(Domain& domain,
                                 std::size_t time_budget,
                                 std::size_t rollout_budget,
                                 std::size_t max_depth,
                                 std::size_t epsilon_moving_average_window,
                                 double epsilon,
                                 double discount,
                                 bool online_node_garbage,
                                 bool debug_logs,
                                 const WatchdogFunctor& watchdog,
                                 std::unique_ptr<TreePolicy> tree_policy,
                                 std::unique_ptr<Expander> expander,
                                 std::unique_ptr<ActionSelectorOptimization> action_selector_optimization,
                                 std::unique_ptr<ActionSelectorExecution> action_selector_execution,
                                 std::unique_ptr<RolloutPolicy> rollout_policy,
                                 std::unique_ptr<BackPropagator> back_propagator)
    : _domain(domain),
      _time_budget(time_budget),
      _rollout_budget(rollout_budget),
      _max_depth(max_depth),
      _epsilon_moving_average_window(epsilon_moving_average_window),
      _epsilon(epsilon),
      _discount(discount),
      _nb_rollouts(0),
      _online_node_garbage(online_node_garbage),
      _debug_logs(debug_logs),
      _watchdog(watchdog),
      _tree_policy(std::move(tree_policy)),
      _expander(std::move(expander)),
      _action_selector_optimization(std::move(action_selector_optimization)),
      _action_selector_execution(std::move(action_selector_execution)),
      _rollout_policy(std::move(rollout_policy)),
      _back_propagator(std::move(back_propagator)),
      _current_state(nullptr),
      _epsilon_moving_average(0) {
    if (debug_logs && (spdlog::get_level() > spdlog::level::debug)) {
        std::string msg = "Debug logs requested for algorithm MCTS but global log level is higher than debug";
        if (spdlog::get_level() <= spdlog::level::warn) {
            spdlog::warn(msg);
        } else {
            msg = "\033[1;33mbold " + msg + "\033[0m";
            std::cerr << msg << std::endl;
        }
    }

    std::random_device rd;
    _gen = std::make_unique<std::mt19937>(rd());
}

SK_MCTS_SOLVER_TEMPLATE_DECL
void SK_MCTS_SOLVER_CLASS::clear() {
    _graph.clear();
}

SK_MCTS_SOLVER_TEMPLATE_DECL
void SK_MCTS_SOLVER_CLASS::solve(const State& s) {
    try {
        spdlog::info("Running " + ExecutionPolicy::print_type() + " MCTS solver from state " + s.print());
        auto start_time = std::chrono::high_resolution_clock::now();
        _nb_rollouts = 0;
        _epsilon_moving_average = 0.0;
        _epsilons.clear();

        // Get the root node
        auto si = _graph.emplace(s);
        StateNode& root_node = const_cast<StateNode&>(*(si.first)); // we won't change the real key (StateNode::state) so we are safe

        boost::integer_range<std::size_t> parallel_rollouts(0, _domain.get_parallel_capacity());

        std::for_each(ExecutionPolicy::policy, parallel_rollouts.begin(), parallel_rollouts.end(), [this, &start_time, &root_node] (const std::size_t& thread_id) {
            std::size_t etime = 0;

            do {
                std::size_t depth = 0;
                double root_node_record_value = root_node.value;
                StateNode* sn = (*_tree_policy)(*this, &thread_id, *_expander, *_action_selector_optimization, root_node, depth);
                (*_rollout_policy)(*this, &thread_id, *sn, depth);
                (*_back_propagator)(*this, &thread_id, *sn);
                update_epsilon_moving_average(root_node, root_node_record_value);
                _nb_rollouts++;
            } while (_watchdog(etime = elapsed_time(start_time), _nb_rollouts,
                                root_node.value,
                                (_epsilons.size() >= _epsilon_moving_average_window) ?
                                    (double) _epsilon_moving_average :
                                    std::numeric_limits<double>::infinity()) &&
                        (etime < _time_budget) &&
                        (_nb_rollouts < _rollout_budget) &&
                        (_epsilon_moving_average > _epsilon));
        });

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
        spdlog::info("MCTS finished to solve from state " + s.print() +
                     " in " + StringConverter::from((double) duration / (double) 1e9) + " seconds with " +
                     StringConverter::from(_nb_rollouts) + " rollouts.");
    } catch (const std::exception& e) {
        spdlog::error("MCTS failed solving from state " + s.print() + ". Reason: " + e.what());
        throw;
    }
}

SK_MCTS_SOLVER_TEMPLATE_DECL
bool SK_MCTS_SOLVER_CLASS::is_solution_defined_for(const State& s) {
    auto si = _graph.find(s);
    if (si == _graph.end()) {
        return false;
    } else {
        return (*_action_selector_execution)(*this, nullptr, *si) != nullptr;
    }
}

SK_MCTS_SOLVER_TEMPLATE_DECL
typename SK_MCTS_SOLVER_CLASS::Action
SK_MCTS_SOLVER_CLASS::get_best_action(const State& s) {
    auto si = _graph.find(s);
    ActionNode* action = nullptr;
    if (si != _graph.end()) {
        action = (*_action_selector_execution)(*this, nullptr, *si);
    }
    if (action == nullptr) {
        spdlog::error("SKDECIDE exception: no best action found in state " + s.print());
        throw std::runtime_error("SKDECIDE exception: no best action found in state " + s.print());
    } else {
        if (_debug_logs) {
            std::string str = "(";
            for (const auto& o : action->outcomes) {
                str += "\n    " + o.first->state.print();
            }
            str += "\n)";
            spdlog::debug("Best action's known outcomes:\n" + str);
        }
        if (_online_node_garbage && _current_state) {
            std::unordered_set<StateNode*> root_subgraph, child_subgraph;
            compute_reachable_subgraph(_current_state, root_subgraph);
            compute_reachable_subgraph(const_cast<StateNode*>(&(*si)), child_subgraph); // we won't change the real key (StateNode::state) so we are safe
            remove_subgraph(root_subgraph, child_subgraph);
        }
        _current_state = const_cast<StateNode*>(&(*si)); // we won't change the real key (StateNode::state) so we are safe
        _action_prefix.push_back(action->action);
        return action->action;
    }
}

SK_MCTS_SOLVER_TEMPLATE_DECL
double SK_MCTS_SOLVER_CLASS::get_best_value(const State& s) {
    auto si = _graph.find(StateNode(s));
    ActionNode* action = nullptr;
    if (si != _graph.end()) {
        action = (*_action_selector_execution)(*this, nullptr, *si);
    }
    if (action == nullptr) {
        spdlog::error("SKDECIDE exception: no best action found in state " + s.print());
        throw std::runtime_error("SKDECIDE exception: no best action found in state " + s.print());
    } else {
        return action->value;
    }
}

SK_MCTS_SOLVER_TEMPLATE_DECL
std::size_t SK_MCTS_SOLVER_CLASS::nb_of_explored_states() const {
    return _graph.size();
}

SK_MCTS_SOLVER_TEMPLATE_DECL
std::size_t SK_MCTS_SOLVER_CLASS::nb_rollouts() const {
    return _nb_rollouts;
}

SK_MCTS_SOLVER_TEMPLATE_DECL
typename MapTypeDeducer<typename SK_MCTS_SOLVER_CLASS::State, std::pair<typename SK_MCTS_SOLVER_CLASS::Action, double>>::Map
SK_MCTS_SOLVER_CLASS::policy() {
    typename MapTypeDeducer<State, std::pair<Action, double>>::Map p;
    for (auto& n : _graph) {
        ActionNode* action = (*_action_selector_execution)(*this, nullptr, n);
        if (action != nullptr) {
            p.insert(std::make_pair(n.state, std::make_pair(action->action, (double) action->value)));
        }
    }
    return p;
}

SK_MCTS_SOLVER_TEMPLATE_DECL
typename SK_MCTS_SOLVER_CLASS::Domain& SK_MCTS_SOLVER_CLASS::domain() { return _domain; }

SK_MCTS_SOLVER_TEMPLATE_DECL
std::size_t SK_MCTS_SOLVER_CLASS::time_budget() const { return _time_budget; }

SK_MCTS_SOLVER_TEMPLATE_DECL
std::size_t SK_MCTS_SOLVER_CLASS::rollout_budget() const { return _rollout_budget; }

SK_MCTS_SOLVER_TEMPLATE_DECL
std::size_t SK_MCTS_SOLVER_CLASS::max_depth() const { return _max_depth; }

SK_MCTS_SOLVER_TEMPLATE_DECL
double SK_MCTS_SOLVER_CLASS::discount() const { return _discount; }

SK_MCTS_SOLVER_TEMPLATE_DECL
typename SK_MCTS_SOLVER_CLASS::ExecutionPolicy&
SK_MCTS_SOLVER_CLASS::execution_policy() { return _execution_policy; }

SK_MCTS_SOLVER_TEMPLATE_DECL
typename SK_MCTS_SOLVER_CLASS::TransitionMode&
SK_MCTS_SOLVER_CLASS::transition_mode() { return _transition_mode; }

SK_MCTS_SOLVER_TEMPLATE_DECL
const typename SK_MCTS_SOLVER_CLASS::TreePolicy&
SK_MCTS_SOLVER_CLASS::tree_policy() { return *_tree_policy; }

SK_MCTS_SOLVER_TEMPLATE_DECL
const typename SK_MCTS_SOLVER_CLASS::Expander&
SK_MCTS_SOLVER_CLASS::expander() { return *_expander; }

SK_MCTS_SOLVER_TEMPLATE_DECL
const typename SK_MCTS_SOLVER_CLASS::ActionSelectorOptimization&
SK_MCTS_SOLVER_CLASS::action_selector_optimization() { return *_action_selector_optimization; }

SK_MCTS_SOLVER_TEMPLATE_DECL
const typename SK_MCTS_SOLVER_CLASS::ActionSelectorExecution&
SK_MCTS_SOLVER_CLASS::action_selector_execution() { return *_action_selector_execution; }

SK_MCTS_SOLVER_TEMPLATE_DECL
const typename SK_MCTS_SOLVER_CLASS::RolloutPolicy&
SK_MCTS_SOLVER_CLASS::rollout_policy() { return *_rollout_policy; }

SK_MCTS_SOLVER_TEMPLATE_DECL
const typename SK_MCTS_SOLVER_CLASS::BackPropagator&
SK_MCTS_SOLVER_CLASS::back_propagator() { return *_back_propagator; }

SK_MCTS_SOLVER_TEMPLATE_DECL
typename SK_MCTS_SOLVER_CLASS::Graph&
SK_MCTS_SOLVER_CLASS::graph() { return _graph; }

SK_MCTS_SOLVER_TEMPLATE_DECL
const std::list<typename SK_MCTS_SOLVER_CLASS::Action>&
SK_MCTS_SOLVER_CLASS::action_prefix() const { return _action_prefix; }

SK_MCTS_SOLVER_TEMPLATE_DECL
std::mt19937& SK_MCTS_SOLVER_CLASS::gen() { return *_gen; }

SK_MCTS_SOLVER_TEMPLATE_DECL
typename SK_MCTS_SOLVER_CLASS::ExecutionPolicy::Mutex&
SK_MCTS_SOLVER_CLASS::gen_mutex() { return _gen_mutex; }

SK_MCTS_SOLVER_TEMPLATE_DECL
bool SK_MCTS_SOLVER_CLASS::debug_logs() const { return _debug_logs; }

SK_MCTS_SOLVER_TEMPLATE_DECL
void SK_MCTS_SOLVER_CLASS::compute_reachable_subgraph(StateNode* node, std::unordered_set<StateNode*>& subgraph) {
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

SK_MCTS_SOLVER_TEMPLATE_DECL
void SK_MCTS_SOLVER_CLASS::remove_subgraph(std::unordered_set<StateNode*>& root_subgraph, std::unordered_set<StateNode*>& child_subgraph) {
    std::unordered_set<StateNode*> removed_subgraph;
    // First pass: look for nodes in root_subgraph but not child_subgraph and remove
    // those nodes from their children's parents
    // Don't actually remove those nodes in the first pass otherwise some children to remove
    // won't exist anymore when looking for their parents
    for (auto& n : root_subgraph) {
        if (child_subgraph.find(n) == child_subgraph.end()) {
            for (auto& action : n->actions) {
                for (auto& outcome : action.outcomes) {
                    // we won't change the real key (ActionNode::action) so we are safe
                    outcome.first->parents.erase(&const_cast<ActionNode&>(action));
                }
            }
            removed_subgraph.insert(n);
        }
    }
    // Second pass: actually remove nodes in root_subgraph but not in child_subgraph
    for (auto& n : removed_subgraph) {
        _graph.erase(StateNode(n->state));
    }
}

SK_MCTS_SOLVER_TEMPLATE_DECL
void SK_MCTS_SOLVER_CLASS::update_epsilon_moving_average(const StateNode& node, const double& node_record_value) {
    if (_epsilon_moving_average_window > 0) {
        double current_epsilon = std::fabs(node_record_value - node.value);
        _execution_policy.protect([this, &current_epsilon](){
            if (_epsilons.size() < _epsilon_moving_average_window) {
                _epsilon_moving_average = ((double) _epsilon_moving_average) +
                                            (current_epsilon / ((double) _epsilon_moving_average_window));
            } else {
                _epsilon_moving_average = ((double) _epsilon_moving_average) +
                                            ((current_epsilon - _epsilons.front()) / ((double) _epsilon_moving_average_window));
                _epsilons.pop_front();
            }
            _epsilons.push_back(current_epsilon);
        }, _epsilons_protect);
    }
}

SK_MCTS_SOLVER_TEMPLATE_DECL
std::size_t SK_MCTS_SOLVER_CLASS::elapsed_time(const std::chrono::time_point<std::chrono::high_resolution_clock>& start_time) {
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

} // namespace skdecide

#endif // SKDECIDE_MCTS_IMPL_HH
