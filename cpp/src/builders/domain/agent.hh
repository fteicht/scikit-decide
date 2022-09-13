/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_AGENT_HH
#define SKDECIDE_AGENT_HH

#include <cstddef>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include "domain_type_importer.hh"
#include "utils/associative_container_deducer.hh"

namespace skdecide {

template <typename CompoundDomain, template <typename...> class CallingFeature,
          template <typename...> class DerivedFeature = CallingFeature>
class AgentTypesImporter {
public:
  DOMAIN_SIMPLE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                              agent, AgentType);

  DOMAIN_TEMPLATE_TYPE_IMPORTER(CompoundDomain, CallingFeature, DerivedFeature,
                                agent_dict, AgentDict);
};

/**
 * @brief A domain must inherit this class if it is multi-agent (i.e hosting
 * multiple independent agents).
 * Agents are identified by (string) agent names.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class MultiAgentDomain {
public:
  /**
   * @brief The type of agents
   */
  typedef typename AgentTypesImporter<
      CompoundDomain, MultiAgentDomain,
      CompoundDomain::Features::template AgentDomain>::
      template import_agent_type<>::result AgentType;

  static_assert(!std::is_same_v<MultiAgentDomain<CompoundDomain>,
                                typename CompoundDomain::Features::
                                    template AgentDomain<CompoundDomain>> ||
                    (!std::is_same_v<AgentType, std::nullopt_t> &&
                     !std::is_same_v<AgentType, std::nullptr_t>),
                "The domain types must define AgentType when the "
                "agent domain feature is skdecide::MultiAgentDomain");

private:
  template <typename... Args>
  using AgentMapTypeDeducer = typename MapTypeDeducer<Args...>::Map;

public:
  /**
   * @brief Type of the dictionary class
   */
  template <typename... Args>
  using AgentDict = typename AgentTypesImporter<
      CompoundDomain, MultiAgentDomain,
      CompoundDomain::Features::template AgentDomain>::
      template import_agent_dict_type<AgentMapTypeDeducer>::template test_with<
          AgentType, char>::template result<Args...>;

  /**
   * @brief Proxy to the type of objects stored by agents
   * For multi agents, this is a dictionary from agent types to the objects
   * mapped from agents
   * @tparam T
   */
  template <typename V, typename... Args>
  using AgentProxy = AgentDict<AgentType, V, Args...>;

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature {
  public:
    typedef MultiAgentDomain<CompoundDomain> FeatureDomain;
  };
};

/**
 * @brief A domain must inherit this class if it is single-agent (i.e hosting
 * only one agent).
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class SingleAgentDomain {
public:
  /**
   * @brief The type of agents
   */
  typedef typename AgentTypesImporter<CompoundDomain, SingleAgentDomain>::
      template import_agent_type<std::nullptr_t>::result AgentType;

  static_assert(std::is_same_v<AgentType, std::nullptr_t>,
                "The domain type AgentType must be equal to std::nullptr_t "
                "when the agent domain feature is skdecide::SingleAgentDomain");

private:
  template <typename K, typename V, typename... Args> struct SingleAgentDict {
    using type = V;
  };

  template <typename... Args>
  using SingleAgentDictAlias = typename SingleAgentDict<Args...>::type;

public:
  /**
   * @brief Type of the dictionary class
   */
  template <typename... Args>
  using AgentDict = typename AgentTypesImporter<CompoundDomain,
                                                SingleAgentDomain>::
      template import_agent_dict_type<SingleAgentDictAlias>::template test_with<
          std::nullptr_t, char>::template result<Args...>;

  static_assert(std::is_same_v<AgentDict<std::nullptr_t, char>, char>,
                "The domain type AgentDict<K, V> must be equal to V when the "
                "agent domain feature is skdecide::SingleAgentDomain");

  /**
   * @brief Proxy to the type of objects stored by this agent.
   * For single agents, this is the template parameter type itself
   * @tparam T
   */
  template <typename T> using AgentProxy = T;

  /**
   * @brief Feature class where the actual methods are defined
   */
  class Feature : public MultiAgentDomain<CompoundDomain>::Feature {
  public:
    typedef SingleAgentDomain<CompoundDomain> FeatureDomain;
  };
};

} // namespace skdecide

#endif // SKDECIDE_AGENT_HH
