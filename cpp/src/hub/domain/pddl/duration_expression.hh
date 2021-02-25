/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_DURATION_EXPRESSION_HH
#define SKDECIDE_PDDL_DURATION_EXPRESSION_HH

#include "expression.hh"

namespace skdecide {

    namespace pddl {

        class DurationExpression : public Expression {
        public :
            static constexpr char class_name[] = "?duration";

            typedef std::shared_ptr<DurationExpression> Ptr;

            DurationExpression() {}

            virtual std::ostream& print(std::ostream& o) const {
                o << "?duration";
                return o;
            }
        };

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_DURATION_EXPRESSION_HH
