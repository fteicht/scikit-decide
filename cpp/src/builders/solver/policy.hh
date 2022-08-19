/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_POLICY_HH
#define SKDECIDE_POLICY_HH

#include "core.hh"

namespace skdecide {

/**
 * @brief A solver must inherit this class if it computes a stochastic policy as
 * part of the solving process.
 *
 * @tparam DerivedCompoundSolver The type of the solver made up of different
 * features and deriving from this particular solver feature.
 * @tparam CompoundDomain The type of the domain made up of different
 * features.
 */
template <typename DerivedCompoundSolver, typename CompoundDomain>
class PolicySolver {
public:
  typedef typename CompoundDomain::template AgentProxy<
      typename CompoundDomain::RawObservation>
      CompoundObservation;
  typedef typename CompoundDomain::template AgentProxy<
      typename CompoundDomain::template ConcurrencyProxy<
          typename CompoundDomain::RawEvent>>
      CompoundEvent;
  typedef typename CompoundDomain::template SmartPointer<CompoundEvent>
      CompoundEventPtr;

  /**
   * @brief Sample an action for the given observation (from the solver's
   * current policy).
   *
   * @param observation The observation for which an action must be sampled.
   * @return The sampled action.
   */
  virtual CompoundEventPtr
  sample_action(const CompoundObservation &observation) = 0;

  /**
   * @brief Check whether the solver's current policy is defined for the given
   * observation.
   *
   * @param observation The observation to consider.
   * @return True if the policy is defined for the given observation memory
   * (False otherwise).
   */
  virtual bool
  is_policy_defined_for(const CompoundObservation &observation) = 0;
};

/**
 * @brief A solver must inherit this class if it computes a stochastic policy
 (providing next action distribution explicitly) as part of the solving process.
 *
 * @tparam DerivedCompoundSolver The type of the solver made up of different
 * features and deriving from this particular solver feature.
 * @tparam CompoundDomain The type of the domain made up of different
 * features.
 * @tparam EventDistribution Type of the distribution on the policy's sampled
 events
 */
template <typename DerivedCompoundSolver, typename CompoundDomain,
          template <typename...> class EventDistribution = ImplicitDistribution>
class UncertainPolicySolver
    : public PolicySolver<DerivedCompoundSolver, CompoundDomain> {
public:
  template <typename... T> using RawEventDistribution = EventDistribution<T...>;

  typedef typename PolicySolver<DerivedCompoundSolver,
                                CompoundDomain>::CompoundObservation
      CompoundObservation;
  typedef typename PolicySolver<DerivedCompoundSolver,
                                CompoundDomain>::CompoundEvent CompoundEvent;
  typedef
      typename PolicySolver<DerivedCompoundSolver,
                            CompoundDomain>::CompoundEventPtr CompoundEventPtr;
  typedef RawEventDistribution<CompoundEvent> CompoundEventDistribution;
  typedef
      typename CompoundDomain::template SmartPointer<CompoundEventDistribution>
          CompoundEventDistributionPtr;

  virtual CompoundEventPtr
  sample_action(const CompoundObservation &observation) {
    return CompoundEventPtr(
        new CompoundEvent(get_next_action_distribution(observation).sample()));
  }

  /**
   * @brief Get the probabilistic distribution of next action for the given
   * observation (from the solver's current policy).
   *
   * @param observation The observation to consider.
   * @return The probabilistic distribution of next action.
   */
  virtual CompoundEventDistributionPtr
  get_next_action_distribution(const CompoundObservation &observation) = 0;
};

/**
 * @brief A solver must inherit this class if it computes a deterministic policy
 * as part of the solving process.
 *
 * @tparam DerivedCompoundSolver The type of the solver made up of different
 * features and deriving from this particular solver feature.
 * @tparam CompoundDomain The type of the domain made up of different
 * features.
 */
template <typename DerivedCompoundSolver, typename CompoundDomain>
class DeterministicPolicySolver
    : public UncertainPolicySolver<DerivedCompoundSolver, CompoundDomain,
                                   SingleValueDistribution> {
public:
  typedef typename UncertainPolicySolver<
      DerivedCompoundSolver, CompoundDomain,
      SingleValueDistribution>::CompoundObservation CompoundObservation;
  typedef typename UncertainPolicySolver<DerivedCompoundSolver, CompoundDomain,
                                         SingleValueDistribution>::CompoundEvent
      CompoundEvent;
  typedef
      typename UncertainPolicySolver<DerivedCompoundSolver, CompoundDomain,
                                     SingleValueDistribution>::CompoundEventPtr
          CompoundEventPtr;
  typedef typename UncertainPolicySolver<DerivedCompoundSolver, CompoundDomain,
                                         SingleValueDistribution>::
      CompoundEventDistributionPtr CompoundEventDistributionPtr;

  /**
   * @brief Get the probabilistic distribution of next action for the given
   * observation (from the solver's current policy).
   *
   * @param observation The observation to consider.
   * @return The probabilistic distribution of next action.
   */
  virtual CompoundEventDistributionPtr
  get_next_action_distribution(const CompoundObservation &observation) {
    return CompoundEventDistributionPtr(
        new SingleValueDistribution<CompoundEvent>(
            get_next_action(observation)));
  }

  /**
   * @brief Get the next deterministic action (from the solver's current
   * policy).
   *
   * @param observation The observation for which next action is requested.
   * @return The next deterministic action.
   */
  virtual CompoundEventPtr
  get_next_action(const CompoundObservation &observation) = 0;
};

} // namespace skdecide

#endif // SKDECIDE_POLICY_HH
