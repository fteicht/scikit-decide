/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_DOMAINS_HH
#define SKDECIDE_DOMAINS_HH

#include "builders/domain/agent.hh"
#include "builders/domain/concurrency.hh"
#include "builders/domain/dynamics.hh"
#include "builders/domain/events.hh"
#include "builders/domain/initialization.hh"
#include "builders/domain/memory.hh"
#include "builders/domain/observability.hh"
#include "builders/domain/renderability.hh"
#include "builders/domain/value.hh"
#include "core.hh"
#include "utils/python_domain_proxy.hh"
#include <cstddef>
#include <type_traits>

namespace skdecide {

template <
    template <typename...> class AgentDomainFeature = MultiAgentDomain,
    template <typename...> class ConcurrencyDomainFeature = ParallelDomain,
    template <typename...> class ConstrainedDomainFeature = std::void_t,
    template <typename...> class DynamicsDomainFeature = EnvironmentDomain,
    template <typename...> class EventDomainFeature = EventDomain,
    template <typename...> class GoalDomainFeature = std::void_t,
    template <typename...> class InitializationDomainFeature = std::void_t,
    template <typename...> class MemoryDomainFeature = HistoryDomain,
    template <typename...> class ObservabilityDomainFeature =
        PartiallyObservableDomain,
    template <typename...> class RenderabilityDomainFeature = std::void_t,
    template <typename...> class ValueDomainFeature = RewardDomain,
    typename AgentType = int, typename Event = int, typename State = int,
    typename Observation = State, typename Reward = double,
    template <typename...> class AgentDict = std::unordered_map,
    template <typename...> class EventList = std::list,
    template <typename...> class EventSpace = ImplicitSpace,
    template <typename...> class ActionSpace = EventSpace,
    template <typename...> class EnabledEventSpace = EventSpace,
    template <typename...> class ApplicableActionSpace = ActionSpace,
    template <typename...> class StateSpace = ImplicitSpace,
    template <typename...> class ObservationSpace = ImplicitSpace,
    template <typename...> class ObservationDistribution =
        SingleValueDistribution>
class HelperDomain {
public:
  static_assert(
      std::is_base_of<MultiAgentDomain<HelperDomain>,
                      AgentDomainFeature<HelperDomain>>::value,
      "MultiAgentDomain<...> must be a base class of AgentDomainFeature<...>");
  typedef std::conditional_t<
      std::is_same_v<AgentDomainFeature<HelperDomain>,
                     MultiAgentDomain<HelperDomain>>,
      MultiAgentDomain<HelperDomain, AgentType, AgentDict>,
      std::conditional_t<std::is_same_v<AgentDomainFeature<HelperDomain>,
                                        SingleAgentDomain<HelperDomain>>,
                         SingleAgentDomain<HelperDomain>,
                         AgentDomainFeature<HelperDomain>>>
      FinalMultiAgentDomain;
};

/**
 * @brief his is the highest level domain class (inheriting top-level class for
 * each mandatory domain characteristic). This helper class can be used as the
 * main base class for domains.
 * @tparam AgentType Type of agent identifiers
 * @tparam Event The type of events for one agent
 * @tparam State Type of an agent's states
 * @tparam Observation Type of an agent's observations
 * @tparam Reward Type of an agent's rewards
 * @tparam AgentDict Type of the dictionary class (std::unordered_map by
 * default)
 * @tparam EventList Type of the sequence class used to store objects run in
 * parallel (std::list by default)
 * @tparam EventSpace The type of event spaces
 * @tparam ActionSpace The type of action spaces
 * @tparam EnabledEventSpace The type of enabled event spaces
 * @tparam ApplicableActionSpace The of applicable action spaces
 * @tparam StateSpace Type of an agent's state space
 * @tparam ObservationSpace Type of an agent's observation space
 * @tparam ObservationDistribution Type of observation distributions
 * @tparam SPointer Type of smart pointers
 */
template <typename AgentType, typename Event, typename State,
          typename Observation, typename Reward,
          template <typename...> class AgentDict = std::unordered_map,
          template <typename...> class EventList = std::list,
          template <typename...> class EventSpace = ImplicitSpace,
          template <typename...> class ActionSpace = EventSpace,
          template <typename...> class EnabledEventSpace = EventSpace,
          template <typename...> class ApplicableActionSpace = ActionSpace,
          template <typename...> class StateSpace = ImplicitSpace,
          template <typename...> class ObservationSpace = ImplicitSpace,
          template <typename...> class ObservationDistribution =
              ImplicitDistribution,
          template <typename...> class SPointer = std::unique_ptr>
class Domain
    : public MultiAgentDomain<
          Domain<AgentType, Event, State, Observation, Reward, AgentDict,
                 EventList, EventSpace, ActionSpace, EnabledEventSpace,
                 ApplicableActionSpace, StateSpace, ObservationSpace,
                 ObservationDistribution>,
          AgentType, AgentDict>,
      public ParallelDomain<
          Domain<AgentType, Event, State, Observation, Reward, AgentDict,
                 EventList, EventSpace, ActionSpace, EnabledEventSpace,
                 ApplicableActionSpace, StateSpace, ObservationSpace,
                 ObservationDistribution>,
          EventList>,
      public EnvironmentDomain<Domain<
          AgentType, Event, State, Observation, Reward, AgentDict, EventList,
          EventSpace, ActionSpace, EnabledEventSpace, ApplicableActionSpace,
          StateSpace, ObservationSpace, ObservationDistribution>>,
      public EventDomain<
          Domain<AgentType, Event, State, Observation, Reward, AgentDict,
                 EventList, EventSpace, ActionSpace, EnabledEventSpace,
                 ApplicableActionSpace, StateSpace, ObservationSpace,
                 ObservationDistribution>,
          Event, EventSpace, ActionSpace, EnabledEventSpace,
          ApplicableActionSpace>,
      public HistoryDomain<Domain<
          AgentType, Event, State, Observation, Reward, AgentDict, EventList,
          EventSpace, ActionSpace, EnabledEventSpace, ApplicableActionSpace,
          StateSpace, ObservationSpace, ObservationDistribution>>,
      public PartiallyObservableDomain<
          Domain<AgentType, Event, State, Observation, Reward, AgentDict,
                 EventList, EventSpace, ActionSpace, EnabledEventSpace,
                 ApplicableActionSpace, StateSpace, ObservationSpace,
                 ObservationDistribution>,
          State, Observation, StateSpace, ObservationSpace,
          ObservationDistribution>,
      public RewardDomain<
          Domain<AgentType, Event, State, Observation, Reward, AgentDict,
                 EventList, EventSpace, ActionSpace, EnabledEventSpace,
                 ApplicableActionSpace, StateSpace, ObservationSpace,
                 ObservationDistribution>,
          Reward> {
public:
  template <typename... T> using SmartPointer = SPointer<T...>;
};

} // namespace skdecide

#endif // SKDECIDE_DOMAINS_HH
