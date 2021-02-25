/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_DERIVED_PREDICATE_HH
#define SKDECIDE_PDDL_DERIVED_PREDICATE_HH

#include "predicate.hh"
#include "formula.hh"

namespace skdecide {

    namespace pddl {

        class DerivedPredicate : public Predicate {
        public :
            static constexpr char class_name[] = "derived predicate";

            typedef std::shared_ptr<DerivedPredicate> Ptr;

            DerivedPredicate(const std::string& name)
            : Predicate(name) {}

            DerivedPredicate(const std::string& name,
                             const Formula::Ptr& formula)
            : Predicate(name), _formula(formula) {}
            
            DerivedPredicate(const Predicate::Ptr& predicate,
                             const Formula::Ptr& formula)
                : Predicate(*predicate), _formula(formula) {}
            
            DerivedPredicate(const DerivedPredicate& other)
            : Predicate(other),
              _formula(other._formula) {}
            
            DerivedPredicate& operator= (const DerivedPredicate& other) {
                dynamic_cast<Predicate&>(*this) = other;
                this->_formula = other._formula;
                return *this;
            }

            void set_formula(const Formula::Ptr& formula) {
                _formula = formula;
            }

            const Formula::Ptr& get_formula() const {
                return _formula;
            }

            std::ostream& print(std::ostream& o) const {
                o << "(:derived " << dynamic_cast<const Predicate&>(*this) << " " << *_formula << ")";
                return o;
            }

        private :
            Formula::Ptr _formula;
        };

        // Derived predicate printing operator
        inline std::ostream& operator<<(std::ostream& o, const DerivedPredicate& d) {
            return d.print(o);
        }

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_DERIVED_PREDICATE_HH
