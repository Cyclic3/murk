#pragma once

#include "murk/flow.hpp"
#include "murk/span.hpp"

#include <chrono>

namespace murk::xflt {
  // For uncultured swine
  std::string brute(nonstd::span<const std::string> options, murk::flow_t<std::string_view, bool> oracle,
                    std::chrono::seconds log_interval = std::chrono::seconds{0});
}
