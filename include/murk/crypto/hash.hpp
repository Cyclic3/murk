#pragma once

#include "murk/data.hpp"

namespace murk::crypto {
  enum class LengthExtendable {
    MD5,
    SHA1,
    SHA2_256,
    SHA2_512,
  };

  struct extend_result {
    murk::data hash;
    murk::data appended;
  };

  template<LengthExtendable HashAlg>
  void calc_padding_length(size_t ptext_len);

  template<LengthExtendable HashAlg>
  extend_result extend(murk::data_const_ref hash, size_t ptext_len, murk::data_const_ref new_data);

//  inline extend_result extend(LengthExtendable hash_alg, murk::data_const_ref hash, size_t ptext_len, murk::data_const_ref new_data) {
//    switch (hash_alg) {
//      case LengthExtendable::MD5: return extend<LengthExtendable::MD5>(hash, ptext_len, new_data);
//      case LengthExtendable::SHA1: return extend<LengthExtendable::SHA1>(hash, ptext_len, new_data);
//      case LengthExtendable::SHA2_256: return extend<LengthExtendable::SHA2_256>(hash, ptext_len, new_data);
//      case LengthExtendable::SHA2_512: return extend<LengthExtendable::SHA2_512>(hash, ptext_len, new_data);
//      default:
//        throw std::logic_error("Invalid murk::crypto::LengthExtendable");
//    }
//  }
}
