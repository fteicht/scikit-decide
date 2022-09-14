/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_RENDERABILITY_HH
#define SKDECIDE_RENDERABILITY_HH

#include "core.hh"
#include "memory.hh"

namespace skdecide {

/**
 * @brief A domain must inherit this class if it can be rendered with any kind
 * of visualization.
 *
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class RenderableDomain {
  static_assert(
      std::is_base_of<typename HistoryDomain<CompoundDomain>::Feature,
                      typename CompoundDomain::Features::template MemoryDomain<
                          CompoundDomain>::Feature>::value,
      "The memory domain feature must be derived from "
      "skdecide::HistoryDomain<CompoundDomain>::Feature");

public:
  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature {
  public:
    typedef typename CompoundDomain::Features::
        template AgentDomain<CompoundDomain>::template AgentProxy<
            typename CompoundDomain::Features::template ObservabilityDomain<
                CompoundDomain>::AgentState>
            CompoundState;
    typedef typename CompoundDomain::Features::template MemoryDomain<
        CompoundDomain>::template MemoryProxy<CompoundState>
        CompoundMemory;

    /**
     * @brief Compute a visual render of the given memory (state or history).
     *
     * @param memory The memory to consider.
     */
    virtual void render(const CompoundMemory &memory) = 0;

    /**
     * @brief Compute a visual render of the internal memory (state or history).
     */
    inline void render() { render(this->_memory); }
  };
};

} // namespace skdecide

#endif // SKDECIDE_RENDERABILITY_HH
