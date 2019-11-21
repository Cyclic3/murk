#pragma once

#include "murk/data.hpp"
#include "murk/xfer.hpp"

#include <cppthings/movable_ptr.hpp>

#include <ios>
#include <memory>
#include <optional>

namespace murk {
  class subprocess {
  public:
    class io_bs;

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
    io_bs get_byte_stream();

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
    ~subprocess();

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

  class subprocess::io_bs : public byte_stream {
  private:
    cppthings::movable_ptr<std::ostream> in;
    cppthings::movable_ptr<std::istream> out;

  public:
    virtual size_t write(data_const_ref b) {
      in->write(reinterpret_cast<const char*>(b.data()), b.size());
      return b.size();
    }
    virtual size_t read(data_ref b) {
      return out->readsome(reinterpret_cast<char*>(b.data()), b.size());
    }
    virtual size_t read(data& b, size_t max = std::numeric_limits<size_t>::max()) {
      auto offset = b.size();
      auto additional = std::min(avail() + 128, max);
      b.resize(offset + additional); // Add a bit, just in case
      return read({b.data() + offset, additional});
    }
    using byte_stream::read;
    virtual size_t avail() {
      auto* buf = out.get()->rdbuf();
      return buf->in_avail();
    }

  public:
    inline io_bs(subprocess* sp) : in{&sp->in()}, out{&sp->out()} {}
  };
}

