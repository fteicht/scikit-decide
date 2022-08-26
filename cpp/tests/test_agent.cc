/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <catch.hpp>
#include <cstddef>
#include <memory>
#include <set>
#include "builders/domain/agent.hh"

class TestCompoundDomain1 {
public:
  struct Types {
    typedef std::string AgentType;
    template <typename... T> using AgentDict = std::map<T...>;
  };

  struct Features {
    template <typename D> using AgentDomain = skdecide::MultiAgentDomain<D>;
  };

  struct Domain : public Features::template AgentDomain<TestCompoundDomain1> {};
};

class TestCompoundDomain2 {
public:
  struct Types {
    typedef std::string AgentType;
  };

  struct Features {
    template <typename D> using AgentDomain = skdecide::MultiAgentDomain<D>;
  };

  struct Domain : public Features::template AgentDomain<TestCompoundDomain2> {};
};

class TestCompoundDomain3 {
public:
  struct Types {};

  struct Features {
    template <typename D> using AgentDomain = skdecide::SingleAgentDomain<D>;
  };

  struct Domain : public Features::template AgentDomain<TestCompoundDomain3> {};
};

TEST_CASE("Agent domain", "[agent-domain]") {
  TestCompoundDomain1();
  REQUIRE(typeid(skdecide::MultiAgentDomain<TestCompoundDomain1>::AgentType) ==
          typeid(std::string));
  REQUIRE(typeid(skdecide::MultiAgentDomain<TestCompoundDomain1>::AgentDict<
                 int, char>) == typeid(std::map<int, char>));

  TestCompoundDomain2();
  REQUIRE(typeid(skdecide::MultiAgentDomain<TestCompoundDomain2>::AgentDict<
                 int, char>) ==
          typeid(std::unordered_map<int, char, skdecide::Hash<int>,
                                    skdecide::Equal<int>>));

  TestCompoundDomain3();
  REQUIRE(typeid(skdecide::SingleAgentDomain<TestCompoundDomain3>::AgentType) ==
          typeid(std::nullptr_t));
  REQUIRE(typeid(skdecide::SingleAgentDomain<TestCompoundDomain3>::AgentDict<
                 int, char>) == typeid(char));
  REQUIRE(typeid(skdecide::MultiAgentDomain<TestCompoundDomain3>::AgentType) ==
          typeid(skdecide::SingleAgentDomain<TestCompoundDomain3>::AgentType));
  REQUIRE(typeid(skdecide::MultiAgentDomain<TestCompoundDomain3>::AgentDict<
                 int, char>) ==
          typeid(skdecide::SingleAgentDomain<TestCompoundDomain3>::AgentDict<
                 int, char>));
}
