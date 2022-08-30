/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_VALUE_HH
#define SKDECIDE_VALUE_HH

#include "core.hh"

namespace skdecide {

/**
 * @brief A domain must inherit this class if it sends rewards (positive and/or
 * negative).
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam Reward Type of an agent's rewards
 */
template <typename DerivedCompoundDomain, typename Reward = double>
class RewardDomain {
public:
  typedef Reward RawValue;

  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawValue>
      CompoundValue;

  /**
   * @brief heck that a value is compliant with its reward specification.
   *
   * !!! Tip.
   *         This function returns always True by default because any kind of
   * reward should be accepted at this level.
   *
   * @param value The value to check.
   * @return Always True.
   */
  inline virtual bool
  check_value(const Value<ValueType::REWARD, CompoundValue> &value) {
    return true;
  }
};

/**
 * @brief A domain must inherit this class if it sends only positive costs (i.e.
 * negative rewards).
 *
 * Having only positive costs is a required assumption for certain solvers to
 * work, such as classical planners.
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam Cost Type of an agent's costs
 */
template <typename DerivedCompoundDomain, typename Cost = double>
class PositiveCostDomain : public RewardDomain<DerivedCompoundDomain, Cost> {
public:
  typedef Cost RawValue;

  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawValue>
      CompoundValue;

  /**
   * @brief Check that a value is compliant with its cost specification (must be
   * positive).
   *
   * @param value The value to check.
   * @return True if the cost is positive (False otherwise).
   */
  inline virtual bool
  check_value(const Value<ValueType::COST, CompoundValue> &value) {
    return value.cost >= 0;
  }
};

} // namespace skdecide

#endif // SKDECIDE_VALUE_HH
