/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_VALUE_HH
#define SKDECIDE_VALUE_HH

#include "builders/domain/domain_type_importer.hh"
#include "core.hh"

namespace skdecide {

template <typename CompoundDomain, template <typename...> class CallingFeature,
          template <typename...> class DerivedFeature = CallingFeature>
class ValueTypesImporter {
public:
  DOMAIN_SIMPLE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                              reward, AgentReward)
  DOMAIN_SIMPLE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                              cost, AgentCost)
};

/**
 * @brief A domain must inherit this class if it sends rewards (positive and/or
 * negative).
 *
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class RewardDomain {
public:
  /**
   * @brief Type of an agent's rewards
   */
  typedef typename ValueTypesImporter<
      CompoundDomain, RewardDomain,
      CompoundDomain::Features::template ValueDomain>::
      template import_reward_type<double>::result AgentReward;

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature {
  public:
    typedef RewardDomain<CompoundDomain> FeatureDomain;

    typedef typename CompoundDomain::Features::template AgentDomain<
        CompoundDomain>::template AgentProxy<AgentReward>
        CompoundValue;

    /**
     * @brief Check that a value is compliant with its reward specification.
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
};

/**
 * @brief A domain must inherit this class if it sends only positive costs (i.e.
 * negative rewards).
 *
 * Having only positive costs is a required assumption for certain solvers to
 * work, such as classical planners.
 *
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class PositiveCostDomain {
public:
  /**
   * @brief Type of an agent's costs
   */
  typedef typename ValueTypesImporter<CompoundDomain, PositiveCostDomain>::
      template import_cost_type<double>::result AgentCost;

  /**
   * @brief Type of an agent's rewards
   */
  typedef typename ValueTypesImporter<CompoundDomain, PositiveCostDomain>::
      template import_reward_type<AgentCost>::result AgentReward;

  static_assert(
      std::is_same_v<AgentCost, AgentReward>,
      "The domain types AgentCost and AgentReward must be the same when the "
      "domain value feature derives from PositiveCostDomain<...>.");

  class Feature : public RewardDomain<CompoundDomain>::Feature {
  public:
    typedef PositiveCostDomain<CompoundDomain> FeatureDomain;

    typedef typename CompoundDomain::Features::template AgentDomain<
        CompoundDomain>::template AgentProxy<AgentCost>
        CompoundValue;

    /**
     * @brief Check that a value is compliant with its reward specification.
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
      return value.cost() >= 0;
    }

    /**
     * @brief Check that a value is compliant with its cost specification (must
     * be positive).
     *
     * @param value The value to check.
     * @return True if the cost is positive (False otherwise).
     */
    inline bool
    check_value(const Value<ValueType::COST, CompoundValue> &value) {
      return value.cost() >= 0;
    }
  };
};

} // namespace skdecide

#endif // SKDECIDE_VALUE_HH
