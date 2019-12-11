#define MURK_MOD_MAKE_NAME_INTERNAL(A, B, C) A##B##C
#define MURK_MOD_MAKE_NAME(A, B, C) MURK_MOD_MAKE_NAME_INTERNAL(A,B,C)

#define MURK_MOD_REGISTER(MOD, ...) \
  void __attribute__((constructor)) MURK_MOD_MAKE_NAME(_,__COUNTER__,_mod_register)() { ::murk::mod::manager::register_path(MOD, __VA_ARGS__); }

#include "murk/mod.hpp"

#include "murk/priv/docker.hpp"

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
}
