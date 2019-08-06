#pragma once

#include <fmt/format.h>

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

  template<typename In, typename Out, typename Param>
  class flow_left : public Flow<In, Out> {
  private:
    flow_left_t<In, Out, Param> f;
    std::decay_t<Param> p;

  public:
    inline Out operator()(In i) override {
      return f(i, p);
    }

  public:
    template<typename ParamConstructor>
    inline flow_left(decltype(f) func, ParamConstructor param) :
      f{std::move(func)}, p{std::move(param)} {}
  };

  template<typename Func, typename ParamConstructor,

           typename Args = boost::callable_traits::args_t<Func>,
           typename In = std::tuple_element_t<0, Args>,
           typename Out = boost::callable_traits::return_type_t<Func>,
           typename Param = std::tuple_element_t<std::tuple_size_v<Args> - 1, Args>>
  flow_left(Func f, ParamConstructor p) -> flow_left<In, Out, Param>;

  template<typename In, typename Out, typename Param>
  class flow_right : public Flow<In, Out> {
  private:
    flow_right_t<In, Out, Param> f;
    std::decay_t<Param> p;

  public:
    inline Out operator()(In i) override {
      return f(p, i);
    }

  public:
    template<typename ParamConstructor>
    inline flow_right(decltype(f) func, ParamConstructor param) :
      f{std::move(func)}, p{std::move(param)} {}
  };

  template<typename Func, typename ParamConstructor,

           typename Args = boost::callable_traits::args_t<Func>,
           typename In = std::tuple_element_t<std::tuple_size_v<Args> - 1, Args>,
           typename Out = boost::callable_traits::return_type_t<Func>,
           typename Param = std::tuple_element_t<0, Args>>
  flow_right(Func f, ParamConstructor p) -> flow_right<In, Out, Param>;

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

  namespace flow_ops {
    template<typename InFunc, typename OutFunc,

             typename In = std::tuple_element_t<0, boost::callable_traits::args_t<InFunc>>,
             typename InOut = boost::callable_traits::return_type_t<InFunc>,
             typename OutIn = std::tuple_element_t<0, boost::callable_traits::args_t<OutFunc>>,
             typename Out = boost::callable_traits::return_type_t<OutFunc>>
    inline auto operator>>(InFunc in_, OutFunc out_) {
      return plumber(flow_t<In, InOut>(std::move(in_)),
                     flow_t<OutIn, Out>(std::move(out_)));
    }

    template<typename Func, typename Param>
    inline auto operator|(Func f, Param p) {
      return flow_right(f, p);
    }

    template<typename Func, typename Param>
    inline auto operator&(Func f, Param p) {
      return flow_left(f, p);
    }

    template<typename InFunc,

             typename In = std::tuple_element_t<0, boost::callable_traits::args_t<InFunc>>,
             typename Out = boost::callable_traits::return_type_t<InFunc>>
    inline Out operator<<(InFunc f, In arg) {
      return f(arg);
    }
  }
}

#define MURK_INVOKE(OBJ, FUNCTION) \
  [&](std::tuple_element_t<1, boost::callable_traits::args_t<decltype(&decltype(OBJ)::FUNCTION)>> x) -> \
  boost::callable_traits::return_type_t<decltype(&decltype(OBJ)::FUNCTION)> { return OBJ.FUNCTION(x); }
#define MURK_INVOKE_COPY(OBJ, FUNCTION) \
  [=](std::tuple_element_t<1, boost::callable_traits::args_t<decltype(&decltype(OBJ)::FUNCTION)>> x) -> \
  boost::callable_traits::return_type_t<decltype(&decltype(OBJ)::FUNCTION)> { return OBJ.FUNCTION(x); }
