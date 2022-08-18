/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_EVENTS_HH
#define SKDECIDE_EVENTS_HH

#include <memory>
#include <type_traits>
#include "core.hh"
#include "memory.hh"

namespace skdecide {

/**
 * @brief A domain must inherit this class if it handles events (controllable or
 * not not by the agents).
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam Event The type of events for one agent
 * @tparam EventSpace The type of event spaces
 * @tparam ActionSpace The type of action spaces
 * @tparam EnabledEventSpace The type of enabled event spaces
 * @tparam ApplicableActionSpace The of applicable action spaces
 */
template <typename DerivedCompoundDomain, typename Event,
          template <typename...> class EventSpace = ImplicitSpace,
          template <typename...> class ActionSpace = EventSpace,
          template <typename...> class EnabledEventSpace = EventSpace,
          template <typename...> class ApplicableActionSpace = ActionSpace>
class EventDomain : public virtual HistoryDomain<DerivedCompoundDomain> {
  static_assert(std::is_base_of<Space<Event>, EventSpace<Event>>::value,
                "Event space type must be derived from skdecide::Space<...>");
  static_assert(std::is_base_of<Space<Event>, ActionSpace<Event>>::value,
                "Action space type must be derived from skdecide::Space<...>");
  static_assert(
      std::is_base_of<Space<Event>, EnabledEventSpace<Event>>::value,
      "Enabled event space type must be derived from skdecide::Space<...>");
  static_assert(
      std::is_base_of<Space<Event>, ApplicableActionSpace<Event>>::value,
      "Applicable action space type must be derived from "
      "skdecide::Space<...>");

public:
  typedef Event RawEvent;
  template <typename... T> using RawEventSpace = EventSpace<T...>;
  template <typename... T> using RawActionSpace = ActionSpace<T...>;
  template <typename... T> using RawEnabledEventSpace = EnabledEventSpace<T...>;
  template <typename... T>
  using RawApplicableActionSpace = ApplicableActionSpace<T...>;

  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawState>
      CompoundState;
  typedef typename DerivedCompoundDomain::template MemoryProxy<CompoundState>
      CompoundMemory;
  typedef typename DerivedCompoundDomain::template AgentProxy<RawEvent>
      CompoundEvent;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawEventSpace<RawEvent>>
      CompoundEventSpace;
  typedef
      typename DerivedCompoundDomain::template SmartPointer<CompoundEventSpace>
          CompoundEventSpacePtr;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawEnabledEventSpace<RawEvent>>
      CompoundEnabledEventSpace;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      CompoundEnabledEventSpace>
      CompoundEnabledEventSpacePtr;
  typedef RawEvent RawAction;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawActionSpace<RawAction>>
      CompoundActionSpace;
  typedef
      typename DerivedCompoundDomain::template SmartPointer<CompoundActionSpace>
          CompoundActionSpacePtr;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawApplicableActionSpace<RawAction>>
      CompoundApplicableActionSpace;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      CompoundApplicableActionSpace>
      CompoundApplicableActionSpacePtr;

  /**
   * @brief Get the (cached) domain event space (finite or infinite set).
   *
   * By default, EventDomain::get_event_space() internally calls
   * EventDomain::make_event_space() the first time and automatically caches its
   * value to make future calls more efficient (since the event space is
   * assumed to be constant).
   *
   * @return The event space.
   */
  const CompoundEventSpace &get_event_space() {
    if (!_event_space) {
      _event_space = make_event_space();
    }
    return *_event_space;
  }

  /**
   * @brief Get the space (finite or infinite set) of enabled uncontrollable
   * events in the given memory (state or history).
   *
   * @param memory The memory to consider.
   * @return The space of enabled events.
   */
  virtual CompoundEnabledEventSpacePtr
  get_enabled_events(const CompoundMemory &memory) = 0;

  /**
   * @brief Get the space (finite or infinite set) of enabled uncontrollable
   * events in the the internal memory (state or history).
   *
   * @return The space of enabled events.
   */
  inline CompoundEnabledEventSpacePtr get_enabled_events() {
    return get_enabled_events(this->_memory);
  }

  /**
   * @brief Indicate whether an uncontrollable event is enabled in the given
   * memory (state or history).
   *
   * @param event The event to consider.
   * @param memory The memory to consider.
   * @return True if the event is enabled (False otherwise).
   */
  inline bool is_enabled_event(const CompoundEvent &event,
                               const CompoundMemory &memory) {
    return get_enabled_events(memory).contains(event);
  }

  /**
   * @brief Indicate whether an uncontrollable event is enabled in the given
   * internal memory (state or history).
   *
   * @param event The event to consider.
   * @return True if the event is enabled (False otherwise).
   */
  inline bool is_enabled_event(const CompoundEvent &event) {
    return is_enabled_event(event, this->_memory);
  }

  /**
   * @brief Get the (cached) domain action space (finite or infinite set).
   *
   * By default, EventDomain::get_action_space() internally calls
   * EventDomain::make_action_space() the first time and automatically caches
   * its value to make future calls more efficient (since the action space is
   * assumed to be constant).
   *
   * @return The action space.
   */
  const CompoundActionSpace &get_action_space() {
    if (!_action_space) {
      _action_space = make_action_space();
    }
    return *_action_space;
  }

  /**
   * @brief Indicate whether an event is an action (i.e. a controllable event
   * for the agents).
   *
   * !!! Tip.
   *        By default, this function is implemented using the
   * skdecide::Space::contains() function on the domain action space provided
   * by EventDomain::get_action_space(), but it can be overridden for faster
   * implementations.
   *
   * @param event The event to consider.
   * @return True if the event is an action (False otherwise).
   */
  inline virtual bool is_action(const RawEvent &event) {
    return get_action_space().contains(event);
  }

  /**
   * @brief Get the space (finite or infinite set) of applicable actions in the
   * given memory (state or history).
   *
   * @param memory The memory to consider.
   * @return The space of applicable actions.
   */
  virtual CompoundApplicableActionSpacePtr
  get_applicable_actions(const CompoundMemory &memory) = 0;

  /**
   * @brief Get the space (finite or infinite set) of applicable actions in the
   * internal memory (state or history).
   *
   * @return The space of applicable actions.
   */
  inline CompoundApplicableActionSpacePtr get_applicable_actions() {
    return get_applicable_actions(this->_memory);
  }

  /**
   * @brief Indicate whether an action is applicable in the given memory (state
   * or history).
   *
   * @param event The event to consider.
   * @param memory The memory to consider.
   * @return True if the action is applicable (False otherwise).
   */
  inline bool is_applicable_action(const CompoundEvent &event,
                                   const CompoundMemory &memory) {
    return get_applicable_actions(memory).contains(event);
  }

  /**
   * @brief Indicate whether an action is applicable in the internal memory
   * (state or history).
   *
   * @param event The event to consider.
   * @return True if the action is applicable (False otherwise).
   */
  inline bool is_applicable_action(const CompoundEvent &event) {
    return is_applicable_action(event, this->_memory);
  }

protected:
  /**
   * @brief Get the domain event space (finite or infinite set).
   *
   *    This is a helper function called by default from
   * EventDomain::get_event_space(), the difference being that the result is not
   * cached here.
   *
   * @return The event space.
   */
  virtual CompoundEventSpacePtr make_event_space() = 0;

  /**
   * @brief Get the domain action space (finite or infinite set).
   *
   *    This is a helper function called by default from
   * EventDomain::get_action_space(), the difference being that the result is
   * not cached here.
   *
   * @return The action space.
   */
  virtual CompoundActionSpacePtr make_action_space() = 0;

private:
  CompoundEventSpacePtr _event_space;
  CompoundActionSpacePtr _action_space;
};

/**
 * @brief A domain must inherit this class if it handles only actions (i.e.
 * controllable events).
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam Action The type of actions for one agent
 * @tparam ActionSpace The type of action spaces
 * @tparam ApplicableActionSpace The of applicable action spaces
 */
template <typename DerivedCompoundDomain, typename Action,
          template <typename...> class ActionSpace = ImplicitSpace,
          template <typename...> class ApplicableActionSpace = ActionSpace>
class ActionDomain
    : public EventDomain<DerivedCompoundDomain, Action, ActionSpace,
                         ActionSpace, ApplicableActionSpace,
                         ApplicableActionSpace> {
public:
  typedef Action RawAction;
  template <typename... T> using RawActionSpace = ActionSpace<T...>;
  template <typename... T>
  using RawApplicableActionSpace = ApplicableActionSpace<T...>;

  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawState>
      CompoundState;
  typedef typename DerivedCompoundDomain::template MemoryProxy<CompoundState>
      CompoundMemory;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawActionSpace<RawAction>>
      CompoundActionSpace;
  typedef
      typename DerivedCompoundDomain::template SmartPointer<CompoundActionSpace>
          CompoundActionSpacePtr;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawApplicableActionSpace<RawAction>>
      CompoundApplicableActionSpace;
  typedef typename DerivedCompoundDomain::template SmartPointer<
      CompoundApplicableActionSpace>
      CompoundApplicableActionSpacePtr;

  /**
   * @brief Get the (cached) domain event space (finite or infinite set).
   *
   * By default, EventDomain::get_event_space() internally calls
   * EventDomain::make_event_space() the first time and automatically caches its
   * value to make future calls more efficient (since the event space is
   * assumed to be constant).
   *
   * @return The event space.
   */
  inline virtual const CompoundActionSpace &get_event_space() {
    return this->get_action_space();
  }

  /**
   * @brief Get the space (finite or infinite set) of enabled uncontrollable
   * events in the given memory (state or history).
   *
   * @param memory The memory to consider.
   * @return The space of enabled events.
   */
  inline virtual CompoundApplicableActionSpacePtr
  get_enabled_events(const CompoundMemory &memory) {
    return this->get_enabled_actions(memory);
  }
};

/**
 * @brief A domain must inherit this class if it handles only actions (i.e.
 controllable events), which are always all applicable.
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam Action The type of actions for one agent
 * @tparam ActionSpace The type of action spaces
 */
template <typename DerivedCompoundDomain, typename Action,
          template <typename...> class ActionSpace = ImplicitSpace>
class UnrestrictedActionDomain
    : public ActionDomain<DerivedCompoundDomain, Action, ActionSpace,
                          ActionSpace> {
public:
  typedef Action RawAction;
  template <typename... T> using RawActionSpace = ActionSpace<T...>;

  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawState>
      CompoundState;
  typedef typename DerivedCompoundDomain::template MemoryProxy<CompoundState>
      CompoundMemory;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      RawActionSpace<RawAction>>
      CompoundActionSpace;
  typedef
      typename DerivedCompoundDomain::template SmartPointer<CompoundActionSpace>
          CompoundActionSpacePtr;

  /**
   * @brief Get the space (finite or infinite set) of applicable actions in the
   * given memory (state or history).
   *
   * @param memory The memory to consider.
   * @return The space of applicable actions.
   */
  inline virtual CompoundActionSpacePtr
  get_applicable_actions(const CompoundMemory &memory) {
    return this->get_action_space();
  }
};

} // namespace skdecide

#endif // SKDECIDE_EVENTS_HH
