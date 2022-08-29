/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_CONCURRENCY_HH
#define SKDECIDE_CONCURRENCY_HH

#include <list>
#include <type_traits>

namespace skdecide {

/**
 * @brief A domain must inherit this class if multiple events/actions can happen
 * in parallel.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class ParallelDomain {
private:
  template <class, class, class = void> struct define_event_list;

  template <class D, class T>
  struct define_event_list<
      D, T,
      std::enable_if_t<std::is_same_v<ParallelDomain<CompoundDomain>, D>>> {

    template <class, class = void> struct try_import_from_domain_types {
      template <typename... Args> using result = std::list<Args...>;
    };

    template <class TT>
    struct try_import_from_domain_types<
        TT, std::void_t<typename TT::template EventList<char>>> {
      template <typename... Args>
      using result = typename TT::template EventList<Args...>;
    };

    template <typename... Args>
    using result =
        typename try_import_from_domain_types<T>::template result<Args...>;
  };

  template <class D, class T>
  struct define_event_list<
      D, T,
      std::enable_if_t<!std::is_same_v<ParallelDomain<CompoundDomain>, D>>> {
    template <typename E, typename... Args> using result = E;
  };

public:
  /**
   * @brief Type of the sequence class used to store objects run in parallel
   * @tparam E Type of events
   */
  template <typename E, typename... Args>
  using EventList = typename define_event_list<
      typename CompoundDomain::Features::template ConcurrencyDomain<
          CompoundDomain>,
      typename CompoundDomain::Types>::template result<E, Args...>;

  /**
   * @brief Proxy to the type of objects to be run in parallel
   * @tparam E type of events
   * @tparam Args Additional sequence container template parameters
   */
  template <typename E, typename... Args>
  using ConcurrencyProxy = EventList<E, Args...>;
};

/**
 * @brief A domain must inherit this class if its events/actions are sequential
 * (non-parallel).
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain>
class SequentialDomain : public ParallelDomain<CompoundDomain> {
private:
  template <class, class = void> struct check_event_list : std::true_type {};

  template <class T>
  struct check_event_list<T, std::void_t<typename T::template EventList<char>>>
      : std::conditional<
            std::is_same_v<char, typename T::template EventList<char>>,
            std::true_type, std::false_type>::type {};

  static_assert(
      !std::is_same_v<SequentialDomain<CompoundDomain>,
                      typename CompoundDomain::Features::
                          template ConcurrencyDomain<CompoundDomain>> ||
          check_event_list<typename CompoundDomain::Types>::value,
      "The domain type EventList<E> must be equal to E when the "
      "concurrency domain feature is skdecide::SequentialDomain");

public:
  /**
   * @brief Type of events
   * @tparam E Type of events
   */
  template <typename E, typename... Args> using EventList = E;

  /**
   * @brief Proxy to the type of objects to be run sequentially
   * @tparam E Type of events
   */
  template <typename E, typename... Args>
  using ConcurrencyProxy = EventList<E, Args...>;
};

} // namespace skdecide

#endif // SKDECIDE_CONCURRENCY_HH
