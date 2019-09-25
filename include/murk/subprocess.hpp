#pragma once

#include "murk/data.hpp"

#include <ios>
#include <memory>
#include <optional>

namespace murk {
  class subprocess {
  public:
    struct impl_t;

  private:
    std::unique_ptr<impl_t> impl;

  public:
    std::optional<int> exit_code();
    void wait_for_exit();
    std::ostream& in();
    std::istream& out();
    std::istream& err();

  public:
    std::string read_all_stdout();
    data read_all_stdout_bytes();

  public:
    subprocess(std::string shell_cmd);
    /// XXX: I have no idea if this one works
    subprocess(std::string program, std::string args);
    subprocess(std::string program, nonstd::span<std::string> args);
    template<typename... Args>
    inline static subprocess start(std::string_view program, Args&&... args) {
      std::array<std::string, sizeof...(args)> cmdline_args = {args...};
      return {program, cmdline_args};
    }

  public:
    template<typename... Args>
    inline static std::string start_and_read_all_stdout(std::string_view program, Args&&... args) {
      auto s = start(program, std::forward<Args&&>(args)...);
      s.wait_for_exit();
      return s.read_all_stdout();
    }
    template<typename... Args>
    inline static std::string start_and_read_all_stdout_bytes(std::string_view program, Args&&... args) {
      auto s = start(program, std::forward<Args&&>(args)...);
      s.wait_for_exit();
      return s.read_all_stdout_bytes();
    }
  };
}

