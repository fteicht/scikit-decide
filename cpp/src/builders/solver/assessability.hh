/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_ASSESSABILITY_HH
#define SKDECIDE_ASSESSABILITY_HH

#include "core.hh"

namespace skdecide {

/**
 * @brief A solver must inherit this class if it can provide the utility
 * function (i.e. value function).
 *
 * @tparam DerivedCompoundSolver The type of the solver made up of different
 * features and deriving from this particular solver feature.
 * @tparam CompoundDomain The type of the domain made up of different
 * features.
 */
template <typename DerivedCompoundSolver, typename CompoundDomain>
class UtilitySolver {
public:
  typedef typename CompoundDomain::template AgentProxy<
      typename CompoundDomain::RawObservation>
      CompoundObservation;
  typedef typename CompoundDomain::template AgentProxy<
      typename CompoundDomain::RawValue>
      CompoundValue;
  typedef Value<CompoundDomain::TransitionValue, CompoundValue> ValueReturn;

  /**
   * @brief Get the estimated on-policy utility of the given observation.
   *
   *    In mathematical terms, for a fully observable domain, this function
   * estimates:
   *    $$V^\\pi(s)=\\underset{\\tau\\sim\\pi}{\\mathbb{E}}[R(\\tau)|s_0=s]$$
   *    where $\\pi$ is the current policy, any $\\tau=(s_0,a_0, s_1, a_1, ...)$
   * represents a trajectory sampled from the policy, $R(\\tau)$ is the return
   * (cumulative reward) and $s_0$ the initial state for the trajectories.
   *
   * @param observation The observation to consider.
   * @return The estimated on-policy utility of the given observation.
   */
  virtual ValueReturn get_utility(const CompoundObservation &observation) = 0;
};

/**
 * @brief A solver must inherit this class if it can provide the Q function
 * (i.e. action-value function).
 *
 * @tparam DerivedCompoundSolver The type of the solver made up of different
 * features and deriving from this particular solver feature.
 * @tparam CompoundDomain The type of the domain made up of different
 * features.
 */
template <typename DerivedCompoundSolver, typename CompoundDomain>
class QValueSolver
    : public UtilitySolver<DerivedCompoundSolver, CompoundDomain> {
public:
  typedef typename UtilitySolver<DerivedCompoundSolver,
                                 CompoundDomain>::CompoundObservation
      CompoundObservation;
  typedef
      typename UtilitySolver<DerivedCompoundSolver, CompoundDomain>::ValueReturn
          ValueReturn;
  typedef typename CompoundDomain::template AgentProxy<
      typename CompoundDomain::template ConcurrencyProxy<
          typename CompoundDomain::RawEvent>>
      CompoundEvent;

  /**
   * @brief et the estimated on-policy Q value of the given observation and
   * action.
   *
   *    In mathematical terms, for a fully observable domain, this function
   * estimates:
   *    $$Q^\\pi(s,a)=\\underset{\\tau\\sim\\pi}{\\mathbb{E}}[R(\\tau)|s_0=s,a_0=a]$$
   *    where $\\pi$ is the current policy, any $\\tau=(s_0,a_0, s_1, a_1, ...)$
   * represents a trajectory sampled from the policy, $R(\\tau)$ is the return
   * (cumulative reward) and $s_0$/$a_0$ the initial state/action for the
   *    trajectories.
   *
   * @param observation The observation to consider.
   * @param event The action to consider.
   * @return ValueReturn The estimated on-policy Q value of the given
   * observation and action.
   */
  virtual ValueReturn get_q_value(const CompoundObservation &observation,
                                  const CompoundEvent &event) = 0;
};

} // namespace skdecide

#endif // SKDECIDE_ASSESSABILITY_HH
