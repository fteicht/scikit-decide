/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_OPERATOR_HH
#define SKDECIDE_PDDL_OPERATOR_HH

#include "identifier.hh"
#include "variable_container.hh"
#include "binary_effect.hh"

namespace skdecide {

    namespace pddl {

        template <typename Derived>
        class Operator : public Identifier,
                         public VariableContainer<Derived>,
                         public BinaryEffect { // BinaryEffect brings in precondition and effect logics
        public :
            typedef std::shared_ptr<Operator<Derived>> Ptr;
            typedef typename VariableContainer<Derived>::VariablePtr VariablePtr;
            typedef typename VariableContainer<Derived>::VariableVector VariableVector;

            Operator(const std::string& name)
                : Identifier(name) {}
            
            Operator(const std::string& name,
                    const Formula::Ptr& precondition,
                    const Effect::Ptr& effect)
                : Identifier(name), BinaryEffect(precondition, effect) {}

            Operator(const Operator<Derived>& other)
                : Identifier(other),
                  VariableContainer<Derived>(other),
                  BinaryEffect(other) {}

            Operator<Derived>& operator=(const Operator<Derived>& other) {
                dynamic_cast<Identifier&>(*this) = other;
                dynamic_cast<VariableContainer<Derived>&>(*this) = other;
                dynamic_cast<BinaryEffect&>(*this) = other;
                return *this;
            }

            virtual ~Operator() {}

            virtual std::ostream& print(std::ostream& o) const {
                o << "(:" << std::string(Derived::class_name) << ' ' << static_cast<const Derived*>(this)->get_name() << std::endl;
                o << ":parameters (";
                for (const auto & v : this->get_variables()) {
                    o << " " << *v;
                }
                o << " )" << std::endl;
                o << ":precondition " << *(this->get_condition()) << std::endl;
                o << ":effect " << *(this->get_effect()) << std::endl;
                o << ")";
                return o;
            }
        };

        class Action : public Operator<Action> {
        public :
            static constexpr char class_name[] = "action";

            typedef std::shared_ptr<Action> Ptr;
            typedef VariableContainer<Action>::VariablePtr VariablePtr;
            typedef VariableContainer<Action>::VariableVector VariableVector;

            Action(const std::string& name)
                : Operator<Action>(name) {}
            
            Action(const std::string& name,
                   const Formula::Ptr& precondition,
                   const Effect::Ptr& effect)
                : Operator<Action>(name, precondition, effect) {}

            Action(const Action& other)
                : Operator<Action>(other) {}

            Action& operator=(const Action& other) {
                dynamic_cast<Operator<Action>&>(*this) = other;
                return *this;
            }

            virtual ~Action() {}
        };

        // Action printing operator
        inline std::ostream& operator<<(std::ostream& o, const Action& a) {
            return a.print(o);
        }


        class DurativeAction : public Operator<DurativeAction> {
        public :
            static constexpr char class_name[] = "durative-action";

            typedef std::shared_ptr<DurativeAction> Ptr;
            typedef VariableContainer<DurativeAction>::VariablePtr VariablePtr;
            typedef VariableContainer<DurativeAction>::VariableVector VariableVector;

            DurativeAction(const std::string& name)
                : Operator<DurativeAction>(name) {}
            
            DurativeAction(const std::string& name,
                           const Formula::Ptr& duration_constraint,
                           const Formula::Ptr& precondition,
                           const Effect::Ptr& effect)
                : Operator<DurativeAction>(name, precondition, effect),
                  _duration_constraint(duration_constraint) {}

            DurativeAction(const DurativeAction& other)
                : Operator<DurativeAction>(other),
                  _duration_constraint(other._duration_constraint) {}

            DurativeAction& operator=(const DurativeAction& other) {
                dynamic_cast<Operator<DurativeAction>&>(*this) = other;
                this->_duration_constraint = other._duration_constraint;
                return *this;
            }

            virtual ~DurativeAction() {}

            void set_duration_constraint(const Formula::Ptr& duration_constraint) {
                _duration_constraint = duration_constraint;
            }

            const Formula::Ptr& get_duration_constraint() const {
                return _duration_constraint;
            }

            virtual std::ostream& print(std::ostream& o) const {
                o << "(:durative-action " << this->get_name() << std::endl;
                o << ":parameters (";
                for (const auto & v : get_variables()) {
                    o << " " << *v;
                }
                o << " )" << std::endl;
                o << ":duration " << *(this->get_duration_constraint()) << std::endl;
                o << ":precondition " << *(this->get_condition()) << std::endl;
                o << ":effect " << *(this->get_effect()) << std::endl;
                o << ")";
                return o;
            }
        
        private :
            Formula::Ptr _duration_constraint;
        };

        // Durative action printing operator
        inline std::ostream& operator<<(std::ostream& o, const DurativeAction& da) {
            return da.print(o);
        }


        class Event : public Operator<Event> {
        public :
            static constexpr char class_name[] = "event";

            typedef std::shared_ptr<Event> Ptr;
            typedef VariableContainer<Event>::VariablePtr VariablePtr;
            typedef VariableContainer<Event>::VariableVector VariableVector;

            Event(const std::string& name)
                : Operator<Event>(name) {}
            
            Event(const std::string& name,
                  const Formula::Ptr& precondition,
                  const Effect::Ptr& effect)
                : Operator<Event>(name, precondition, effect) {}

            Event(const Event& other)
                : Operator<Event>(other) {}

            Event& operator=(const Event& other) {
                dynamic_cast<Operator<Event>&>(*this) = other;
                return *this;
            }

            virtual ~Event() {}
        };

        // Event printing operator
        inline std::ostream& operator<<(std::ostream& o, const Event& e) {
            return e.print(o);
        }


        class Process : public Operator<Process> {
        public :
            static constexpr char class_name[] = "process";

            typedef std::shared_ptr<Process> Ptr;
            typedef VariableContainer<Process>::VariablePtr VariablePtr;
            typedef VariableContainer<Process>::VariableVector VariableVector;

            Process(const std::string& name)
                : Operator<Process>(name) {}
            
            Process(const std::string& name,
                    const Formula::Ptr& precondition,
                    const Effect::Ptr& effect)
                : Operator<Process>(name, precondition, effect) {}

            Process(const Process& other)
                : Operator<Process>(other) {}

            Process& operator=(const Process& other) {
                dynamic_cast<Operator<Process>&>(*this) = other;
                return *this;
            }

            virtual ~Process() {}
        };

        // Process printing operator
        inline std::ostream& operator<<(std::ostream& o, const Process& p) {
            return p.print(o);
        }

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_OPERATOR_HH