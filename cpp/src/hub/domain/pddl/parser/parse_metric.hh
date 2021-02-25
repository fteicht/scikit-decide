/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef SKDECIDE_PDDL_PARSE_METRIC_HH
#define SKDECIDE_PDDL_PARSE_METRIC_HH

#include "pegtl.hpp"

#include "parser_state.hh"
#include "parser_action.hh"

#include "parse_operation.hh"
#include "parse_function_head.hh"
#include "parse_number.hh"

#include "optimization_expression.hh"
#include "totaltime_expression.hh"
#include "violation_expression.hh"

namespace pegtl = TAO_PEGTL_NAMESPACE;  // NOLINT

namespace skdecide {

    namespace pddl {

        namespace parser {

            struct totaltime_expression : pegtl::sor<
                                                keyword<'t', 'o', 't', 'a', 'l', '-', 't', 'i', 'm', 'e'>,
                                                pegtl::seq<
                                                    pegtl::one<'('>,
                                                    ignored,
                                                    keyword<'t', 'o', 't', 'a', 'l', '-', 't', 'i', 'm', 'e'>,
                                                    ignored,
                                                    pegtl::one<')'>
                                                >
                                          > {};
            
            template <>
            struct action<totaltime_expression> {
                static void apply0(state& s) {
                    s.expression = std::make_shared<TotalTimeExpression>();
                }
            };

            struct violation_expression : pegtl::seq<
                                                pegtl::one<'('>,
                                                ignored,
                                                keyword<'i', 's', '-', 'v', 'i', 'o', 'l', 'a', 't', 'e', 'd'>,
                                                ignored,
                                                name,
                                                ignored,
                                                pegtl::one<')'>
                                          > {};
            
            template <>
            struct action<violation_expression> {
                template <typename Input>
                static void apply(const Input& in, state& s) {
                    try {
                        s.expression = std::make_shared<ViolationExpression>(s.domain->get_preference(s.name));
                    } catch (...) {
                        try {
                            s.expression = std::make_shared<ViolationExpression>(s.problem->get_preference(s.name));
                        } catch (const std::exception& e) {
                            throw pegtl::parse_error("no preference with name '" + s.name +
                                                     "' found in domain '" + s.domain->get_name() +
                                                     "' nor in problem '" + s.problem->get_name() + "'", in.position());
                        }
                    }
                }
            };

            struct ground_f_exp : pegtl::sor<
                                        operation_expression<AddOperator, ground_f_exp>,
                                        operation_expression<SubOperator, ground_f_exp>,
                                        operation_expression<MulOperator, ground_f_exp>,
                                        operation_expression<DivOperator, ground_f_exp>,
                                        function_head,
                                        number_expression,
                                        totaltime_expression,
                                        violation_expression
                                  > {};

            struct MinimizeOperator {
                typedef keyword<'m', 'i', 'n', 'i', 'm', 'i', 'z', 'e'> keyword;
                typedef MinimizeExpression type;
            };

            struct MaximizeOperator {
                typedef keyword<'m', 'a', 'x', 'i', 'm', 'i', 'z', 'e'> keyword;
                typedef MaximizeExpression type;
            };

            template <typename Operator>
            struct open_optimization_metric : pegtl::action<
                                                action,
                                                pegtl::seq<
                                                    typename Operator::keyword,
                                                    ignored
                                                >
                                              > {
                typedef Operator OP;
            };

            template <typename Rule>
            struct open_optimization_metric_action {
                static void apply0(state& s) {
                    typedef typename Rule::OP OP;
                    s.expressions.push(std::make_shared<typename OP::type>());
                }
            };
            
            template <typename Operator>
            struct close_optimization_metric : pegtl::action<
                                                    action,
                                                    ground_f_exp
                                               > {
                typedef Operator OP;
            };

            template <typename Rule>
            struct close_optimization_metric_action {
                static void apply0(state& s) {
                    typedef typename Rule::OP OP;
                    std::static_pointer_cast<typename OP::type>(s.expressions.top())->set_expression(s.expression);
                    s.expression = s.expressions.top();
                    s.expressions.pop();
                }
            };

            template <typename Operator>
            struct optimization_metric : pegtl::if_must<
                                            pegtl::action<
                                                open_optimization_metric_action,
                                                open_optimization_metric<Operator>
                                            >,
                                            pegtl::action<
                                                close_optimization_metric_action,
                                                close_optimization_metric<Operator>
                                            >
                                         > {};

            struct metric : pegtl::sor<
                                optimization_metric<MinimizeOperator>,
                                optimization_metric<MaximizeOperator>
                            > {};

        } // namespace parser

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_PARSE_METRIC_HH
