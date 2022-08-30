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
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class PartiallyObservableDomain {
private:
  template <class T, class = void> struct define_state_type {
    static_assert(!std::is_same_v<typename CompoundDomain::Types, T>,
                  "The domain types must define AgentState.");
  };

  template <class T>
  struct define_state_type<T, std::void_t<typename T::AgentState>> {
    typedef typename T::AgentState result;
  };

public:
  /**
   * @brief Type of an agent's states
   */
  typedef typename define_state_type<typename CompoundDomain::Types>::result
      AgentState;

private:
  template <class T, class = void> struct define_state_space_type {
    template <typename... Args> using result = ImplicitSpace<Args...>;
  };

  template <class T>
  struct define_state_space_type<
      T, std::void_t<typename T::template AgentStateSpace<char>>> {
    template <typename... Args>
    using result = typename T::template AgentStateSpace<Args...>;
  };

public:
  /**
   * @brief Type of an agent's state space
   *
   * @tparam Args Type of states
   */
  template <typename... Args>
  using AgentStateSpace = typename define_state_space_type<
      typename CompoundDomain::Types>::template result<Args...>;

private:
  template <class T, class D, class = void> struct define_observation_type {
    template <class, class = void> struct check_observability_domain;

    template <class DD>
    struct check_observability_domain<
        DD, std::enable_if_t<std::is_same_v<
                PartiallyObservableDomain<CompoundDomain>, DD>>> {
      static_assert(
          !std::is_same_v<PartiallyObservableDomain<CompoundDomain>, DD>,
          "The domain types must define AgentObservation when the "
          "observability domain feature is "
          "skdecide::PartiallyObservableDomain");
    };

    template <class DD>
    struct check_observability_domain<
        DD, std::enable_if_t<!std::is_same_v<
                PartiallyObservableDomain<CompoundDomain>, DD>>> {
      typedef AgentState result;
    };

    typedef typename check_observability_domain<D>::result result;
  };

  template <class T, class D>
  struct define_observation_type<T, D,
                                 std::void_t<typename T::AgentObservation>> {
    typedef typename T::AgentObservation result;
  };

public:
  /**
   * @brief Type of an agent's observations
   */
  typedef typename define_observation_type<
      typename CompoundDomain::Types,
      typename CompoundDomain::Features::template ObservabilityDomain<
          CompoundDomain>>::result AgentObservation;

private:
  template <class T, class = void> struct define_observation_space_type {
    template <typename... Args> using result = AgentStateSpace<Args...>;
  };

  template <class T>
  struct define_observation_space_type<
      T, std::void_t<typename T::template AgentObservationSpace<char>>> {
    template <typename... Args>
    using result = typename T::template AgentObservationSpace<Args...>;
  };

public:
  /**
   * @brief Type of an agent's observation space
   */
  template <typename... Args>
  using AgentObservationSpace = typename define_observation_space_type<
      typename CompoundDomain::Types>::template result<Args...>;

private:
  template <class, class, class = void>
  struct define_observation_distribution_type;

  template <class D, class T>
  struct define_observation_distribution_type<
      D, T,
      std::enable_if_t<
          std::is_same_v<PartiallyObservableDomain<CompoundDomain>, D>>> {

    template <class, class = void> struct try_import_from_domain_types {
      template <typename... Args> using result = ImplicitDistribution<Args...>;
    };

    template <class TT>
    struct try_import_from_domain_types<
        TT,
        std::void_t<typename TT::template AgentObservationDistribution<char>>> {
      template <typename... Args>
      using result =
          typename TT::template AgentObservationDistribution<Args...>;
    };

    template <typename... Args>
    using result =
        typename try_import_from_domain_types<T>::template result<Args...>;
  };

  template <class D, class T>
  struct define_observation_distribution_type<
      D, T,
      std::enable_if_t<
          !std::is_same_v<PartiallyObservableDomain<CompoundDomain>, D>>> {
    template <typename... Args> using result = SingleValueDistribution<Args...>;
  };

public:
  /**
   * @brief Type of an agent's observation distribution
   */
  template <typename... Args>
  using AgentObservationDistribution =
      typename define_observation_distribution_type<
          typename CompoundDomain::Features::template ObservabilityDomain<
              CompoundDomain>,
          typename CompoundDomain::Types>::template result<Args...>;

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
  typedef
      typename CompoundDomain::Features::template AgentDomain<CompoundDomain>::
          template AgentProxy<AgentObservationSpace<AgentObservation>>
              CompoundObservationSpace;
  typedef std::unique_ptr<CompoundObservationSpace> CompoundObservationSpacePtr;
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
 * @brief A domain must inherit this class if it is transformed observable.
 *
 *  "Transformed observable" means that the observation provided to the agent is
 * deterministically computed from (but generally not equal to) the internal
 * state of the domain.
 *
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain>
class TransformedObservableDomain
    : public PartiallyObservableDomain<CompoundDomain> {
public:
  /**
   * @brief Type of an agent's states
   */
  typedef
      typename PartiallyObservableDomain<CompoundDomain>::AgentState AgentState;

  /**
   * @brief Type of an agent's state space
   *
   * @tparam Args Type of states
   */
  template <typename... Args>
  using AgentStateSpace = typename PartiallyObservableDomain<
      CompoundDomain>::template AgentStateSpace<Args...>;

  /**
   * @brief Type of an agent's observation space
   */
  template <typename... Args>
  using AgentObservationSpace = typename PartiallyObservableDomain<
      CompoundDomain>::template AgentObservationSpace<Args...>;

private:
  template <class, class = void>
  struct check_observation_distribution : std::true_type {};

  template <class T>
  struct check_observation_distribution<
      T, std::void_t<typename T::AgentObservationDistribution>>
      : std::conditional<
            std::is_same_v<
                SingleValueDistribution<char>,
                typename T::template AgentObservationDistribution<char>>,
            std::true_type, std::false_type>::type {};

  static_assert(
      check_observation_distribution<typename CompoundDomain::Types>::value,
      "The domain type AgentObservationDistribution must be equal to "
      "skdecide::SingleValueDistirbution when the agent domain feature is "
      "skdecide::TransformedObservableDomain");

public:
  /**
   * @brief Type of an agent's observation distribution
   */
  template <typename... Args>
  using AgentObservationDistribution = SingleValueDistribution<Args...>;

  typedef typename PartiallyObservableDomain<CompoundDomain>::CompoundState
      CompoundState;
  typedef typename PartiallyObservableDomain<CompoundDomain>::CompoundEventPtr
      CompoundEventPtr;
  typedef
      typename PartiallyObservableDomain<CompoundDomain>::CompoundObservation
          CompoundObservation;
  typedef typename std::unique_ptr<CompoundObservation> CompoundObservationPtr;
  typedef SingleValueDistribution<std::shared_ptr<CompoundObservation>>
      CompoundObservationDistribution;
  typedef std::unique_ptr<CompoundObservationDistribution>
      CompoundObservationDistributionPtr;

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
template <typename CompoundDomain>
class FullyObservableDomain
    : public virtual TransformedObservableDomain<CompoundDomain> {
public:
  typedef typename TransformedObservableDomain<CompoundDomain>::AgentState
      AgentState;
  template <typename... Args>
  using AgentStateSpace = typename TransformedObservableDomain<
      CompoundDomain>::template AgentStateSpace<Args...>;
  template <typename... Args>
  using AgentObservationDistribution = typename TransformedObservableDomain<
      CompoundDomain>::template AgentObservationDistribution<Args...>;

private:
  template <class, class = void>
  struct check_observation_type : std::true_type {};

  template <class T>
  struct check_observation_type<T, std::void_t<typename T::AgentObservation>>
      : std::conditional<
            std::is_same_v<AgentState, typename T::AgentObservation>,
            std::true_type, std::false_type>::type {};

  static_assert(
      check_observation_type<typename CompoundDomain::Types>::value,
      "The domain type AgentObservation must be equal to AgentState "
      "when the agent domain feature is skdecide::FullyObservableDomain");

public:
  /**
   * @brief Type of an agent's observations
   */
  typedef AgentState AgentObservation;

private:
  template <class, class = void>
  struct check_observation_space_type : std::true_type {};

  template <class T>
  struct check_observation_space_type<
      T, std::void_t<typename T::template AgentObservationSpace<char>>>
      : std::conditional<
            std::is_same_v<AgentStateSpace<char>,
                           typename T::template AgentObservationSpace<char>>,
            std::true_type, std::false_type>::type {};

  static_assert(
      check_observation_space_type<typename CompoundDomain::Types>::value,
      "The domain type AgentObservationSpace must be equal to AgentStateSpace "
      "when the agent domain feature is skdecide::FullyObservableDomain");

public:
  /**
   * @brief Type of an agent's observation space
   */
  template <typename... Args>
  using AgentObservationSpace = AgentStateSpace<Args...>;

public:
  typedef typename TransformedObservableDomain<CompoundDomain>::CompoundState
      CompoundState;
  typedef typename TransformedObservableDomain<CompoundDomain>::CompoundEventPtr
      CompoundEventPtr;
  typedef std::unique_ptr<CompoundState> CompoundObservationPtr;
  typedef typename TransformedObservableDomain<
      CompoundDomain>::CompoundStateSpacePtr CompoundStateSpacePtr;

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

} // namespace skdecide

#endif // SKDECIDE_OBSERVABILITY_HH
