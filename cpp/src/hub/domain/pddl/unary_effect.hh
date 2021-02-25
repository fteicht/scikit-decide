/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_UNARY_EFFECT_HH
#define SKDECIDE_PDDL_UNARY_EFFECT_HH

#include "effect.hh"

namespace skdecide {

    namespace pddl {

        template <typename Derived, typename Eff = Effect>
        class UnaryEffect : public Effect {
        public :
            typedef std::shared_ptr<UnaryEffect<Derived, Eff>> Ptr;

            UnaryEffect() {}

            UnaryEffect(const typename Eff::Ptr& effect)
                : _effect(effect) {}
            
            UnaryEffect(const UnaryEffect<Derived, Eff>& other)
                : _effect(other._effect) {}
            
            UnaryEffect<Derived, Eff>& operator= (const UnaryEffect<Derived, Eff>& other) {
                this->_effect = other._effect;
                return *this;
            }

            virtual ~UnaryEffect() {}

            void set_effect(const typename Eff::Ptr& effect) {
                _effect = effect;
            }

            const typename Eff::Ptr& get_effect() const {
                return _effect;
            }

            virtual std::ostream& print(std::ostream& o) const {
                o << "(" << Derived::class_name << " " << *_effect << ")";
                return o;
            }

        protected :
            typename Eff::Ptr _effect;
        };

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_UNARY_EFFECT_HH
