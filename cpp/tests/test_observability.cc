/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <list>
#include <set>
#include <cmath>

#include "core.hh"
#include "builders/domain/observability.hh"
#include "builders/domain/agent.hh"
#include "builders/domain/concurrency.hh"
#include "builders/domain/events.hh"

class TestPartiallyObservableDomain {
public:
  struct Types {
    typedef int AgentEvent;
    typedef float AgentState;
    typedef int AgentObservation;
  };

  struct Features {
    template <typename D> using ActivityDomain = skdecide::EventDomain<D>;
    template <typename D> using AgentDomain = skdecide::SingleAgentDomain<D>;
    template <typename D> using ConcurrencyDomain = skdecide::ParallelDomain<D>;
    template <typename D>
    using ObservabilityDomain = skdecide::PartiallyObservableDomain<D>;
  };

  struct Domain : public Features::template ObservabilityDomain<
                      TestPartiallyObservableDomain>::Feature {
  public:
    virtual CompoundObservationSpacePtr make_observation_space() {
      return std::make_unique<CompoundObservationSpace>(
          [](const int &o) -> bool { return o >= 0 && o <= 10; });
    }

    virtual CompoundStateSpacePtr make_state_space() {
      return std::make_unique<CompoundStateSpace>(
          [](const float &s) -> bool { return s >= -0.5 && s <= 10.5; });
    }

    virtual CompoundObservationDistributionPtr get_observation_distribution(
        const CompoundState &state,
        const CompoundEventPtr &event = CompoundEventPtr()) {
      return std::make_unique<CompoundObservationDistribution>(
          [&state]() -> CompoundObservationPtr {
            return std::make_shared<CompoundObservation>(
                std::min(std::max(0, int(std::round(state))), 10));
          });
    }
  };

  Domain *operator->() { return &domain; }

private:
  Domain domain;
};

TEST_CASE("Partially observable domain", "[partially-observable-domain") {
  TestPartiallyObservableDomain tpod;
  REQUIRE(tpod->is_observation(5) == true);
  REQUIRE(tpod->is_observation(-1) == false);
  REQUIRE(tpod->is_state(5.7) == true);
  REQUIRE(tpod->is_state(11) == false);
  REQUIRE(*(tpod->get_observation_distribution(2.8, nullptr)->sample()) == 3);
}

template <typename Letter = char>
class AlphabetSpace : public skdecide::Space<Letter> {
public:
  AlphabetSpace() {
    std::vector<Letter> v(26);
    std::iota(v.begin(), v.end(), 'a');
    std::copy(v.begin(), v.end(), std::inserter(_alphabet, _alphabet.end()));
  }

  inline virtual bool contains(const Letter &x) const {
    return _alphabet.find(x) != _alphabet.end();
  }

private:
  std::set<Letter> _alphabet;
};

class TestFullyObservableDomain {
public:
  struct Types {
    typedef int AgentAction;
    typedef char AgentState;
    template <typename... T> using AgentStateSpace = AlphabetSpace<T...>;
  };

  struct Features {
    template <typename D> using ActivityDomain = skdecide::ActionDomain<D>;
    template <typename D> using AgentDomain = skdecide::SingleAgentDomain<D>;
    template <typename D>
    using ConcurrencyDomain = skdecide::SequentialDomain<D>;
    template <typename D>
    using ObservabilityDomain = skdecide::FullyObservableDomain<D>;
  };

  class Domain : public Features::template ObservabilityDomain<
                     TestFullyObservableDomain>::Feature {
  private:
    virtual CompoundStateSpacePtr make_state_space() {
      return std::make_unique<CompoundStateSpace>();
    };
  };

  Domain *operator->() { return &domain; }

private:
  Domain domain;
};

TEST_CASE("Fully observable domain", "[fully-observable-domain]") {
  TestFullyObservableDomain tfod;
  REQUIRE(tfod->get_observation_space().contains('Z') == false);
  REQUIRE(tfod->get_state_space().contains('i') == true);
  REQUIRE(*(tfod->get_observation_distribution('a', nullptr)->sample()) == 'a');
}
