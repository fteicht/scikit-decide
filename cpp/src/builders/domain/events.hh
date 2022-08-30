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
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class EventDomain {
private:
  template <class T, class D, class = void> struct define_event_type {
    template <class, class = void> struct check_event_domain;

    template <class DD>
    struct check_event_domain<
        DD, std::enable_if_t<std::is_same_v<EventDomain<CompoundDomain>, DD>>> {
      static_assert(!std::is_same_v<EventDomain<CompoundDomain>, DD>,
                    "The domain types must define AgentEvent when the "
                    "agent domain feature is skdecide::EventDomain");
    };

    template <class DD>
    struct check_event_domain<
        DD,
        std::enable_if_t<!std::is_same_v<EventDomain<CompoundDomain>, DD>>> {
      template <class TT, class = void> struct try_import_action_type {
        static_assert(
            std::is_same_v<EventDomain<CompoundDomain>, DD>,
            "The domain types must define either AgentEvent or AgentAction "
            "when "
            "the agent domain feature is different from skdecide::EventDomain");
      };

      template <class TT>
      struct try_import_action_type<TT, std::void_t<typename TT::AgentAction>> {
        typedef typename TT::AgentAction result;
      };

      typedef typename try_import_action_type<T>::result result;
    };

    typedef typename check_event_domain<D>::result result;
  };

  template <class T, class D>
  struct define_event_type<T, D, std::void_t<typename T::AgentEvent>> {
    template <class TT, class = void> struct try_import_action_type {
      typedef typename T::AgentEvent result;
    };

    template <class TT>
    struct try_import_action_type<TT, std::void_t<typename TT::AgentAction>> {
      static_assert(
          std::is_same_v<typename TT::AgentAction, typename T::AgentEvent>,
          "The domain types AgentEvent and AgentAction must be the same.");
      typedef typename TT::AgentAction result;
    };

    typedef typename try_import_action_type<T>::result result;
  };

public:
  /**
   * @brief Type of an agent's events
   */
  typedef typename define_event_type<
      typename CompoundDomain::Types,
      typename CompoundDomain::Features::template ActivityDomain<
          CompoundDomain>>::result AgentEvent;

  /**
   * @brief Type of an agent's actions
   */
  typedef AgentEvent AgentAction;

private:
  template <class T, class = void> struct define_action_space_type {
    template <typename... Args> using result = ImplicitSpace<Args...>;
  };

  template <class T>
  struct define_action_space_type<
      T, std::void_t<typename T::template AgentActionSpace<char>>> {
    template <typename... Args>
    using result = typename T::template AgentActionSpace<Args...>;
  };

public:
  /**
   * @brief Type of an agent's action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentActionSpace = typename define_action_space_type<
      typename CompoundDomain::Types>::template result<Args...>;

private:
  template <class T, class = void> struct define_event_space_type {
    template <typename... Args> using result = AgentActionSpace<Args...>;
  };

  template <class T>
  struct define_event_space_type<
      T, std::void_t<typename T::template AgentEventSpace<char>>> {
    template <typename... Args>
    using result = typename T::template AgentEventSpace<Args...>;
  };

public:
  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEventSpace = typename define_event_space_type<
      typename CompoundDomain::Types>::template result<Args...>;

private:
  template <class T, class = void> struct define_applicable_action_space_type {
    template <typename... Args> using result = AgentActionSpace<Args...>;
  };

  template <class T>
  struct define_applicable_action_space_type<
      T, std::void_t<typename T::template AgentApplicableActionSpace<char>>> {
    template <typename... Args>
    using result = typename T::template AgentApplicableActionSpace<Args...>;
  };

public:
  /**
   * @brief Type of an agent's applicable action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentApplicableActionSpace =
      typename define_applicable_action_space_type<
          typename CompoundDomain::Types>::template result<Args...>;

private:
  template <class T, class = void> struct define_enabled_event_space_type {
    template <typename... Args>
    using result = AgentApplicableActionSpace<Args...>;
  };

  template <class T>
  struct define_enabled_event_space_type<
      T, std::void_t<typename T::template AgentEnabledEventSpace<char>>> {
    template <typename... Args>
    using result = typename T::template AgentEnabledEventSpace<Args...>;
  };

public:
  /**
   * @brief Type of an agent's enabled event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEnabledEventSpace = typename define_enabled_event_space_type<
      typename CompoundDomain::Types>::template result<Args...>;

private:
  static_assert(
      std::is_base_of<HistoryDomain<CompoundDomain>,
                      typename CompoundDomain::Features::template MemoryDomain<
                          CompoundDomain>>::value,
      "The memory domain feature must be derived from "
      "skdecide::HistoryDomain<CompoundDomain>");
  static_assert(
      std::is_base_of<Space<AgentEvent>, AgentEventSpace<AgentEvent>>::value,
      "AgentEventSpace<...> must be derived from skdecide::Space<...>");
  static_assert(
      std::is_base_of<Space<AgentAction>, AgentActionSpace<AgentAction>>::value,
      "AgentActionSpace<...> must be derived from skdecide::Space<...>");
  static_assert(std::is_base_of<Space<AgentEvent>,
                                AgentEnabledEventSpace<AgentEvent>>::value,
                "AgentEnabledEventSpace<...> must be derived from "
                "skdecide::Space<...>");
  static_assert(std::is_base_of<Space<AgentAction>,
                                AgentApplicableActionSpace<AgentAction>>::value,
                "AgentApplicableActionSpace<...> must be derived from "
                "skdecide::Space<...>");

public:
  typedef typename CompoundDomain::Features::
      template AgentDomain<CompoundDomain>::template AgentProxy<
          typename CompoundDomain::Features::template ObservabilityDomain<
              CompoundDomain>::AgentState>
          CompoundState;
  typedef typename CompoundDomain::Features::template MemoryDomain<
      CompoundDomain>::template MemoryProxy<CompoundState>
      CompoundMemory;
  typedef typename CompoundDomain::Features::template AgentDomain<
      CompoundDomain>::template AgentProxy<AgentEvent>
      CompoundEvent;
  typedef typename CompoundDomain::Features::template AgentDomain<
      CompoundDomain>::template AgentProxy<AgentEventSpace<AgentEvent>>
      CompoundEventSpace;
  typedef std::unique_ptr<CompoundEventSpace> CompoundEventSpacePtr;
  typedef typename CompoundDomain::Features::template AgentDomain<
      CompoundDomain>::template AgentProxy<AgentEnabledEventSpace<AgentEvent>>
      CompoundEnabledEventSpace;
  typedef std::unique_ptr<CompoundEnabledEventSpace>
      CompoundEnabledEventSpacePtr;
  typedef typename CompoundDomain::Features::template AgentDomain<
      CompoundDomain>::template AgentProxy<AgentActionSpace<AgentAction>>
      CompoundActionSpace;
  typedef std::unique_ptr<CompoundActionSpace> CompoundActionSpacePtr;
  typedef
      typename CompoundDomain::Features::template AgentDomain<CompoundDomain>::
          template AgentProxy<AgentApplicableActionSpace<AgentAction>>
              CompoundApplicableActionSpace;
  typedef std::unique_ptr<CompoundApplicableActionSpace>
      CompoundApplicableActionSpacePtr;

  /**
   * @brief Get the (cached) domain event space (finite or infinite set).
   *
   * By default, EventDomain::get_event_space() internally calls
   * EventDomain::make_event_space() the first time and automatically caches
   * its value to make future calls more efficient (since the event space is
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
  inline virtual bool is_action(const AgentEvent &event) {
    return get_action_space().contains(event);
  }

  /**
   * @brief Get the space (finite or infinite set) of applicable actions in
   * the given memory (state or history).
   *
   * @param memory The memory to consider.
   * @return The space of applicable actions.
   */
  virtual CompoundApplicableActionSpacePtr
  get_applicable_actions(const CompoundMemory &memory) = 0;

  /**
   * @brief Get the space (finite or infinite set) of applicable actions in
   * the internal memory (state or history).
   *
   * @return The space of applicable actions.
   */
  inline CompoundApplicableActionSpacePtr get_applicable_actions() {
    return get_applicable_actions(this->_memory);
  }

  /**
   * @brief Indicate whether an action is applicable in the given memory
   * (state or history).
   *
   * @param event The event to consider.
   * @param memory The memory to consider.
   * @return True if the action is applicable (False otherwise).
   */
  inline bool is_applicable_action(const CompoundEvent &event,
                                   const CompoundMemory &memory) {
    return get_applicable_actions(memory)->contains(event);
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
   * EventDomain::get_event_space(), the difference being that the result is
   * not cached here.
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
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain>
class ActionDomain : public EventDomain<CompoundDomain> {
private:
  template <class T, class = void> struct define_action_type {
    static_assert(std::is_void_v<T>,
                  "The domain type must define AgentAction when the event "
                  "domain feature is skdecide::ActionDomain");
  };

  template <class T>
  struct define_action_type<T, std::void_t<typename T::AgentAction>> {
    template <class TT, class = void> struct try_import_event_type {
      typedef typename T::AgentAction result;
    };

    template <class TT>
    struct try_import_event_type<TT, std::void_t<typename TT::AgentEvent>> {
      static_assert(
          std::is_same_v<typename TT::AgentEvent, typename T::AgentAction>,
          "The domain types AgentEvent and AgentAction must be the same.");
      typedef typename TT::AgentEvent result;
    };

    typedef typename try_import_event_type<T>::result result;
  };

public:
  /**
   * @brief Type of an agent's actions
   */
  typedef typename define_action_type<typename CompoundDomain::Types>::result
      AgentAction;

  /**
   * @brief Type of an agent's events
   */
  typedef AgentAction AgentEvent;

public:
  /**
   * @brief Type of an agent's action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentActionSpace =
      typename EventDomain<CompoundDomain>::template AgentActionSpace<Args...>;

private:
  template <class, class = void>
  struct check_event_space_type : std::true_type {};

  template <class T>
  struct check_event_space_type<
      T, std::void_t<typename T::template AgentEventSpace<char>>>
      : std::conditional<
            std::is_same_v<AgentActionSpace<char>,
                           typename T::template AgentEventSpace<char>>,
            std::true_type, std::false_type>::type {};

  static_assert(
      check_event_space_type<typename CompoundDomain::Types>::value,
      "The domain type AgentEventSpace must be equal to AgentActionSpace "
      "when the event domain feature derives from skdecide::ActionDomain");

public:
  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args> using AgentEventSpace = AgentActionSpace<Args...>;

  /**
   * @brief Type of an agent's applicable action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentApplicableActionSpace = typename EventDomain<
      CompoundDomain>::template AgentApplicableActionSpace<Args...>;

private:
  template <class, class = void>
  struct check_enabled_event_space_type : std::true_type {};

  template <class T>
  struct check_enabled_event_space_type<
      T, std::void_t<typename T::template AgentEnabledEventSpace<char>>>
      : std::conditional<
            std::is_same_v<AgentApplicableActionSpace<char>,
                           typename T::template AgentEnabledEventSpace<char>>,
            std::true_type, std::false_type>::type {};

  static_assert(
      check_enabled_event_space_type<typename CompoundDomain::Types>::value,
      "The domain type AgentEnabledEventSpace must be equal to "
      "AgentApplicableActionSpace when the event domain feature derives from "
      "skdecide::ActionDomain");

public:
  /**
   * @brief Type of an agent's enabled event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEnabledEventSpace = AgentApplicableActionSpace<Args...>;

public:
  typedef typename EventDomain<CompoundDomain>::CompoundState CompoundState;
  typedef typename EventDomain<CompoundDomain>::CompoundMemory CompoundMemory;
  typedef typename CompoundDomain::Features::template AgentDomain<
      CompoundDomain>::template AgentProxy<AgentActionSpace<AgentAction>>
      CompoundActionSpace;
  typedef std::unique_ptr<CompoundActionSpace> CompoundActionSpacePtr;
  typedef
      typename CompoundDomain::Features::template AgentDomain<CompoundDomain>::
          template AgentProxy<AgentApplicableActionSpace<AgentAction>>
              CompoundApplicableActionSpace;
  typedef std::unique_ptr<CompoundApplicableActionSpace>
      CompoundApplicableActionSpacePtr;

  /**
   * @brief Get the (cached) domain event space (finite or infinite set).
   *
   * By default, EventDomain::get_event_space() internally calls
   * EventDomain::make_event_space() the first time and automatically caches
   * its value to make future calls more efficient (since the event space is
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
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain>
class UnrestrictedActionDomain : public ActionDomain<CompoundDomain> {
public:
  /**
   * @brief Type of an agent's actions
   */
  typedef typename ActionDomain<CompoundDomain>::AgentAction AgentAction;

  /**
   * @brief Type of an agent's events
   */
  typedef typename ActionDomain<CompoundDomain>::AgentEvent AgentEvent;

  /**
   * @brief Type of an agent's action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentActionSpace =
      typename ActionDomain<CompoundDomain>::template AgentActionSpace<Args...>;

private:
  template <class, class = void>
  struct check_applicable_action_space_type : std::true_type {};

  template <class T>
  struct check_applicable_action_space_type<
      T, std::void_t<typename T::template AgentApplicableActionSpace<char>>>
      : std::conditional<
            std::is_same_v<
                AgentActionSpace<char>,
                typename T::template AgentApplicableActionSpace<char>>,
            std::true_type, std::false_type>::type {};

  static_assert(
      check_applicable_action_space_type<typename CompoundDomain::Types>::value,
      "The domain type AgentAplicableActionSpace must be equal to "
      "AgentActionSpace when the event domain feature derives from "
      "skdecide::UnrestrictedActionDomain");

public:
  /**
   * @brief Type of an agent's applicable action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentApplicableActionSpace = AgentActionSpace<Args...>;

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args> using AgentEventSpace = AgentActionSpace<Args...>;

  /**
   * @brief Type of an agent's enabled event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEnabledEventSpace = AgentActionSpace<Args...>;

public:
  typedef typename ActionDomain<CompoundDomain>::CompoundState CompoundState;
  typedef typename ActionDomain<CompoundDomain>::CompoundMemory CompoundMemory;
  typedef typename ActionDomain<CompoundDomain>::CompoundActionSpace
      CompoundActionSpace;
  typedef std::unique_ptr<CompoundActionSpace> CompoundActionSpacePtr;

  /**
   * @brief Get the space (finite or infinite set) of applicable actions in
   * the given memory (state or history).
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
