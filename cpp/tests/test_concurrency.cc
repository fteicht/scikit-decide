/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <catch.hpp>
#include <vector>
#include "builders/domain/concurrency.hh"

class TestCompoundDomain1 {
public:
  struct Types {
    template <typename... T> using EventList = std::vector<T...>;
  };

  struct Features {
    template <typename D> using ConcurrencyDomain = skdecide::ParallelDomain<D>;
  };

  struct Domain
      : public Features::template ConcurrencyDomain<TestCompoundDomain1> {};
};

class TestCompoundDomain2 {
public:
  struct Types {};

  struct Features {
    template <typename D> using ConcurrencyDomain = skdecide::ParallelDomain<D>;
  };

  struct Domain
      : public Features::template ConcurrencyDomain<TestCompoundDomain2> {};
};

class TestCompoundDomain3 {
public:
  struct Types {};

  struct Features {
    template <typename D>
    using ConcurrencyDomain = skdecide::SequentialDomain<D>;
  };

  struct Domain
      : public Features::template ConcurrencyDomain<TestCompoundDomain3> {};
};

TEST_CASE("Agent domain", "[agent-domain]") {
  TestCompoundDomain1();
  REQUIRE(
      typeid(skdecide::ParallelDomain<TestCompoundDomain1>::EventList<char>) ==
      typeid(std::vector<char>));

  TestCompoundDomain2();
  REQUIRE(
      typeid(skdecide::ParallelDomain<TestCompoundDomain2>::EventList<char>) ==
      typeid(std::list<char>));

  TestCompoundDomain3();
  REQUIRE(
      typeid(
          skdecide::SequentialDomain<TestCompoundDomain3>::EventList<char>) ==
      typeid(char));
  REQUIRE(
      typeid(skdecide::ParallelDomain<TestCompoundDomain3>::EventList<char>) ==
      typeid(skdecide::SequentialDomain<TestCompoundDomain3>::EventList<char>));
}
