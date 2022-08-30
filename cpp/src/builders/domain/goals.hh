/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_GOALS_HH
#define SKDECIDE_GOALS_HH

#include "core.hh"
#include <memory>

namespace skdecide {

/**
 * @brief A domain must inherit this class if it has formalized goals.
 *
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam GoalSpace The type of goal spaces
 */
template <typename DerivedCompoundDomain,
          template <typename...> class GoalSpace = ImplicitSpace>
class GoalDomain {
public:
  template <typename... T> using RawGoalSpace = GoalSpace<T...>;

  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawObservation>
      CompoundObservation;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::template RawGoalSpace<
          typename DerivedCompoundDomain::RawObservation>>
      CompoundGoalSpace;
  typedef typename std::unique_ptr<CompoundGoalSpace> CompoundGoalSpacePtr;

  /**
   * @brief Get the (cached) domain goals space (finite or infinite set).
   *
   *    By default, GoalDomain::get_goals() internally calls
   * GoalDomain.make_goals() the first time and automatically caches its value
   * to make future calls more efficient (since the goals space is assumed to be
   * constant).
   *
   * !!! Warning.
   *        Goal states are assumed to be fully observable (i.e. observation =
   * state) so that there is never uncertainty about whether the goal has been
   * reached or not. This assumption guarantees that any policy that does not
   * reach the goal with certainty incurs in infinite expected cost.
   * - Geffner, 2013: A Concise Introduction to Models and Methods for Automated
   * Planning
   *
   * @return The goals space.
   */
  const CompoundGoalSpace &get_goals() {
    if (!_goals) {
      _goals = make_goals();
    }
    return *_goals;
  }

  /**
   * @brief Indicate whether an observation belongs to the goals.
   *
   * !!! Tip.
   *        By default, this function is implemented using the
   * skdecide::Space::contains() function on the domain goals space provided
   * by GoalDomain::get_goals(), but it can be overridden for faster
   * implementations.
   *
   * @param observation The observation to consider.
   * @return True if the observation is a goal (False otherwise).
   */
  inline virtual bool is_goal(const CompoundObservation &observation) {
    return get_goals().contains(observation);
  }

protected:
  /**
   * @brief Get the domain goals space (finite or infinite set).
   *
   *    This is a helper function called by default from
   * GoalDomain::get_goals(), the difference being that the result is not cached
   * here.
   *
   * @return The goals space.
   */
  virtual CompoundGoalSpacePtr make_goals() = 0;

private:
  CompoundGoalSpacePtr _goals;
};

} // namespace skdecide

#endif // SKDECIDE_GOALS_HH
