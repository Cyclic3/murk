#include "murk/xflt/dns.hpp"

#include <iostream>

int main() {
  boost::asio::io_context io_ctx;
  murk::xflt::dns d{&io_ctx, 8053};
  d.recv.connect([](auto i) {
    for (auto& part : i)
      std::cout << part << std::endl;
  });
  io_ctx.run();
}
