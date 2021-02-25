/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_PREDICATE_HH
#define SKDECIDE_PDDL_PREDICATE_HH

#include "identifier.hh"
#include "variable_container.hh"

namespace skdecide {

    namespace pddl {

        class Predicate : public Identifier,
                          public VariableContainer<Predicate> {
        public :
            static constexpr char class_name[] = "predicate";

            typedef std::shared_ptr<Predicate> Ptr;

            Predicate(const std::string& name)
                : Identifier(name) {}

            Predicate(const Predicate& other)
                : Identifier(other), VariableContainer<Predicate>(other) {}

            Predicate& operator=(const Predicate& other) {
                dynamic_cast<Identifier&>(*this) = other;
                dynamic_cast<VariableContainer<Predicate>&>(*this) = other;
                return *this;
            }

            virtual ~Predicate() {}

            typedef VariableContainer<Predicate>::VariablePtr VariablePtr;
            typedef VariableContainer<Predicate>::VariableVector VariableVector;
        };

        // Predicate printing operator
        inline std::ostream& operator<<(std::ostream& o, const Predicate& p) {
            return p.print(o);
        }

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_PREDICATE_HH
