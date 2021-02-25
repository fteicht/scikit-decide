/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef SKDECIDE_PDDL_NUMBER_HH
#define SKDECIDE_PDDL_NUMBER_HH

#include <memory>

namespace skdecide {

    namespace pddl {

        class Number {
        public :
            typedef std::shared_ptr<Number> Ptr;
            
            template <typename T>
            Number(const T& n) {
                _impl = std::make_unique<Impl<T>>(n);
            }

            bool is_double() const { return _impl->is_double(); }
            double as_double() const { return _impl->as_double(); }
            long as_long() const { return _impl->as_long(); }
            std::ostream& print(std::ostream& o) const { return _impl->print(o); };

        private :
            struct ImplBase {
                virtual ~ImplBase() {}
                virtual bool is_double() const =0;
                virtual double as_double() const =0;
                virtual long as_long() const =0;
                virtual std::ostream& print(std::ostream& o) const =0;
            };

            template <typename T>
            struct Impl : public ImplBase {
                T _n;
                Impl(const T& n) : _n(n) {}
                virtual ~Impl() {}
                virtual bool is_double() const { return std::is_floating_point<T>::value; }
                virtual double as_double() const { return (double) _n; }
                virtual long as_long() const { return (long) _n; }
                virtual std::ostream& print(std::ostream& o) const { o << _n; return o; }
            };

            std::unique_ptr<ImplBase> _impl;
        };

        // Number printing operator
        inline std::ostream& operator<<(std::ostream& o, const Number& n) {
            return n.print(o);
        }

    } // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_NUMBER_HH
