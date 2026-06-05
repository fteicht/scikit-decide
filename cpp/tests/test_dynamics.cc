/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <catch2/catch_test_macros.hpp>

#include "core.hh"
#include "builders/domain/dynamics.hh"
#include "builders/domain/observability.hh"
#include "builders/domain/agent.hh"
#include "builders/domain/concurrency.hh"
#include "builders/domain/events.hh"

class TestEnvironmentDomain {
public:
  struct Types {
    typedef int AgentAction;
    typedef float AgentState;
  };

  struct Features {
    template <typename D> using ActivityDomain = skdecide::ActionDomain<D>;
    template <typename D> using AgentDomain = skdecide::SingleAgentDomain<D>;
    template <typename D>
    using ConcurrencyDomain = skdecide::SequentialDomain<D>;
    template <typename D>
    using ObservabilityDomain = skdecide::FullyObservableDomain<D>;
    template <typename D> using DynamicsDomain = skdecide::EnvironmentDomain<D>;
  };

  struct Domain : public Features::template DynamicsDomain<
                      TestEnvironmentDomain>::Feature {};

  Domain *operator->() { return &domain; }

private:
  Domain domain;
};
