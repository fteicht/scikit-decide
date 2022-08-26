/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_AGENT_HH
#define SKDECIDE_AGENT_HH

#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include "utils/associative_container_deducer.hh"

namespace skdecide {

/**
 * @brief A domain must inherit this class if it is multi-agent (i.e hosting
 * multiple independent agents).
 * Agents are identified by (string) agent names.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class MultiAgentDomain {
public:
  template <class, class, class = void> struct define_agent_type;

  template <class D, class T>
  struct define_agent_type<
      D, T,
      std::enable_if_t<std::is_same_v<MultiAgentDomain<CompoundDomain>, D>>> {
    template <class, class = void> struct check_types {
      static_assert(!std::is_same_v<MultiAgentDomain<CompoundDomain>, D>,
                    "The domain types must define AgentType when the "
                    "agent domain feature is "
                    "skdecide::MultiAgentDomain");
    };

    template <class TT>
    struct check_types<TT, std::void_t<typename TT::AgentType>> {
      typedef typename TT::AgentType result;
    };

    typedef typename check_types<T>::result result;
  };

  template <class D, class T>
  struct define_agent_type<
      D, T,
      std::enable_if_t<!std::is_same_v<MultiAgentDomain<CompoundDomain>, D>>> {
    typedef std::nullptr_t result;
  };

  /**
   * @brief The type of agents
   */
  typedef typename define_agent_type<
      typename CompoundDomain::Features::template AgentDomain<CompoundDomain>,
      typename CompoundDomain::Types>::result AgentType;

  template <class, class, class = void> struct define_agent_dict;

  template <class D, class T>
  struct define_agent_dict<
      D, T,
      std::enable_if_t<std::is_same_v<MultiAgentDomain<CompoundDomain>, D>>> {

    template <class, class = void> struct check_types {
      template <typename... Args> using result = typename MapTypeDeducer<Args...>::Map;
    };

    template <class TT>
    struct check_types<
        TT, std::void_t<typename TT::template AgentDict<int, char>>> {
      template <typename... Args>
      using result = typename TT::template AgentDict<Args...>;
    };

    template <typename... Args>
    using result = typename check_types<T>::template result<Args...>;
  };

  template <class D, class T>
  struct define_agent_dict<
      D, T,
      std::enable_if_t<!std::is_same_v<MultiAgentDomain<CompoundDomain>, D>>> {
    template <typename K, typename V, typename... Args> using result = V;
  };

  /**
   * @brief Type of the dictionary class
   */
  template <typename K, typename V, typename... Args>
  using AgentDict = typename define_agent_dict<
      typename CompoundDomain::Features::template AgentDomain<CompoundDomain>,
      typename CompoundDomain::Types>::template result<K, V, Args...>;

  /**
   * @brief Proxy to the type of objects stored by agents
   * For multi agents, this is a dictionary from agent types to the objects
   * mapped from agents
   * @tparam T
   */
  template <typename V, typename... Args>
  using AgentProxy = AgentDict<AgentType, V, Args...>;
};

/**
 * @brief A domain must inherit this class if it is single-agent (i.e hosting
 * only one agent).
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain>
class SingleAgentDomain : public MultiAgentDomain<CompoundDomain> {
public:
  typedef
      typename CompoundDomain::Features::template AgentDomain<CompoundDomain>
          ActualAgentDomain;

  template <class, class = void> struct check_agent_type : std::true_type {};

  template <class T>
  struct check_agent_type<T, std::void_t<typename T::AgentType>>
      : std::conditional<std::is_same_v<std::nullptr_t, typename T::AgentType>,
                         std::true_type, std::false_type>::type {};

  static_assert(
      !std::is_same_v<SingleAgentDomain<CompoundDomain>, ActualAgentDomain> ||
          check_agent_type<typename CompoundDomain::Types>::value,
      "The domain type AgentType must be equal to std::nullptr_t when the "
      "agent domain feature is skdecide::SingleAgentDomain");

  typedef std::nullptr_t AgentType;

  template <class, class = void> struct check_agent_dict : std::true_type {};

  template <class T>
  struct check_agent_dict<
      T, std::void_t<typename T::template AgentDict<int, char>>>
      : std::conditional<
            std::is_same_v<char, typename T::template AgentDict<int, char>>,
            std::true_type, std::false_type>::type {};

  static_assert(
      !std::is_same_v<SingleAgentDomain<CompoundDomain>, ActualAgentDomain> ||
          check_agent_dict<typename CompoundDomain::Types>::value,
      "The domain type AgentDict<K, V> must be equal to V when the "
      "agent domain feature is skdecide::SingleAgentDomain");

  template <typename K, typename V, typename... T> using AgentDict = V;

  /**
   * @brief Proxy to the type of objects stored by this agent.
   * For single agents, this is the template parameter type itself
   * @tparam T
   */
  template <typename T> using AgentProxy = T;
};

} // namespace skdecide

#endif // SKDECIDE_AGENT_HH
