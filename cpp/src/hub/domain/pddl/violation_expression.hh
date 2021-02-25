/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_VIOLATION_EXPRESSION_HH
#define SKDECIDE_PDDL_VIOLATION_EXPRESSION_HH

#include "expression.hh"
#include "preference.hh"

namespace skdecide {

    namespace pddl {

        class ViolationExpression : public Expression {
        public :
            static constexpr char class_name[] = "is-violated";

            typedef std::shared_ptr<ViolationExpression> Ptr;

            ViolationExpression() {}

            ViolationExpression(const Preference::Ptr& preference)
            : _preference(preference) {}

            virtual ~ViolationExpression() {}

            void set_preference(const Preference::Ptr& preference) {
                _preference = preference;
            }

            const Preference::Ptr& get_preference() const {
                return _preference;
            }

            virtual std::ostream& print(std::ostream& o) const {
                o << "(is-violated " << *_preference << ")";
                return o;
            }
        
        private :
            Preference::Ptr _preference;
        };

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_VIOLATION_EXPRESSION_HH
