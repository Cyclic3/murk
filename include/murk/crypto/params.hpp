#pragma once

#include "murk/data.hpp"

namespace murk::crypto {
  struct enciphered_msg {
    data ciphertext;
    /// For unknown ivs, this may be left as a block of zeros
    ///
    /// This will often lead to corruption, but some useful data may be retreived
    data iv;
  };
}
