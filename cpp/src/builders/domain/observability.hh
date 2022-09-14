/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_OBSERVABILITY_HH
#define SKDECIDE_OBSERVABILITY_HH

#include <cstddef>
#include <memory>
#include <optional>
#include <type_traits>
#include "builders/domain/domain_type_importer.hh"
#include "core.hh"

namespace skdecide {

template <typename CompoundDomain, template <typename...> class CallingFeature,
          template <typename...> class DerivedFeature = CallingFeature>
class ObservabilityTypesImporter {
public:
  DOMAIN_SIMPLE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                              state, AgentState);

  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                state_space, AgentStateSpace);

  DOMAIN_SIMPLE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                              observation, AgentObservation);

  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                observation_space, AgentObservationSpace);

  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                observation_distribution,
                                AgentObservationDistribution);
};

/**
 * @brief A domain must inherit this class if it is partially observable.
 * "Partially observable" means that the observation provided to the agent is
 * computed from (but generally not equal to) the internal state of the domain.
 * Additionally, according to literature, a partially observable domain must
 * provide the probability distribution of the observation given a state and
 * action.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class PartiallyObservableDomain {
public:
  /**
   * @brief Type of an agent's states
   */
  typedef typename ObservabilityTypesImporter<CompoundDomain,
                                              PartiallyObservableDomain>::
      template import_state_type<>::result AgentState;

  static_assert(!std::is_same_v<AgentState, std::nullopt_t> &&
                    !std::is_same_v<AgentState, std::nullptr_t>,
                "The domain types must define AgentState.");

  /**
   * @brief Type of an agent's state space
   *
   * @tparam Args Type of states
   */
  template <typename... Args>
  using AgentStateSpace =
      typename ObservabilityTypesImporter<CompoundDomain,
                                          PartiallyObservableDomain>::
          template import_state_space_type<ImplicitSpace>::template test_with<
              AgentState>::template result<Args...>;

  /**
   * @brief Type of an agent's observations
   */
  typedef typename ObservabilityTypesImporter<
      CompoundDomain, PartiallyObservableDomain,
      CompoundDomain::Features::template ObservabilityDomain>::
      template import_observation_type<>::result AgentObservation;

  static_assert(!std::is_same_v<AgentObservation, std::nullopt_t> &&
                    !std::is_same_v<AgentObservation, std::nullptr_t>,
                "The domain types must define AgentObservation when the "
                "observability domain feature is "
                "skdecide::PartiallyObservableDomain");

  /**
   * @brief Type of an agent's observation space
   */
  template <typename... Args>
  using AgentObservationSpace = typename ObservabilityTypesImporter<
      CompoundDomain, PartiallyObservableDomain,
      CompoundDomain::Features::template ObservabilityDomain>::
      template import_observation_space_type<ImplicitSpace>::template test_with<
          AgentObservation>::template result<Args...>;

  /**
   * @brief Type of an agent's observation distribution
   */
  template <typename... Args>
  using AgentObservationDistribution = typename ObservabilityTypesImporter<
      CompoundDomain, PartiallyObservableDomain,
      CompoundDomain::Features::template ObservabilityDomain>::
      template import_observation_distribution_type<ImplicitDistribution>::
          template test_with<AgentObservation>::template result<Args...>;

private:
  static_assert(
      std::is_base_of<Space<AgentState>, AgentStateSpace<AgentState>>::value,
      "AgentStateSpace<...> must be derived from skdecide::Space<...>");
  static_assert(std::is_base_of<Space<AgentObservation>,
                                AgentObservationSpace<AgentObservation>>::value,
                "AgentObservationSpace<...> type must be derived from "
                "skdecide::Space<...>");
  static_assert(
      std::is_base_of<Distribution<AgentObservation>,
                      AgentObservationDistribution<AgentObservation>>::value,
      "AgentObservationDistribution<...> must be derived from "
      "skdecide::Distribution<...>");

public:
  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature {
  public:
    typedef PartiallyObservableDomain<CompoundDomain> FeatureDomain;

    typedef typename CompoundDomain::Features::template AgentDomain<
        CompoundDomain>::template AgentProxy<AgentState>
        CompoundState;
    typedef typename CompoundDomain::Features::template AgentDomain<
        CompoundDomain>::template AgentProxy<AgentStateSpace<AgentState>>
        CompoundStateSpace;
    typedef std::unique_ptr<CompoundStateSpace> CompoundStateSpacePtr;
    typedef typename CompoundDomain::Features::template AgentDomain<
        CompoundDomain>::template AgentProxy<AgentObservation>
        CompoundObservation;
    typedef typename CompoundDomain::Features::
        template AgentDomain<CompoundDomain>::template AgentProxy<
            AgentObservationSpace<AgentObservation>>
            CompoundObservationSpace;
    typedef std::unique_ptr<CompoundObservationSpace>
        CompoundObservationSpacePtr;
    typedef AgentObservationDistribution<std::shared_ptr<CompoundObservation>>
        CompoundObservationDistribution;
    typedef std::unique_ptr<CompoundObservationDistribution>
        CompoundObservationDistributionPtr;
    typedef typename CompoundDomain::Features::
        template AgentDomain<CompoundDomain>::template AgentProxy<
            typename CompoundDomain::Features::template ConcurrencyDomain<
                CompoundDomain>::
                template ConcurrencyProxy<
                    typename CompoundDomain::Features::template ActivityDomain<
                        CompoundDomain>::AgentEvent>>
            CompoundEvent;
    typedef std::unique_ptr<CompoundEvent> CompoundEventPtr;

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
     * this function represents: $P(O|s, a)$, where $O$ is the random variable
     * of the observation.
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
};

/**
 * @brief A domain must inherit this class if it is transformed observable.
 *
 *  "Transformed observable" means that the observation provided to the agent is
 * deterministically computed from (but generally not equal to) the internal
 * state of the domain.
 *
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class TransformedObservableDomain {
public:
  /**
   * @brief Type of an agent's states
   */
  typedef typename ObservabilityTypesImporter<CompoundDomain,
                                              TransformedObservableDomain>::
      template import_state_type<>::result AgentState;

  /**
   * @brief Type of an agent's state space
   *
   * @tparam Args Type of states
   */
  template <typename... Args>
  using AgentStateSpace =
      typename ObservabilityTypesImporter<CompoundDomain,
                                          TransformedObservableDomain>::
          template import_state_space_type<ImplicitSpace>::template test_with<
              AgentState>::template result<Args...>;

  /**
   * @brief Type of an agent's observations
   */
  typedef typename ObservabilityTypesImporter<
      CompoundDomain, TransformedObservableDomain,
      CompoundDomain::Features::template ObservabilityDomain>::
      template import_observation_type<>::result AgentObservation;

  /**
   * @brief Type of an agent's observation space
   */
  template <typename... Args>
  using AgentObservationSpace = typename ObservabilityTypesImporter<
      CompoundDomain, TransformedObservableDomain,
      CompoundDomain::Features::template ObservabilityDomain>::
      template import_observation_space_type<ImplicitSpace>::template test_with<
          AgentObservation>::template result<Args...>;

  /**
   * @brief Type of an agent's observation distribution
   */
  template <typename... Args>
  using AgentObservationDistribution = typename ObservabilityTypesImporter<
      CompoundDomain, TransformedObservableDomain>::
      template import_observation_distribution_type<SingleValueDistribution>::
          template test_with<AgentObservation>::template result<Args...>;

  static_assert(std::is_same_v<AgentObservationDistribution<AgentObservation>,
                               SingleValueDistribution<AgentObservation>>,
                "The domain type AgentObservationDistribution must be equal to "
                "skdecide::SingleValueDistirbution when the agent domain "
                "feature derives from skdecide::TransformedObservableDomain");

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature : public PartiallyObservableDomain<CompoundDomain>::Feature {
  public:
    typedef TransformedObservableDomain<CompoundDomain> FeatureDomain;

    typedef typename PartiallyObservableDomain<
        CompoundDomain>::Feature::CompoundState CompoundState;
    typedef typename PartiallyObservableDomain<
        CompoundDomain>::Feature::CompoundEventPtr CompoundEventPtr;
    typedef typename PartiallyObservableDomain<
        CompoundDomain>::Feature::CompoundObservation CompoundObservation;
    typedef typename std::unique_ptr<CompoundObservation>
        CompoundObservationPtr;
    typedef SingleValueDistribution<std::shared_ptr<CompoundObservation>>
        CompoundObservationDistribution;
    typedef std::unique_ptr<CompoundObservationDistribution>
        CompoundObservationDistributionPtr;

    /**
     * @brief Get the probability distribution of the observation given a state
     * and action. In mathematical terms (discrete case), given an action $a$,
     * this function represents: $P(O|s, a)$, where $O$ is the random variable
     * of the observation.
     * @param state The state to be observed.
     * @param event The last applied action (or None if the state is an initial
     * state).
     * @return ObservationDistributionPtr The probability distribution of the
     * observation.
     */
    virtual CompoundObservationDistributionPtr get_observation_distribution(
        const CompoundState &state,
        const CompoundEventPtr &event = CompoundEventPtr()) {
      return std::make_unique<
          SingleValueDistribution<std::shared_ptr<CompoundObservation>>>(
          get_observation(state, event));
    }

    /**
     * @brief Get the deterministic observation given a state and action.
     *
     * @param state The state to be observed.
     * @param event The last applied action (or None if the state is an initial
     * state).
     * @return The observation given the state and action.
     */
    virtual CompoundObservationPtr
    get_observation(const CompoundState &state,
                    const CompoundEventPtr &event = CompoundEventPtr()) = 0;
  };
};

/**
 * @brief A domain must inherit this class if it is fully observable.
 * "Fully observable" means that the observation provided to the agent is
 * equal to the internal state of the domain.
 * !!! warning.
 * In the case of fully observable domains, make sure that the observation
 * type D.T_observation is equal to the state type D.T_state.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class FullyObservableDomain {
public:
  /**
   * @brief Type of an agent's states
   */
  typedef typename ObservabilityTypesImporter<
      CompoundDomain,
      FullyObservableDomain>::template import_state_type<>::result AgentState;

  /**
   * @brief Type of an agent's state space
   *
   * @tparam Args Type of states
   */
  template <typename... Args>
  using AgentStateSpace = typename ObservabilityTypesImporter<
      CompoundDomain,
      FullyObservableDomain>::template import_state_space_type<ImplicitSpace>::
      template test_with<AgentState>::template result<Args...>;

  /**
   * @brief Type of an agent's observations
   */
  typedef typename ObservabilityTypesImporter<CompoundDomain,
                                              FullyObservableDomain>::
      template import_observation_type<AgentState>::result AgentObservation;

  static_assert(
      std::is_same_v<AgentObservation, AgentState>,
      "The domain type AgentObservation must be equal to AgentState when the "
      "agent domain feature derives from skdecide::FullyObservableDomain");

  /**
   * @brief Type of an agent's observation space
   */
  template <typename... Args>
  using AgentObservationSpace =
      typename ObservabilityTypesImporter<CompoundDomain,
                                          FullyObservableDomain>::
          template import_observation_space_type<AgentStateSpace>::
              template test_with<AgentObservation>::template result<Args...>;

  static_assert(std::is_same_v<AgentObservationSpace<AgentState>,
                               AgentStateSpace<AgentState>>,
                "The domain type AgentObservationSpace must be equal to "
                "AgentStateSpace when the agent domain feature derives from "
                "skdecide::FullyObservableDomain");

  /**
   * @brief Type of an agent's observation distribution
   */
  template <typename... Args>
  using AgentObservationDistribution = typename ObservabilityTypesImporter<
      CompoundDomain, FullyObservableDomain>::
      template import_observation_distribution_type<SingleValueDistribution>::
          template test_with<AgentObservation>::template result<Args...>;

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature : public TransformedObservableDomain<CompoundDomain>::Feature {
  public:
    typedef FullyObservableDomain<CompoundDomain> FeatureDomain;

    typedef typename TransformedObservableDomain<
        CompoundDomain>::Feature::CompoundState CompoundState;
    typedef typename TransformedObservableDomain<
        CompoundDomain>::Feature::CompoundEventPtr CompoundEventPtr;
    typedef std::unique_ptr<CompoundState> CompoundObservationPtr;
    typedef typename TransformedObservableDomain<
        CompoundDomain>::Feature::CompoundStateSpacePtr CompoundStateSpacePtr;

    /**
     * @brief Get the deterministic observation given a state and action.
     *
     * @param state The state to be observed.
     * @param event The last applied action (or None if the state is an
     * initial state).
     * @return The observation given the state and action.
     */
    virtual CompoundObservationPtr
    get_observation(const CompoundState &state,
                    const CompoundEventPtr &event = CompoundEventPtr()) {
      return std::make_unique<CompoundState>(state);
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
};

} // namespace skdecide

#endif // SKDECIDE_OBSERVABILITY_HH
