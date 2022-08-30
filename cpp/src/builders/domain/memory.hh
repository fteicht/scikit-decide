/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_MEMORY_HH
#define SKDECIDE_MEMORY_HH

#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>

#include "core.hh"

namespace skdecide {

/**
 * @brief A domain must inherit this class if its full state history must be
 * stored to compute its dynamics (non-Markovian domain).
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain> class HistoryDomain {
public:
  typedef typename CompoundDomain::Features::
      template AgentDomain<CompoundDomain>::template AgentProxy<
          typename CompoundDomain::Features::template ObservabilityDomain<
              CompoundDomain>::AgentState>
          CompoundState;
  typedef Memory<CompoundState> CompoundMemory;
  typedef std::unique_ptr<CompoundMemory> CompoundMemoryPtr;

  /**
   * @brief Proxy to the type of memorized trajectories
   * @tparam T Type of memorized objects (usually Tstate)
   */
  template <typename T> using MemoryProxy = Memory<T>;

  /**
   * @brief Checks the consistency of the given memory object with this class'
   * memory. In the general history domains, memory can be infinite so this
   * check always returns true.
   * @param memory State memory object
   * @return Always true
   */
  inline virtual bool check_memory(const CompoundMemory &memory) {
    return true;
  }

  /**
   * @brief Checks the consistency of the internal memory object with this
   * class' memory. In the general history domains, memory can be infinite so
   * this check always returns true. An exception is thrown if the internal
   * memory object has not yet been initialized.
   * @return Always true
   */
  inline virtual bool check_memory() {
    if (!_memory)
      throw std::invalid_argument("Uninitialized internal state memory");

    return check_memory(*_memory);
  }

  /**
   * @brief Get the last state object of the given memory object.
   * An exception is thrown if the given memory object is empty (i.e. it
   * contains no state).
   * @param memory State memory object
   * @return Last state of the memory
   */
  static CompoundState &get_last_state(CompoundMemory &memory) {
    if (memory.size() > 0) {
      return memory.back();
    } else {
      throw std::out_of_range(
          "Attempting to get last state of empty memory object");
    }
  }

  /**
   * @brief Get the last state object of the internal memory object.
   * An exception is thrown if the given memory object is empty (i.e. it
   * contains no state).
   * @return Last state of the internal memory
   */
  inline CompoundState &get_last_state() {
    if (!_memory)
      throw std::invalid_argument("Uninitialized internal state memory");

    return get_last_state(*_memory);
  }

protected:
  CompoundMemoryPtr _memory;

  /**
   * Protected constructor because the class must be specialized to properly
   * initialize the state memory
   */
  HistoryDomain() {}

  /**
   * @brief Initialize memory and return it. This function is automatically
   * called by Initializable::reset() to reinitialize the internal memory
   * whenever the domain is used as an environment.
   * @tparam InputIt Type of the memory sequence iterator
   * @param iBegin Begin of the memory sequence iterator
   * @param iEnd End of the memory sequence iterator
   * @return The new initialized memory.
   */
  template <typename InputIt>
  inline CompoundMemoryPtr _init_memory(InputIt iBegin, InputIt iEnd) {
    return std::make_unique<CompoundMemory>(iBegin, iEnd, get_memory_maxlen());
  }

  /**
   * @brief Initialize memory and return it. This function is automatically
   * called by Initializable::reset() to reinitialize the internal memory
   * whenever the domain is used as an environment.
   * @param iList Memory sequence initializer list
   * @return The new initialized memory.
   */
  inline CompoundMemoryPtr
  _init_memory(std::initializer_list<CompoundState> iList) {
    return std::make_unique<CompoundMemory>(iList, get_memory_maxlen());
  }

  /**
   * @brief Get the memory max length (or None if unbounded).
   * !!! tip.
   * This function returns always None by default because the memory
   * length is unbounded at this level.
   * @return The memory max length (or
   * std::numeric_limits<std::size_t>::max() if unbounded).
   */
  inline virtual std::size_t get_memory_maxlen() {
    return std::numeric_limits<std::size_t>::max();
  }
};

/**
 * @brief A domain must inherit this class if the last N states must be stored
 * to compute its dynamics (Markovian domain of order N). N is specified by the
 * return value of the FiniteHistory::get_memory_maxlen() function.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain>
class FiniteHistoryDomain : public HistoryDomain<CompoundDomain> {
public:
  typedef typename HistoryDomain<CompoundDomain>::CompoundState CompoundState;
  typedef typename HistoryDomain<CompoundDomain>::CompoundMemory CompoundMemory;

  /**
   * @brief Proxy to the type of memorized trajectories
   * @tparam T Type of memorized objects (usually Tstate)
   */
  template <typename T> using MemoryProxy = Memory<T>;

  /**
   * @brief Checks the consistency of the given memory object with this class'
   * memory
   * @param memory State memory object
   * @return True if the given memory's max length is equal to this class'
   * memory max length (False otherwise)
   */
  inline virtual bool check_memory(const CompoundMemory &memory) {
    return memory.maxlen() == get_memory_maxlen();
  }

  /**
   * @brief Checks the consistency of the internal memory object with this
   * class' memory. An exception is thrown if the internal memory object has not
   * yet been initialized.
   * @return True if the internal memory's max length is equal to this class'
   * memory max length (False otherwise)
   */
  inline virtual bool check_memory() {
    if (!(this->_memory))
      throw std::invalid_argument("Uninitialized internal state memory");

    return check_memory(*(this->_memory));
  }

protected:
  /**
   * Protected constructor because the class must be specialized to properly
   * initialize the state memory
   */
  FiniteHistoryDomain() {}

  /**
   * @brief Get the (cached) memory max length. By default,
   * FiniteHistory::get_memory_maxlen() internally calls
   * FiniteHistory::make_memory_maxlen() the first time and automatically caches
   * its value to make future calls more efficient (since the memory max length
   * is assumed to be constant).
   * @return The memory max length.
   */
  inline virtual std::size_t get_memory_maxlen() {
    if (!_memory_maxlen) {
      _memory_maxlen = std::make_unique<std::size_t>(make_memory_maxlen());
    }
    return *_memory_maxlen;
  }

protected:
  std::unique_ptr<std::size_t> _memory_maxlen;

  /**
   * @brief Get the memory max length.
   * This is a helper function called by default from
   * FiniteHistory::get_memory_maxlen(), the difference being that the result is
   * not cached here.
   * @return The memory max length.
   */
  virtual std::size_t make_memory_maxlen() = 0;
};

/**
 * @brief A domain must inherit this class if only its last state must be stored
 to compute its dynamics (pure Markovian domain).
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain>
class MarkovianDomain : public FiniteHistoryDomain<CompoundDomain> {
public:
  typedef
      typename FiniteHistoryDomain<CompoundDomain>::CompoundState CompoundState;

  /**
   * @brief Proxy to the type of last visited object
   * @tparam T Type of last visited object (usually Tstate)
   */
  template <typename T> using MemoryProxy = T;

protected:
  /**
   * Protected constructor because the class must be specialized to properly
   * initialize the state memory
   */
  MarkovianDomain() {}

  /**
   * @brief Get the memory max length which is always equal to 1 for Markovian
   * domains. This is a helper function called by default from
   * FiniteHistory::get_memory_maxlen(), the difference being that the result is
   * not cached here.
   * @return The memory max length which is constantly equal to 1.
   */
  inline virtual std::size_t make_memory_maxlen() { return 1; }
};

/**
 * @brief A domain must inherit this class if it does not require any previous
 * state(s) to be stored to compute its dynamics. A dice roll simulator is an
 * example of memoryless domain (next states are independent of previous ones).
 * !!! Tip.
 * Whenever an existing domain (environment, simulator...) needs to be wrapped
 * instead of implemented fully in scikit-decide (e.g. compiled ATARI games),
 * Memoryless can be used because the domain memory (if any) would be  handled
 *externally.
 * @tparam CompoundDomain The type of the domain made up of different
 * features and deriving from this particular domain feature.
 */
template <typename CompoundDomain>
class MemorylessDomain : public MarkovianDomain<CompoundDomain> {
public:
  typedef typename MarkovianDomain<CompoundDomain>::CompoundState CompoundState;

  /**
   * @brief Proxy to the null type (no memorized object)
   * @tparam T Unused type
   */
  template <typename T> using MemoryProxy = std::nullptr_t;

  MemorylessDomain() { this->_memory = this->_init_memory({}); }

protected:
  /**
   * @brief Get the memory max length which is always equal to 0 for memoryless
   * domains. This is a helper function called by default from
   * FiniteHistory::get_memory_maxlen(), the difference being that the result is
   * not cached here.
   * @return The memory max length which is constantly equal to 0.
   */
  inline virtual std::size_t make_memory_maxlen() { return 0; }
};

} // namespace skdecide

#endif // SKDECIDE_MEMORY_HH
