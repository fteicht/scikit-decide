/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_AGENT_HH
#define SKDECIDE_AGENT_HH

#include <unordered_map>

namespace skdecide {

/**
 * @brief A domain must inherit this class if it is multi-agent (i.e hosting
 * multiple independent agents).
 * Agents are identified by (string) agent names.
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 * @tparam Dict Type of the dictionary class (std::unordered_map by default)
 */
template <typename DerivedCompoundDomain,
          template <typename...> class Dict = std::unordered_map>
class MultiAgentDomain {
public:
  /**
   * @brief Proxy to the type of objects stored by agents
   * For multi agents, this is a dictionary from agent types to the objects
   * mapped from agents
   * @tparam T
   */
  template <typename T>
  using AgentProxy = Dict<typename DerivedCompoundDomain::Agent, T>;
};

/**
 * @brief A domain must inherit this class if it is single-agent (i.e hosting
 * only one agent).
 * @tparam DerivedCompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename DerivedCompoundDomain> class SingleAgentDomain {
public:
  /**
   * @brief Proxy to the type of objects stored by this agent.
   * For single agents, this is the template parameter type itself
   * @tparam T
   */
  template <typename T> using AgentProxy = T;
};

} // namespace skdecide

#endif // SKDECIDE_AGENT_HH
