/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_OPTIMIZATION_EXPRESSION_HH
#define SKDECIDE_PDDL_OPTIMIZATION_EXPRESSION_HH

#include "unary_expression.hh"

namespace skdecide {

    namespace pddl {

        class MinimizeExpression : public UnaryExpression<MinimizeExpression> {
        public :
            static constexpr char class_name[] = "minimize";

            typedef std::shared_ptr<MinimizeExpression> Ptr;

            MinimizeExpression() {}

            MinimizeExpression(const Expression::Ptr& expression)
                : UnaryExpression<MinimizeExpression>(expression) {}
            
            MinimizeExpression(const MinimizeExpression& other)
                : UnaryExpression<MinimizeExpression>(other) {}
            
            MinimizeExpression& operator= (const MinimizeExpression& other) {
                dynamic_cast<UnaryExpression<MinimizeExpression>&>(*this) = other;
                return *this;
            }

            virtual ~MinimizeExpression() {}
        };


        class MaximizeExpression : public UnaryExpression<MaximizeExpression> {
        public :
            static constexpr char class_name[] = "maximize";

            typedef std::shared_ptr<MaximizeExpression> Ptr;

            MaximizeExpression() {}

            MaximizeExpression(const Expression::Ptr& expression)
                : UnaryExpression<MaximizeExpression>(expression) {}
            
            MaximizeExpression(const MaximizeExpression& other)
                : UnaryExpression<MaximizeExpression>(other) {}
            
            MaximizeExpression& operator= (const MaximizeExpression& other) {
                dynamic_cast<UnaryExpression<MaximizeExpression>&>(*this) = other;
                return *this;
            }

            virtual ~MaximizeExpression() {}
        };

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_OPTIMIZATION_EXPRESSION_HH
