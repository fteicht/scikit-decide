/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_DYNAMICS_HH
#define SKDECIDE_DYNAMICS_HH

#include "core.hh"
#include "observability.hh"
#include "memory.hh"

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
 * @tparam Tagent Type of agents (MultiAgent or SingleAgent)
 * @tparam Tstate Type of states (any kind of type)
 * @tparam Tobservation Type of observations for each agent (any kind of type)
 * @tparam Tconcurrency Type of concurrency (Parallel or Sequential)
 * @tparam Tevent Type of event for each agent (any kind of type)
 * @tparam VT Value type (either ValueType::REWARD or ValueType::COST)
 * @tparam Tvalue Floating point type for values (usually double)
 * @tparam Tpredicate Boolean test type (usually bool)
 * @tparam Tinfo Transition info type (any kind of type)
 * @tparam TstateSpace Type of state space for each agent (any kind of type
 * containing Tstate elements)
 * @tparam TobservationSpace Type of observation space for each agent (any kind
 * of type containing Tobservation elements)
 * @tparam TobservationDistribution Type of observation distribution for each
 * agent (any kind of distribution type containing Tobservation random elements)
 * @tparam TsmartPointer (any kind of std-like smart pointer)
 */
template <typename Tagent, typename Tstate, typename Tobservation,
          typename Tconcurrency, typename Tevent, ValueType VT, typename Tvalue,
          typename Tpredicate, typename Tinfo,
          template <typename...> class TstateSpace = Space,
          template <typename...> class TobservationSpace = Space,
          template <typename...> class TobservationDistribution = Distribution,
          template <typename...> class TsmartPointer = std::unique_ptr>
class EnvironmentDomain
    : public virtual PartiallyObservableDomain<
          Tagent, Tstate, Tobservation, Tconcurrency, Tevent, TstateSpace,
          TobservationSpace, TobservationDistribution, TsmartPointer>,
      public virtual HistoryDomain<Tstate> {
public:
  typedef typename Tagent::template Proxy<Tstate> State;
  typedef typename Tagent::template Proxy<Tobservation> Observation;
  typedef typename Tagent::template Proxy<Tvalue> Value;
  typedef typename Tagent::template Proxy<Tpredicate> Predicate;
  typedef typename Tagent::template Proxy<Tinfo> Info;
  typedef EnvironmentOutcome<Observation, VT, Value, Predicate, Info>
      EnvironmentOutcomeReturn;
  typedef TransitionOutcome<State, VT, Value, Predicate, Info>
      TransitionOutcomeReturn;
  typedef TsmartPointer<TransitionOutcomeReturn> TransitionOutcomePtr;
  typedef typename Tagent::template Proxy<
      typename Tconcurrency::template Proxy<Tevent>>
      Event;

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
   * @return EnvironmentOutcomeReturn The environment outcome of this step.
   */
  EnvironmentOutcomeReturn step(const Event &event) {
    const TransitionOutcomeReturn &transition_outcome =
        *(this->make_step(event));
    const State &next_state = transition_outcome.state;
    Observation observation =
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
   * @return TransitionOutcomePtr The transition outcome of this step.
   */
  virtual TransitionOutcomePtr make_step(const Event &event) = 0;
};

template <typename Tagent, typename Tstate, typename Tobservation,
          typename Tconcurrency, typename Tevent, ValueType VT, typename Tvalue,
          typename Tpredicate, typename Tinfo,
          template <typename...> class TstateSpace = Space,
          template <typename...> class TobservationSpace = Space,
          template <typename...> class TobservationDistribution = Distribution,
          template <typename...> class TsmartPointer = std::unique_ptr>
class SimulationDomain
    : public EnvironmentDomain<Tagent, Tstate, Tobservation, Tconcurrency,
                               Tevent, VT, Tvalue, Tpredicate, Tinfo,
                               TstateSpace, TobservationSpace,
                               TobservationDistribution, TsmartPointer> {
public:
  typedef typename Tagent::template Proxy<Tstate> State;
  typedef typename Tagent::template Proxy<Tobservation> Observation;
  typedef typename Tagent::template Proxy<Tvalue> Value;
  typedef typename Tagent::template Proxy<Tpredicate> Predicate;
  typedef typename Tagent::template Proxy<Tinfo> Info;
  typedef EnvironmentOutcome<Observation, VT, Value, Predicate, Info>
      EnvironmentOutcomeReturn;
  typedef TransitionOutcome<State, VT, Value, Predicate, Info>
      TransitionOutcomeReturn;
  typedef TsmartPointer<TransitionOutcomeReturn> TransitionOutcomePtr;
  typedef typename Tagent::template Proxy<
      typename Tconcurrency::template Proxy<Tevent>>
      Event;
  typedef Memory<Tstate> StateMemory;

  EnvironmentOutcomeReturn sample(const StateMemory &memory,
                                  const Event &event) {
    const TransitionOutcomeReturn &transition_outcome =
        *(make_sample(memory, event));
    const State &next_state = transition_outcome.state;
    Observation observation =
        get_observation_distribution(next_state, event).sample();
    return EnvironmentOutcomeReturn(observation, transition_outcome.value,
                                    transition_outcome.termination,
                                    transition_outcome.info);
  }

  inline void set_memory(const StateMemory &m) { this->_memory = m; }

protected:
  inline virtual TransitionOutcomePtr make_step(const Event &event) {
    return make_sample(this->_memory, event);
  }

  virtual TransitionOutcomePtr make_sample(const StateMemory &memory,
                                           const Event &event) = 0;
};

template <typename Tstate, typename Tobservation, typename Tevent,
          TransitionType TT, typename Tvalue, typename Tinfo,
          typename TstateDistribution = Distribution<Tstate>,
          typename TstateSpace = Space<Tstate>,
          typename TobservationSpace = Space<Tobservation>,
          typename TobservationDistribution = Distribution<Tobservation>,
          template <typename...> class TsmartPointer = std::unique_ptr>
class UncertainTransitionDomain
    : public SimulationDomain<Tstate, Tobservation, Tevent, TT, Tvalue, Tinfo,
                              TstateSpace, TobservationSpace,
                              TobservationDistribution, TsmartPointer> {
public:
  typedef Tevent Event;
  typedef Tstate State;
  typedef Memory<State> StateMemory;
  typedef TstateDistribution NextStateDistribution;
  typedef TsmartPointer<NextStateDistribution> NextStateDistributionPtr;
  typedef Tinfo Info;
  typedef Value<TT, Tvalue> TransitionValueReturn;
  typedef TransitionOutcome<State, TT, Tvalue, Info> TransitionOutcomeReturn;
  typedef TsmartPointer<TransitionOutcomeReturn> TransitionOutcomePtr;

  virtual NextStateDistributionPtr
  get_next_state_distribution(const StateMemory &memory,
                              const Event &event) = 0;
  virtual TransitionValueReturn
  get_transition_value(const StateMemory &memory, const Event &event,
                       const State &next_state) = 0;
  virtual bool is_terminal(const State &state) = 0;

protected:
  virtual TransitionOutcomePtr make_sample(const StateMemory &memory,
                                           const Event &event) {
    State next_state = get_next_state_distribution(memory, event).sample();
    TransitionValueReturn value =
        get_transition_value(memory, event, next_state);
    bool termination = is_terminal(next_state);
    return std::make_unique<TransitionOutcomeReturn>(next_state, value,
                                                     termination);
  }
};

template <typename Tstate, typename Tobservation, typename Tevent,
          TransitionType TT, typename Tvalue, typename Tinfo,
          typename TstateDistribution = DiscreteDistribution<Tstate>,
          typename TstateSpace = Space<Tstate>,
          typename TobservationSpace = Space<Tobservation>,
          typename TobservationDistribution = Distribution<Tobservation>,
          template <typename...> class TsmartPointer = std::unique_ptr>
class EnumerableTransitionDomain
    : public UncertainTransitionDomain<
          Tstate, Tobservation, Tevent, TT, Tvalue, Tinfo,
          Distribution<Tstate>, // Not the specialized
                                // DiscreteDistribution<Tstate> to allow for
                                // common base class recognition with multiple
                                // inheritance
          TstateSpace, TobservationSpace, TobservationDistribution,
          TsmartPointer> {
  static_assert(
      std::is_base_of<DiscreteDistribution<Tstate>, TstateDistribution>::value,
      "State distribution type must be derived from "
      "skdecide::DiscreteDistribution<Tstate>");

public:
  typedef Tevent Event;
  typedef Tstate State;
  typedef Memory<State> StateMemory;
  typedef Distribution<State> NextStateDistribution;
  typedef TsmartPointer<NextStateDistribution> NextStateDistributionPtr;

  virtual NextStateDistributionPtr
  get_next_state_distribution(const StateMemory &memory,
                              const Event &event) = 0;
};

template <typename Tstate, typename Tobservation, typename Tevent,
          TransitionType TT, typename Tvalue, typename Tinfo,
          typename TstateSpace = Space<Tstate>,
          typename TobservationSpace = Space<Tobservation>,
          typename TobservationDistribution = Distribution<Tobservation>,
          template <typename...> class TsmartPointer = std::unique_ptr>
class DeterministicTransitionDomain
    : public EnumerableTransitionDomain<
          Tstate, Tobservation, Tevent, TT, Tvalue, Tinfo,
          Distribution<Tstate>, // Not the specialized
                                // SingleValueDistribution<Tstate> to allow for
                                // common base class recognition with multiple
                                // inheritance
          TstateSpace, TobservationSpace, TobservationDistribution,
          TsmartPointer> {
public:
  typedef Tevent Event;
  typedef Tstate State;
  typedef TsmartPointer<State> StatePtr;
  typedef Memory<State> StateMemory;
  typedef Distribution<State> NextStateDistribution;
  typedef TsmartPointer<NextStateDistribution> NextStateDistributionPtr;

  inline virtual NextStateDistributionPtr
  get_next_state_distribution(const StateMemory &memory, const Event &event) {
    return std::make_unique<SingleValueDistribution<State>>(
        get_next_state(memory, event));
  }

  virtual StatePtr get_next_state(const StateMemory &memory,
                                  const Event &event) = 0;
};

} // namespace skdecide

#endif // SKDECIDE_DYNAMICS_HH
