/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <catch.hpp>
#include <memory>
#include <set>
#include "builders/domain/concurrency.hh"
#include "builders/domain/agent.hh"
#include "builders/domain/observability.hh"
#include "builders/domain/memory.hh"
#include "builders/domain/events.hh"

template <typename T = char> class AlphabetSpace : public skdecide::Space<T> {
public:
  AlphabetSpace(bool with_capitals) {
    std::vector<T> v(26);
    std::iota(v.begin(), v.end(), 'a');
    std::copy(v.begin(), v.end(), std::inserter(_alphabet, _alphabet.end()));
    if (with_capitals) {
      std::iota(v.begin(), v.end(), 'A');
      std::copy(v.begin(), v.end(), std::inserter(_alphabet, _alphabet.end()));
    }
  }

  inline virtual bool contains(const T &x) const {
    return _alphabet.find(x) != _alphabet.end();
  }

private:
  std::set<T> _alphabet;
};

class TestEventDomain1 {
public:
  struct Types {
    typedef int AgentObservation;
    typedef int AgentState;
    typedef char AgentEvent;
    template <typename... Args> using AgentEventSpace = AlphabetSpace<Args...>;
    template <typename... Args> using AgentActionSpace = AlphabetSpace<Args...>;
    template <typename... Args>
    using AgentApplicableActionSpace = skdecide::ImplicitSpace<Args...>;
  };

  struct Features {
    template <typename D>
    using ConcurrencyDomain = skdecide::SequentialDomain<D>;
    template <typename D> using AgentDomain = skdecide::SingleAgentDomain<D>;
    template <typename D>
    using ObservabilityDomain = skdecide::PartiallyObservableDomain<D>;
    template <typename D> using MemoryDomain = skdecide::HistoryDomain<D>;
    template <typename D> using ActivityDomain = skdecide::EventDomain<D>;
  };

  class Domain
      : public Features::template ActivityDomain<TestEventDomain1>::Feature {
  public:
    virtual CompoundEnabledEventSpacePtr
    get_enabled_events(const CompoundMemory &memory) {
      return std::make_unique<CompoundEnabledEventSpace>(
          [this, &memory](const char &c) -> bool {
            return (std::find(memory.begin(), memory.end(), int(c)) !=
                    memory.end()) &&
                   (this->get_event_space().contains(c));
          });
    }

    virtual CompoundApplicableActionSpacePtr
    get_applicable_actions(const skdecide::Memory<int> &memory) {
      return std::make_unique<CompoundApplicableActionSpace>(
          [this, &memory](const char &c) -> bool {
            return (std::find(memory.begin(), memory.end(), int(c)) !=
                    memory.end()) &&
                   (this->get_action_space().contains(c));
          });
    }

  private:
    inline virtual CompoundEventSpacePtr make_event_space() {
      return std::make_unique<CompoundEventSpace>(true);
    }

    inline virtual CompoundActionSpacePtr make_action_space() {
      return std::make_unique<CompoundActionSpace>(false);
    }
  };

  Domain *operator->() { return &domain; }

private:
  Domain domain;
};

class TestEventDomain2 {
public:
  struct Types {
    typedef int AgentState;
    typedef char AgentAction;
    template <typename... Args> using AgentActionSpace = AlphabetSpace<Args...>;
  };

  struct Features {
    template <typename D>
    using ConcurrencyDomain = skdecide::SequentialDomain<D>;
    template <typename D> using AgentDomain = skdecide::SingleAgentDomain<D>;
    template <typename D>
    using ObservabilityDomain = skdecide::FullyObservableDomain<D>;
    template <typename D> using MemoryDomain = skdecide::MarkovianDomain<D>;
    template <typename D>
    using ActivityDomain = skdecide::UnrestrictedActionDomain<D>;
  };

  class Domain
      : public Features::template ActivityDomain<TestEventDomain2>::Feature {
  private:
    inline virtual CompoundActionSpacePtr make_action_space() {
      return std::make_unique<CompoundActionSpace>(false);
    }
  };

  Domain *operator->() { return &domain; }

private:
  Domain domain;
};

TEST_CASE("Event domain", "[event-domain]") {

  TestEventDomain1 ted;
  std::vector<char> v(26);
  std::iota(v.begin(), v.end(), 'a');
  std::iota(v.begin(), v.end(), 'A');
  bool test1 = true;
  bool test2 = true;
  for (char c : v) {
    test1 = test1 && (!ted->get_action_space().contains(c) ||
                      ted->get_event_space().contains(c));
    test2 = test2 && ted->get_event_space().contains(c);
  }
  REQUIRE(test1 == true);
  REQUIRE(test2 == true);
  REQUIRE(
      ted->get_enabled_events({int('t'), int('@'), int('R')})->contains('t') ==
      true);
  REQUIRE(
      ted->get_enabled_events({int('t'), int('@'), int('R')})->contains('@') ==
      false);
  REQUIRE(
      ted->get_enabled_events({int('t'), int('@'), int('R')})->contains('R') ==
      true);
  REQUIRE(ted->get_applicable_actions({int('t'), int('@'), int('R')})
              ->contains('t') == true);
  REQUIRE(ted->get_applicable_actions({int('t'), int('@'), int('R')})
              ->contains('@') == false);
  REQUIRE(ted->get_applicable_actions({int('t'), int('@'), int('R')})
              ->contains('R') == false);
}
