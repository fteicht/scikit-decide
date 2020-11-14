/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PYTHON_DOMAIN_PROXY_BASE_IMPL_HH
#define SKDECIDE_PYTHON_DOMAIN_PROXY_BASE_IMPL_HH

#include "utils/python_gil_control.hh"
#include "utils/python_globals.hh"
#include "utils/python_hash_eq.hh"
#include "utils/execution.hh"

namespace skdecide {

// === PyObj implementation ===

template <typename Texecution>
template <typename Derived, typename Tpyobj>
struct PythonDomainProxyBase<Texecution>::PyObj<Derived, Tpyobj>::TypeProxy {
    
    typedef typename PythonDomainProxyBase<Texecution>::PyObj<Derived, Tpyobj> PyObj;

    template <typename TTpyobj = Tpyobj,
              std::enable_if_t<std::is_same<TTpyobj, py::object>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po) {
        typename GilControl<Texecution>::Acquire acquire;
        po = std::make_unique<Tpyobj>(py::none());
    }

    template <typename TTpyobj = Tpyobj,
              std::enable_if_t<!std::is_same<TTpyobj, py::object>::value &&
                               std::is_base_of<py::object, TTpyobj>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po) {
        typename GilControl<Texecution>::Acquire acquire;
        po = std::make_unique<Tpyobj>(Tpyobj());
    }

    template <typename TTpyobj = Tpyobj,
              std::enable_if_t<std::is_same<TTpyobj, py::object>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po, std::unique_ptr<py::object>&& o, bool check = true) {
        typename GilControl<Texecution>::Acquire acquire; // !(*po) class python if *po is of type py::bool_
        po = std::move(o);
        if (check && (!po || !(*po))) {
            throw std::runtime_error(std::string("Unitialized python ") +
                                        Derived::class_name + " object!");
        }
    }

    template <typename TTpyobj = Tpyobj,
              std::enable_if_t<!std::is_same<TTpyobj, py::object>::value &&
                               std::is_base_of<py::object, TTpyobj>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po, std::unique_ptr<py::object>&& o, bool check = true) {
        typename GilControl<Texecution>::Acquire acquire;
        if (check && (!o || !(*o) || !py::isinstance<Tpyobj>(*o))) {
            throw std::runtime_error(std::string("Python ") + Derived::class_name + " object not initialized as a " +
                                        std::string(py::str(Tpyobj().attr("__class__").attr("__name__"))));
        }
        po = std::make_unique<Tpyobj>(o->template cast<Tpyobj>());
        o.reset();
    }

    template <typename TTpyobj = Tpyobj,
              std::enable_if_t<!std::is_same<TTpyobj, py::object>::value &&
                               std::is_base_of<py::object, TTpyobj>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po, std::unique_ptr<TTpyobj>&& o, bool check = true) {
        typename GilControl<Texecution>::Acquire acquire; // !(*po) class python if *po is of type py::bool_
        po = std::move(o);
        if (check && (!po || !(*po))) {
            throw std::runtime_error(std::string("Unitialized python ") +
                                        Derived::class_name + " object!");
        }
    }

    template <typename TTpyobj = Tpyobj,
              std::enable_if_t<!std::is_base_of<py::object, TTpyobj>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po, std::unique_ptr<TTpyobj>&& o, bool check = true) {
        po = std::move(o);
        if (check && !po) {
            throw std::runtime_error(std::string("Unitialized python ") +
                                        Derived::class_name + " object!");
        }
    }

    template <typename TTpyobj = Tpyobj,
              std::enable_if_t<std::is_same<TTpyobj, py::object>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po, const py::object& o, bool check = true) {
        typename GilControl<Texecution>::Acquire acquire;
        po = std::make_unique<py::object>(o);
        if (check && !(*po)) {
            throw std::runtime_error(std::string("Unitialized python ") +
                                        Derived::class_name + " object!");
        }
    }

    template <typename TTpyobj = Tpyobj,
              std::enable_if_t<!std::is_same<TTpyobj, py::object>::value &&
                               std::is_base_of<py::object, TTpyobj>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po, const py::object& o, bool check = true) {
        if (check && (!o || !py::isinstance<Tpyobj>(o))) {
            throw std::runtime_error(std::string("Python ") + Derived::class_name + " object not initialized as a " +
                                        std::string(py::str(Tpyobj().attr("__class__").attr("__name__"))));
        }
        po = std::make_unique<Tpyobj>(o.template cast<Tpyobj>());
    }

    template <typename TTpyobj,
              std::enable_if_t<!std::is_same<TTpyobj, py::object>::value &&
                               std::is_base_of<py::object, TTpyobj>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po, const TTpyobj& o, bool check = true) {
        typename GilControl<Texecution>::Acquire acquire;
        po = std::make_unique<Tpyobj>(o);
        if (check && !(*po)) {
            throw std::runtime_error(std::string("Unitialized python ") +
                                        Derived::class_name + " object!");
        }
    }

    template <typename TTpyobj,
              std::enable_if_t<!std::is_base_of<py::object, TTpyobj>::value &&
                               !std::is_base_of<PyObj, TTpyobj>::value, int> = 0>
    static void construct(std::unique_ptr<Tpyobj>& po, const TTpyobj& o, [[maybe_unused]] bool check = true) {
        typename GilControl<Texecution>::Acquire acquire;
        po = std::make_unique<Tpyobj>(o);
    }
};

#define SK_PYOBJ_TEMPLATE_DECL \
template <typename Texecution> \
template <typename Derived, typename Tpyobj>

#define SK_PY_OBJ_CLASS \
PythonDomainProxyBase<Texecution>::PyObj<Derived, Tpyobj>

#define SK_PY_OBJ_TYPE \
typename PythonDomainProxyBase<Texecution>::template PyObj<Derived, Tpyobj>

SK_PYOBJ_TEMPLATE_DECL
SK_PY_OBJ_CLASS::PyObj() {
    TypeProxy::construct(_pyobj);
}

SK_PYOBJ_TEMPLATE_DECL
template <typename TTpyobj>
SK_PY_OBJ_CLASS::PyObj(std::unique_ptr<TTpyobj>&& o, bool check) {
    TypeProxy::construct(_pyobj, std::move(o), check);
}

SK_PYOBJ_TEMPLATE_DECL
SK_PY_OBJ_CLASS::PyObj(const PyObj& other) {
    typename GilControl<Texecution>::Acquire acquire;
    this->_pyobj = std::make_unique<Tpyobj>(*other._pyobj);
}

SK_PYOBJ_TEMPLATE_DECL
SK_PY_OBJ_TYPE& SK_PY_OBJ_CLASS::operator=(const PyObj& other) {
    typename GilControl<Texecution>::Acquire acquire;
    *(this->_pyobj) = *other._pyobj;
    return *this;
}

SK_PYOBJ_TEMPLATE_DECL
SK_PY_OBJ_CLASS::~PyObj() {
    typename GilControl<Texecution>::Acquire acquire;
    _pyobj.reset();
}

SK_PYOBJ_TEMPLATE_DECL
const Tpyobj& SK_PY_OBJ_CLASS::pyobj() const {
    return *_pyobj;
}

SK_PYOBJ_TEMPLATE_DECL
std::string SK_PY_OBJ_CLASS::print() const {
    typename GilControl<Texecution>::Acquire acquire;
    return py::str(*_pyobj);
}

SK_PYOBJ_TEMPLATE_DECL
std::size_t SK_PY_OBJ_CLASS::Hash::operator()(const PyObj<Derived, Tpyobj>& o) const {
    try {
        return skdecide::PythonHash<Texecution>()(*o._pyobj);
    } catch(const std::exception& e) {
        spdlog::error(std::string("SKDECIDE exception when hashing ") +
                        Derived::class_name + "s: " + std::string(e.what()));
        throw;
    }
}

SK_PYOBJ_TEMPLATE_DECL
bool SK_PY_OBJ_CLASS::Equal::operator()(const PyObj<Derived, Tpyobj>& o1, const PyObj<Derived, Tpyobj>& o2) const {
    try {
        return skdecide::PythonEqual<Texecution>()(*o1._pyobj, *o2._pyobj);
    } catch(const std::exception& e) {
        spdlog::error(std::string("SKDECIDE exception when testing ") +
                        Derived::class_name + "s equality: " + std::string(e.what()));
        throw;
    }
}

// === PyIter implementation ===

template <typename Texecution>
template<typename T, typename Titerator>
struct PythonDomainProxyBase<Texecution>::PyIter<T, Titerator>::TypeProxy {
    template <typename TTiterator = Titerator,
              std::enable_if_t<!std::is_same<TTiterator, py::detail::dict_iterator>::value, int> = 0>
    static T dereference_object(Titerator& pit) {
        typename GilControl<Texecution>::Acquire acquire;
        return T(py::reinterpret_borrow<py::object>(*pit));
    }

    template <typename TTiterator = Titerator,
              std::enable_if_t<std::is_same<TTiterator, py::detail::dict_iterator>::value, int> = 0>
    static T dereference_object(Titerator& pit) {
        typename GilControl<Texecution>::Acquire acquire;
        return T(py::make_tuple(py::reinterpret_borrow<py::object>(pit->first),
                                py::reinterpret_borrow<py::object>(pit->second)));
    }

    template <typename TTiterator = Titerator,
              std::enable_if_t<!std::is_same<TTiterator, py::detail::dict_iterator>::value, int> = 0>
    static std::unique_ptr<T> dereference_pointer(Titerator& pit) {
        typename GilControl<Texecution>::Acquire acquire;
        return std::make_unique<T>(py::reinterpret_borrow<py::object>(*pit));
    }

    template <typename TTiterator = Titerator,
              std::enable_if_t<std::is_same<TTiterator, py::detail::dict_iterator>::value, int> = 0>
    static std::unique_ptr<T> dereference_pointer(Titerator& pit) {
        typename GilControl<Texecution>::Acquire acquire;
        return std::make_unique<T>(py::make_tuple(py::reinterpret_borrow<py::object>(pit->first),
                                                  py::reinterpret_borrow<py::object>(pit->second)));
    }
};

#define SK_PY_ITER_TEMPLATE_DECL \
template <typename Texecution> \
template <typename T, typename Titerator>

#define SK_PY_ITER_CLASS \
PythonDomainProxyBase<Texecution>::PyIter<T, Titerator>

#define SK_PY_ITER_TYPE \
typename PythonDomainProxyBase<Texecution>::template PyIter<T, Titerator>

SK_PY_ITER_TEMPLATE_DECL
SK_PY_ITER_CLASS::PyIter(const Titerator& iterator)
: PyObj<PyIter<T, Titerator>, Titerator>(iterator, false) {}

SK_PY_ITER_TEMPLATE_DECL
SK_PY_ITER_CLASS::PyIter(const PyIter<T, Titerator>& other)
: PyObj<PyIter<T, Titerator>, Titerator>(other) {}

SK_PY_ITER_TEMPLATE_DECL
SK_PY_ITER_TYPE& SK_PY_ITER_CLASS::operator=(const PyIter<T, Titerator>& other) {
    static_cast<PyObj<PyIter<T, Titerator>, Titerator>&>(*this) = other;
    return *this;
}

SK_PY_ITER_TEMPLATE_DECL
SK_PY_ITER_CLASS::~PyIter() {}

SK_PY_ITER_TEMPLATE_DECL
SK_PY_ITER_TYPE& SK_PY_ITER_CLASS::operator++() {
    typename GilControl<Texecution>::Acquire acquire;
    ++(*(this->_pyobj));
    return *this;
}

SK_PY_ITER_TEMPLATE_DECL
SK_PY_ITER_TYPE SK_PY_ITER_CLASS::operator++(int) {
    typename GilControl<Texecution>::Acquire acquire;
    Titerator rv = (*(this->_pyobj))++;
    return PyIter<T, Titerator>(rv);
}

SK_PY_ITER_TEMPLATE_DECL
T SK_PY_ITER_CLASS::operator*() const {
    return typename SK_PY_ITER_CLASS::TypeProxy::dereference_object(this->_pyobj);
}

SK_PY_ITER_TEMPLATE_DECL
std::unique_ptr<T> SK_PY_ITER_CLASS::operator->() const {
    return typename SK_PY_ITER_CLASS::TypeProxy::dereference_pointer(this->_pyobj);
}

SK_PY_ITER_TEMPLATE_DECL
bool SK_PY_ITER_CLASS::operator==(const PyIter<T, Titerator>& other) const {
    typename GilControl<Texecution>::Acquire acquire;
    return *(this->_pyobj) == *(other._pyobj);
}

SK_PY_ITER_TEMPLATE_DECL
bool SK_PY_ITER_CLASS::operator!=(const PyIter<T, Titerator>& other) const {
    typename GilControl<Texecution>::Acquire acquire;
    return *(this->_pyobj) != *(other._pyobj);
}

// === State implementation ===

#define SK_PY_STATE_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_STATE_CLASS \
PythonDomainProxyBase<Texecution>::State

#define SK_PY_STATE_TYPE \
typename PythonDomainProxyBase<Texecution>::State

SK_PY_STATE_TEMPLATE_DECL
SK_PY_STATE_CLASS::State() : PyObj<State>() {}

SK_PY_STATE_TEMPLATE_DECL
SK_PY_STATE_CLASS::State(std::unique_ptr<py::object>&& s)
: PyObj<State>(std::move(s)) {}

SK_PY_STATE_TEMPLATE_DECL
SK_PY_STATE_CLASS::State(const py::object& s)
: PyObj<State>(s) {}

SK_PY_STATE_TEMPLATE_DECL
SK_PY_STATE_CLASS::State(const State& other)
: PyObj<State>(other) {}

SK_PY_STATE_TEMPLATE_DECL
SK_PY_STATE_TYPE& SK_PY_STATE_CLASS::operator=(const State& other) {
    static_cast<PyObj<State>&>(*this) = other;
    return *this;
}

SK_PY_STATE_TEMPLATE_DECL
SK_PY_STATE_CLASS::~State() {}

// === Observation implementation ===

#define SK_PY_OBSERVATION_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_OBSERVATION_CLASS \
PythonDomainProxyBase<Texecution>::Observation

#define SK_PY_OBSERVATION_TYPE \
typename PythonDomainProxyBase<Texecution>::Observation

SK_PY_OBSERVATION_TEMPLATE_DECL
SK_PY_OBSERVATION_CLASS::Observation() : PyObj<Observation>() {}

SK_PY_OBSERVATION_TEMPLATE_DECL
SK_PY_OBSERVATION_CLASS::Observation(std::unique_ptr<py::object>&& o)
: PyObj<Observation>(std::move(o)) {}

SK_PY_OBSERVATION_TEMPLATE_DECL
SK_PY_OBSERVATION_CLASS::Observation(const py::object& o)
: PyObj<Observation>(o) {}

SK_PY_OBSERVATION_TEMPLATE_DECL
SK_PY_OBSERVATION_CLASS::Observation(const Observation& other)
: PyObj<Observation>(other) {}

SK_PY_OBSERVATION_TEMPLATE_DECL
SK_PY_OBSERVATION_TYPE& SK_PY_OBSERVATION_CLASS::operator=(const Observation& other) {
    static_cast<PyObj<Observation>&>(*this) = other;
    return *this;
}

SK_PY_OBSERVATION_TEMPLATE_DECL
SK_PY_OBSERVATION_CLASS::~Observation() {}

// === Event implementation ===

#define SK_PY_EVENT_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_EVENT_CLASS \
PythonDomainProxyBase<Texecution>::Event

#define SK_PY_EVENT_TYPE \
typename PythonDomainProxyBase<Texecution>::Event

SK_PY_EVENT_TEMPLATE_DECL
SK_PY_EVENT_CLASS::Event() : PyObj<Event>() {}

SK_PY_EVENT_TEMPLATE_DECL
SK_PY_EVENT_CLASS::Event(std::unique_ptr<py::object>&& e)
: PyObj<Event>(std::move(e)) {}

SK_PY_EVENT_TEMPLATE_DECL
SK_PY_EVENT_CLASS::Event(const py::object& e)
: PyObj<Event>(e) {}

SK_PY_EVENT_TEMPLATE_DECL
SK_PY_EVENT_CLASS::Event(const Event& other) : PyObj<Event>(other) {}

SK_PY_EVENT_TEMPLATE_DECL
SK_PY_EVENT_TYPE& SK_PY_EVENT_CLASS::operator=(const Event& other) {
    static_cast<PyObj<Event>&>(*this) = other;
    return *this;
}

SK_PY_EVENT_TEMPLATE_DECL
SK_PY_EVENT_CLASS::~Event() {}

// === Action implementation ===

#define SK_PY_ACTION_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_ACTION_CLASS \
PythonDomainProxyBase<Texecution>::Action

#define SK_PY_ACTION_TYPE \
typename PythonDomainProxyBase<Texecution>::Action

SK_PY_ACTION_TEMPLATE_DECL
SK_PY_ACTION_CLASS::Action() : PyObj<Action>() {}

SK_PY_ACTION_TEMPLATE_DECL
SK_PY_ACTION_CLASS::Action(std::unique_ptr<py::object>&& a)
: PyObj<Action>(std::move(a)) {}

SK_PY_ACTION_TEMPLATE_DECL
SK_PY_ACTION_CLASS::Action(const py::object& a)
: PyObj<Action>(a) {}

SK_PY_ACTION_TEMPLATE_DECL
SK_PY_ACTION_CLASS::Action(const Action& other)
: PyObj<Action>(other) {}

SK_PY_ACTION_TEMPLATE_DECL
SK_PY_ACTION_TYPE& SK_PY_ACTION_CLASS::operator=(const Action& other) {
    static_cast<PyObj<Action>&>(*this) = other;
    return *this;
}

SK_PY_ACTION_TEMPLATE_DECL
SK_PY_ACTION_CLASS::~Action() {}

// === ApplicableActionSpaceElements implementation ===

#define SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS \
PythonDomainProxyBase<Texecution>::ApplicableActionSpace::Elements

#define SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TYPE \
typename PythonDomainProxyBase<Texecution>::ApplicableActionSpace::Elements

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::Elements() : PyObj<Elements>() {}

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::Elements(std::unique_ptr<py::object>&& applicable_action_space_elements)
: PyObj<Elements>(std::move(applicable_action_space_elements)) {}

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::Elements(const py::object& applicable_action_space_elements)
: PyObj<Elements>(applicable_action_space_elements) {}

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::Elements(const Elements& other)
: PyObj<Elements>(other) {}

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TYPE&
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::operator=(const Elements& other) {
    static_cast<PyObj<Elements>&>(*this) = other;
    return *this;
}

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::~Elements() {}

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
typename SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::PyIter
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::begin() const {
    typename GilControl<Texecution>::Acquire acquire;
    return PyIter(this->_pyobj->begin());
}

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
typename SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::PyIter
SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::end() const {
    typename GilControl<Texecution>::Acquire acquire;
    return PyIter(this->_pyobj->end());
}

SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_TEMPLATE_DECL
bool SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS::empty() const {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        if (py::isinstance<py::list>(*(this->_pyobj))) {
            return py::cast<py::list>(*(this->_pyobj)).empty();
        } else if (py::isinstance<py::tuple>(*(this->_pyobj))) {
            return py::cast<py::tuple>(*(this->_pyobj)).empty();
        } else if (py::isinstance<py::dict>(*(this->_pyobj))) {
            return py::cast<py::dict>(*(this->_pyobj)).empty();
        } else if (py::isinstance<py::set>(*(this->_pyobj))) {
            return py::cast<py::set>(*(this->_pyobj)).empty();
        } else if (py::isinstance<py::sequence>(*(this->_pyobj))) {
            return py::cast<py::sequence>(*(this->_pyobj)).empty();
        } else {
            throw std::runtime_error("SKDECIDE exception: applicable action space elements must be iterable.");
        }
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when checking emptiness of applicable action space's elements: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}

// === ApplicableActionSpace implementation ===

#define SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_APPLICABLE_ACTION_SPACE_CLASS \
PythonDomainProxyBase<Texecution>::ApplicableActionSpace

#define SK_PY_APPLICABLE_ACTION_SPACE_TYPE \
typename PythonDomainProxyBase<Texecution>::ApplicableActionSpace

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_CLASS::ApplicableActionSpace()
: PyObj<ApplicableActionSpace>() {
    construct();
}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_CLASS::ApplicableActionSpace(std::unique_ptr<py::object>&& applicable_action_space)
: PyObj<ApplicableActionSpace>(std::move(applicable_action_space)) {
    construct();
}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_CLASS::ApplicableActionSpace(const py::object& applicable_action_space)
: PyObj<ApplicableActionSpace>(applicable_action_space) {
    construct();
}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
void SK_PY_APPLICABLE_ACTION_SPACE_CLASS::construct() {
    typename GilControl<Texecution>::Acquire acquire;
    if (this->_pyobj->is_none()) {
        this->_pyobj = std::make_unique<py::object>(skdecide::Globals::skdecide().attr("EmptySpace")());
    }
}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_CLASS::ApplicableActionSpace(const ApplicableActionSpace& other)
: PyObj<ApplicableActionSpace>(other) {}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_TYPE& SK_PY_APPLICABLE_ACTION_SPACE_CLASS::operator=(const ApplicableActionSpace& other) {
    static_cast<PyObj<ApplicableActionSpace>&>(*this) = other;
    return *this;
}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
SK_PY_APPLICABLE_ACTION_SPACE_CLASS::~ApplicableActionSpace() {}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
typename SK_PY_APPLICABLE_ACTION_SPACE_ELEMENTS_CLASS SK_PY_APPLICABLE_ACTION_SPACE_CLASS::get_elements() const {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        if (!py::hasattr(*(this->_pyobj), "get_elements")) {
            throw std::invalid_argument("SKDECIDE exception: python applicable action object must implement get_elements()");
        }
        return Elements(this->_pyobj->attr("get_elements")());
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when getting applicable action space's elements: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    } catch (const std::exception& e) {
        spdlog::error(std::string("SKDECIDE exception when getting applicable action space's elements: ") + std::string(e.what()));
        throw;
    }
}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
bool SK_PY_APPLICABLE_ACTION_SPACE_CLASS::empty() const {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        return py::isinstance(*(this->_pyobj), skdecide::Globals::skdecide().attr("EmptySpace")) ||
            this->get_elements().empty();
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when checking emptyness of applicable action space's elements: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
SK_PY_ACTION_TYPE SK_PY_APPLICABLE_ACTION_SPACE_CLASS::sample() const {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        if (!py::hasattr(*(this->_pyobj), "sample")) {
            throw std::invalid_argument("SKDECIDE exception: python applicable action object must implement sample()");
        } else {
            return Action(this->_pyobj->attr("sample")());
        }
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when sampling action data: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    } catch (const std::exception& e) {
        spdlog::error(std::string("SKDECIDE exception when sampling action data: ") + std::string(e.what()));
        throw;
    }
}

SK_PY_APPLICABLE_ACTION_SPACE_TEMPLATE_DECL
bool SK_PY_APPLICABLE_ACTION_SPACE_CLASS::contains(const Action& action) {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        if (!py::hasattr(*(this->_pyobj), "contains")) {
            throw std::invalid_argument("SKDECIDE exception: python applicable action object must implement contains()");
        } else {
            return this->_pyobj->attr("contains")(action.pyobj()).template cast<bool>();
        }
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when checking inclusion of action data: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}

// === Value implementation ===

#define SK_PY_VALUE_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_VALUE_CLASS \
PythonDomainProxyBase<Texecution>::Value

#define SK_PY_VALUE_TYPE \
typename PythonDomainProxyBase<Texecution>::Value

SK_PY_VALUE_TEMPLATE_DECL
SK_PY_VALUE_CLASS::Value()
: PyObj<Value>() {
    construct();
}

SK_PY_VALUE_TEMPLATE_DECL
SK_PY_VALUE_CLASS::Value(std::unique_ptr<py::object>&& v)
: PyObj<Value>(std::move(v)) {
    construct();
}

SK_PY_VALUE_TEMPLATE_DECL
SK_PY_VALUE_CLASS::Value(const py::object& v)
: PyObj<Value>(v) {
    construct();
}

SK_PY_VALUE_TEMPLATE_DECL
SK_PY_VALUE_CLASS::Value(const double& value, const bool& reward_or_cost)
: PyObj<Value>() {
    construct(value, reward_or_cost);
}

SK_PY_VALUE_TEMPLATE_DECL
SK_PY_VALUE_CLASS::Value(const Value& other)
: PyObj<Value>(other) {}

SK_PY_VALUE_TEMPLATE_DECL
SK_PY_VALUE_TYPE& SK_PY_VALUE_CLASS::operator=(const Value& other) {
    static_cast<PyObj<Value>&>(*this) = other;
    return *this;
}

SK_PY_VALUE_TEMPLATE_DECL
SK_PY_VALUE_CLASS::~Value() {}

SK_PY_VALUE_TEMPLATE_DECL
void SK_PY_VALUE_CLASS::construct(const double& value, const bool& reward_or_cost) {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        if (this->_pyobj->is_none()) {
            this->_pyobj = std::make_unique<py::object>(skdecide::Globals::skdecide().attr("Value")());
            if (reward_or_cost) {
                this->reward(value);
            } else {
                this->cost(value);
            }
        } else {
            if (!py::hasattr(*(this->_pyobj), "cost")) {
                throw std::invalid_argument("SKDECIDE exception: python value object must provide the 'cost' attribute");
            }
            if (!py::hasattr(*(this->_pyobj), "reward")) {
                throw std::invalid_argument("SKDECIDE exception: python value object must provide the 'reward' attribute");
            }
        }
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when importing value data: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}

SK_PY_VALUE_TEMPLATE_DECL
double SK_PY_VALUE_CLASS::cost() const {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        return py::cast<double>(this->_pyobj->attr("cost"));
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when getting value's cost: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}


SK_PY_VALUE_TEMPLATE_DECL
void SK_PY_VALUE_CLASS::cost(const double& c) {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        this->_pyobj->attr("cost") = py::float_(c);
        this->_pyobj->attr("reward") = py::float_(-c);
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when setting value's cost: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}

SK_PY_VALUE_TEMPLATE_DECL
double SK_PY_VALUE_CLASS::reward() const {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        return this->_pyobj->attr("reward").template cast<double>();
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when getting value's reward: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}

SK_PY_VALUE_TEMPLATE_DECL
void SK_PY_VALUE_CLASS::reward(const double& r) {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        this->_pyobj->attr("reward") = py::float_(r);
        this->_pyobj->attr("cost") = py::float_(-r);
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when setting value's reward: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}

// === Predicate implementation ===

#define SK_PY_PREDICATE_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_PREDICATE_CLASS \
PythonDomainProxyBase<Texecution>::Predicate

#define SK_PY_PREDICATE_TYPE \
typename PythonDomainProxyBase<Texecution>::Predicate

SK_PY_PREDICATE_TEMPLATE_DECL
SK_PY_PREDICATE_CLASS::Predicate()
: PyObj<Predicate, py::bool_>() {
    construct();
}

SK_PY_PREDICATE_TEMPLATE_DECL
SK_PY_PREDICATE_CLASS::Predicate(std::unique_ptr<py::object>&& v)
: PyObj<Predicate, py::bool_>(std::move(v)) {
    construct();
}

SK_PY_PREDICATE_TEMPLATE_DECL
SK_PY_PREDICATE_CLASS::Predicate(const py::object& v)
: PyObj<Predicate, py::bool_>(v) {
    construct();
}

SK_PY_PREDICATE_TEMPLATE_DECL
SK_PY_PREDICATE_CLASS::Predicate(const bool& predicate)
: PyObj<Predicate, py::bool_>() {
    construct(predicate);
}

SK_PY_PREDICATE_TEMPLATE_DECL
SK_PY_PREDICATE_CLASS::Predicate(const Predicate& other)
: PyObj<Predicate, py::bool_>(other) {}

SK_PY_PREDICATE_TEMPLATE_DECL
SK_PY_PREDICATE_TYPE& SK_PY_PREDICATE_CLASS::operator=(const Predicate& other) {
    static_cast<PyObj<Predicate, py::bool_>&>(*this) = other;
    return *this;
}

SK_PY_PREDICATE_TEMPLATE_DECL
SK_PY_PREDICATE_CLASS::~Predicate() {}

SK_PY_PREDICATE_TEMPLATE_DECL
void SK_PY_PREDICATE_CLASS::construct(const bool& predicate) {
    typename GilControl<Texecution>::Acquire acquire;
    if (this->_pyobj->is_none()) {
        this->_pyobj = std::make_unique<py::bool_>(predicate);
    }
}

SK_PY_PREDICATE_TEMPLATE_DECL
bool SK_PY_PREDICATE_CLASS::predicate() const {
    typename GilControl<Texecution>::Acquire acquire;
    return this->_pyobj->template cast<bool>();
}

SK_PY_PREDICATE_TEMPLATE_DECL
SK_PY_PREDICATE_CLASS::operator bool() const {
    return predicate();
}

SK_PY_PREDICATE_TEMPLATE_DECL
void SK_PY_PREDICATE_CLASS::predicate(const bool& p)  {
    typename GilControl<Texecution>::Acquire acquire;
    *(this->_pyobj) = py::bool_(p);
}

SK_PY_PREDICATE_TEMPLATE_DECL
void SK_PY_PREDICATE_CLASS::operator=(const bool& p) {
    predicate(p);
}

// === OutcomeInfo implementation ===

#define SK_PY_OUTCOME_INFO_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_OUTCOME_INFO_CLASS \
PythonDomainProxyBase<Texecution>::OutcomeInfo

#define SK_PY_OUTCOME_INFO_TYPE \
typename PythonDomainProxyBase<Texecution>::OutcomeInfo

SK_PY_OUTCOME_INFO_TEMPLATE_DECL
SK_PY_OUTCOME_INFO_CLASS::OutcomeInfo() : PyObj<OutcomeInfo>() {}

SK_PY_OUTCOME_INFO_TEMPLATE_DECL
SK_PY_OUTCOME_INFO_CLASS::OutcomeInfo(std::unique_ptr<py::object>&& s)
: PyObj<OutcomeInfo>(std::move(s)) {}

SK_PY_OUTCOME_INFO_TEMPLATE_DECL
SK_PY_OUTCOME_INFO_CLASS::OutcomeInfo(const py::object& s)
: PyObj<OutcomeInfo>(s) {}

SK_PY_OUTCOME_INFO_TEMPLATE_DECL
SK_PY_OUTCOME_INFO_CLASS::OutcomeInfo(const OutcomeInfo& other)
: PyObj<OutcomeInfo>(other) {}

SK_PY_OUTCOME_INFO_TEMPLATE_DECL
SK_PY_OUTCOME_INFO_TYPE& SK_PY_OUTCOME_INFO_CLASS::operator=(const OutcomeInfo& other) {
    static_cast<PyObj<OutcomeInfo>&>(*this) = other;
    return *this;
}

SK_PY_OUTCOME_INFO_TEMPLATE_DECL
SK_PY_OUTCOME_INFO_CLASS::~OutcomeInfo() {}

SK_PY_OUTCOME_INFO_TEMPLATE_DECL
std::size_t SK_PY_OUTCOME_INFO_CLASS::get_depth() const {
    typename GilControl<Texecution>::Acquire acquire;
    try {
        if (py::hasattr(*(this->_pyobj), "depth")) {
            return py::cast<std::size_t>(this->_pyobj->attr("depth")());
        } else {
            return 0;
        }
    } catch(const py::error_already_set* e) {
        spdlog::error(std::string("SKDECIDE exception when getting outcome's depth info: ") + std::string(e->what()));
        std::runtime_error err(e->what());
        delete e;
        throw err;
    }
}

// === Agent implementation ===

template <typename Texecution>
struct PythonDomainProxyBase<Texecution>::AgentDataAccess::Agent::TypeProxy {
    template <typename Tother,
              std::enable_if_t<std::is_same<py::object, Tother>::value, int> = 0>
    static const py::object& agent(const Tother& other) {
        return other;
    }

    template <typename Tother,
              std::enable_if_t<std::is_same<Agent, Tother>::value, int> = 0>
    static const Agent& agent(const Tother& other) {
        return other;
    }

    template <typename Tother,
              std::enable_if_t<!std::is_base_of<py::object, Tother>::value &&
                               !std::is_base_of<Agent, Tother>::value, int> = 0>
    static const py::object& agent(const Tother& other) {
        return other.pyobj();
    }
};

#define SK_PY_AGENT_DATA_AGENT_TEMPLATE_DECL \
template <typename Texecution>

#define SK_PY_AGENT_DATA_AGENT_CLASS \
PythonDomainProxyBase<Texecution>::AgentDataAccess::Agent

SK_PY_AGENT_DATA_AGENT_TEMPLATE_DECL
SK_PY_AGENT_DATA_AGENT_CLASS::Agent() : PyObj<Agent>() {}

SK_PY_AGENT_DATA_AGENT_TEMPLATE_DECL
SK_PY_AGENT_DATA_AGENT_CLASS::Agent(std::unique_ptr<py::object>&& a)
: PyObj<Agent>(std::move(a)) {}

SK_PY_AGENT_DATA_AGENT_TEMPLATE_DECL
template <typename Tother>
SK_PY_AGENT_DATA_AGENT_CLASS::Agent(const Tother& other)
: Agent(TypeProxy::agent(other.pyobj())) {}

SK_PY_AGENT_DATA_AGENT_TEMPLATE_DECL
typename SK_PY_AGENT_DATA_AGENT_CLASS& SK_PY_AGENT_DATA_AGENT_CLASS::operator=(const Agent& other) {
    static_cast<PyObj<Agent>&>(*this) = other;
    return *this;
}

SK_PY_AGENT_DATA_AGENT_TEMPLATE_DECL
SK_PY_AGENT_DATA_AGENT_CLASS::~Agent() {}

// === Item implementation ===

#define SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL \
template <typename Texecution> template <typename TagentData>

#define SK_PY_AGENT_DATA_ITEM_CLASS \
PythonDomainProxyBase<Texecution>::AgentDataAccess::Item<TagentData>

#define SK_PY_AGENT_DATA_ITEM_TYPE \
typename PythonDomainProxyBase<Texecution>::AgentDataAccess::template Item<TagentData>

SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL
SK_PY_AGENT_DATA_ITEM_CLASS::Item()
: PyObj<Item<TagentData>, py::tuple>() {}

SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL
SK_PY_AGENT_DATA_ITEM_CLASS::Item(std::unique_ptr<py::object>&& a)
: PyObj<Item<TagentData>, py::tuple>(std::move(a)) {}

SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL
SK_PY_AGENT_DATA_ITEM_CLASS::Item(const py::object& a)
: PyObj<Item<TagentData>, py::tuple>(a) {}

SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL
SK_PY_AGENT_DATA_ITEM_CLASS::Item(const Item<TagentData>& other)
: PyObj<Item<TagentData>, py::tuple>(other) {}

SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL
SK_PY_AGENT_DATA_ITEM_TYPE& SK_PY_AGENT_DATA_ITEM_CLASS::operator=(const Item<TagentData>& other) {
    static_cast<PyObj<Item<TagentData>, py::tuple>&>(*this) = other;
    return *this;
}

SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL
SK_PY_AGENT_DATA_ITEM_CLASS::~Item() {}

SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL
typename SK_PY_AGENT_DATA_AGENT_CLASS SK_PY_AGENT_DATA_ITEM_CLASS::agent() {
    typename GilControl<Texecution>::Acquire acquire;
    return Agent(py::cast<py::object>((*(this->_pyobj))[0]));
}

SK_PY_AGENT_DATA_ITEM_TEMPLATE_DECL
TagentData SK_PY_AGENT_DATA_ITEM_CLASS::data() {
    typename GilControl<Texecution>::Acquire acquire;
    return TagentData(py::cast<py::object>((*(this->_pyobj))[1]));
}

// === AgentDataAccessor implementation ===

#define SK_PY_AGENT_DATA_ACCESSOR_TEMPLATE_DECL \
template <typename Texecution> template <typename TagentData>

#define SK_PY_AGENT_DATA_ACCESSOR_CLASS \
PythonDomainProxyBase<Texecution>::AgentDataAccess::AgentDataAccessor<TagentData>

#define SK_PY_AGENT_DATA_ACCESSOR_TYPE \
typename PythonDomainProxyBase<Texecution>::AgentDataAccess::template AgentDataAccessor<TagentData>

SK_PY_AGENT_DATA_ACCESSOR_TEMPLATE_DECL
SK_PY_AGENT_DATA_ACCESSOR_CLASS::AgentDataAccessor(const py::detail::item_accessor& a)
: PyObj<AgentDataAccessor<TagentData>, py::detail::item_accessor>(a), TagentData(a) {}

SK_PY_AGENT_DATA_ACCESSOR_TEMPLATE_DECL
SK_PY_AGENT_DATA_ACCESSOR_TYPE& SK_PY_AGENT_DATA_ACCESSOR_CLASS::operator=(AgentDataAccessor<TagentData>&& other) && {
    std::forward<AgentDataAccessor<TagentData>&&>(*this) = static_cast<const TagentData&>(other);
    return *this;
}

SK_PY_AGENT_DATA_ACCESSOR_TEMPLATE_DECL
void SK_PY_AGENT_DATA_ACCESSOR_CLASS::operator=(const TagentData& other) && {
    typename GilControl<Texecution>::Acquire acquire;
        std::forward<py::detail::item_accessor&&>(
            *static_cast<PyObj<AgentDataAccessor, py::detail::item_accessor>*>(this)->_pyobj) =
                other.pyobj();
    *static_cast<TagentData*>(this)->_pyobj = other.pyobj();
}

SK_PY_AGENT_DATA_ACCESSOR_TEMPLATE_DECL
SK_PY_AGENT_DATA_ACCESSOR_CLASS::~AgentDataAccessor() {}

} // namespace skdecide

#endif // SKDECIDE_PYTHON_DOMAIN_PROXY_BASE_IMPL_HH
