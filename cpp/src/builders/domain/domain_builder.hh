/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_DOMAIN_BUILDER_HH
#define SKDECIDE_DOMAIN_BUILDER_HH

#include <type_traits>

#include "agent.hh"
#include "concurrency.hh"
#include "constraints.hh"
#include "dynamics.hh"
#include "events.hh"
#include "goals.hh"
#include "initialization.hh"
#include "memory.hh"
#include "observability.hh"
#include "renderability.hh"
#include "value.hh"

namespace skdecide {

template <typename DomainTypes, typename DomainFeatures> class DomainBuilder {
public:
  typedef DomainTypes Types;
  typedef DomainFeatures Features;
  typedef DomainBuilder<Types, DomainFeatures> Builder;

  template <typename F>
  using required_agent_domain_feature =
      typename F::template AgentDomain<Builder>;

  template <typename F>
  using required_concurrency_domain_feature =
      typename F::template ConcurrencyDomain<Builder>;

  template <typename F>
  using required_dynamics_domain_feature =
      typename F::template DynamicsDomain<Builder>;

  template <typename F>
  using required_activity_domain_feature =
      typename F::template ActivityDomain<Builder>;

  template <typename F>
  using required_history_domain_feature =
      typename F::template HistoryDomain<Builder>;

  template <typename F>
  using required_observability_domain_feature =
      typename F::template ObservabilityDomain<Builder>;

  template <typename F>
  using required_value_domain_feature =
      typename F::template ValueDomain<Builder>;

  template <typename T> struct empty_feature {};

  template <typename F, typename = void>
  struct optional_constrained_domain_feature
      : public empty_feature<ConstrainedDomain<Builder>> {};

  template <typename F>
  struct optional_constrained_domain_feature<
      F, std::void_t<typename F::template ConstrainedDomain<Builder>>>
      : public F::template ConstrainedDomain<Builder> {};

  template <typename F, typename = void>
  struct optional_goal_domain_feature
      : public empty_feature<GoalDomain<Builder>> {};

  template <typename F>
  struct optional_goal_domain_feature<
      F, std::void_t<typename F::template GoalDomain<Builder>>>
      : public F::template GoalDomain<Builder> {};

  template <typename F, typename = void>
  struct optional_initialization_domain_feature
      : public empty_feature<InitializableDomain<Builder>> {};

  template <typename F>
  struct optional_initialization_domain_feature<
      F, std::void_t<typename F::template InitializationDomain<Builder>>>
      : public F::template InitializationDomain<Builder> {};

  template <typename F, typename = void>
  struct optional_renderable_domain_feature
      : public empty_feature<RenderableDomain<Builder>> {};

  template <typename F>
  struct optional_renderable_domain_feature<
      F, std::void_t<typename F::template RenderableDomain<Builder>>>
      : public F::template RenderableDomain<Builder> {};

  class Domain : public required_agent_domain_feature<Features>,
                 public required_concurrency_domain_feature<Features>,
                 public required_dynamics_domain_feature<Features>,
                 public required_activity_domain_feature<Features>,
                 public required_history_domain_feature<Features>,
                 public required_observability_domain_feature<Features>,
                 public required_value_domain_feature<Features>,
                 public optional_constrained_domain_feature<Features>,
                 public optional_goal_domain_feature<Features>,
                 public optional_initialization_domain_feature<Features>,
                 public optional_renderable_domain_feature<Features> {};
};

} // namespace skdecide

#endif // SKDECIDE_DOMAIN_BUILDER_HH
