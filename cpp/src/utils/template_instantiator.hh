/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_TEMPLATE_INSTANTIATOR_HH
#define SKDECIDE_TEMPLATE_INSTANTIATOR_HH

namespace skdecide {

/**
 * TemplateInstantiator is a recursive template instantiation helper class that
 * enables to selectively instantiate templates from a list of template selection
 * functors. The typical use case is when we want to instantiate a template
 * class object whose each template instantiation depends on a runtime test
 * of some variables. Without this template instantiation helper class,
 * such instantiations require cumbersome nested tests whose size exponentially
 * grows with the number of test cases (i.e. templates to selectively instantiate).
 * Below is an example use with two selection functors FirstSelector and
 * SecondSelector and a final template class instantiator Instantiator that
 * takes two template parameters (one non-templated type and one template class as
 * template parameters) whose actual instantiations result from internal tests in
 * FirstSelector and SecondSelector.
 * 
 * struct FirstSelector {
 *     bool _test;
 *     struct FirstCaseType {};
 *     struct SecondCaseType {};
 * 
 *     FirstSelector(bool test)
 *     : _test(first_selector_test) {}
 * 
 *     template <typename Propagator>
 *     struct Select {
 *         template <typename... Args>
 *         Select(FirstSelector& This, Args... args) {
 *             if (This._test) {
 *                 typename Propagator::template PushType<FirstCaseType>(args...);
 *             } else {
 *                 typename Propagator::template PushType<SecondCaseType>(args...);
 *             }
 *         }
 *     };
 * };
 *
 * struct SecondSelector {
 *     typedef enum {
 *         CASE_1,
 *         CASE_2,
 *         CASE_3
 *     } Cases;
 *     Cases _cases;
 * 
 *     template <typename T> struct Case1TemplateClass {};
 *     template <typename T> struct Case2TemplateClass {};
 *     template <typename T> struct Case3TemplateClass {};
 * 
 *     SecondSelector(Cases cases)
 *     : _cases(cases) {}
 * 
 *     template <typename Propagator>
 *     struct Select {
 *         template <typename... Args>
 *         Select(SecondSelector& This, Args... args) {
 *             switch (This._cases) {
 *                 case CASE_1 :
 *                     typename Propagator::template PushTemplate<Case1TemplateClass>(args...);
 *                     break;
 *                 case CASE_2 :
 *                     typename Propagator::template PushTemplate<Case2TemplateClass>(args...);
 *                     break;
 *                 case CASE_3 :
 *                     typename Propagator::template PushTemplate<Case3TemplateClass>(args...);
 *                     break;
 *             }
 *         }
 *     };
 * };
 * 
 * struct BaseClass {};
 * template <typename T, template <typename...> class C> struct TemplateClass : public BaseClass {
 *     C<T> my_object;  // just an example (not mandatory to use template template parameters)
 *     TemplateClass(int param1, double param2, bool param3) {}  // do something with those parameters and my_object
 * };
 *
 * struct Instantiator {
 *     std::unique_ptr<BaseClass>& _template_object;
 * 
 *     Instantiator(std::unique_ptr<BaseClass>& template_object)
 *     : _template_object(template_object) {}
 * 
 *     template <typename... TypeInstantiations>
 *       struct TypeList {
 *          template<template <typename...> class... TemplateInstantiations>
 *          struct TemplateList {
 *              struct Instantiate {
 *                  template <typename... Args>
 *                  Instantiate(Instantiator& This, Args... args) {
 *                      This._template_object = std::make_unique<TemplateClass<TypeInstantiations..., TemplateInstantiations...>>(args...);
 *                  }
 *              };
 *          };
 *     };
 * };
 *
 * std::unique_ptr<BaseClass> template_object;
 * bool first_selector_test = true;
 * SecondSelector::Cases second_selector_case = CASE_3;
 * TemplateInstantiator::select(FirstSelector(first_selector_test),           // static call to TemplateInstantiator::select(...)
 *                              SecondSelector(second_selector_case),
 *                              Instantiator(template_object)).instantiate(   // object call to <InternalObject>.instantiate(...)
 *                                 // here we have the parameters of Instantiator::Instantiate(...) constructor
 *                                 // which are forwarded to TemplateClass' constructor
 *                                 2, -1.3, false
 *                              );
 * // Now the actual type of template_object's pointer is TemplateClass<FirstSelector::FirstCaseType, SecondSelector::Case3TemplateClass>
 */

struct TemplateInstantiator {

    template <typename... Instantiators>
    struct Implementation {};

    template <typename FirstInstantiator,
              typename SecondInstantiator,
              typename... RemainingInstantiators>
    struct Implementation<FirstInstantiator, SecondInstantiator, RemainingInstantiators...> {
        FirstInstantiator _current_instantiator;
        Implementation<SecondInstantiator, RemainingInstantiators...> _remaining_instantiators;

        Implementation(FirstInstantiator&& first_instantiator,
                       SecondInstantiator&& second_instantiator,
                       RemainingInstantiators&&... remaining_instantiators)
            : _current_instantiator(first_instantiator),
              _remaining_instantiators(std::forward<SecondInstantiator>(second_instantiator),
                                       std::forward<RemainingInstantiators>(remaining_instantiators)...) {}
        
        template <typename... CurrentTypeInstantiations>
        struct TypeInstantiationPropagator {
            template <template <typename...> class... CurrentTemplateInstantiations>
            struct TemplateInstantiationPropagator {
                template <typename NewInstantiation>
                using PushType = typename TypeInstantiationPropagator<CurrentTypeInstantiations..., NewInstantiation>::
                        template TemplateInstantiationPropagator<CurrentTemplateInstantiations...>;

                template <template <typename...> class NewInstantiation>
                using PushTemplate = typename TypeInstantiationPropagator<CurrentTypeInstantiations...>::
                        template TemplateInstantiationPropagator<CurrentTemplateInstantiations..., NewInstantiation>;

                template <typename... Args>
                TemplateInstantiationPropagator(Implementation& impl, Args... args) {
                    typedef typename FirstInstantiator::template Select<
                        typename Implementation<SecondInstantiator, RemainingInstantiators...>::template TypeInstantiationPropagator<
                                CurrentTypeInstantiations...>::template TemplateInstantiationPropagator<CurrentTemplateInstantiations...>> Select;
                        Select(impl._current_instantiator, impl._remaining_instantiators, args...);
                }
            };
        };

        template <typename... Args>
        void instantiate(Args... args) {
            typedef typename TypeInstantiationPropagator<>::template TemplateInstantiationPropagator<> TIP;
            TIP(*this, args...);
        }
    };

    template <typename FinalInstantiator>
    struct Implementation<FinalInstantiator> {
        FinalInstantiator _final_instantiator;

        Implementation(FinalInstantiator&& final_instantiator)
            : _final_instantiator(final_instantiator) {}
        
        template <typename... CurrentTypeInstantiations>
        struct TypeInstantiationPropagator {
            template <template <typename...> class... CurrentTemplateInstantiations>
            struct TemplateInstantiationPropagator {
                template <typename NewInstantiation>
                using PushType = typename TypeInstantiationPropagator<CurrentTypeInstantiations..., NewInstantiation>::
                        template TemplateInstantiationPropagator<CurrentTemplateInstantiations...>;

                template <template <typename...> class NewInstantiation>
                using PushTemplate = typename TypeInstantiationPropagator<CurrentTypeInstantiations...>::
                        template TemplateInstantiationPropagator<CurrentTemplateInstantiations..., NewInstantiation>;

                template <typename... Args>
                TemplateInstantiationPropagator(Implementation& impl, Args... args) {
                typedef typename FinalInstantiator::template TypeList<CurrentTypeInstantiations...>
                                                  ::template TemplateList<CurrentTemplateInstantiations...>::Instantiate Instantiate;
                 Instantiate(impl._final_instantiator, args...);
                }
            };

            template <>
            struct TemplateInstantiationPropagator<> {
                template <typename NewInstantiation>
                using PushType = typename TypeInstantiationPropagator<CurrentTypeInstantiations..., NewInstantiation>::
                        template TemplateInstantiationPropagator<>;

                template <template <typename...> class NewInstantiation>
                using PushTemplate = typename TypeInstantiationPropagator<CurrentTypeInstantiations...>::
                        template TemplateInstantiationPropagator<NewInstantiation>;

                template <typename... Args>
                TemplateInstantiationPropagator(Implementation& impl, Args... args) {
                typedef typename FinalInstantiator::template Instantiate<CurrentTypeInstantiations...> Instantiate;
                Instantiate(impl._final_instantiator, args...);
                }
            };
        };

        template <>
        struct TypeInstantiationPropagator<> {
            template <template <typename...> class... CurrentTemplateInstantiations>
            struct TemplateInstantiationPropagator {
                template <typename NewInstantiation>
                using PushType = typename TypeInstantiationPropagator<NewInstantiation>::
                        template TemplateInstantiationPropagator<CurrentTemplateInstantiations...>;

                template <template <typename...> class NewInstantiation>
                using PushTemplate = typename TypeInstantiationPropagator<>::
                        template TemplateInstantiationPropagator<CurrentTemplateInstantiations..., NewInstantiation>;

                template <typename... Args>
                TemplateInstantiationPropagator(Implementation& impl, Args... args) {
                typedef typename FinalInstantiator::template Instantiate<CurrentTemplateInstantiations...> Instantiate;
                Instantiate(impl._final_instantiator, args...);
                }
            };
        };
    };

    template <typename... Instantiators>
    static Implementation<Instantiators...> select(Instantiators&&... instantiators) {
       return Implementation<Instantiators...>(std::forward<Instantiators>(instantiators)...);
    }
};

} // namespace skdecide

#endif // SKDECIDE_TEMPLATE_INSTANTIATOR_HH
