#pragma once

#include "murk/xfer.hpp"

#include <memory>

namespace murk::web {
  class telnet : public byte_dgram {
  private:
    std::unique_ptr<byte_stream>&& base;
    data extra;

  public:
    constexpr static std::array<uint8_t, 2> eol = {0x0a, 0x0d};

  private:
    decltype(extra)::iterator get_next_msg() {
      decltype(extra)::iterator iter;
      while ((iter = std::search(extra.begin(), extra.end(), eol.begin(), eol.end())) != extra.end())
        base->read(extra);
      return iter;
    }

  public:
    void send_control_code(uint8_t code) {
      std::array<uint8_t, 4> msg = {0xff, code, 0x0a, 0x0d};
      base->write(msg);
    }

  public:
    inline size_t write(data_const_ref b) override {
      // Will be end, which suits us perfectly
      auto begin = b.begin();
      auto iter = std::find(b.begin(), b.end(), 0xff);
      while (true) {
        base->write(data_const_ref{begin, iter});
        if (iter == b.end())
          break;
        std::array<uint8_t, 1> buf = {0xff};
        base->write(buf);
        begin = iter;
        iter = std::find(begin, b.end(), 0xff);
      }
      base->write(eol);
    }
    inline virtual size_t read(data_ref b) override {
      auto iter = get_next_msg();
      size_t to_copy = std::min(static_cast<size_t>(iter - extra.begin()), b.size());
      std::copy(extra.begin(), extra.begin() + to_copy, b.begin());
      extra.erase(extra.begin(), iter + eol.size());
      return to_copy;
    }
    /// XXX: completely ignores max
    virtual data read(size_t /*max*/ = std::numeric_limits<size_t>::max()) override {
      auto iter = get_next_msg();
      data ret{iter + eol.size(), extra.end()};
      std::swap(ret, extra);
      extra.erase(iter, extra.end());
      return extra;
    }
    virtual bool avail() override { return base->avail() >= 2; }
    /// Returns 0 if not well defined
    virtual size_t max_buf() { return 0; }
  };

  std::unique_ptr<byte_dgram> telnet(std::unique_ptr<byte_stream>&&);
}
