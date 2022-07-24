/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_FORMULA_HH
#define SKDECIDE_PDDL_FORMULA_HH

#include <memory>
#include <ostream>
#include <sstream>

namespace skdecide {

namespace pddl {

class Formula {
public:
  typedef std::shared_ptr<Formula> Ptr;

  virtual ~Formula();

  virtual std::ostream &print(std::ostream &o) const = 0;
  std::string print() const;
};

// Formula printing operator
std::ostream &operator<<(std::ostream &o, const Formula &f);

} // namespace pddl

} // namespace skdecide

#endif // SKDECIDE_PDDL_FORMULA_HH