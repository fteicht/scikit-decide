/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_CORE_HH
#define SKDECIDE_CORE_HH

#include <cstddef>
#include <functional>
#include <memory>
#include <random>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>

#include <json.hpp>
using json = nlohmann::json;

namespace skdecide {

/**
 * @brief A space representing a finite or infinite set.
 *
 * This class (or any of its descendant) is typically used to specify action,
 * observation or goal spaces.
 *
 * @tparam T Type of elements of the space
 */
template <typename T> class Space {
public:
  /**
   * Type of elements of the space
   */
  typedef T element_type;

  /**
   * Default destructor
   */
  virtual ~Space() {}

  /**
   * @brief Check whether x is a valid member of this space.
   *
   * @param x The member to consider.
   * @return True if x is a valid member of this space (False otherwise).
   */
  virtual bool contains(const T &x) const = 0;
};

/**
 * @brief A space formalized implicitly, i.e. by a black-box contains()
 * function.
 *
 * @tparam T Type of elements of the space
 */
template <typename T> class ImplicitSpace : public Space<T> {
public:
  /**
   * Type of elements of the space
   */
  typedef T element_type;

  /**
   * Constructor
   * @param containsFunctor Functor (can be a lambda expression returning
   *                        boolean specifying if x is a valid member of
   *                        this space)
   */
  ImplicitSpace(std::function<bool(const T &)> containsFunctor)
      : m_containsFunctor(containsFunctor) {}

  /**
   * @brief Check whether x is a valid member of this space.
   *
   * @param x The member to consider.
   * @return True if x is a valid member of this space (False otherwise).
   */
  virtual bool contains(const T &x) const { return m_containsFunctor(x); }

private:
  std::function<bool(const T &)> m_containsFunctor;
};

/**
 * @brief A space which elements can be enumerated.
 *
 * @tparam T Type of elements of the space
 * @tparam Container Type of container used to store the elements of the space
 */
template <typename T,
          template <typename...> class Container = std::unordered_set>
class EnumerableSpace : public Space<T> {
public:
  /**
   * Type of elements of the space
   */
  typedef T element_type;

  /**
   * Type of container used to store the elements of the space
   */
  template <typename... TT> using container_class = Container<TT...>;

  /**
   * @brief Get the elements of this space.
   *
   * @return The elements of this space.
   */
  virtual const Container<T> &get_elements() const = 0;
};

/**
 * @brief A space without elements.
 *
 * @tparam T Type of elements of the space
 * @tparam Container Type of container used to store the elements of the space
 */
template <typename T = std::nullptr_t,
          template <typename...> class Container = std::unordered_set>
class EmptySpace : public EnumerableSpace<T> {
public:
  /**
   * Type of elements of the space
   */
  typedef T element_type;

  /**
   * Type of container used to store the elements of the space
   */
  template <typename... TT> using container_class = Container<TT...>;

  /**
   * @brief Construct a new Empty Space object
   */
  EmptySpace() {}

  /**
   * @brief Get the elements of this space.
   *
   * @return The elements of this space.
   */
  virtual const Container<T> &get_elements() const { return _container; }

private:
  Container<T> _container;
};

/**
 * @brief A space which can be sampled (uniformly randomly).
 *
 * @tparam T Type of elements of the space
 */
template <typename T> class SamplableSpace : public Space<T> {
public:
  /**
   * Type of elements of the space
   */
  typedef T element_type;

  /**
   * @brief Uniformly randomly sample a random element of this space.
   *
   * @return T The sampled element.
   */
  virtual T sample() const = 0;
};

/**
 * @brief A space which can be serialized (to/from JSON).
 *
 * @tparam T Type of elements of the space
 * @tparam Container Type of container used to store the (de)serialized elements
 * of the space
 */
template <typename T,
          template <typename...> class Container = std::unordered_set>
class SerializableSpace : public Space<T> {
public:
  /**
   * Type of elements of the space
   */
  typedef T element_type;

  /**
   * Type of container used to store the (de)serialized elements of the space
   */
  template <typename... TT> using container_class = Container<TT...>;

  /**
   * @brief Convert a batch of samples from this space to a JSONable data type.
   *
   * @param sample_n The batch of samples to convert.
   * @return The resulting JSONable data type.
   */
  virtual json to_jsonable(const Container<T> &sample_n) const {
    // By default, assume identity is JSONable
    // See https://github.com/nlohmann/json#arbitrary-types-conversions
    return json(sample_n);
  }

  /**
   * @brief Convert a JSONable data type to a batch of samples from this space.
   *
   * @param sample_n The JSONable data type to convert.
   * @return The resulting batch of samples.
   */
  virtual Container<T> from_jsonable(const json &sample_n) const {
    // By default, assume identity is JSONable
    // See https://github.com/nlohmann/json#arbitrary-types-conversions
    return Container<T>(sample_n.get<Container<T>>());
  }
};

/**
 * @brief A probability distribution.
 *
 * @tparam T Type of elements of the distribution
 */
template <typename T> class Distribution {
public:
  /**
   * Type of elements of the distribution
   */
  typedef T element_type;

  /**
   * Destructor
   */
  virtual ~Distribution() {}

  /**
   * @brief Sample from this distribution.
   *
   * @return T The sampled element.
   */
  virtual T sample() = 0;
};

/**
 * @brief A probability distribution formalized implicitly, i.e. by a black-box
 * sample() function.
 *
 * @tparam T Type of elements of the distribution
 */
template <typename T> class ImplicitDistribution : public Distribution<T> {
public:
  /**
   * Type of elements of the distribution
   */
  typedef T element_type;

  /**
   * Constructor
   * @param sampleFunctor Functor (can be a lambda expression) returning
   *                      a sample from the distribution
   */
  ImplicitDistribution(std::function<T()> sampleFunctor)
      : m_sampleFunctor(sampleFunctor) {}

  /**
   * @brief Sample from this distribution.
   *
   * @return T The sampled element.
   */
  virtual T sample() { return m_sampleFunctor(); }

private:
  std::function<T()> m_sampleFunctor;
};

/**
 * @brief A discrete probability distribution.
 *
 * @tparam T Type of elements of the distribution
 * @tparam Container Type of container used to map the elements to their
 * probabilities
 * @tparam Generator Type of the sample generator
 * @tparam IntType Type of integers
 */
template <typename T,
          template <typename...> class Container = std::unordered_map,
          typename Generator = std::mt19937, typename IntType = int>
class DiscreteDistribution : public Distribution<T> {
public:
  /**
   * Type of elements of the distribution
   */
  typedef T element_type;

  /**
   * Type of container used to map the elements to their probabilities
   */
  template <typename... TT> using container_class = Container<TT...>;

  /**
   * Constructor
   * @param iBegin Associative container begin iterator
   * @param iEnd Associative container end iterator
   * @param g Random number generator
   */
  template <typename InputIt>
  DiscreteDistribution(InputIt iBegin, InputIt iEnd)
      : m_generator(Generator(std::random_device()())) {
    for (InputIt i = iBegin; i != iEnd; i++) {
      std::pair<typename Container<T, double>::iterator, bool> r =
          m_values.insert(*i);
      if (r.second) {
        m_indexes.push_back(r.first);
      } else {
        r.first->second += i->second;
      }
    }

    if (m_values.empty()) {
      m_indexes.push_back(m_values.insert(std::make_pair(T(), 1.0)).first);
    }

    std::vector<double> probabilities;
    std::for_each(m_indexes.begin(), m_indexes.end(),
                  [&](const auto &i) { probabilities.push_back(i->second); });
    m_distribution.param(
        typename std::discrete_distribution<IntType>::param_type(
            probabilities.begin(), probabilities.end()));
    probabilities =
        m_distribution.probabilities(); // get normalized probabilities

    for (std::size_t i = 0; i < probabilities.size(); i++) {
      m_indexes[i]->second = probabilities[i];
    }
  }

  /**
   * Constructor
   */
  DiscreteDistribution(std::initializer_list<std::pair<T, double>> iList)
      : DiscreteDistribution(iList.begin(), iList.end()) {}

  /**
   * @brief Sample from this distribution.
   *
   * @return T The sampled element.
   */
  virtual T sample() { return m_indexes[m_distribution(m_generator)]->first; }

  /**
   * @brief Get the list of (element, probability) pairs.
   *
   * @return The (element, probability) pairs.
   */
  const Container<T, double> &get_values() const { return m_values; }

private:
  Container<T, double> m_values;
  std::vector<typename Container<T, double>::iterator> m_indexes;
  Generator m_generator;
  std::discrete_distribution<IntType> m_distribution;
};

/**
 * @brief A single value distribution (i.e. Dirac distribution).
 *
 * @tparam T Type of elements of the distribution
 */
template <typename T>
class SingleValueDistribution : public DiscreteDistribution<T> {
public:
  /**
   * Type of elements of the distribution
   */
  typedef T element_type;

  /**
   * Constructor
   */
  SingleValueDistribution(const T &value)
      : DiscreteDistribution<T>({{value, 1.0}}), m_value(value) {}

  /**
   * @brief Sample from this distribution.
   *
   * @return T The sampled element.
   */
  virtual T sample() { return m_value; }

  /**
   * @brief Get the single value of this distribution.
   *
   * @return The single value of this distribution.
   */
  const T &get_value() const { return m_value; }

private:
  T m_value;
};

/**
 * @brief Type of values: either rewards or costs
 */
enum class ValueType { REWARD, COST };

/**
 * @brief A value (reward or cost).
 *
 * !!! Warning.
 *      It is recommended to use either the reward or the cost parameter. If no
 * one is used, a reward/cost of 0 is assumed. If both are used, reward will be
 * considered and cost ignored. In any case, both reward and cost attributes
 * will be defined after initialization.
 *
 * @tparam TT Type (enum) of values: either rewards or costs
 * @tparam T Type of floats used to encode values
 */
template <ValueType TT = ValueType::REWARD, typename T = double> class Value;

/**
 * @brief Specialized value class for reward values
 *
 * @tparam T Type of floats used to encode values
 */
template <typename T> class Value<ValueType::REWARD, T> {
public:
  /**
   * @brief Construct a new Value object
   *
   * @param value Value considered as a reward
   */
  Value(const T &value) : m_value(value) {}

  /**
   * @brief Returns the value as a reward
   *
   * @return T The reward value.
   */
  inline virtual T reward() const { return m_value; }

  /**
   * @brief Return the value as a cost
   *
   * @return T The cost value.
   */
  inline virtual T cost() const { return -m_value; }

private:
  T m_value;
};

/**
 * @brief Specialized value class for cost values
 *
 * @tparam T Type of floats used to encode values
 */
template <typename T> class Value<ValueType::COST, T> {
public:
  /**
   * @brief Construct a new Value object
   *
   * @param value Value considered as a cost
   */
  Value(const T &value) : m_value(value) {}

  /**
   * @brief Returns the value as a reward
   *
   * @return T The reward value.
   */
  inline virtual T reward() const { return -m_value; }

  /**
   * @brief Return the value as a cost
   *
   * @return T The cost value.
   */
  inline virtual T cost() const { return m_value; }

private:
  T m_value;
};

/**
 * @brief An environment outcome for an internal transition.
 *
 * @tparam Tobservation Type of the agent's observation of the current
 * environment.
 * @tparam TT Type (enum) of values (either rewards or costs) returned after
 * previous action.
 * @tparam Tvalue Type of floats used to encode values returned after previous
 * action.
 * @tparam Tpredicate Type of predicates used to check whether the episode has
 * ended, in which case further step() calls will return undefined results.
 * @tparam Tinfo Type of optional auxiliary diagnostic information (helpful for
 * debugging, and sometimes learning).
 */
template <typename Tobservation, ValueType TT, typename Tvalue,
          typename Tpredicate, typename Tinfo = std::nullptr_t>
struct EnvironmentOutcome {
  EnvironmentOutcome(const Tobservation &observation, const Tvalue &value,
                     const Tpredicate &termination, const Tinfo &info = Tinfo())
      : observation(observation), value(value), termination(termination),
        info(info) {}

  Tobservation observation;
  Value<TT, Tvalue> value;
  Tpredicate termination;
  Tinfo info;
};

/**
 * @brief A transition outcome.
 *
 * @tparam Tstate Type of the new state after the transition.
 * @tparam TT Type (enum) of values (either rewards or costs) returned after
 * previous action.
 * @tparam Tvalue Type of floats used to encode values returned after previous
 * action.
 * @tparam Tpredicate Type of predicates used to check whether the episode has
 * ended, in which case further step() calls will return undefined results.
 * @tparam Tinfo Type of optional auxiliary diagnostic information (helpful for
 * debugging, and sometimes learning).
 */
template <typename Tstate, ValueType TT, typename Tvalue, typename Tpredicate,
          typename Tinfo = std::nullptr_t>
struct TransitionOutcome {
  TransitionOutcome(const Tstate &state, const Tvalue &value,
                    const Tpredicate &termination, const Tinfo &info = Tinfo())
      : state(state), value(value), termination(termination), info(info) {}

  Tstate state;
  Value<TT, Tvalue> value;
  Tpredicate termination;
  Tinfo info;
};

/**
 * Deque class with maxlen feature like python deque; only for push_back and
 * push_front!
 */
template <typename T> class Memory : public std::deque<T> {
public:
  Memory(std::size_t maxlen = std::numeric_limits<std::size_t>::max())
      : std::deque<T>(), _maxlen(maxlen) {}

  template <typename InputIt>
  Memory(InputIt iBegin, InputIt iEnd,
         std::size_t maxlen = std::numeric_limits<std::size_t>::max())
      : std::deque<T>(iBegin, iEnd), _maxlen(maxlen) {
    if (this->size() > maxlen) {
      std::deque<T>::erase(std::deque<T>::begin(), std::deque<T>::begin() +
                                                       std::deque<T>::size() -
                                                       maxlen);
    }
  }

  Memory(std::initializer_list<T> iList,
         std::size_t maxlen = std::numeric_limits<std::size_t>::max())
      : Memory(iList.begin(), iList.end(), maxlen) {}

  Memory(const Memory &m)
      : std::deque<T>(static_cast<const std::deque<T> &>(m)),
        _maxlen(m._maxlen) {}

  void operator=(const Memory &m) {
    static_cast<std::deque<T> &>(*this) = static_cast<const std::deque<T> &>(m);
    _maxlen = m._maxlen;
  }

  bool operator==(const Memory &m) const {
    return static_cast<const std::deque<T> &>(*this) ==
               static_cast<const std::deque<T> &>(m) &&
           _maxlen == m._maxlen;
  }

  std::size_t maxlen() const { return _maxlen; }

  void push_back(const T &value) {
    std::deque<T>::push_back(value);
    if (this->size() > _maxlen) {
      std::deque<T>::pop_front();
    }
  }

  void push_back(T &&value) {
    std::deque<T>::push_back(value);
    if (this->size() > _maxlen) {
      std::deque<T>::pop_front();
    }
  }

  void push_front(const T &value) {
    std::deque<T>::push_front(value);
    if (this->size() > _maxlen) {
      std::deque<T>::pop_back();
    }
  }

  void push_front(T &&value) {
    std::deque<T>::push_front(value);
    if (this->size() > _maxlen) {
      std::deque<T>::pop_back();
    }
  }

protected:
  std::size_t _maxlen;
};

// === CONSTRAINTS ===

/**
 * @brief A constraint.
 * @tparam Tmemory Type of memory
 * @tparam Tstate Type of states
 * @tparam Tevent Type of events/actions
 */
template <typename Tmemory, typename Tstate, typename Tevent> class Constraint {
public:
  typedef Tmemory Memory;
  typedef Tstate State;
  typedef std::unique_ptr<State> StatePtr;
  typedef Tevent Event;

  virtual ~Constraint() {}

  /**
   * @brief Check this constraint.
   * !!! tip.
   * If this function never depends on the next_state parameter for its
   * computation, it is recommended to indicate it by overriding
   * Constraint::check_constraint_dependency_on_next_state() to return False.
   * This information can then be exploited by solvers to avoid computing next
   * state to evaluate the constraint (more efficient).
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @param next_state The next state in which the transition ends (if needed
   * for the computation).
   * @return true The constraint is satisfied.
   * @return false The constraint is violated.
   */
  virtual bool check(const Memory &memory, const Event &event,
                     const StatePtr &next_state = StatePtr()) const = 0;

  /**
   * @brief Indicate whether this constraint requires the next_state parameter
   * for its computation (cached).
   * By default, Constraint::is_constraint_dependent_on_next_state() internally
   * calls Constraint::check_constraint_dependency_on_next_state_() the first
   * time and automatically caches its value to make future calls more efficient
   * (since the returned value is assumed to be constant).
   * @return true The constraint computation depends on next_state.
   * @return false The constraint computation does not depend on next_state.
   */
  bool is_constraint_dependent_on_next_state() const {
    if (!_constraint_dependent_on_next_state) {
      _constraint_dependent_on_next_state = std::make_unique<bool>(
          this->check_constraint_dependency_on_next_state());
    }
    return *_constraint_dependent_on_next_state;
  }

protected:
  mutable std::unique_ptr<bool> _constraint_dependent_on_next_state;

  /**
   * @brief Indicate whether this constraint requires the next_state parameter
   * for its computation.
   * This is a helper function called by default from
   * Constraint::is_constraint_dependent_on_next_state(), the difference being
   * that the result is not cached here.
   * !!! tip.
   * The underscore at the end of this function's name is a convention to remind
   * that its result should be constant.
   * @return true The constraint computation depends on next_state.
   * @return false The constraint computation does not depend on next_state.
   */
  virtual bool check_constraint_dependency_on_next_state() const {
    return true;
  }
};

/**
 * @brief A constraint formalized implicitly, i.e. by a black-box check()
 * function.
 * @tparam Tmemory Type of memory
 * @tparam Tstate Type of states
 * @tparam Tevent Type of events/actions
 */
template <typename Tmemory, typename Tstate, typename Tevent>
class ImplicitConstraint : public Constraint<Tmemory, Tstate, Tevent> {
public:
  typedef Tmemory Memory;
  typedef Tstate State;
  typedef std::unique_ptr<State> StatePtr;
  typedef Tevent Event;
  typedef std::function<bool(const Memory &, const Event &, const StatePtr &)>
      CheckFunction;

  /**
   * @brief Construct a new Implicit Constraint object
   *
   * @param check_function The check() function to use.
   * @param depends_on_next_state  Whether the check() function requires the
   * next_state parameter for its computation.
   */
  ImplicitConstraint(const CheckFunction &check_function,
                     bool depends_on_next_state = true)
      : _check_function(check_function),
        _depends_on_next_state(depends_on_next_state) {}

  virtual ~ImplicitConstraint() {}

  /**
   * @brief Check this constraint.
   * !!! tip.
   * If this function never depends on the next_state parameter for its
   * computation, it is recommended to indicate it by overriding
   * Constraint::check_constraint_dependency_on_next_state() to return False.
   * This information can then be exploited by solvers to avoid computing next
   * state to evaluate the constraint (more efficient).
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @param next_state The next state in which the transition ends (if needed
   * for the computation).
   * @return true The constraint is satisfied.
   * @return false The constraint is violated.
   */
  virtual bool check(const Memory &memory, const Event &event,
                     const StatePtr &next_state = StatePtr()) const {
    return this->_check_function(memory, event, next_state);
  }

protected:
  CheckFunction _check_function;
  bool _depends_on_next_state;

  /**
   * @brief Indicate whether this constraint requires the next_state parameter
   * for its computation.
   * This is a helper function called by default from
   * Constraint::is_constraint_dependent_on_next_state(), the difference being
   * that the result is not cached here.
   * !!! tip.
   * The underscore at the end of this function's name is a convention to remind
   * that its result should be constant.
   * @return true The constraint computation depends on next_state.
   * @return false The constraint computation does not depend on next_state.
   */
  virtual bool check_constraint_dependency_on_next_state() const {
    return this->_depends_on_next_state;
  }
};

/**
 * @brief A constraint characterized by an evaluation function, an inequality
 * and a bound.
 * # Example.
 * A BoundConstraint with inequality '>=' is checked if (and only if) its
 * BoundConstraint::evaluate() function returns a float greater than or equal to
 * its bound.
 * @tparam Tmemory Type of memory
 * @tparam Tstate Type of states
 * @tparam Tevent Type of events/actions
 */
template <typename Tmemory, typename Tstate, typename Tevent>
class BoundConstraint : public Constraint<Tmemory, Tstate, Tevent> {
public:
  typedef Tmemory Memory;
  typedef Tstate State;
  typedef std::unique_ptr<State> StatePtr;
  typedef Tevent Event;
  typedef std::function<double(const Memory &, const Event &, const StatePtr &)>
      EvaluateFunction;

  typedef enum { LESS, LESS_EQ, GREATER, GREATER_EQ } InequalityType;

  /**
   * @brief Construct a new Bound Constraint object
   *
   * @param evaluate_function The evaluate() function to use.
   * @param inequality_type The BoundConstraint::InequalityType enum describing
   * the constraint inequality.
   * @param bound The bound of the constraint.
   * @param depends_on_next_state Whether the evaluate() function requires the
   * next_state parameter for its computation.
   */
  BoundConstraint(const EvaluateFunction &evaluate_function,
                  InequalityType inequality_type, double bound,
                  bool depends_on_next_state = true)
      : _evaluate_function(evaluate_function),
        _inequality_type(inequality_type), _bound(bound),
        _depends_on_next_state(depends_on_next_state) {}

  virtual ~BoundConstraint() {}

  /**
   * @brief Check this constraint.
   * !!! tip.
   * If this function never depends on the next_state parameter for its
   * computation, it is recommended to indicate it by overriding
   * Constraint::check_constraint_dependency_on_next_state() to return False.
   * This information can then be exploited by solvers to avoid computing next
   * state to evaluate the constraint (more efficient).
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @param next_state The next state in which the transition ends (if needed
   * for the computation).
   * @return true The constraint is satisfied.
   * @return false The constraint is violated.
   */
  virtual bool check(const Memory &memory, const Event &event,
                     const StatePtr &next_state = StatePtr()) const {
    switch (_inequality_type) {
    case BoundConstraint<Memory, State, Event>::LESS:
      return this->_evaluate_function(memory, event, next_state) < this->_bound;
    case BoundConstraint<Memory, State, Event>::LESS_EQ:
      return this->_evaluate_function(memory, event, next_state) <=
             this->_bound;
    case BoundConstraint<Memory, State, Event>::GREATER:
      return this->_evaluate_function(memory, event, next_state) > this->_bound;
    case BoundConstraint<Memory, State, Event>::GREATER_EQ:
      return this->_evaluate_function(memory, event, next_state) >=
             this->_bound;
    }
  }

  /**
   * @brief Evaluate the left side of this BoundConstraint.
   * !!! tip
   * If this function never depends on the next_state parameter for its
   * computation, it is recommended to indicate it by overriding
   * Constraint::check_constraint_dependency_on_next_state() to return False.
   * This information can then be exploited by solvers to avoid computing next
   * state to evaluate the constraint (more efficient).
   * @param memory The source memory (state or history) of the transition.
   * @param event The action taken in the given memory (state or history)
   * triggering the transition.
   * @param next_state The next state in which the transition ends (if needed
   * for the computation).
   * @return double Value resulting from the evaluation.
   */
  double evaluate(const Memory &memory, const Event &event,
                  const StatePtr &next_state = StatePtr()) const {
    return this->_evaluate_function(memory, event, next_state);
  }

  /**
   * @brief Get the inequality type describing the constraint inequality.
   * @return InequalityType The type of inequality: see
   * BoundConstraint::InequalityType
   */
  InequalityType get_inequality() const { return this->_inequality_type; }

  /**
   * @brief Get the bound of the constraint.
   * @return double The constraint bound.
   */
  double get_bound() const { return this->_bound; }

protected:
  const EvaluateFunction &_evaluate_function;
  InequalityType _inequality_type;
  double _bound;
  bool _depends_on_next_state;

  /**
   * @brief Indicate whether this constraint requires the next_state parameter
   * for its computation.
   * This is a helper function called by default from
   * Constraint::is_constraint_dependent_on_next_state(), the difference being
   * that the result is not cached here.
   * !!! tip.
   * The underscore at the end of this function's name is a convention to remind
   * that its result should be constant.
   * @return true The constraint computation depends on next_state.
   * @return false The constraint computation does not depend on next_state.
   */
  virtual bool check_constraint_dependency_on_next_state() {
    return this->_depends_on_next_state;
  }
};

} // namespace skdecide

#endif // SKDECIDE_CORE_HH
