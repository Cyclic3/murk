#pragma once

#include "murk/crypto/group.hpp"

namespace murk::crypto::dh {
  template<typename T, typename Int = uint64_t>
  T get_key(const aa::cyclic_group<T, Int>& g, Int key) {
    return g ^ key;
  }

  template<typename T, typename Int = uint64_t>
  T gen_shared_secret(const aa::cyclic_group<T, Int>& g, Int key, T pub) {
    return g.exp(pub, key);
  }
}
