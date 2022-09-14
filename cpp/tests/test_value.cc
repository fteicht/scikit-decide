/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <algorithm>
#include <catch.hpp>
#include <vector>
#include "builders/domain/agent.hh"
#include "builders/domain/value.hh"

namespace std {
std::vector<int> operator-(const std::vector<int> &v) {
  std::vector<int> vv(v);
  std::for_each(vv.begin(), vv.end(), [](int &i) { i = -i; });
  return vv;
}

bool operator>=(const std::vector<int> &v, const int &vv) {
  return std::all_of(v.begin(), v.end(),
                     [&vv](const int &i) { return i >= vv; });
}

std::unordered_map<std::string, double, skdecide::Hash<std::string>,
                   skdecide::Equal<std::string>>
operator-(
    const std::unordered_map<std::string, double, skdecide::Hash<std::string>,
                             skdecide::Equal<std::string>> &m) {
  std::unordered_map<std::string, double, skdecide::Hash<std::string>,
                     skdecide::Equal<std::string>>
      mm(m);
  std::for_each(mm.begin(), mm.end(), [](auto &kv) { kv.second = -kv.second; });
  return mm;
}

bool operator>=(
    const std::unordered_map<std::string, double, skdecide::Hash<std::string>,
                             skdecide::Equal<std::string>> &m,
    const int &v) {
  return std::all_of(m.begin(), m.end(),
                     [&v](const auto &kv) { return kv.second >= v; });
}
} // namespace std

class TestCompoundDomain1 {
public:
  struct Types {
    typedef std::string AgentType;
  };

  struct Features {
    template <typename D> using AgentDomain = skdecide::MultiAgentDomain<D>;
    template <typename D> using ValueDomain = skdecide::RewardDomain<D>;
  };

  struct Domain
      : public Features::template ValueDomain<TestCompoundDomain1>::Feature {};

  Domain *operator->() { return &domain; }

private:
  Domain domain;
};

class TestCompoundDomain2 {
public:
  struct Types {
    typedef std::vector<int> AgentCost;
  };

  struct Features {
    template <typename D> using AgentDomain = skdecide::SingleAgentDomain<D>;
    template <typename D> using ValueDomain = skdecide::PositiveCostDomain<D>;
  };

  struct Domain
      : public Features::template ValueDomain<TestCompoundDomain2>::Feature {};

  Domain *operator->() { return &domain; }

private:
  Domain domain;
};

TEST_CASE("Value domain", "[value-domain]") {
  TestCompoundDomain1 tcd1;
  REQUIRE(typeid(skdecide::RewardDomain<TestCompoundDomain1>::AgentReward) ==
          typeid(double));
  REQUIRE(
      typeid(skdecide::RewardDomain<
             TestCompoundDomain1>::Feature::CompoundValue) ==
      typeid(
          std::unordered_map<std::string, double, skdecide::Hash<std::string>,
                             skdecide::Equal<std::string>>));
  REQUIRE(
      tcd1->check_value(
          skdecide::Value<skdecide::ValueType::REWARD,
                          TestCompoundDomain1::Features::template ValueDomain<
                              TestCompoundDomain1>::Feature::CompoundValue>(
              TestCompoundDomain1::Features::template ValueDomain<
                  TestCompoundDomain1>::Feature::CompoundValue())) == true);

  TestCompoundDomain2 tcd2;
  std::vector<int> v(2, 1);
  v[1] = -1;
  REQUIRE(typeid(skdecide::RewardDomain<TestCompoundDomain2>::AgentReward) ==
          typeid(std::vector<int>));
  REQUIRE(
      tcd2->check_value(
          skdecide::Value<skdecide::ValueType::COST,
                          TestCompoundDomain2::Features::template ValueDomain<
                              TestCompoundDomain2>::Feature::CompoundValue>(
              v)) == false);
}
