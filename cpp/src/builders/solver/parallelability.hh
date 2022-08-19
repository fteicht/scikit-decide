/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PARALLELABILTY_HH
#define SKDECIDE_PARALLELABILTY_HH

namespace skdecide {

/**
 * @brief A solver must inherit this class if it wants to call several cloned
 * parallel domains in separate concurrent processes.
 *
 * @tparam DerivedCompoundSolver The type of the solver made up of different
 * features and deriving from this particular solver feature.
 * @tparam CompoundDomain The type of the domain made up of different
 * features.
 */
template <typename DerivedCompoundSolver, typename CompoundDomain>
class ParallelSolver {
  // TODO
};

} // namespace skdecide

#endif // SKDECIDE_PARALLELABILTY_HH
