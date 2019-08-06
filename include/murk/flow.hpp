#pragma once

#include <fmt/format.h>
#include <fmt/color.h>

#include <boost/callable_traits.hpp>

#include <functional>
#include <tuple>
#include <iostream>

namespace murk {
  inline void disable_stdout_buffer() {
    std::cout.setf(std::ios::unitbuf);
  }

  // Allows us to use normal functions
  template<typename In, typename Out = In>
  using flow_t = std::function<Out(In)>;

  template<typename In, typename Out, typename Param>
  using flow_right_t = std::function<Out(Param, In)>;

  template<typename In, typename Out, typename Param>
  using flow_left_t = std::function<Out(In, Param)>;

  template<typename In, typename Out>
  struct Flow {
    virtual Out operator()(In) = 0;
    virtual ~Flow() = default;
  };

  template<typename In, typename Out, typename InOut, typename OutIn = InOut>
  class plumber : public Flow<In, Out> {
  private:
    flow_t<In, InOut> in;
    flow_t<OutIn, Out> out;

  public:
    inline Out operator()(In i) override {
      return out(in(i));
    }

  public:
    inline plumber(flow_t<In, InOut> in_, flow_t<OutIn, Out> out_) :
      in{std::move(in_)}, out{std::move(out_)} {}
  };

  template<typename In>
  flow_t<In> in() {
    return [](In i) -> In { return i; };
  }

  template<typename From, typename To>
  To cast(From f) {
    return static_cast<To>(f);
  }

  template<typename T>
  T log(std::string format, T t) {
    std::cout << fmt::format(format, t) << std::endl;
    return t;
  }

  template<typename T>
  T alert(std::string format, T t) {
    fmt::print(fmt::bg(fmt::color::red), format, t);
    fmt::print("\n");
    fflush(stdout);
    return t;
  }

  template<typename T>
  T accept(flow_t<T, bool> checker, T t) {
    if (checker(t))
      return t;
    else
      throw std::runtime_error("Rejected");
  }

  template<typename T>
  T reject(flow_t<T, bool> checker, T t) {
    if (checker(t))
      throw std::runtime_error("Rejected");
    else
      return t;
  }

  template<typename T, typename... Args>
  T construct(T, Args&&... args) {
    return {std::forward<Args&&>(args)...};
  }

  struct link_t {};
  inline constexpr link_t link;

  template<typename ...Args>
  using args = std::tuple<Args...>;

  namespace flow_ops {
    template<typename InFunc, typename OutFunc>
    inline auto operator>>(InFunc in, OutFunc out) {
      return [in{std::move(in)}, out{std::move(out)}] (auto... args) {
        return out(in(args...));
      };
    }

    template<typename Func, typename P>
    inline auto operator<(Func f, P p) {
      return [f{std::move(f)}, p{std::move(p)}](auto... args) {
        return f(p, args...);
      };
    }

    template<typename Func, typename P>
    inline auto operator>(Func f, P p) {
      return [f{std::move(f)}, p{std::move(p)}](auto... args) {
        return f(args..., p);
      };
    }

    template<typename InFunc, typename Arg>
    inline auto operator<<(InFunc f, Arg arg) {
      return f(std::forward<Arg>(arg));
    }
  }
}

#define MURK_INVOKE(OBJ, FUNCTION) \
  [&](std::tuple_element_t<1, boost::callable_traits::args_t<decltype(&decltype(OBJ)::FUNCTION)>> x) -> \
  boost::callable_traits::return_type_t<decltype(&decltype(OBJ)::FUNCTION)> { return OBJ.FUNCTION(x); }
#define MURK_INVOKE_COPY(OBJ, FUNCTION) \
  [=](std::tuple_element_t<1, boost::callable_traits::args_t<decltype(&decltype(OBJ)::FUNCTION)>> x) -> \
  boost::callable_traits::return_type_t<decltype(&decltype(OBJ)::FUNCTION)> { return OBJ.FUNCTION(x); }
