/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_COMPARISON_FORMULA_HH
#define SKDECIDE_PDDL_COMPARISON_FORMULA_HH

#include "formula.hh"
#include "binary_expression.hh"

namespace skdecide {

namespace pddl {

template <typename Derived>
class ComparisonFormula : public Formula, public BinaryExpression<Derived> {
public:
  typedef std::shared_ptr<ComparisonFormula<Derived>> Ptr;

  ComparisonFormula();
  ComparisonFormula(const Expression::Ptr &left_expression,
                    const Expression::Ptr &right_expression);
  ComparisonFormula(const ComparisonFormula &other);
  ComparisonFormula &operator=(const ComparisonFormula &other);

  virtual std::ostream &print(std::ostream &o) const;
  std::string print() const;
};

class GreaterFormula : public ComparisonFormula<GreaterFormula> {
public:
  static constexpr char class_name[] = ">";

  typedef std::shared_ptr<GreaterFormula> Ptr;

  GreaterFormula();
  GreaterFormula(const Expression::Ptr &left_expression,
                 const Expression::Ptr &right_expression);
  GreaterFormula(const GreaterFormula &other);
  GreaterFormula &operator=(const GreaterFormula &other);
  virtual ~GreaterFormula();
};

class GreaterEqFormula : public ComparisonFormula<GreaterEqFormula> {
public:
  static constexpr char class_name[] = ">=";

  typedef std::shared_ptr<GreaterEqFormula> Ptr;

  GreaterEqFormula();
  GreaterEqFormula(const Expression::Ptr &left_expression,
                   const Expression::Ptr &right_expression);
  GreaterEqFormula(const GreaterEqFormula &other);
  GreaterEqFormula &operator=(const GreaterEqFormula &other);
  virtual ~GreaterEqFormula();
};

class LessEqFormula : public ComparisonFormula<LessEqFormula> {
public:
  static constexpr char class_name[] = "<=";

  typedef std::shared_ptr<LessEqFormula> Ptr;

  LessEqFormula();
  LessEqFormula(const Expression::Ptr &left_expression,
                const Expression::Ptr &right_expression);
  LessEqFormula(const LessEqFormula &other);
  LessEqFormula &operator=(const LessEqFormula &other);
  virtual ~LessEqFormula();
};

class LessFormula : public ComparisonFormula<LessFormula> {
public:
  static constexpr char class_name[] = "<";

  typedef std::shared_ptr<LessFormula> Ptr;

  LessFormula();
  LessFormula(const Expression::Ptr &left_expression,
              const Expression::Ptr &right_expression);
  LessFormula(const LessFormula &other);
  LessFormula &operator=(const LessFormula &other);
  virtual ~LessFormula();
};

class EqFormula : public ComparisonFormula<EqFormula> {
public:
  static constexpr char class_name[] = "=";

  typedef std::shared_ptr<EqFormula> Ptr;

  EqFormula();
  EqFormula(const Expression::Ptr &left_expression,
            const Expression::Ptr &right_expression);
  EqFormula(const EqFormula &other);
  EqFormula &operator=(const EqFormula &other);
  virtual ~EqFormula();
};

} // namespace pddl

} // namespace skdecide

#ifdef SKDECIDE_HEADERS_ONLY
#include "impl/comparison_formula_impl.hh"
#endif

#endif // SKDECIDE_PDDL_COMPARISON_FORMULA_HH
