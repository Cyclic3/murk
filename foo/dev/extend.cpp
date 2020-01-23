#include <murk/crypto/hash.hpp>
#include <murk/common.hpp>

#include <openssl/sha.h>
#include <openssl/md5.h>

#include <cstring>

int main() {
  {
    murk::data msg = murk::serialise("test\n");
    murk::data hash(128 / 8);

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, msg.data(), msg.size());
    MD5_Final(hash.data(), &ctx);

    auto new_data = murk::serialise("evil");

    auto ret = murk::crypto::extend<murk::crypto::LengthExtendable::MD5>(hash, msg.size(), new_data);

    auto new_msg = msg + ret.appended;

    murk::data new_hash(128 / 8);
    MD5_Init(&ctx);
    MD5_Update(&ctx, new_msg.data(), new_msg.size());
    MD5_Final(new_hash.data(), &ctx);

    if (new_hash != ret.hash)
      abort();
  }

  {
    murk::data msg = murk::serialise("test\n");
    murk::data hash(256 / 8);

    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, msg.data(), msg.size());
    SHA256_Final(hash.data(), &ctx);

    auto new_data = murk::serialise("evil");

    auto ret = murk::crypto::extend<murk::crypto::LengthExtendable::SHA2_256>(hash, msg.size(), new_data);

    auto new_msg = msg + ret.appended;

    murk::data new_hash(256 / 8);
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, new_msg.data(), new_msg.size());
    SHA256_Final(new_hash.data(), &ctx);

    if (new_hash != ret.hash)
      abort();
  }

  {
    murk::data msg = murk::serialise("test\n");
    murk::data hash(512 / 8);

    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, msg.data(), msg.size());
    SHA512_Final(hash.data(), &ctx);

    auto new_data = murk::serialise("evil");

    auto ret = murk::crypto::extend<murk::crypto::LengthExtendable::SHA2_512>(hash, msg.size(), new_data);

    auto new_msg = msg + ret.appended;

    murk::data new_hash(512 / 8);
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, new_msg.data(), new_msg.size());
    SHA512_Final(new_hash.data(), &ctx);

    if (new_hash != ret.hash)
      abort();
  }
  
  {
    murk::data hash = "f59ac0828b9a32293b348e398a0efd342b1e4377a687f3a9055ee2871dff35e4"_hex;
    
    auto ret = murk::crypto::extend<murk::crypto::LengthExtendable::SHA2_256>(hash, 4, murk::serialise("hacked"));

    std::cout << murk::hex_encode(ret.appended) << std::endl;
    std::cout << murk::hex_encode(ret.hash) << std::endl;
  }

  return 0;
}
