/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PYTHON_DOMAIN_PROXY_HH
#define SKDECIDE_PYTHON_DOMAIN_PROXY_HH

#include "python_domain_proxy_base.hh"

namespace pybind11 {
    class dict;
    class list;
    namespace detail {
        template <typename Policy> class generic_iterator;
        namespace iterator_policies{
            class dict_readonly;
        }
        using dict_iterator = generic_iterator<iterator_policies::dict_readonly>;
    }
}

namespace py = pybind11;

namespace skdecide {

struct SingleAgent { SingleAgent() {} };
struct MultiAgent { MultiAgent() {} };

struct PartiallyObservable { PartiallyObservable() {} };
struct FullyObservable { FullyObservable() {} };

struct PartiallyControllable { PartiallyControllable() {} };
struct FullyControllable { FullyControllable() {} };

struct Markovian { Markovian() {} };
struct History { History() {} };

template <typename Texecution,
          typename Tagent = SingleAgent,
          typename Tobservability = FullyObservable,
          typename Tcontrollability = FullyControllable,
          typename Tmemory = Markovian>
class PythonDomainProxy {
public :

    template <typename Derived, typename Tpyobj = py::object>
    using PyObj = typename PythonDomainProxyBase<Texecution>::template PyObj<Derived, Tpyobj>;

    template<typename T, typename Titerator = py::iterator>
    using PyIter = typename PythonDomainProxyBase<Texecution>::template PyIter<T, Titerator>;

    template <typename DData, typename TTagent, typename Enable = void>
    class AgentDataAccess {};

    template <typename DData, typename TTagent>
    class AgentDataAccess<DData, TTagent,
                          typename std::enable_if<std::is_same<TTagent, SingleAgent>::value>::type> {
    public :
        typedef DData Data;
        typedef void Agent;
        typedef Data AgentData;
    };

    template <typename DData, typename TTagent>
    class AgentDataAccess<DData, TTagent,
                          typename std::enable_if<std::is_same<TTagent, MultiAgent>::value>::type> : public PyObj<DData, py::dict> {
    public :
        // AgentDataAccess inherits from pyObj<> to manage its python object but DData is passed to
        // PyObj as template parameter to print DData::class_name when managing AgentDataAccess objects

        typedef AgentDataAccess<DData, TTagent> Data;

        // AgentData are dict values
        typedef DData AgentData;

        AgentDataAccess();
        AgentDataAccess(std::unique_ptr<py::object>&& ad);
        AgentDataAccess(const py::object& ad);
        AgentDataAccess(const AgentDataAccess& other);
        AgentDataAccess& operator=(const AgentDataAccess& other);
        virtual ~AgentDataAccess();

        std::size_t size() const;

        typedef typename PythonDomainProxyBase<Texecution>::AgentDataAccess::Agent Agent;
        typedef typename PythonDomainProxyBase<Texecution>::AgentDataAccess::template Item<AgentData> Item;
        typedef typename PythonDomainProxyBase<Texecution>::AgentDataAccess::template AgentDataAccessor<AgentData> AgentDataAccessor;
        typedef PyIter<Item, py::detail::dict_iterator> PyIter;

        AgentDataAccessor operator[](const Agent& a);
        const AgentData operator[](const Agent& a) const;
        PyIter begin() const;
        PyIter end() const;
    };

    typedef typename AgentDataAccess<typename PythonDomainProxyBase<Texecution>::State, Tagent>::Data State;
    typedef typename AgentDataAccess<typename PythonDomainProxyBase<Texecution>::Observation, Tagent>::Data _Observation;

    typedef typename std::conditional<std::is_same<Tobservability, FullyObservable>::value,
                                        State,
                                        typename std::conditional<std::is_same<Tobservability, PartiallyObservable>::value,
                                                                    _Observation,
                                                                    void
                                                                 >::type
                                     >::type Observation;
    
    class MemoryState : public PyObj<MemoryState, py::list> {
    public :
        typedef State State;
        typedef typename State::AgentData AgentData;
        static constexpr char class_name[] = "memory";

        MemoryState();
        MemoryState(std::unique_ptr<py::object>&& m);
        MemoryState(const py::object& m);
        MemoryState(const MemoryState& other);
        MemoryState& operator=(const MemoryState& other);
        virtual ~MemoryState();

        void push_state(const State& s);
        State last_state();
    };

    typedef typename std::conditional<std::is_same<Tmemory, Markovian>::value,
                                        State,
                                        typename std::conditional<std::is_same<Tmemory, History>::value,
                                                                    MemoryState,
                                                                    void
                                                                 >::type
                                     >::type Memory;

    typedef typename AgentDataAccess<typename PythonDomainProxyBase<Texecution>::Action, Tagent>::Data _Action;

    typedef typename std::conditional<std::is_same<Tcontrollability, FullyControllable>::value,
                                        _Action,
                                        void
                                     >::type Action;
    
    typedef typename AgentDataAccess<typename PythonDomainProxyBase<Texecution>::Event, Tagent>::Data _Event;

    typedef typename std::conditional<std::is_same<Tcontrollability, FullyControllable>::value,
                                        Action,
                                        typename std::conditional<std::is_same<Tcontrollability, PartiallyControllable>::value,
                                                                    _Event,
                                                                    void
                                                                 >::type
                                     >::type Event;

    typedef typename AgentDataAccess<typename PythonDomainProxyBase<Texecution>::ApplicableActionSpace, Tagent>::Data ApplicableActionSpace;
    typedef typename AgentDataAccess<typename PythonDomainProxyBase<Texecution>::Value, Tagent>::Data Value;
    typedef typename AgentDataAccess<typename PythonDomainProxyBase<Texecution>::Predicate, Tagent>::Data Predicate;

    template <typename Derived, typename SSituation>
    class Outcome : public PyObj<Derived> {
    public :
        typedef SSituation Situation;
        typedef Value Value;
        typedef Predicate Predicate;
        typedef typename AgentDataAccess<typename PythonDomainProxyBase<Texecution>::OutcomeInfo, Tagent>::Data Info;

        Outcome();
        Outcome(std::unique_ptr<py::object>&& outcome);
        Outcome(const py::object& outcome);
        Outcome(const Situation& situation, const Value& transition_value,
                const Predicate& termination, const Info& info);
        Outcome(const Outcome& other);
        Outcome& operator=(const Outcome& other);
        virtual ~Outcome();

        Situation situation() const;
        void situation(const Situation& s);
        Value transition_value() const;
        void transition_value(const Value& tv);
        Predicate termination() const;
        void termination(const Predicate& t);
        Info info() const;
        void info(const Info& i);
    
    private :
        void construct(const Situation& situation = Situation(),
                       const Value& transition_value = Value(),
                       const Predicate& termination = Predicate(),
                       const Info& info = Info());
    };

    class TransitionOutcome : public Outcome<TransitionOutcome, State> {
    public :
        typedef State State;

        static constexpr char pyclass[] = "TransitionOutcome";
        static constexpr char class_name[] = "transition outcome";
        static constexpr char situation_name[] = "state"; // mandatory since State == Observation in fully observable domains

        TransitionOutcome();
        TransitionOutcome(std::unique_ptr<py::object>&& outcome);
        TransitionOutcome(const py::object& outcome);
        TransitionOutcome(const State& state,
                          const Value& transition_value,
                          const Predicate& termination,
                          const typename Outcome<TransitionOutcome, State>::Info& info);
        TransitionOutcome(const Outcome<TransitionOutcome, State>& other);
        TransitionOutcome& operator=(const TransitionOutcome& other);
        virtual ~TransitionOutcome();

        State state();
        void state(const State& s);
    };

    class EnvironmentOutcome : public Outcome<EnvironmentOutcome, Observation> {
    public :
        typedef Observation Observation;

        static constexpr char pyclass[] = "EnvironmentOutcome";
        static constexpr char class_name[] = "environment outcome";
        static constexpr char situation_name[] = "observation"; // mandatory since State == Observation in fully observable domains

        EnvironmentOutcome();
        EnvironmentOutcome(std::unique_ptr<py::object>&& outcome);
        EnvironmentOutcome(const py::object& outcome);
        EnvironmentOutcome(const Observation& observation,
                           const Value& transition_value,
                           const Predicate& termination,
                           const typename Outcome<EnvironmentOutcome, Observation>::Info& info);
        EnvironmentOutcome(const Outcome<EnvironmentOutcome, Observation>& other);
        EnvironmentOutcome& operator=(const EnvironmentOutcome& other);

        virtual ~EnvironmentOutcome();
        Observation observation();
        void observation(const Observation& o);
    };

    class NextStateDistribution : public PyObj<NextStateDistribution> {
    public :
        static constexpr char class_name[] = "next state distribution";

        NextStateDistribution();
        NextStateDistribution(std::unique_ptr<py::object>&& next_state_distribution);
        NextStateDistribution(const py::object& next_state_distribution);
        NextStateDistribution(const NextStateDistribution& other);
        NextStateDistribution& operator=(const NextStateDistribution& other);
        virtual ~NextStateDistribution();

        class DistributionValue {
        public :
            typedef State State;
            static constexpr char class_name[] = "distribution value";
            State _state;
            double _probability;

            DistributionValue();
            DistributionValue(const py::object& o);
            DistributionValue(const DistributionValue& other);
            DistributionValue& operator=(const DistributionValue& other);

            const State& state() const;
            const double& probability() const;
        };

        class NextStateDistributionValues : public PyObj<NextStateDistributionValues> {
        public :
            typedef PyIter<DistributionValue> PyIter;
            static constexpr char class_name[] = "next state distribution values";

            NextStateDistributionValues();
            NextStateDistributionValues(std::unique_ptr<py::object>&& next_state_distribution);
            NextStateDistributionValues(const py::object& next_state_distribution);
            NextStateDistributionValues(const NextStateDistributionValues& other);
            NextStateDistributionValues& operator=(const NextStateDistributionValues& other);
            virtual ~NextStateDistributionValues();

            PyIter begin() const;
            PyIter end() const;
        };

        NextStateDistributionValues get_values() const;
    
    private :
        void construct();
    };

    PythonDomainProxy(const py::object& domain);
    ~PythonDomainProxy();

    std::size_t get_parallel_capacity();
    ApplicableActionSpace get_applicable_actions(const Memory& m, const std::size_t* thread_id = nullptr);

    template <typename TTagent = Tagent,
              typename TTaction = Action,
              typename TactionAgent = typename PythonDomainProxyBase<Texecution>::Action,
              typename TagentApplicableActions = typename PythonDomainProxyBase<Texecution>::ApplicableActionSpace>
    std::enable_if_t<std::is_same<TTagent, MultiAgent>::value, TagentApplicableActions>
    get_agent_applicable_actions(const Memory& m,
                                 const TTaction& other_agents_actions,
                                 const TactionAgent& agent,
                                 const std::size_t* thread_id = nullptr);

    Observation reset(const std::size_t* thread_id = nullptr);
    EnvironmentOutcome step(const Event& e, const std::size_t* thread_id = nullptr);
    EnvironmentOutcome sample(const Memory& m, const Event& e, const std::size_t* thread_id = nullptr);
    State get_next_state(const Memory& m, const Event& e, const std::size_t* thread_id = nullptr);
    NextStateDistribution get_next_state_distribution(const Memory& m, const Event& e, const std::size_t* thread_id = nullptr);
    Value get_transition_value(const Memory& m, const Event& e, const State& sp, const std::size_t* thread_id = nullptr);
    bool is_goal(const State& s, const std::size_t* thread_id = nullptr);
    bool is_terminal(const State& s, const std::size_t* thread_id = nullptr);

    template <typename Tfunction, typename ... Types>
    std::unique_ptr<py::object> call(const std::size_t* thread_id, const Tfunction& func, const Types& ... args);

protected :

    template <typename TexecutionPolicy = Texecution, typename Enable = void>
    struct Implementation {};

    std::unique_ptr<Implementation<Texecution>> _implementation;
};

} // namespace skdecide

#ifdef SKDECIDE_HEADERS_ONLY
#include "impl/python_domain_proxy_pyobj_impl.hh"
#include "impl/python_domain_proxy_call_impl.hh"
#include "impl/python_domain_proxy_impl.hh"
#endif

#endif // SKDECIDE_PYTHON_DOMAIN_PROXY_HH
