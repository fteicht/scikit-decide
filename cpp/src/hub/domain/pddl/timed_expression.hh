/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_TIMED_EXPRESSION_HH
#define SKDECIDE_PDDL_TIMED_EXPRESSION_HH

#include "expression.hh"

namespace skdecide {

    namespace pddl {

        class TimeExpression : public Expression {
        public :
            static constexpr char class_name[] = "#t";

            typedef std::shared_ptr<TimeExpression> Ptr;

            TimeExpression() {}

            virtual std::ostream& print(std::ostream& o) const {
                o << "#t";
                return o;
            }
        };

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_TIMED_EXPRESSION_HH
