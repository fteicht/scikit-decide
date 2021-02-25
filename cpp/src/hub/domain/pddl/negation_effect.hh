/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_NEGATION_EFFECT_HH
#define SKDECIDE_PDDL_NEGATION_EFFECT_HH

#include "unary_effect.hh"
#include "predicate_effect.hh"

namespace skdecide {

    namespace pddl {

        class NegationEffect : public UnaryEffect<NegationEffect, PredicateEffect> {
        public :
            static constexpr char class_name[] = "not";

            typedef std::shared_ptr<NegationEffect> Ptr;

            NegationEffect() {}

            NegationEffect(const PredicateEffect::Ptr& effect)
                : UnaryEffect<NegationEffect, PredicateEffect>(effect) {}
            
            NegationEffect(const NegationEffect& other)
                : UnaryEffect<NegationEffect, PredicateEffect>(other) {}
            
            NegationEffect& operator= (const NegationEffect& other) {
                dynamic_cast<UnaryEffect<NegationEffect, PredicateEffect>&>(*this) = other;
                return *this;
            }

            virtual ~NegationEffect() {}
        };

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_NEGATION_EFFECT_HH
