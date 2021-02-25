/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef SKDECIDE_PDDL_PARSE_EFFECT_HH
#define SKDECIDE_PDDL_PARSE_EFFECT_HH

#include "pegtl.hpp"

#include "parser_state.hh"
#include "parser_action.hh"

#include "parse_aggregation.hh"
#include "parse_predicate_head.hh"
#include "parse_negation.hh"
#include "parse_implication.hh"
#include "parse_quantification.hh"
#include "parse_assignment.hh"

namespace pegtl = TAO_PEGTL_NAMESPACE;  // NOLINT

namespace skdecide {

    namespace pddl {

        namespace parser {

            struct effect;

            struct p_effect : pegtl::sor<
                                    negation<predicate_head<effect>>,
                                    assignment<expression>,
                                    predicate_head<effect>
                              > {};

            struct c_effect : pegtl::sor<
                                    aggregation<Conjunction, p_effect>,
                                    implication<effect>,
                                    quantification<Universal, effect>,
                                    p_effect
                              > {};
            
            struct p_effect_da : pegtl::sor<
                                    negation<predicate_head<effect>>,
                                    assignment<durative_expression>,
                                    predicate_head<effect>
                                 > {};
            
            struct a_effect_da : pegtl::sor<
                                    aggregation<Conjunction, p_effect_da>,
                                    p_effect_da
                                 > {};
            
            struct empty_effect : pegtl::seq<
                                    pegtl::one<'('>,
                                    ignored,
                                    pegtl::opt<
                                        pegtl::seq<
                                            keyword<'a', 'n', 'd'>,
                                            ignored
                                        >
                                    >,
                                    pegtl::one<')'>
                                  > {};
            
            template <> struct action<empty_effect> {
                static void apply0(state& s) {
                    s.effect = std::make_shared<ConjunctionEffect>();
                }
            };
            
            struct effect : pegtl::sor<
                                empty_effect,
                                aggregation<Conjunction, c_effect>,
                                negation<predicate_head<effect>>,
                                implication<effect>,
                                quantification<Universal, effect>,
                                predicate_head<effect>
                            > {};
            
            struct process_effect : pegtl::sor<
                                        empty_effect,
                                        aggregation<Conjunction, process_effect>,
                                        operation_expression<IncreaseOperator, timed_expression>,
                                        operation_expression<DecreaseOperator, timed_expression>
                                    > {};
            
            struct timed_effect : pegtl::sor<
                                        empty_effect,
                                        aggregation<Conjunction, timed_effect>,
                                        quantification<Universal, timed_effect>,
                                        implication<timed_effect>,
                                        unary_constraint<AtStartEffectOperator>,
                                        unary_constraint<AtEndEffectOperator>,
                                        operation_expression<IncreaseOperator, timed_expression>,
                                        operation_expression<DecreaseOperator, timed_expression>,
                                        assignment<expression>
                                  > {};

        } // namespace parser

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_PARSE_EFFECT_HH
