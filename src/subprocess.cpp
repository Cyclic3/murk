#include "murk/subprocess.hpp"

#include <boost/process.hpp>

namespace murk {
  struct subprocess::impl_t {
    boost::process::opstream in;
    boost::process::ipstream out, err;
    boost::process::child p;

    template<typename... Args>
    impl_t(Args&&... args) :
      in{}, out{}, err{},
      p{std::forward<Args>(args)...,
        boost::process::std_in < in,
        boost::process::std_out > out,
        boost::process::std_err > err} {}
  };


  std::optional<int> subprocess::exit_code() {
    if (impl->p.running())
      return std::nullopt;
    else
      return impl->p.exit_code();
  }
  void subprocess::wait_for_exit() {
    impl->p.wait();
  }
  std::ostream& subprocess::in() {
    return impl->in;
  }
  std::istream& subprocess::out() {
    return impl->out;
  }
  std::istream& subprocess::err() {
    return impl->err;
  }

  std::string subprocess::read_all_stdout() {
    impl->out.sync();
    return {std::istreambuf_iterator<char>(impl->out), {}};
  }
  data subprocess::read_all_stdout_bytes() {
    impl->out.sync();
    return data(std::istreambuf_iterator<char>(impl->out), {});
  }

  subprocess::subprocess(std::string shell_cmd) : impl{std::make_unique<impl_t>(shell_cmd)} {}
  subprocess::subprocess(std::string program, std::string args) :
    impl{std::make_unique<impl_t>(program, boost::process::args(args))} {}
  subprocess::subprocess(std::string program, nonstd::span<std::string> args) :
    impl{std::make_unique<impl_t>(program, boost::process::args(args))} {}

}


