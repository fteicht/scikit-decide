/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_OBSERVABILITY_HH
#define SKDECIDE_OBSERVABILITY_HH

#include <memory>
#include <type_traits>
#include "core.hh"

namespace skdecide {

/**
 * @brief A domain must inherit this class if it is partially observable.
 * "Partially observable" means that the observation provided to the agent is
 * computed from (but generally not equal to) the internal state of the domain.
 * Additionally, according to literature, a partially observable domain must
 * provide the probability distribution of the observation given a state and
 * action.
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam State Type of an agent's states
 * @tparam Observation Type of an agent's observations
 * @tparam StateSpace Type of an agent's state space
 * @tparam ObservationSpace Type of an agent's observation space
 * @tparam ObservationDistribution Type of observation distributions
 */
template <typename DerivedCompoundDomain, typename State, typename Observation,
          template <typename...> class StateSpace = ImplicitSpace,
          template <typename...> class ObservationSpace = ImplicitSpace,
          template <typename...> class ObservationDistribution =
              ImplicitDistribution>
class PartiallyObservableDomain {
  static_assert(std::is_base_of<Space<State>, StateSpace<State>>::value,
                "StateSpace must be derived from skdecide::Space<...>");
  static_assert(
      std::is_base_of<Space<Observation>, ObservationSpace<Observation>>::value,
      "ObservationSpace type must be derived from "
      "skdecide::Space<...>");
  static_assert(std::is_base_of<Distribution<Observation>,
                                ObservationDistribution<Observation>>::value,
                "ObservationDistribution must be derived from "
                "skdecide::Distribution<...>");

public:
  typedef State RawState;
  typedef Observation RawObservation;
  template <typename... T> using RawStateSpace = StateSpace<T...>;
  template <typename... T> using RawObservationSpace = ObservationSpace<T...>;
  template <typename... T>
  using RawObservationDistribution = ObservationDistribution<T...>;

  typedef typename DerivedCompoundDomain::template AgentProxy<RawObservation>
      CompoundObservation;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawObservationSpace<RawObservation>>
      CompoundObservationSpace;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      CompoundObservationSpace>
      CompoundObservationSpacePtr;
  typedef RawObservationDistribution<CompoundObservation>
      CompoundObservationDistribution;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      CompoundObservationDistribution>
      CompoundObservationDistributionPtr;
  typedef typename DerivedCompoundDomain::template AgentProxy<RawState>
      CompoundState;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawStateSpace<RawState>>
      CompoundStateSpace;
  typedef
      typename DerivedCompoundDomain::template SmartPointer<CompoundStateSpace>
          CompoundStateSpacePtr;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::template ConcurrencyProxy<
          typename DerivedCompoundDomain::RawEvent>>
      CompoundEvent;
  typedef typename DerivedCompoundDomain::template SmartPointer<CompoundEvent>
      CompoundEventPtr;

  /**
   * @brief Get the (cached) observation space (finite or infinite set).
   *
   * By default, PartiallyObservable::get_observation_space() internally calls
   * PartiallyObservable::make_observation_space() the first time and
   * automatically caches its value to make future calls more efficient (since
   * the observation space is assumed to be constant).
   *
   * @return const ObservationSpace& The observation space.
   */
  const CompoundObservationSpace &get_observation_space() {
    if (!_observation_space) {
      _observation_space = make_observation_space();
    }
    return *_observation_space;
  }

  /**
   * @brief Get the (cached) state space (finite or infinite set).
   *
   * By default, PartiallyObservable::get_state_space() internally calls
   * PartiallyObservable::make_state_space() the first time and
   * automatically caches its value to make future calls more efficient (since
   * the state space is assumed to be constant).
   *
   * @return const StateSpace& The state space.
   */
  const CompoundStateSpace &get_state_space() {
    if (!_state_space) {
      _state_space = make_state_space();
    }
    return *_state_space;
  }

  /**
   * @brief Check that an observation indeed belongs to the domain observation
   * space.
   * !!! tip.
   * By default, this function is implemented using the
   * skdecide::Space::contains() function on the domain observation space
   * provided by PartiallyObservable::get_observation_space(), but it can be
   * overridden for faster implementations.
   * @param observation The observation to consider.
   * @return true The observation belongs to the domain observation space.
   * @return false The observation does not belong to the domain observation
   * space.
   */
  inline virtual bool is_observation(const CompoundObservation &observation) {
    return get_observation_space().contains(observation);
  }

  /**
   * @brief Check that a state indeed belongs to the domain state space.
   * !!! tip.
   * By default, this function is implemented using the
   * skdecide::Space::contains() function on the domain state space
   * provided by PartiallyObservable::get_state_space(), but it can be
   * overridden for faster implementations.
   * @param observation The state to consider.
   * @return true The state belongs to the domain state space.
   * @return false The state does not belong to the domain state space.
   */
  inline virtual bool is_state(const CompoundState &state) {
    return get_state_space().contains(state);
  }

  /**
   * @brief Get the probability distribution of the observation given a state
   * and action. In mathematical terms (discrete case), given an action $a$,
   * this function represents: $P(O|s, a)$, where $O$ is the random variable of
   * the observation.
   * @param state The state to be observed.
   * @param event The last applied action (or None if the state is an initial
   * state).
   * @return ObservationDistributionPtr The probability distribution of the
   * observation.
   */
  virtual CompoundObservationDistributionPtr get_observation_distribution(
      const CompoundState &state,
      const CompoundEventPtr &event = CompoundEventPtr()) = 0;

protected:
  /**
   * @brief Get the observation space (finite or infinite set).
   * @return const ObservationSpace& The observation space.
   */
  virtual CompoundObservationSpacePtr make_observation_space() = 0;

  /**
   * @brief Get the state space (finite or infinite set).
   * @return const StateSpace& The state space.
   */
  virtual CompoundStateSpacePtr make_state_space() = 0;

private:
  CompoundObservationSpacePtr _observation_space;
  CompoundStateSpacePtr _state_space;
};

/**
 * @brief A domain must inherit this class if it is fully observable.
 * "Fully observable" means that the observation provided to the agent is equal
 * to the internal state of the domain.
 * !!! warning.
 * In the case of fully observable domains, make sure that the observation
 * type D.T_observation is equal to the state type D.T_state.
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam State Type of an agent's states
 * @tparam StateSpace Type of an agent's state space
 */
template <typename DerivedCompoundDomain, typename State,
          template <typename...> class StateSpace = ImplicitSpace>
class FullyObservableDomain
    : public virtual PartiallyObservableDomain<DerivedCompoundDomain, State,
                                               State, StateSpace, StateSpace,
                                               SingleValueDistribution> {
public:
  typedef State RawState;
  template <typename... T> using RawStateSpace = StateSpace<T...>;

  typedef typename DerivedCompoundDomain::template AgentProxy<RawState>
      CompoundState;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawStateSpace<RawState>>
      CompoundStateSpace;
  typedef
      typename DerivedCompoundDomain::template SmartPointer<CompoundStateSpace>
          CompoundStateSpacePtr;
  typedef SingleValueDistribution<CompoundState>
      CompoundObservationDistribution;

  typedef typename DerivedCompoundDomain::template SmartPointer<
      CompoundObservationDistribution>
      CompoundObservationDistributionPtr;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::template ConcurrencyProxy<
          typename DerivedCompoundDomain::RawEvent>>
      CompoundEvent;
  typedef typename DerivedCompoundDomain::template SmartPointer<CompoundEvent>
      CompoundEventPtr;

  /**
   * @brief Get the probability distribution of the observation given a state
   * and action. In mathematical terms (discrete case), given an action $a$,
   * this function represents: $P(O|s, a)$, where $O$ is the random variable of
   * the observation.
   * @param state The state to be observed.
   * @param event The last applied action (or None if the state is an initial
   * state).
   * @return ObservationDistributionPtr The probability distribution of the
   * observation.
   */
  inline virtual CompoundObservationDistributionPtr
  get_observation_distribution(
      const CompoundState &state,
      const CompoundEventPtr &event = CompoundEventPtr()) {
    return CompoundObservationDistributionPtr(
        new CompoundObservationDistribution(state));
  }

protected:
  /**
   * @brief Get the observation space (finite or infinite set).
   * @return const ObservationSpace& The observation space.
   */
  inline virtual CompoundStateSpacePtr make_observation_space() {
    return this->make_state_space();
  }
};

} // namespace skdecide

#endif // SKDECIDE_OBSERVABILITY_HH
