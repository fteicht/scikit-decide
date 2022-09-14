/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_CONCURRENCY_HH
#define SKDECIDE_CONCURRENCY_HH

#include "builders/domain/domain_type_importer.hh"
#include <list>
#include <type_traits>

namespace skdecide {

template <typename CompoundDomain, template <typename...> class CallingFeature,
          template <typename...> class DerivedFeature = CallingFeature>
class ConcurrencyTypesImporter {
public:
  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                event_list, EventList);
};

/**
 * @brief A domain must inherit this class if multiple events/actions can happen
 * in parallel.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class ParallelDomain {
public:
  /**
   * @brief Type of the sequence class used to store objects run in parallel
   * @tparam Args Type of events
   */
  template <typename... Args>
  using EventList = typename ConcurrencyTypesImporter<
      CompoundDomain, ParallelDomain,
      CompoundDomain::Features::template ConcurrencyDomain>::
      template import_event_list_type<std::list>::template test_with<
          char>::template result<Args...>;

  /**
   * @brief Proxy to the type of objects to be run in parallel
   * @tparam Args type of events
   * @tparam Args Additional sequence container template parameters
   */
  template <typename... Args> using ConcurrencyProxy = EventList<Args...>;

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature {
  public:
    typedef ParallelDomain<CompoundDomain> FeatureDomain;
  };
};

/**
 * @brief A domain must inherit this class if its events/actions are sequential
 * (non-parallel).
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class SequentialDomain {
private:
  template <typename E, typename... Args> struct SequentialDomainEventList {
    using type = E;
  };

  template <typename... Args>
  using EventListAlias = typename SequentialDomainEventList<Args...>::type;

public:
  /**
   * @brief Type of the sequence class used to store objects run in parallel
   * @tparam Args Type of events
   */
  template <typename... Args>
  using EventList =
      typename ConcurrencyTypesImporter<CompoundDomain, SequentialDomain>::
          template import_event_list_type<EventListAlias>::template test_with<
              char>::template result<Args...>;

  static_assert(
      std::is_same_v<EventList<char>, char>,
      "The domain type EventList<E> must be equal to E when the "
      "concurrency domain feature derives skdecide::SequentialDomain");

  /**
   * @brief Proxy to the type of objects to be run sequentially
   * @tparam Args Type of events
   */
  template <typename... Args> using ConcurrencyProxy = EventList<Args...>;

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature : public ParallelDomain<CompoundDomain>::Feature {
  public:
    typedef SequentialDomain<CompoundDomain> FeatureDomain;
  };
};

} // namespace skdecide

#endif // SKDECIDE_CONCURRENCY_HH
