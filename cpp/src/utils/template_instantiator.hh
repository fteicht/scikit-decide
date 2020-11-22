/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_TEMPLATE_INSTANTIATOR_HH
#define SKDECIDE_TEMPLATE_INSTANTIATOR_HH

namespace skdecide {

/**
 * This is a recursive template instantiation helper that enables to
 * selectively instantiate templates from a list of template selection
 * functors. The typical use case is when we want to instantiate a template
 * class object whose each template instantiation depends on a runtime test
 * of some variables. Without this template instantiation helper class,
 * such instantiations require cumbersome nested tests whose size exponentially
 * grows with the number of test cases (i.e. templates to selectively instantiate).
 * Below is an example use with two selection functors FirstSelector and
 * SecondSelector and a final template class instantiator Instantiator that
 * takes two template parameters whose actual instantiations result from
 * internal tests in FirstSelector and SecondSelector.
 * 
 * template <template <typename ... Instantiations> struct Instantiator,
 *           typename ... CurrentInstantiations>
 * struct FirstSelector {
 *     template <typename ... Args>
 *     void operator(Args... args) {
 *         if (first_case) {
 *             Instantiator::Recursion<CurrentInstantiations..., FirstTemplateFirstCase>()(args);
 *         } else {
 *             Instantiator::Recursion<CurrentInstantiations..., FirstTemplateSecondCase>()(args);
 *         }
 *     }
 * };
 *
 * template <template <typename ... Instantiations> struct Instantiator,
 *           typename ... CurrentInstantiations>
 * struct SecondSelector {
 *     template <typename ... Args>
 *     void operator(Args... args) {
 *         switch (cases) {
 *             case case1:
 *                 Instantiator::Recursion<CurrentInstantiations..., SecondTemplateCase1>()(args);
 *                 break;
 *
 *             case case 2:
 *                 Instantiator::Recursion<CurrentInstantiations..., SecondTemplateCase2>()(args);
 *                 break;
 *         }
 *     }
 * };
 *
 * template <typename ... Instantiations>
 * struct Instantiator {
 *     template <typename ... Args>
 *     void operator(Args ... args) {
 *         _myobject = MyClass<Instantiations>(args);
 *     }
 * };
 *
 * TemplateInstantiator<Instantiator, FirstSelector, SecondSelector>()(instantiation_args);
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
                static void propagate(Implementation& impl, Args... args) {
                    FirstInstantiator::Propagate<
                        typename Implementation<SecondInstantiator, RemainingInstantiators...>::template TypeInstantiationPropagator<
                                CurrentTypeInstantiations...>::template TemplateInstantiationPropagator<CurrentTemplateInstantiations...>>(
                            impl._current_instantiator,
                            impl._remaining_instantiators,
                            args...);
                }
            };
        };

        template <typename... Args>
        void instantiate(Args... args) {
            TypeInstantiationPropagator<>::TemplateInstantiationPropagator<>::propagate(*this, args...);
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
                static void propagate(Implementation& impl, Args... args) {
                FinalInstantiator::TypeList<CurrentTypeInstantiations...>
                                 ::TemplateList<CurrentTemplateInstantiations...>::Instantiate(
                                     impl._final_instantiator, args...);
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
                static void propagate(Implementation& impl, Args... args) {
                FinalInstantiator::Instantiate<CurrentTypeInstantiations...>(
                                     impl._final_instantiator, args...);
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
                static void propagate(Implementation& impl, Args... args) {
                FinalInstantiator::Instantiate<CurrentTemplateInstantiations...>(
                                     impl._final_instantiator, args...);
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
