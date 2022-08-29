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
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class ConstrainedDomain {
private:
  template <class, class = void> struct define_constraint_list {
    template <typename... Args> using result = std::list<Args...>;
  };

  template <class T>
  struct define_constraint_list<
      T, std::void_t<typename T::template ConstraintList<char>>> {
    template <typename... Args>
    using result = typename T::template ConstraintList<Args...>;
  };

public:
  /**
   * @brief Type of sequence to store constraints
   * @tparam T Type of constraints
   * @tparam Args Additional sequence container template parameters
   */
  template <typename T, typename... Args>
  using ConstraintList = typename define_constraint_list<
      typename CompoundDomain::Types>::template result<T, Args...>;

  typedef typename CompoundDomain::Features::
      template AgentDomain<CompoundDomain>::template AgentProxy<
          typename CompoundDomain::Features::template ObservabilityDomain<
              CompoundDomain>::AgentState>
          CompoundState;
  typedef typename CompoundDomain::Features::template HistoryDomain<
      CompoundDomain>::template MemoryProxy<CompoundState>
      CompoundMemory;
  typedef typename CompoundDomain::Features::
      template AgentDomain<CompoundDomain>::template AgentProxy<
          typename CompoundDomain::Features::template ConcurrencyDomain<
              CompoundDomain>::
              template ConcurrencyProxy<
                  typename CompoundDomain::Features::template AgentDomain<
                      CompoundDomain>::AgentEvent>>
          CompoundEvent;
  typedef std::shared_ptr<
      Constraint<CompoundMemory, CompoundState, CompoundEvent>>
      ConstraintPtr;

  const ConstraintList<ConstraintPtr> &get_constraints() {
    if (!(this->_constraints)) {
      this->_constraints = this->make_constraints();
    }
    return *(this->_constraints);
  }

protected:
  std::unique_ptr<ConstraintList<ConstraintPtr>> _constraints;

  virtual std::unique_ptr<ConstraintList<ConstraintPtr>> make_constraints() = 0;
};

} // namespace skdecide

#endif // SKDECIDE_CONSTRAINTS_HH
