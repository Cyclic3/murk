#define MURK_MOD_MAKE_NAME_INTERNAL(A, B, C) A##B##C
#define MURK_MOD_MAKE_NAME(A, B, C) MURK_MOD_MAKE_NAME_INTERNAL(A,B,C)

#define MURK_MOD_REGISTER(MOD, ...) \
  void __attribute__((constructor)) MURK_MOD_MAKE_NAME(_,__COUNTER__,_mod_register)() { ::murk::mod::manager::register_path(MOD, __VA_ARGS__); }

#include "murk/mod.hpp"
#include "murk/filt/zealous.hpp"

#include "murk/priv/docker.hpp"

#include "murk/web/uri.hpp"
#include "murk/web/ssti.hpp"

namespace murk {
  MURK_MOD_REGISTER(mod::adapt_simple_func_print([](auto... args) {
    system(murk::mod::module::root().act(args...).value().c_str());
    return std::nullopt;
  }), {"shelldo"});

  MURK_MOD_REGISTER(mod::adapt_simple_func_print([](std::string args) {
    return args;
  }), {"echo"});

  namespace priv {
    MURK_MOD_REGISTER(mod::adapt_default_func_print([](auto... args){return docker(args...);}), {"priv", "docker"});
  }

  namespace filt {
    MURK_MOD_REGISTER(mod::adapt_default_func_print([](auto... args) -> std::optional<std::string> {
      if constexpr (sizeof...(args) == 0) {
        std::string s;
        if (std::getline(std::cin, s))
          return zealous_escape_c(s);
        else
          return std::nullopt;
      }
      else {
        return zealous_escape_c(args...);
      }
    }), {"filt", "escape_c"});
  }

  namespace web {
    MURK_MOD_REGISTER(mod::adapt_default_func_print([](auto... args) -> std::optional<std::string> {
      if constexpr (sizeof...(args) == 0) {
        std::string s;
        if (std::getline(std::cin, s))
          return flask(s);
        else
          return std::nullopt;
      }
      else {
        return flask(args...);
      }
    }), {"web", "flask"});
    MURK_MOD_REGISTER(mod::adapt_default_func_print([](auto... args) -> std::optional<std::string> {
      if constexpr (sizeof...(args) == 0) {
        std::string s;
        if (std::getline(std::cin, s))
          return uri::encode(s);
        else
          return std::nullopt;
      }
      else {
        return uri::encode(args...);
      }
    }), {"web", "uriencode"});
    MURK_MOD_REGISTER(mod::adapt_simple_func_print([](std::string args) -> std::optional<std::string> {
      return uri::encode(murk::mod::module::root().act(args).value());
    }), {"web", "uriencodedo"});
    MURK_MOD_REGISTER(mod::adapt_simple_func_print([](std::string uri) -> std::optional<std::string> {
      murk::interactive([&](std::string line) { return flask_get(uri, line); });
      return std::nullopt;
    }), {"web", "flask_get"});
  }
}
