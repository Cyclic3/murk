#include "murk/crypto/hash.hpp"
#include "murk/data.hpp"

#include <cppthings/number.hpp>

#include <openssl/sha.h>
#include <openssl/md5.h>

#include <cstring>

#include "murk/common.hpp"

namespace murk::crypto {
  template<>
  extend_result extend<LengthExtendable::MD5>(murk::data_const_ref hash, size_t ptext_len, murk::data_const_ref new_data) {
    if (hash.size() != (128/8))
      throw std::invalid_argument("Bad hash length");

    extend_result ret;
    ret.hash.resize(128 / 8);
    ret.appended.push_back(0b10000000);
    while ((ret.appended.size() + ptext_len) % (512/8) != (448 / 8))
      ret.appended.push_back(0);
    ret.appended += to_little_endian<uint64_t>(static_cast<uint64_t>(ptext_len) * 8);

    ::MD5_CTX ctx;
    ::memset(&ctx, 0, sizeof(ctx));
    auto count = ptext_len + ret.appended.size();
    ctx.Nl = count * 8;
    ctx.Nh = count >> (std::numeric_limits<MD5_LONG>::digits - 3);

    std::copy(hash.begin()     , hash.begin() +  4, reinterpret_cast<uint8_t*>(&ctx.A));
    std::copy(hash.begin() +  4, hash.begin() +  8, reinterpret_cast<uint8_t*>(&ctx.B));
    std::copy(hash.begin() +  8, hash.begin() + 12, reinterpret_cast<uint8_t*>(&ctx.C));
    std::copy(hash.begin() + 12, hash.end()       , reinterpret_cast<uint8_t*>(&ctx.D));

    ctx.A = l2he(ctx.A);
    ctx.B = l2he(ctx.B);
    ctx.C = l2he(ctx.C);
    ctx.D = l2he(ctx.D);

    ret.appended.insert(ret.appended.end(), new_data.begin(), new_data.end());

    ::MD5_Update(&ctx, new_data.data(), new_data.size());
    ::MD5_Final(ret.hash.data(), &ctx);

    return ret;
  }

  template<>
  extend_result extend<LengthExtendable::SHA1>(murk::data_const_ref hash, size_t ptext_len, murk::data_const_ref new_data) {
    if (hash.size() != (160/8))
      throw std::invalid_argument("Bad hash length");

    extend_result ret;
    ret.hash.resize(160 / 8);
    ret.appended.push_back(0b10000000);
    while ((ret.appended.size() + ptext_len) % (512/8) != ((512 - 64) / 8))
      ret.appended.push_back(0);
    ret.appended += to_big_endian<uint64_t>(static_cast<uint64_t>(ptext_len) * 8);

    ::SHA_CTX ctx;
    ::memset(&ctx, 0, sizeof(ctx));
    auto count = ptext_len + ret.appended.size();
    ctx.Nl = count * 8;
    ctx.Nh = count >> (std::numeric_limits<SHA_LONG>::digits - 3);

    std::copy(hash.begin()     , hash.begin() +  4, reinterpret_cast<uint8_t*>(&ctx.h0));
    std::copy(hash.begin() +  4, hash.begin() +  8, reinterpret_cast<uint8_t*>(&ctx.h1));
    std::copy(hash.begin() +  8, hash.begin() + 12, reinterpret_cast<uint8_t*>(&ctx.h2));
    std::copy(hash.begin() + 12, hash.begin() + 16, reinterpret_cast<uint8_t*>(&ctx.h3));
    std::copy(hash.begin() + 16, hash.end()       , reinterpret_cast<uint8_t*>(&ctx.h4));

    ctx.h0 = b2he(ctx.h0);
    ctx.h1 = b2he(ctx.h1);
    ctx.h2 = b2he(ctx.h2);
    ctx.h3 = b2he(ctx.h3);
    ctx.h4 = b2he(ctx.h4);

    ::SHA1_Update(&ctx, new_data.data(), new_data.size());

    ret.appended.insert(ret.appended.end(), new_data.begin(), new_data.end());
    ret.hash.resize(160 / 8);
    ::SHA1_Final(ret.hash.data(), &ctx);

    return ret;
  }

  template<>
  extend_result extend<LengthExtendable::SHA2_256>(murk::data_const_ref hash, size_t ptext_len, murk::data_const_ref new_data) {
    if (hash.size() != (256/8))
      throw std::invalid_argument("Bad hash length");

    extend_result ret;
    ret.hash.resize(256 / 8);
    ret.appended.push_back(0b10000000);
    while ((ret.appended.size() + ptext_len) % (512/8) != ((512 - 64) / 8))
      ret.appended.push_back(0);
    ret.appended += to_big_endian<uint64_t>(static_cast<uint64_t>(ptext_len) * 8);

    ::SHA256_CTX ctx;
    ::memset(&ctx, 0, sizeof(ctx));
    ctx.md_len = SHA256_DIGEST_LENGTH;
    auto count = ptext_len + ret.appended.size();
    ctx.Nl = count * 8;
    ctx.Nh = count >> (std::numeric_limits<SHA_LONG>::digits - 3);

    auto* begin = reinterpret_cast<const SHA_LONG*>(hash.begin());
    auto* end = reinterpret_cast<const SHA_LONG*>(hash.end());
    std::copy(begin, end, &ctx.h[0]);

    for (size_t i = 0; i < 8; ++i)
      ctx.h[i] = b2he(ctx.h[i]);

    ::SHA256_Update(&ctx, new_data.data(), new_data.size());

    ret.appended.insert(ret.appended.end(), new_data.begin(), new_data.end());
    ret.hash.resize(256 / 8);
    ::SHA256_Final(ret.hash.data(), &ctx);

    return ret;
  }

  template<>
  extend_result extend<LengthExtendable::SHA2_512>(murk::data_const_ref hash, size_t ptext_len, murk::data_const_ref new_data) {
    if (hash.size() != (512/8))
      throw std::invalid_argument("Bad hash length");

    extend_result ret;
    ret.hash.resize(512 / 8);
    ret.appended.push_back(0b10000000);
    while ((ret.appended.size() + ptext_len) % (1024/8) != ((1024 - 64) / 8))
      ret.appended.push_back(0);
    // TODO: Yeah I don't care about messages > 2^56 bytes
    ret.appended += to_big_endian<uint64_t>(static_cast<uint64_t>(ptext_len) * 8);

    ::SHA512_CTX ctx;
    ::memset(&ctx, 0, sizeof(ctx));
    ctx.md_len = SHA512_DIGEST_LENGTH;
    auto count = ptext_len + ret.appended.size();
    ctx.Nl = count * 8;
    ctx.Nh = count >> (std::numeric_limits<SHA_LONG64>::digits - 3);

    auto* begin = reinterpret_cast<const SHA_LONG64*>(hash.begin());
    auto* end = reinterpret_cast<const SHA_LONG64*>(hash.end());
    std::copy(begin, end, &ctx.h[0]);

    for (size_t i = 0; i < 8; ++i)
      ctx.h[i] = b2he<uint64_t>(ctx.h[i]);

    ::SHA512_Update(&ctx, new_data.data(), new_data.size());

    ret.appended.insert(ret.appended.end(), new_data.begin(), new_data.end());
    ret.hash.resize(512 / 8);
    ::SHA512_Final(ret.hash.data(), &ctx);

    return ret;
  }
}
