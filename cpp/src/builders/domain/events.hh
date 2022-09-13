/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_EVENTS_HH
#define SKDECIDE_EVENTS_HH

#include <cstddef>
#include <memory>
#include <optional>
#include <type_traits>
#include "core.hh"
#include "memory.hh"
#include "domain_type_importer.hh"

namespace skdecide {

template <typename CompoundDomain, template <typename...> class CallingFeature,
          template <typename...> class DerivedFeature = CallingFeature>
class ActivityTypesImporter {
public:
  DOMAIN_SIMPLE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                              event, AgentEvent);

  DOMAIN_SIMPLE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                              action, AgentAction);

  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                event_space, AgentEventSpace);

  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                action_space, AgentActionSpace);

  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                enabled_event_space, AgentEnabledEventSpace);

  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                applicable_action_space,
                                AgentApplicableActionSpace);
};

/**
 * @brief A domain must inherit this class if it handles events (controllable
 * or not not by the agents).
 *
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class EventDomain {
public:
  /**
   * @brief Type of an agent's events
   */
  typedef typename ActivityTypesImporter<
      CompoundDomain, EventDomain,
      CompoundDomain::Features::template ActivityDomain>::
      template import_event_type<>::result AgentEvent;

  // typedef typename IMPORT_DOMAIN_TYPE(NAME, DEFAULT_TYPE)

  static_assert(!std::is_same_v<AgentEvent, std::nullptr_t> &&
                    !std::is_same_v<AgentEvent, std::nullopt_t>,
                "The domain types must define AgentEvent when the "
                "agent domain feature is skdecide::EventDomain");

  /**
   * @brief Type of an agent's actions
   */
  typedef typename ActivityTypesImporter<
      CompoundDomain, EventDomain,
      CompoundDomain::Features::template ActivityDomain>::
      template import_action_type<AgentEvent>::result AgentAction;

  static_assert(std::is_same_v<AgentEvent, AgentAction>,
                "The domain types AgentEvent and AgentAction must be the "
                "same when they are both defined.");

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEventSpace = typename ActivityTypesImporter<
      CompoundDomain, EventDomain,
      CompoundDomain::Features::template ActivityDomain>::
      template import_event_space_type<ImplicitSpace>::template test_with<
          AgentEvent>::template result<Args...>;

  /**
   * @brief Type of an agent's action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentActionSpace = typename ActivityTypesImporter<
      CompoundDomain, EventDomain,
      CompoundDomain::Features::template ActivityDomain>::
      template import_action_space_type<ImplicitSpace>::template test_with<
          AgentAction>::template result<Args...>;

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEnabledEventSpace = typename ActivityTypesImporter<
      CompoundDomain, EventDomain,
      CompoundDomain::Features::template ActivityDomain>::
      template import_enabled_event_space_type<ImplicitSpace>::
          template test_with<AgentEvent>::template result<Args...>;

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentApplicableActionSpace = typename ActivityTypesImporter<
      CompoundDomain, EventDomain,
      CompoundDomain::Features::template ActivityDomain>::
      template import_applicable_action_space_type<ImplicitSpace>::
          template test_with<AgentAction>::template result<Args...>;

private:
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
  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature {
  public:
    typedef EventDomain<CompoundDomain> FeatureDomain;

  private:
    static_assert(
        std::is_base_of<HistoryDomain<CompoundDomain>,
                        typename CompoundDomain::Features::
                            template MemoryDomain<CompoundDomain>>::value,
        "The memory domain feature must derive from "
        "skdecide::HistoryDomain<...> when using activity domains");

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
    typedef typename CompoundDomain::Features::
        template AgentDomain<CompoundDomain>::template AgentProxy<
            AgentApplicableActionSpace<AgentAction>>
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
     * EventDomain::make_action_space() the first time and automatically
     * caches its value to make future calls more efficient (since the action
     * space is assumed to be constant).
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
     * @brief Indicate whether an event is an action (i.e. a controllable
     * event for the agents).
     *
     * !!! Tip.
     *        By default, this function is implemented using the
     * skdecide::Space::contains() function on the domain action space
     * provided by EventDomain::get_action_space(), but it can be overridden
     * for faster implementations.
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
     * EventDomain::get_action_space(), the difference being that the result
     * is not cached here.
     *
     * @return The action space.
     */
    virtual CompoundActionSpacePtr make_action_space() = 0;

  private:
    CompoundEventSpacePtr _event_space;
    CompoundActionSpacePtr _action_space;
  };
};

/**
 * @brief A domain must inherit this class if it handles only actions (i.e.
 * controllable events).
 *
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class ActionDomain {
public:
  /**
   * @brief Type of an agent's actions
   */
  typedef typename ActivityTypesImporter<CompoundDomain, ActionDomain>::
      template import_action_type<>::result AgentAction;

  static_assert(!std::is_same_v<AgentAction, std::nullptr_t> &&
                    !std::is_same_v<AgentAction, std::nullopt_t>,
                "The domain types must define AgentAction when the "
                "agent domain feature derives from skdecide::EventDomain");

  /**
   * @brief Type of an agent's events
   */
  typedef typename ActivityTypesImporter<CompoundDomain, ActionDomain>::
      template import_event_type<AgentAction>::result AgentEvent;

  static_assert(std::is_same_v<AgentEvent, AgentAction>,
                "The domain types AgentEvent and AgentAction must be the "
                "same when they are both defined.");

  /**
   * @brief Type of an agent's action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentActionSpace =
      typename ActivityTypesImporter<CompoundDomain, ActionDomain>::
          template import_action_space_type<ImplicitSpace>::template test_with<
              AgentAction>::template result<Args...>;

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEventSpace =
      typename ActivityTypesImporter<CompoundDomain, ActionDomain>::
          template import_event_space_type<AgentActionSpace>::
              template test_with<AgentEvent>::template result<Args...>;

  static_assert(
      std::is_same_v<AgentActionSpace<char>, AgentEventSpace<char>>,
      "AgentActionSpace<...> and AgentEventSpace<...> must be the same when "
      "the activity domain feature derives from ActionDomain.");

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentApplicableActionSpace = typename ActivityTypesImporter<
      CompoundDomain, ActionDomain,
      CompoundDomain::Features::template ActivityDomain>::
      template import_applicable_action_space_type<ImplicitSpace>::
          template test_with<AgentAction>::template result<Args...>;

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEnabledEventSpace =
      typename ActivityTypesImporter<CompoundDomain, ActionDomain>::
          template import_enabled_event_space_type<AgentApplicableActionSpace>::
              template test_with<AgentEvent>::template result<Args...>;

  static_assert(
      std::is_same_v<AgentActionSpace<char>, AgentEventSpace<char>>,
      "AgentApplicableActionSpace<...> and AgentEnabledEventSpace<...> must "
      "be "
      "the same when the activity domain feature derives from ActionDomain.");

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature : public EventDomain<CompoundDomain>::Feature {
  public:
    typedef ActionDomain<CompoundDomain> FeatureDomain;

    typedef typename EventDomain<CompoundDomain>::Feature::CompoundMemory
        CompoundMemory;
    typedef typename CompoundDomain::Features::template AgentDomain<
        CompoundDomain>::template AgentProxy<AgentActionSpace<AgentAction>>
        CompoundActionSpace;
    typedef std::unique_ptr<CompoundActionSpace> CompoundActionSpacePtr;
    typedef typename CompoundDomain::Features::
        template AgentDomain<CompoundDomain>::template AgentProxy<
            AgentApplicableActionSpace<AgentAction>>
            CompoundApplicableActionSpace;
    typedef std::unique_ptr<CompoundApplicableActionSpace>
        CompoundApplicableActionSpacePtr;

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
    virtual CompoundActionSpacePtr make_event_space() {
      return this->make_action_space();
    }
  };
};

/**
 * @brief A domain must inherit this class if it handles only actions (i.e.
 controllable events), which are always all applicable.
 *
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class UnrestrictedActionDomain {
public:
  /**
   * @brief Type of an agent's actions
   */
  typedef
      typename ActivityTypesImporter<CompoundDomain, UnrestrictedActionDomain>::
          template import_action_type<>::result AgentAction;

  /**
   * @brief Type of an agent's events
   */
  typedef
      typename ActivityTypesImporter<CompoundDomain, UnrestrictedActionDomain>::
          template import_event_type<AgentAction>::result AgentEvent;

  /**
   * @brief Type of an agent's action space
   * @tparam Args Type of actions
   */
  template <typename... Args>
  using AgentActionSpace =
      typename ActivityTypesImporter<CompoundDomain, UnrestrictedActionDomain>::
          template import_action_space_type<ImplicitSpace>::template test_with<
              AgentAction>::template result<Args...>;

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEventSpace =
      typename ActivityTypesImporter<CompoundDomain, UnrestrictedActionDomain>::
          template import_event_space_type<AgentActionSpace>::
              template test_with<AgentEvent>::template result<Args...>;

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentApplicableActionSpace =
      typename ActivityTypesImporter<CompoundDomain, UnrestrictedActionDomain>::
          template import_applicable_action_space_type<AgentActionSpace>::
              template test_with<AgentAction>::template result<Args...>;

  static_assert(
      std::is_same_v<AgentActionSpace<char>, AgentApplicableActionSpace<char>>,
      "AgentApplicableActionSpace<...> and AgentActionSpace<...> must be the "
      "same when the activity domain feature derives from "
      "UnrestrictedActionDomain.");

  /**
   * @brief Type of an agent's event space
   * @tparam Args Type of events
   */
  template <typename... Args>
  using AgentEnabledEventSpace =
      typename ActivityTypesImporter<CompoundDomain, UnrestrictedActionDomain>::
          template import_enabled_event_space_type<AgentApplicableActionSpace>::
              template test_with<AgentEvent>::template result<Args...>;

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature : public ActionDomain<CompoundDomain>::Feature {
  public:
    typedef UnrestrictedActionDomain<CompoundDomain> FeatureDomain;

    typedef typename ActionDomain<CompoundDomain>::Feature::CompoundState
        CompoundState;
    typedef typename ActionDomain<CompoundDomain>::Feature::CompoundMemory
        CompoundMemory;
    typedef typename ActionDomain<CompoundDomain>::Feature::CompoundActionSpace
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
};

} // namespace skdecide

#endif // SKDECIDE_EVENTS_HH
