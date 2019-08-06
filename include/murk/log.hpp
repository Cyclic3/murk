#pragma once

namespace murk {
  /// A defaul constructed log_params is silent
  struct log_params {
    bool enabled = true;
    bool binary = false;

    constexpr operator bool() const noexcept { return enabled; }
  };
}
