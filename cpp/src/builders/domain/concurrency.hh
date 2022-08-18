/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_CONCURRENCY_HH
#define SKDECIDE_CONCURRENCY_HH

#include <list>

namespace skdecide {

/**
 * @brief A domain must inherit this class if multiple events/actions can happen
 * in parallel.
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam List Type of the sequence class used to store objects run in parallel
 * (std::list by default)
 */
template <typename DerivedCompoundDomain,
          template <typename...> class List = std::list>
class ParallelDomain {
public:
  /**
   * @brief Proxy to the type of objects to be run in parallel
   * @tparam T
   */
  template <typename T> using ConccurencyProxy = List<T>;
};

/**
 * @brief A domain must inherit this class if its events/actions are sequential
 * (non-parallel).
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename DerivedCompoundDomain> class SequentialDomain {
public:
  /**
   * @brief Proxy to the type of objects to be run sequentially
   * @tparam T
   */
  template <typename T> using ConccurencyProxy = T;
};

} // namespace skdecide

#endif // SKDECIDE_CONCURRENCY_HH
