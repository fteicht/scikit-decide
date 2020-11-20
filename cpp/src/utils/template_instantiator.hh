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

template <template <typename ... Instantiations> struct FinalInstantiator,
          typename ... IntermediateInstantiators>
struct TemplateInstantiator {

    template <typename ... RemainingInstantiators>
    struct Impl {};

    template <template <typename ... AArgs> struct CurrentInstantiator,
              typename ... RemainingInstantiators>
    struct Impl {
        template <typename ... CurrentInstantiations>
        struct Recursion {
            template <typename ... Args>
            void operator(Args ... args) {
                CurrentInstantiator<Impl<RemainingInstantiators>, CurrentInstantiations>()(args);
            }
        };
    };

    template <>
    struct Impl {
        template <typename ... CurrentInstantiations>
        struct Recursion {
            template <typename ... Args>
            void operator(Args ... args) {
                FinalInstantiator<CurrentInstantiations>()(args);
            }
        };
    };

    template <typename ... FinalInstantiatorArgs>
    void operator(FinalInstantiator ... args) {
        Impl<IntermediateInstantiators>::Recursion<>()(args);
    }
};

} // namespace skdecide

#endif // SKDECIDE_TEMPLATE_INSTANTIATOR_HH
