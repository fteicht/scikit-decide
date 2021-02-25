/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_OPERATOR_CONTAINER_HH
#define SKDECIDE_PDDL_OPERATOR_CONTAINER_HH

#include "associative_container.hh"

namespace skdecide {

    namespace pddl {

        class Action;
        class DurativeAction;
        class Event;
        class Process;

        // Operator container

        template <typename Derived, typename Operator>
        class OperatorContainer : public AssociativeContainer<Derived, Operator> {
        public :
            typedef typename AssociativeContainer<Derived, Operator>::SymbolPtr OperatorPtr;
            typedef typename AssociativeContainer<Derived, Operator>::SymbolSet OperatorSet;

            OperatorContainer(const OperatorContainer& other)
                : AssociativeContainer<Derived, Operator>(other) {}
            
            OperatorContainer& operator=(const OperatorContainer& other) {
                dynamic_cast<AssociativeContainer<Derived, Operator>&>(*this) = other;
                return *this;
            }

            template <typename T>
            inline const OperatorPtr& add_operator(const T& op) {
                return AssociativeContainer<Derived, Operator>::add(op);
            }

            template <typename T>
            inline void remove_operator(const T& op) {
                AssociativeContainer<Derived, Operator>::remove(op);
            }

            template <typename T>
            inline const OperatorPtr& get_operator(const T& op) const {
                return AssociativeContainer<Derived, Operator>::get(op);
            }

            inline const OperatorSet& get_operators() const {
                return AssociativeContainer<Derived, Operator>::get_container();
            }
        
        protected :
            OperatorContainer() {}
        };


        // Action container
        
        template <typename Derived>
        class ActionContainer : public OperatorContainer<Derived, Action> {
        public :
            typedef typename OperatorContainer<Derived, Action>::OperatorPtr ActionPtr;
            typedef typename OperatorContainer<Derived, Action>::OperatorSet ActionSet;

            ActionContainer(const ActionContainer& other)
                : OperatorContainer<Derived, Action>(other) {}
            
            ActionContainer& operator=(const ActionContainer& other) {
                dynamic_cast<OperatorContainer<Derived, Action>&>(*this) = other;
                return *this;
            }

            template <typename T>
            inline const ActionPtr& add_action(const T& op) {
                return OperatorContainer<Derived, Action>::add_operator(op);
            }

            template <typename T>
            inline void remove_action(const T& op) {
                OperatorContainer<Derived, Action>::remove_operator(op);
            }

            template <typename T>
            inline const ActionPtr& get_action(const T& op) const {
                return OperatorContainer<Derived, Action>::get_operator(op);
            }

            inline const ActionSet& get_actions() const {
                return OperatorContainer<Derived, Action>::get_operators();
            }
        
        protected :
            ActionContainer() {}
        };


        // Durative action container
        
        template <typename Derived>
        class DurativeActionContainer : public OperatorContainer<Derived, DurativeAction> {
        public :
            typedef typename OperatorContainer<Derived, DurativeAction>::OperatorPtr DurativeActionPtr;
            typedef typename OperatorContainer<Derived, DurativeAction>::OperatorSet DurativeActionSet;

            DurativeActionContainer(const DurativeActionContainer& other)
                : OperatorContainer<Derived, DurativeAction>(other) {}
            
            DurativeActionContainer& operator=(const DurativeActionContainer& other) {
                dynamic_cast<OperatorContainer<Derived, DurativeAction>&>(*this) = other;
                return *this;
            }

            template <typename T>
            inline const DurativeActionPtr& add_durative_action(const T& op) {
                return OperatorContainer<Derived, DurativeAction>::add_operator(op);
            }

            template <typename T>
            inline void remove_durative_action(const T& op) {
                OperatorContainer<Derived, DurativeAction>::remove_operator(op);
            }

            template <typename T>
            inline const DurativeActionPtr& get_durative_action(const T& op) const {
                return OperatorContainer<Derived, DurativeAction>::get_operator(op);
            }

            inline const DurativeActionSet& get_durative_actions() const {
                return OperatorContainer<Derived, DurativeAction>::get_operators();
            }
        
        protected :
            DurativeActionContainer() {}
        };


        // Event container
        
        template <typename Derived>
        class EventContainer : public OperatorContainer<Derived, Event> {
        public :
            typedef typename OperatorContainer<Derived, Event>::OperatorPtr EventPtr;
            typedef typename OperatorContainer<Derived, Event>::OperatorSet EventSet;

            EventContainer(const EventContainer& other)
                : OperatorContainer<Derived, Event>(other) {}
            
            EventContainer& operator=(const EventContainer& other) {
                dynamic_cast<OperatorContainer<Derived, Event>&>(*this) = other;
                return *this;
            }

            template <typename T>
            inline const EventPtr& add_event(const T& op) {
                return OperatorContainer<Derived, Event>::add_operator(op);
            }

            template <typename T>
            inline void remove_event(const T& op) {
                OperatorContainer<Derived, Event>::remove_operator(op);
            }

            template <typename T>
            inline const EventPtr& get_event(const T& op) const {
                return OperatorContainer<Derived, Event>::get_operator(op);
            }

            inline const EventSet& get_events() const {
                return OperatorContainer<Derived, Event>::get_operators();
            }
        
        protected :
            EventContainer() {}
        };


        // Process container
        
        template <typename Derived>
        class ProcessContainer : public OperatorContainer<Derived, Process> {
        public :
            typedef typename OperatorContainer<Derived, Process>::OperatorPtr ProcessPtr;
            typedef typename OperatorContainer<Derived, Process>::OperatorSet ProcessSet;

            ProcessContainer(const ProcessContainer& other)
                : OperatorContainer<Derived, Process>(other) {}
            
            ProcessContainer& operator=(const ProcessContainer& other) {
                dynamic_cast<OperatorContainer<Derived, Process>&>(*this) = other;
                return *this;
            }

            template <typename T>
            inline const ProcessPtr& add_process(const T& op) {
                return OperatorContainer<Derived, Process>::add_operator(op);
            }

            template <typename T>
            inline void remove_process(const T& op) {
                OperatorContainer<Derived, Process>::remove_operator(op);
            }

            template <typename T>
            inline const ProcessPtr& get_process(const T& op) const {
                return OperatorContainer<Derived, Process>::get_operator(op);
            }

            inline const ProcessSet& get_processes() const {
                return OperatorContainer<Derived, Process>::get_operators();
            }
        
        protected :
            ProcessContainer() {}
        };

    } // namespace pddl
    
} // namespace skdecide

#endif // SKDECIDE_PDDL_OPERATOR_CONTAINER_HH
