#pragma once

#include <functional>

namespace murk::crypto::aa {
  template<typename T, typename Int = uint64_t>
  class cyclic_group {
  public:
    T generator;
    std::function<T(T, T)> group_op;

  public:
    inline T operator()(T a, T b) const { return group_op(std::move(a), std::move(b)); }
    inline virtual T exp(T base, Int i) const {
      T acc = base;
      while (i)
        acc = group_op(acc, i);
      return acc;
    }
    inline T operator^(Int i) const {
      return exp(generator, i);
    }

  public:
    template<typename Generator, typename GroupOp>
    inline cyclic_group(Generator&& generator_, GroupOp&& group_op_) :
      generator{std::forward<Generator&&>(generator_)}, group_op{std::forward<GroupOp&&>(group_op_)} {}
  };

  template<typename T, typename Int = uint64_t>
  class optimised_cyclic_group : public cyclic_group<T, Int> {
  public:
    std::function<T(T, Int)> repeated_group_op;

  public:
    inline T exp(T base, Int i) const override {
      return repeated_group_op(base, i);
    }

  public:
    template<typename Generator, typename GroupOp, typename RepeatedGroupOp>
    inline optimised_cyclic_group(Generator&& generator_, GroupOp&& group_op_, RepeatedGroupOp&& repeated_group_op_) :
      cyclic_group<T, Int>{std::forward<Generator&&>(generator_), std::forward<GroupOp&&>(group_op_)},
      repeated_group_op{std::forward<RepeatedGroupOp&&>(repeated_group_op_)} {}
  };

  template<typename T, typename Int = uint64_t>
  inline optimised_cyclic_group<T, Int> prime_modulo(T generator, T modulo) {
    return {
      generator,
      [=](T a, T b) { return (a * b) % modulo; },
      [=](T a, T b) {
        Int ret = 1;
        a = a % modulo;

        while (b > 0) {
          if (b % 2 == 1)
            ret = (ret * a) % modulo;
          b /= 2;
          a = (a * a) % modulo;
        }

        return ret;
      }
    };
  }
}
