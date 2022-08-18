/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_CONSTRAINTS_HH
#define SKDECIDE_CONSTRAINTS_HH

#include <memory>
#include <list>

#include "core.hh"

namespace skdecide {

/**
 * @brief A domain must inherit this class if it has constraints.
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam List Type of sequence to store constraints
 */
template <typename DerivedCompoundDomain,
          template <typename...> class List = std::list>
class ConstrainedDomain {
public:
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::RawState>
      CompoundState;
  typedef typename DerivedCompoundDomain::template MemoryProxy<CompoundState>
      CompoundMemory;
  typedef typename DerivedCompoundDomain::template AgentProxy<
      typename DerivedCompoundDomain::template ConcurrencyProxy<
          typename DerivedCompoundDomain::RawEvent>>
      CompoundEvent;
  typedef std::shared_ptr<
      Constraint<CompoundMemory, CompoundState, CompoundEvent>>
      ConstraintPtr;

  const List<ConstraintPtr> &get_constraints() {
    if (!(this->_constraints)) {
      this->_constraints = this->make_constraints();
    }
    return *(this->_constraints);
  }

protected:
  std::unique_ptr<List<ConstraintPtr>> _constraints;

  virtual std::unique_ptr<List<ConstraintPtr>> make_constraints() = 0;
};

} // namespace skdecide

#endif // SKDECIDE_CONSTRAINTS_HH
