#include <murk/crypto/block.hpp>

#include <botan/block_cipher.h>
#include <botan/auto_rng.h>

#include <murk/common.hpp>

static thread_local Botan::AutoSeeded_RNG rng{};

std::unique_ptr<Botan::BlockCipher> gen_cypher() {
  auto cypher = Botan::BlockCipher::create_or_throw("AES-128");
  cypher->set_key(rng.random_vec(16));
  return cypher;
}

murk::flow_t<murk::data_const_ref, murk::data> gen_ecb() {
  std::shared_ptr<Botan::BlockCipher> cypher = gen_cypher();

  return [=](murk::data_const_ref b){ return murk::crypto::ecb_encrypt(b, *cypher); };
}

murk::flow_t<murk::data_const_ref, murk::data> gen_cbc() {
  std::shared_ptr<Botan::BlockCipher> cypher = gen_cypher();
  auto iv = rng.random_vec(16);

  return [=](murk::data_const_ref b){ return murk::crypto::cbc_encrypt(b, iv, *cypher); };
}

int main() {
  bool failed = false;

  for (uint64_t i = 0; i < 1 << 14; ++i) {
    bool is_ecb = rng.next_byte() & 1;

    murk::flow_t<murk::data_const_ref, murk::data> gen = is_ecb ? gen_ecb() : gen_cbc();

    if (murk::crypto::ecb_oracle(gen, 16) != is_ecb) {
      murk::alert("ORACLE FAILED on iter {}. Expected {}", i, is_ecb);
      failed = true;
    }
  }

  murk::log("Did it fail?: {}", failed);
}
