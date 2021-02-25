/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_TOTALCOST_EXPRESSION_HH
#define SKDECIDE_PDDL_TOTALCOST_EXPRESSION_HH

#include "expression.hh"

namespace skdecide {

    namespace pddl {

        class TotalCostExpression : public Expression {
        public :
            static constexpr char class_name[] = "total-cost";

            typedef std::shared_ptr<TotalCostExpression> Ptr;

            TotalCostExpression() {}

            virtual ~TotalCostExpression() {}

            virtual std::ostream& print(std::ostream& o) const {
                o << "total-cost";
                return o;
            }
        };

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_TOTALCOST_EXPRESSION_HH
