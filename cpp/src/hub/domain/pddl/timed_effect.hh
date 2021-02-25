/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_TIMED_EFFECT_HH
#define SKDECIDE_PDDL_TIMED_EFFECT_HH

#include "unary_effect.hh"

namespace skdecide {

    namespace pddl {

        class AtStartEffect : public UnaryEffect<AtStartEffect> {
        public :
            static constexpr char class_name[] = "at start";

            typedef std::shared_ptr<AtStartEffect> Ptr;

            AtStartEffect() {}

            AtStartEffect(const Effect::Ptr& effect)
                : UnaryEffect<AtStartEffect>(effect) {}
            
            AtStartEffect(const AtStartEffect& other)
                : UnaryEffect<AtStartEffect>(other) {}
            
            AtStartEffect& operator= (const AtStartEffect& other) {
                dynamic_cast<UnaryEffect<AtStartEffect>&>(*this) = other;
                return *this;
            }
        };


        class AtEndEffect : public UnaryEffect<AtEndEffect> {
        public :
            static constexpr char class_name[] = "at end";

            typedef std::shared_ptr<AtEndEffect> Ptr;

            AtEndEffect() {}

            AtEndEffect(const Effect::Ptr& effect)
                : UnaryEffect<AtEndEffect>(effect) {}
            
            AtEndEffect(const AtEndEffect& other)
                : UnaryEffect<AtEndEffect>(other) {}
            
            AtEndEffect& operator= (const AtEndEffect& other) {
                dynamic_cast<UnaryEffect<AtEndEffect>&>(*this) = other;
                return *this;
            }
        };

        class AtTimeEffect : public UnaryEffect<AtTimeEffect> {
        public :
            static constexpr char class_name[] = "at";

            typedef std::shared_ptr<AtTimeEffect> Ptr;

            AtTimeEffect()
            : _time(std::make_shared<Number>(0)) {}

            AtTimeEffect(const Number::Ptr& time, const Effect::Ptr& effect)
                : UnaryEffect<AtTimeEffect>(effect), _time(time) {}
            
            AtTimeEffect(const AtTimeEffect& other)
                : UnaryEffect<AtTimeEffect>(other), _time(other._time) {}
            
            AtTimeEffect& operator= (const AtTimeEffect& other) {
                dynamic_cast<UnaryEffect<AtTimeEffect>&>(*this) = other;
                this->_time = other._time;
                return *this;
            }

            void set_time(const Number::Ptr& time) {
                _time = time;
            }

            const Number::Ptr& get_time() const {
                return _time;
            }

            virtual std::ostream& print(std::ostream& o) const {
                o << "(at " << *_time << " " << *_effect << ")";
                return o;
            }
        
        private :
            Number::Ptr _time;
        };

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_TIMED_EFFECT_HH
