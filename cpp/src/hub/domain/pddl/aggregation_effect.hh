/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef SKDECIDE_PDDL_AGGREGATION_EFFECT_HH
#define SKDECIDE_PDDL_AGGREGATION_EFFECT_HH

#include "effect.hh"
#include <vector>

namespace skdecide {

namespace pddl {

template <typename Derived> class AggregationEffect : public Effect {
public:
  typedef std::shared_ptr<AggregationEffect<Derived>> Ptr;
  typedef Effect::Ptr EffectPtr;
  typedef std::vector<Effect::Ptr> EffectVector;

  AggregationEffect();

  AggregationEffect(const AggregationEffect &other);

  AggregationEffect &operator=(const AggregationEffect &other);

  virtual ~AggregationEffect();

  AggregationEffect &append_effect(const Effect::Ptr &effect);

  /**
   * Removes the effect at a given index.
   * Throws an exception if the given index exceeds the size of the
   * aggregation effect
   */
  AggregationEffect &remove_effect(const std::size_t &index);

  /**
   * Gets the effect at a given index.
   * Throws an exception if the given index exceeds the size of the
   * aggregation effect
   */
  const Effect::Ptr &effect_at(const std::size_t &index);

  const EffectVector &get_effects() const;

  virtual std::ostream &print(std::ostream &o) const;

private:
  EffectVector _effects;
};

class ConjunctionEffect : public AggregationEffect<ConjunctionEffect> {
public:
  static constexpr char class_name[] = "and";

  typedef std::shared_ptr<ConjunctionEffect> Ptr;
  typedef AggregationEffect<ConjunctionEffect>::EffectPtr EffectPtr;
  typedef std::vector<EffectPtr> EffectVector;

  ConjunctionEffect();

  ConjunctionEffect(const ConjunctionEffect &other);

  ConjunctionEffect &operator=(const ConjunctionEffect &other);

  virtual ~ConjunctionEffect();
};

class DisjunctionEffect : public AggregationEffect<DisjunctionEffect> {
public:
  static constexpr char class_name[] = "oneof";

  typedef std::shared_ptr<DisjunctionEffect> Ptr;
  typedef AggregationEffect<DisjunctionEffect>::EffectPtr EffectPtr;
  typedef std::vector<EffectPtr> EffectVector;

  DisjunctionEffect();

  DisjunctionEffect(const DisjunctionEffect &other);

  DisjunctionEffect &operator=(const DisjunctionEffect &other);

  virtual ~DisjunctionEffect();
};

} // namespace pddl

} // namespace skdecide

#ifdef SKDECIDE_HEADERS_ONLY
#include "impl/aggregation_effect_impl.hh"
#endif

#endif // SKDECIDE_PDDL_AGGREGATION_EFFECT_HH