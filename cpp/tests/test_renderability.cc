/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <algorithm>
#include <catch.hpp>
#include <memory>
#include <string>
#include <vector>
#include "core.hh"
#include "builders/domain/agent.hh"
#include "builders/domain/concurrency.hh"
#include "builders/domain/events.hh"
#include "builders/domain/constraints.hh"
#include "builders/domain/memory.hh"
#include "builders/domain/observability.hh"
#include "builders/domain/renderability.hh"

class TestCompoundDomain {
public:
  struct Types {
    typedef std::string AgentType;
    typedef int AgentState;
  };

  struct Features {
    template <typename D> using AgentDomain = skdecide::MultiAgentDomain<D>;
    template <typename D> using MemoryDomain = skdecide::FiniteHistoryDomain<D>;
    template <typename D>
    using ObservabilityDomain = skdecide::FullyObservableDomain<D>;
    template <typename D>
    using RenderabilityDomain = skdecide::RenderableDomain<D>;
  };

  struct Domain : public Features::template RenderabilityDomain<
                      TestCompoundDomain>::Feature {
    std::string _rendered_string;
    virtual void render(const CompoundMemory &memory) {
      _rendered_string = "[";
      int cnt = 0;
      for (const CompoundState &cs : memory) {
        _rendered_string += "t=" + std::to_string(cnt) + ": (";
        for (const auto &kv : cs) {
          _rendered_string +=
              kv.first + " -> " + std::to_string(kv.second) + "; ";
        }
        _rendered_string =
            _rendered_string.substr(0, _rendered_string.size() - 2);
        _rendered_string += "); ";
        cnt++;
      }
      _rendered_string =
          _rendered_string.substr(0, _rendered_string.size() - 2);
      _rendered_string += "]";
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
  tcd->render(m);
  REQUIRE(tcd->_rendered_string ==
          "[t=0: (0 -> 0; 1 -> 1; 2 -> 2; 3 -> 3; 4 -> 4); t=1: (0 -> 1; 1 -> "
          "2; 2 -> 3; 3 -> 4; 4 -> 5); t=2: (0 -> 2; 1 -> 3; 2 -> 4; 3 -> 5; 4 "
          "-> 6)]");
}
