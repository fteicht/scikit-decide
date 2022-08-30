/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_INITIALIZATION_HH
#define SKDECIDE_INITIALIZATION_HH

#include "observability.hh"
#include "memory.hh"
#include <memory>

namespace skdecide {

/**
 * @brief A domain must inherit this class if it can be initialized.
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename DerivedCompoundDomain> class InitializableDomain {
  static_assert(
      std::is_base_of<
          PartiallyObservableDomain<
              DerivedCompoundDomain, typename DerivedCompoundDomain::RawState,
              typename DerivedCompoundDomain::RawObservation,
              DerivedCompoundDomain::template RawStateSpace,
              DerivedCompoundDomain::template RawObservationSpace,
              DerivedCompoundDomain::template RawObservationDistribution>,
          DerivedCompoundDomain>::value,
      "DerivedCompoundDomain must be derived from "
      "skdecide::PartiallyObservableDomain<DerivedCompoundDomain>");
  static_assert(std::is_base_of<HistoryDomain<DerivedCompoundDomain>,
                                DerivedCompoundDomain>::value,
                "DerivedCompoundDomain must be derived from "
                "skdecide::HistoryDomain<DerivedCompoundDomain>");

public:
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawObservation>
      CompoundObservation;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawState>
      CompoundState;

  /**
   * @brief Reset the state of the environment and return an initial
   * observation.
   *
   *    By default, InitializableDomain::reset() provides some boilerplate code
   * and internally calls InitializableDomain::make_reset() (which returns an
   * initial state). The boilerplate code automatically stores the initial state
   * into the _memory attribute and samples a corresponding observation.
   *
   * @return An initial observation.
   */
  CompoundObservation reset() {
    CompoundState initial_state = make_reset();
    CompoundObservation initial_observation =
        this->get_observation_distribution(initial_state, nullptr)->sample();
    auto _memory = this->_init_memory({initial_state});
    return initial_observation;
  }

protected:
  /**
   * @brief Reset the state of the environment and return an initial state.
   *
   *    This is a helper function called by default from
   * InitializableDomain::reset(). It focuses on the state level, as opposed to
   * the observation one for the latter.
   *
   * @return An initial state.
   */
  virtual CompoundState make_reset() = 0;
};

/**
 * @brief A domain must inherit this class if its states are initialized
 according to a probability distribution known as white-box.
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam InitialStateDistribution The type of an agent's initial state
 distribution
 */
template <typename DerivedCompoundDomain, template <typename...>
                                          class InitialStateDistribution =
                                              ImplicitDistribution>
class UncertainInitializedDomain
    : public InitializableDomain<DerivedCompoundDomain> {
public:
  template <typename... T>
  using RawInitialStateDistribution = InitialStateDistribution<T...>;

  typedef typename InitializableDomain<DerivedCompoundDomain>::CompoundState
      CompoundState;
  typedef RawInitialStateDistribution<std::shared_ptr<CompoundState>>
      CompoundInitialStateDistribution;
  typedef std::unique_ptr<CompoundInitialStateDistribution>
      CompoundInitialStateDistributionPtr;

  /**
   * @brief Get the (cached) probability distribution of initial states.
   *
   *    By default, UncertainInitializedDomain::get_initial_state_distribution()
   * internally calls
   * UncertainInitializedDomain::make_initial_state_distribution() the first
   * time and automatically caches its value to make future calls more efficient
   * (since the initial state distribution is assumed to be constant).
   *
   * @return The probability distribution of initial states.
   */
  CompoundInitialStateDistribution &get_initial_state_distribution() {
    if (!_initial_state_distribution) {
      _initial_state_distribution = make_initial_state_distribution();
    }
    return *_initial_state_distribution;
  }

protected:
  /**
   * @brief Get the probability distribution of initial states.
   *
   *    This is a helper function called by default from
   * UncertainInitializedDomain::get_initial_state_distribution(), the
   * difference being that the result is not cached here.
   *
   * @return The probability distribution of initial states.
   */
  virtual CompoundInitialStateDistributionPtr
  make_initial_state_distribution() = 0;

  /**
   * @brief Reset the state of the environment and return an initial state.
   *
   *    This is a helper function called by default from
   * InitializableDomain::reset(). It focuses on the state level, as opposed to
   * the observation one for the latter.
   *
   * @return An initial state.
   */
  virtual CompoundState make_reset() {
    return get_initial_state_distribution().sample();
  }

private:
  CompoundInitialStateDistributionPtr _initial_state_distribution;
};

/**
 * @brief A domain must inherit this class if it has a deterministic initial
 * state known as white-box.
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename DerivedCompoundDomain>
class DeterministicInitializedDomain
    : public UncertainInitializedDomain<DerivedCompoundDomain,
                                        SingleValueDistribution> {
public:
  typedef typename UncertainInitializedDomain<
      DerivedCompoundDomain, SingleValueDistribution>::CompoundState
      CompoundState;
  typedef std::unique_ptr<CompoundState> CompoundStatePtr;
  typedef typename UncertainInitializedDomain<DerivedCompoundDomain,
                                              SingleValueDistribution>::
      CompoundInitialStateDistribution CompoundInitialStateDistribution;
  typedef typename UncertainInitializedDomain<DerivedCompoundDomain,
                                              SingleValueDistribution>::
      CompoundInitialStateDistributionPtr CompoundInitialStateDistributionPtr;

  /**
   * @brief Get the (cached) initial state.
   *
   *    By default, DeterministicInitializedDomain::get_initial_state()
   * internally calls DeterministicInitializedDomain::make_initial_state() the
   * first time and automatically caches its value to make future calls more
   * efficient (since the initial state is assumed to be constant).
   *
   * @return The initial state.
   */
  const CompoundState &get_initial_state() {
    if (!_initial_state) {
      _initial_state = make_initial_state();
    }
    return *_initial_state;
  }

protected:
  /**
   * @brief Get the initial state.
   *
   *    This is a helper function called by default from
   * DeterministicInitializedDomain::get_initial_state(), the difference being
   * that the result is not cached here.
   *
   * @return The initial state.
   */
  virtual CompoundStatePtr make_initial_state() = 0;

private:
  CompoundStatePtr _initial_state;

  /**
   * @brief Get the probability distribution of initial states.
   *
   *    This is a helper function called by default from
   * UncertainInitializedDomain::get_initial_state_distribution(), the
   * difference being that the result is not cached here.
   *
   * @return The probability distribution of initial states.
   */
  virtual CompoundInitialStateDistributionPtr
  make_initial_state_distribution() {
    return std::make_unique<
        SingleValueDistribution<std::shared_ptr<CompoundState>>>(
        get_initial_state());
  }
};

} // namespace skdecide

#endif // SKDECIDE_INITIALIZATION_HH
