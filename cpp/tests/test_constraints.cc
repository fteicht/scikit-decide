/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <algorithm>
#include <catch.hpp>
#include <memory>
#include <vector>
#include "core.hh"
#include "builders/domain/agent.hh"
#include "builders/domain/concurrency.hh"
#include "builders/domain/events.hh"
#include "builders/domain/constraints.hh"
#include "builders/domain/memory.hh"
#include "builders/domain/observability.hh"

class TestCompoundDomain {
public:
  struct Types {
    typedef std::string AgentType;
    typedef char AgentAction;
    typedef int AgentState;
    template <typename... T> using ConstraintList = std::vector<T...>;
  };

  struct Features {
    template <typename D> using AgentDomain = skdecide::MultiAgentDomain<D>;
    template <typename D>
    using ConcurrencyDomain = skdecide::SequentialDomain<D>;
    template <typename D> using ActivityDomain = skdecide::ActionDomain<D>;
    template <typename D> using MemoryDomain = skdecide::FiniteHistoryDomain<D>;
    template <typename D>
    using ObservabilityDomain = skdecide::FullyObservableDomain<D>;
    template <typename D>
    using ConstraintDomain = skdecide::ConstrainedDomain<D>;
  };

  struct Domain : public Features::template ConstraintDomain<
                      TestCompoundDomain>::Feature {
    struct Constraint
        : public skdecide::ImplicitConstraint<CompoundMemory, CompoundState,
                                              CompoundEvent> {};
    virtual ConstraintListPtr make_constraints() {
      ConstraintPtr constraint = std::make_shared<skdecide::ImplicitConstraint<
          CompoundMemory, CompoundState, CompoundEvent>>(
          [](const CompoundMemory &m, const CompoundEvent &e,
             const std::unique_ptr<CompoundState> &) -> bool {
            return std::all_of(e.begin(), e.end(),
                               [&m](const auto &kv) -> bool {
                                 const CompoundState &cs = m.back();
                                 return cs.at(kv.first) == int(kv.second);
                               });
          },
          false);
      return std::make_unique<ConstraintList>(1, constraint);
    }
  };

  Domain *operator->() { return &domain; }

private:
  Domain domain;
};

TEST_CASE("Constrained domain", "[constraint-domain]") {
  TestCompoundDomain tcd;
  TestCompoundDomain::Domain::CompoundMemory m(10);
  for (int i = 0; i < 3; i++) {
    TestCompoundDomain::Domain::CompoundState s;
    for (int a = 0; a < 5; a++) {
      s[std::to_string(a)] = a + i;
    }
    m.push_back(s);
  }
  TestCompoundDomain::Domain::CompoundEvent e;
  for (int a = 0; a < 5; a++) {
    e[std::to_string(a)] = char(a + 2);
  }
  REQUIRE(tcd->get_constraints().front()->check(m, e) == true);
  REQUIRE(typeid(skdecide::ConstrainedDomain<
                 TestCompoundDomain>::ConstraintList<char>) ==
          typeid(std::vector<char>));
}
