/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DOMAIN_TYPE_IMPORTER_HH
#define DOMAIN_TYPE_IMPORTER_HH

#define DOMAIN_SIMPLE_TYPE_IMPORTER(CompoundDomain, CallingFeature,            \
                                    DerivedFeature, EntityName, TypeName)      \
  template <typename DefaultType = std::nullopt_t>                             \
  struct import_##EntityName##_type {                                          \
    template <typename T, typename Found = void> struct try_import {           \
      template <typename CF, typename DF, typename Enable = void>              \
      struct delegate_import;                                                  \
                                                                               \
      template <typename CF, typename DF>                                      \
      struct delegate_import<CF, DF,                                           \
                             std::enable_if_t<std::is_same_v<CF, DF>>> {       \
        typedef DefaultType result;                                            \
      };                                                                       \
                                                                               \
      template <typename CF, typename DF>                                      \
      struct delegate_import<CF, DF,                                           \
                             std::enable_if_t<!std::is_same_v<CF, DF>>> {      \
        typedef typename DF::TypeName result;                                  \
      };                                                                       \
                                                                               \
      typedef typename delegate_import<CallingFeature<CompoundDomain>,         \
                                       DerivedFeature<CompoundDomain>>::result \
          result;                                                              \
    };                                                                         \
                                                                               \
    template <typename T>                                                      \
    struct try_import<T, std::void_t<typename T::TypeName>> {                  \
      typedef typename T::TypeName result;                                     \
    };                                                                         \
                                                                               \
    typedef                                                                    \
        typename try_import<typename CompoundDomain::Types>::result result;    \
  };

#define DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature,          \
                                      DerivedFeature, EntityName, TypeName)    \
  template <template <typename...> class DefaultType = std::void_t>            \
  struct import_##EntityName##_type {                                          \
    template <typename T, typename Found = void> struct try_import {           \
      template <typename CF, typename DF, typename Enable = void>              \
      struct delegate_import;                                                  \
                                                                               \
      template <typename CF, typename DF>                                      \
      struct delegate_import<CF, DF,                                           \
                             std::enable_if_t<std::is_same_v<CF, DF>>> {       \
        template <typename... Args> using result = DefaultType<Args...>;       \
      };                                                                       \
                                                                               \
      template <typename CF, typename DF>                                      \
      struct delegate_import<CF, DF,                                           \
                             std::enable_if_t<!std::is_same_v<CF, DF>>> {      \
        template <typename... Args>                                            \
        using result = typename DF::template TypeName<Args...>;                \
      };                                                                       \
                                                                               \
      template <typename... Args>                                              \
      using result = typename delegate_import<                                 \
          CallingFeature<CompoundDomain>,                                      \
          DerivedFeature<CompoundDomain>>::template result<Args...>;           \
    };                                                                         \
                                                                               \
    template <typename T>                                                      \
    struct try_import<T, std::void_t<typename T::template TypeName<char>>> {   \
      template <typename... Args>                                              \
      using result = typename T::template TypeName<Args...>;                   \
    };                                                                         \
                                                                               \
    template <typename... Args>                                                \
    using result = typename try_import<                                        \
        typename CompoundDomain::Types>::template result<Args...>;             \
  };

#endif // DOMAIN_TYPE_IMPORTER_HH
