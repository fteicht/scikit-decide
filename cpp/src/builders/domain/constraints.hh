/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_CONSTRAINTS_HH
#define SKDECIDE_CONSTRAINTS_HH

#include <memory>
#include <list>

#include "core.hh"
#include "builders/domain/domain_type_importer.hh"

namespace skdecide {

template <typename CompoundDomain, template <typename...> class CallingFeature,
          template <typename...> class DerivedFeature = CallingFeature>
class ConstraintsTypesImporter {
public:
  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                constraint_list, ConstraintList);
};

/**
 * @brief A domain must inherit this class if it has constraints.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class ConstrainedDomain {
public:
  /**
   * @brief Type of sequence to store constraints
   * @tparam Args Type of constraints
   */
  template <typename... Args>
  using ConstraintList =
      typename ConstraintsTypesImporter<CompoundDomain, ConstrainedDomain>::
          template import_constraint_list_type<std::list>::template test_with<
              char>::template result<Args...>;

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature {
  public:
    typedef ConstrainedDomain<CompoundDomain> FeatureDomain;

    typedef typename CompoundDomain::Features::
        template AgentDomain<CompoundDomain>::template AgentProxy<
            typename CompoundDomain::Features::template ObservabilityDomain<
                CompoundDomain>::AgentState>
            CompoundState;
    typedef typename CompoundDomain::Features::template MemoryDomain<
        CompoundDomain>::template MemoryProxy<CompoundState>
        CompoundMemory;
    typedef typename CompoundDomain::Features::
        template AgentDomain<CompoundDomain>::template AgentProxy<
            typename CompoundDomain::Features::template ConcurrencyDomain<
                CompoundDomain>::
                template ConcurrencyProxy<
                    typename CompoundDomain::Features::template ActivityDomain<
                        CompoundDomain>::AgentEvent>>
            CompoundEvent;
    typedef std::shared_ptr<
        Constraint<CompoundMemory, CompoundState, CompoundEvent>>
        ConstraintPtr;
    typedef ConstraintList<ConstraintPtr> ConstraintList;
    typedef std::unique_ptr<ConstraintList> ConstraintListPtr;

    const ConstraintList &get_constraints() {
      if (!(this->_constraints)) {
        this->_constraints = this->make_constraints();
      }
      return *(this->_constraints);
    }

  protected:
    std::unique_ptr<ConstraintList> _constraints;

    virtual ConstraintListPtr make_constraints() = 0;
  };
};

} // namespace skdecide

#endif // SKDECIDE_CONSTRAINTS_HH
