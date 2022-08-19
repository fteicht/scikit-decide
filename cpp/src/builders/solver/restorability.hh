/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_RESTORABILITY_HH
#define SKDECIDE_RESTORABILITY_HH

#include "core.hh"
#include <filesystem>
#include <functional>

namespace skdecide {

/**
 * @brief A solver must inherit this class if its state can be saved and
 reloaded (to continue computation later on or reuse its solution).
 *
 * @tparam DerivedCompoundSolver The type of the solver made up of different
 * features and deriving from this particular solver feature.
 * @tparam CompoundDomain The type of the domain made up of different
 * features.
 */
template <typename DerivedCompoundSolver, typename CompoundDomain>
class RestorableSolver {
public:
  typedef typename CompoundDomain::template SmartPointer<CompoundDomain>
      CompoundDomainPtr;

  /**
   * @brief ave the solver state to given path.
   *
   * @param path The path to store the saved state.
   */
  virtual void save(const std::filesystem::path &path) = 0;

  /**
   * @brief Restore the solver state from given path.
   *
   * @param path The path where the solver state was saved.
   * @param domain_factory A callable with no argument returning the domain to
   * solve (useful in some implementations).
   */
  virtual void
  load(const std::filesystem::path &path,
       const std::function<CompoundDomainPtr()> &domain_factory) = 0;
};

} // namespace skdecide

#endif // SKDECIDE_RESTORABILITY_HH
