/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_DYNAMICS_HH
#define SKDECIDE_DYNAMICS_HH

#include "core.hh"
#include "observability.hh"
#include "memory.hh"
#include <memory>
#include <type_traits>

namespace skdecide {

/**
 * @brief A domain must inherit this class if agents interact with it like a
 * black-box environment.
 * Black-box environment examples include: the real world, compiled ATARI
 * games, etc.
 * !!! Tip.
 * Environment domains are typically stateful: they must keep the current
 * state or history in their memory to compute next steps (automatically done by
 * default in the #_memory attribute).
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename DerivedCompoundDomain> class EnvironmentDomain {
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
      typename DerivedCompoundDomain::RawState>
      CompoundState;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawObservation>
      CompoundObservation;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawValue>
      CompoundValue;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawPredicate>
      CompoundPredicate;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawInfo>
      CompoundInfo;
  typedef EnvironmentOutcome<CompoundObservation,
                             DerivedCompoundDomain::TransitionValue,
                             CompoundValue, CompoundPredicate, CompoundInfo>
      EnvironmentOutcomeReturn;
  typedef TransitionOutcome<CompoundState,
                            DerivedCompoundDomain::TransitionValue,
                            CompoundValue, CompoundPredicate, CompoundInfo>
      TransitionOutcomeReturn;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      TransitionOutcomeReturn>
      TransitionOutcomePtr;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::template ConcurrencyProxy<
          typename DerivedCompoundDomain::RawEvent>>
      CompoundEvent;

  /**
   * @brief Run one step of the environment's dynamics.
   *
   *     By default, Environment::step() provides some boilerplate code and
   * internally calls Environment::make_step() (which returns a transition
   * outcome).
   * The boilerplate code automatically stores next state into the #_memory
   * attribute and samples a corresponding observation.
   *
   * !!! Tip.
   *        Whenever an existing environment needs to be wrapped instead of
   * implemented fully in scikit-decide (e.g. compiled ATARI games), it is
   * recommended to overwrite Environment::step() to call the external
   * environment
   * and not use the Environment.make_step() helper function.
   *
   * !!! Warning.
   *         Before calling Environment::step() the first time or when the end
   * of an episode is reached, Initializable::reset() must be called to reset
   * the environment's state.
   *
   * @param event The action taken in the current memory (state or history)
   * triggering the transition.
   * @return The environment outcome of this step.
   */
  EnvironmentOutcomeReturn step(const CompoundEvent &event) {
    const TransitionOutcomeReturn &transition_outcome =
        *(this->make_step(event));
    const CompoundState &next_state = transition_outcome.state;
    CompoundObservation observation =
        this->get_observation_distribution(next_state, event).sample();
    if (this->get_memory_maxlen() > 0) {
      this->_memory.push_back(next_state);
    }
    return EnvironmentOutcomeReturn(observation, transition_outcome.value,
                                    transition_outcome.termination,
                                    transition_outcome.info);
  }

protected:
  /**
   * @brief Compute one step of the transition's dynamics.
   *
   * This is a helper function called by default from #Environment._step().
   * It focuses on the state level, as opposed to the observation one for the
   * latter.
   *
   * @param event The action taken in the current memory (state or history)
   * triggering the transition.
   * @return The transition outcome of this step.
   */
  virtual TransitionOutcomePtr make_step(const CompoundEvent &event) = 0;
};

/**
 * @brief A domain must inherit this class if agents interact with it like a
 * simulation.
 *
 *  Compared to pure environment domains, simulation ones have the additional
 * ability to sample transitions from any given state.
 *
 * !!! Tip.
 *      Simulation domains are typically stateless: they do not need to store
 * the current state or history in memory since it is usually passed as
 * parameter of their functions. By default, they only become stateful whenever
 * they are used as environments (e.g. via InitializableDomain::reset() and
 * EnvironmentDomain::step() functions).
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename DerivedCompoundDomain>
class SimulationDomain : public EnvironmentDomain<DerivedCompoundDomain> {
public:
  typedef typename EnvironmentDomain<DerivedCompoundDomain>::CompoundState
      CompoundState;
  typedef typename EnvironmentDomain<DerivedCompoundDomain>::CompoundObservation
      CompoundObservation;
  typedef typename EnvironmentDomain<DerivedCompoundDomain>::CompoundValue
      CompoundValue;
  typedef typename EnvironmentDomain<DerivedCompoundDomain>::CompoundPredicate
      CompoundPredicate;
  typedef typename EnvironmentDomain<DerivedCompoundDomain>::CompoundInfo
      CompoundInfo;
  typedef typename EnvironmentDomain<
      DerivedCompoundDomain>::EnvironmentOutcomeReturn EnvironmentOutcomeReturn;
  typedef
      typename EnvironmentDomain<DerivedCompoundDomain>::TransitionOutcomeReturn
          TransitionOutcomeReturn;
  typedef
      typename EnvironmentDomain<DerivedCompoundDomain>::TransitionOutcomePtr
          TransitionOutcomePtr;
  typedef typename EnvironmentDomain<DerivedCompoundDomain>::CompoundEvent
      CompoundEvent;
  typedef typename DerivedCompoundDomain::template MemoryProxy<CompoundState>
      CompoundMemory;

  /**
   * @brief Sample one transition of the simulator's dynamics.
   *
   *    By default, Simulation::sample() provides some boilerplate code and
   * internally calls Simulation::make_sample() (which returns a transition
   * outcome). The boilerplate code automatically samples an observation
   * corresponding to the sampled next state.
   *
   * !!! Tip.
   *        Whenever an existing simulator needs to be wrapped instead of
   * implemented fully in scikit-decide (e.g. a simulator), it is recommended to
   * overwrite Simulation::sample() to call the external simulator and not use
   * the Simulation::make_sample() helper function.
   *
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @return The environment outcome of the sampled
   * transition.
   */
  EnvironmentOutcomeReturn sample(const CompoundMemory &memory,
                                  const CompoundEvent &event) {
    const TransitionOutcomeReturn &transition_outcome =
        *(make_sample(memory, event));
    const CompoundState &next_state = transition_outcome.state;
    CompoundObservation observation =
        get_observation_distribution(next_state, event).sample();
    return EnvironmentOutcomeReturn(observation, transition_outcome.value,
                                    transition_outcome.termination,
                                    transition_outcome.info);
  }

  /**
   * @brief Set internal memory attribute _memory to given one.

   *    This can be useful to set a specific "starting point" before doing a
   * rollout with successive Environment::make_step() calls.
   *
   * @param m The memory to set internally.
   */
  inline void set_memory(const CompoundMemory &m) { this->_memory = m; }

protected:
  /**
   * @brief Compute one step of the transition's dynamics.
   *
   * This is a helper function called by default from #Environment._step().
   * It focuses on the state level, as opposed to the observation one for the
   * latter.
   *
   * @param event The action taken in the current memory (state or history)
   * triggering the transition.
   * @return The transition outcome of this step.
   */
  inline virtual TransitionOutcomePtr make_step(const CompoundEvent &event) {
    return make_sample(this->_memory, event);
  }

  /**
   * @brief Compute one sample of the transition's dynamics.
   *
   *    This is a helper function called by default from #Simulation._sample().
   * It focuses on the state level, as opposed to the observation one for the
   * latter.
   *
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @return The transition outcome of the sampled
   * transition.
   */
  virtual TransitionOutcomePtr make_sample(const CompoundMemory &memory,
                                           const CompoundEvent &event) = 0;
};

/**
 * @brief A domain must inherit this class if its dynamics is uncertain and
 * provided as a white-box model.
 *
 *  Compared to pure simulation domains, uncertain transition ones provide in
 * addition the full probability distribution of next states given a memory and
 * action.
 *
 * !!! Tip.
 *      Uncertain transition domains are typically stateless: they do not need
 * to store the current state or history in memory since it is usually passed as
 * parameter of their functions. By default, they only become stateful whenever
 * they are used as environments (e.g. via InitializableDomain::reset() and
 * EnvironmentDomain::step() functions).
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam NextStateDistribution The type of the distribution used to sample
 * next states
 */
template <typename DerivedCompoundDomain, template <typename...>
                                          class NextStateDistribution =
                                              ImplicitDistribution>
class UncertainTransitionDomain
    : public SimulationDomain<DerivedCompoundDomain> {
public:
  typedef typename SimulationDomain<DerivedCompoundDomain>::CompoundEvent
      CompoundEvent;
  typedef typename SimulationDomain<DerivedCompoundDomain>::CompoundState
      CompoundState;
  typedef typename DerivedCompoundDomain::template SmartPointer<CompoundState>
      CompoundStatePtr;
  typedef typename SimulationDomain<DerivedCompoundDomain>::CompoundMemory
      CompoundMemory;
  typedef NextStateDistribution<CompoundState> NextCompoundStateDistribution;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      NextCompoundStateDistribution>
      NextCompoundStateDistributionPtr;
  typedef typename SimulationDomain<DerivedCompoundDomain>::CompoundValue
      CompoundValue;
  typedef typename SimulationDomain<DerivedCompoundDomain>::CompoundInfo
      CompoundInfo;
  typedef Value<DerivedCompoundDomain::TransitionValue, CompoundValue>
      TransitionValueReturn;
  typedef
      typename SimulationDomain<DerivedCompoundDomain>::TransitionOutcomeReturn
          TransitionOutcomeReturn;
  typedef typename SimulationDomain<DerivedCompoundDomain>::TransitionOutcomePtr
      TransitionOutcomePtr;

  /**
   * @brief Get the probability distribution of next state given a memory and
   * action.
   *
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @return The probability distribution of
   * next state.
   */
  virtual NextCompoundStateDistributionPtr
  get_next_state_distribution(const CompoundMemory &memory,
                              const CompoundEvent &event) = 0;

  /**
   * @brief Get the value (reward or cost) of a transition.
   *
   *     The transition to consider is defined by the function parameters.
   *
   * !!! Tip.
   *         If this function never depends on the next_state parameter for its
   * computation, it is recommended to indicate it by overriding
   * UncertainTransitions::is_transition_value_dependent_on_next_state() to
   * return False. This information can then be exploited by solvers to avoid
   * computing next state to evaluate a transition value (more efficient).
   *
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @param next_state The next state in which the transition ends (if needed
   * for the computation).
   * @return The transition value (reward or cost).
   */
  virtual TransitionValueReturn get_transition_value(
      const CompoundMemory &memory, const CompoundEvent &event,
      const CompoundStatePtr &next_state = CompoundStatePtr()) = 0;

  /**
   * @brief Indicate whether _get_transition_value() requires the next_state
   * parameter for its computation (cached).
   *
   * By default,
   * UncertainTransitions::is_transition_value_dependent_on_next_state()
   * internally calls
   * UncertainTransitions::check_transition_value_dependency_on_next_state_()
   * the first time and automatically caches its value to make future calls more
   * efficient (since the returned value is assumed to be constant).
   *
   * @return true The transition value computation depends on next_state.
   * @return false The transition value computation does not depend on
   * next_state.
   */
  bool is_transition_value_dependent_on_next_state() {
    if (!_transition_value_dependent_on_next_state) {
      _transition_value_dependent_on_next_state = std::make_unique<bool>(
          this->check_transition_value_dependency_on_next_state());
    }
    return *_transition_value_dependent_on_next_state;
  }

  /**
   * @brief Indicate whether a state is terminal.
   *
   *    A terminal state is a state with no outgoing transition (except to
   * itself with value 0).
   *
   * @param state The state to consider.
   * @return true The state is terminal.
   * @return false The state is not terminal.
   */
  virtual bool is_terminal(const CompoundState &state) = 0;

protected:
  mutable std::unique_ptr<bool> _transition_value_dependent_on_next_state;

  /**
   * @brief Indicate whether _get_transition_value() requires the next_state
   * parameter for its computation.
   *
   * This is a helper function called by default from
   * UncertainTransitions::is_transition_value_dependent_on_next_state(), the
   * difference being that the result is not cached here.
   *
   * @return true The transition value computation depends on next_state.
   * @return false The transition value computation does not depend on
   * next_state.
   */
  virtual bool check_transition_value_dependency_on_next_state() const {
    return true;
  }

  /**
   * @brief Compute one sample of the transition's dynamics.
   *
   *    This is a helper function called by default from #Simulation._sample().
   * It focuses on the state level, as opposed to the observation one for the
   * latter.
   *
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @return The transition outcome of the sampled
   * transition.
   */
  virtual TransitionOutcomePtr make_sample(const CompoundMemory &memory,
                                           const CompoundEvent &event) {
    CompoundState next_state =
        get_next_state_distribution(memory, event).sample();
    TransitionValueReturn value =
        get_transition_value(memory, event, next_state);
    bool termination = is_terminal(next_state);
    return TransitionOutcomePtr(
        new TransitionOutcomeReturn(next_state, value, termination));
  }
};

/**
 * @brief A domain must inherit this class if its dynamics is uncertain (with
 * enumerable transitions) and provided as a white-box model.
 *
 * Compared to pure uncertain transition domains, enumerable transition ones
 * guarantee that all probability distributions of next state are discrete.
 *
 * !!! Tip.
 *      Enumerable transition domains are typically stateless: they do not need
 * to store the current state or history in memory since it is usually passed as
 * parameter of their functions. By default, they only become stateful whenever
 * they are used as environments (e.g. via InitializableDomain::reset() and
 * EnvironmentDomain::step() functions).
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam NextStateDistribution The type of the distribution used to sample
 * next states (must inherit from skdecide::DiscreteDistribution<...>)
 */
template <typename DerivedCompoundDomain, template <typename...>
                                          class NextStateDistribution =
                                              DiscreteDistribution>
class EnumerableTransitionDomain
    : public UncertainTransitionDomain<DerivedCompoundDomain,
                                       NextStateDistribution> {
  static_assert(
      std::is_base_of<DiscreteDistribution<int>,
                      NextStateDistribution<int>>::value,
      "NextStateDistribution template class parameter must be derived from "
      "skdecide::DiscreteDistribution<...>");

public:
  typedef
      typename UncertainTransitionDomain<DerivedCompoundDomain,
                                         NextStateDistribution>::CompoundEvent
          CompoundEvent;
  typedef
      typename UncertainTransitionDomain<DerivedCompoundDomain,
                                         NextStateDistribution>::CompoundState
          CompoundState;
  typedef
      typename UncertainTransitionDomain<DerivedCompoundDomain,
                                         NextStateDistribution>::CompoundMemory
          CompoundMemory;
  typedef NextStateDistribution<CompoundState> NextCompoundStateDistribution;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      NextCompoundStateDistribution>
      NextCompoundStateDistributionPtr;

  /**
   * @brief Get the discrete probability distribution of next state given a
   * memory and action.
   *
   * !!! Tip.
   *        In the Markovian case (memory only holds last state $s$), given an
   * action $a$, this function can be mathematically represented by $P(S'|s,
   * a)$, where $S'$ is the next state random variable.
   *
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @return The discrete probability distribution of next state.
   */
  virtual NextCompoundStateDistributionPtr
  get_next_state_distribution(const CompoundMemory &memory,
                              const CompoundEvent &event) = 0;
};

/**
 * @brief A domain must inherit this class if its dynamics is deterministic and
 * provided as a white-box model.
 *
 *  Compared to pure enumerable transition domains, deterministic transition
 * ones guarantee that there is only one next state for a given source memory
 * (state or history) and action.
 *
 * !!! Tip.
 *       Deterministic transition domains are typically stateless: they do not
 * need to store the current state or history in memory since it is usually
 * passed as parameter of their functions. By default, they only become stateful
 * whenever they are used as environments (e.g. via InitializableDomain::reset()
 * and EnvironmentDomain::step() functions).
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename DerivedCompoundDomain>
class DeterministicTransitionDomain
    : public EnumerableTransitionDomain<DerivedCompoundDomain,
                                        SingleValueDistribution> {
public:
  typedef typename EnumerableTransitionDomain<
      DerivedCompoundDomain, SingleValueDistribution>::CompoundEvent
      CompoundEvent;
  typedef typename EnumerableTransitionDomain<
      DerivedCompoundDomain, SingleValueDistribution>::CompoundState
      CompoundState;
  typedef typename DerivedCompoundDomain::template SmartPointer<CompoundState>
      CompoundStatePtr;
  typedef typename EnumerableTransitionDomain<
      DerivedCompoundDomain, SingleValueDistribution>::CompoundMemory
      CompoundMemory;
  typedef SingleValueDistribution<CompoundState> NextCompoundStateDistribution;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      NextCompoundStateDistribution>
      NextCompoundStateDistributionPtr;

  /**
   * @brief Get the discrete probability distribution of next state given a
   * memory and action.
   *
   * !!! Tip.
   *        In the Markovian case (memory only holds last state $s$), given an
   * action $a$, this function can be mathematically represented by $P(S'|s,
   * a)$, where $S'$ is the next state random variable.
   *
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @return The discrete probability distribution of next state.
   */
  inline virtual NextCompoundStateDistributionPtr
  get_next_state_distribution(const CompoundMemory &memory,
                              const CompoundEvent &event) {
    return NextCompoundStateDistributionPtr(
        new NextCompoundStateDistribution(get_next_state(memory, event)));
  }

  /**
   * @brief Get the next state given a memory and action.
   *
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @return The deterministic next state.
   */
  virtual CompoundStatePtr get_next_state(const CompoundMemory &memory,
                                          const CompoundEvent &event) = 0;
};

} // namespace skdecide

#endif // SKDECIDE_DYNAMICS_HH
