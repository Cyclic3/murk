#pragma once

#include <fmt/format.h>
#include <fmt/color.h>

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

  template<typename In>
  flow_t<In> in() {
    return [](In i) -> In { return i; };
  }

  template<typename From, typename To>
  To cast(From f) {
    return static_cast<To>(f);
  }

  template<typename First, typename... Args>
  First log(std::string format, First&& first, Args&&... args) {
    fmt::print(format, first, args...);
    fmt::print("\n");
    fflush(stdout);
    return first;
  }

  template<typename First, typename... Args>
  First alert(std::string format, First&& first, Args&&... args) {
    fmt::print(fmt::bg(fmt::color::red), format, first, args...);
    fmt::print("\n");
    fflush(stdout);
    return first;
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

  template<typename T>
  T expect(const T& val, const T& t) {
    if (val != t)
      throw std::runtime_error("Unexpected");
    else
      return t;
  }

  template<typename T, typename... Args>
  T construct(T, Args&&... args) {
    return {std::forward<Args&&>(args)...};
  }

  template<typename F>
  void interactive(F f, std::string prompt_name = "") {
    std::string line;
    while (((void)(std::cout << prompt_name << "> "), std::getline(std::cin, line)) && !line.empty())
      std::cout << prompt_name << "< " << f(line) << std::endl;
  }

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
  [&](auto&&... args) { return OBJ.FUNCTION(args...); }
#define MURK_INVOKE_COPY(OBJ, FUNCTION) \
  [=](auto&&... args) { return OBJ.FUNCTION(args...); }
