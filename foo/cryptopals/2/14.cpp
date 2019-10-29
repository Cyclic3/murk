#include <murk/crypto/block.hpp>

#include <botan/block_cipher.h>
#include <botan/chacha_rng.h>
#include <botan/auto_rng.h>

#include <murk/common.hpp>

static thread_local Botan::AutoSeeded_RNG slow_rng;
static thread_local Botan::ChaCha_RNG rng{slow_rng};

auto make_bc() {
  murk::data key = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  auto bc = Botan::BlockCipher::create_or_throw("AES-128");
  bc->set_key(key);
  return bc;
}

static auto bc = make_bc();

murk::data encrypt(murk::data_const_ref b) {
  auto a = murk::crypto::pkcs7_add(b, 16);
  return murk::crypto::ecb_encrypt(a, *bc);
}

murk::data decrypt(murk::data_const_ref b) {
  auto a = murk::crypto::ecb_decrypt(b, *bc);
  murk::crypto::pkcs7_remove_inplace(a);
  return a;
}

int main() {
  murk::data ptext = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK"_b64;

  // Easy mode
  {
    auto v = rng.random_vec(rng.next_byte());

    auto oracle = murk::threaded_flow_t<murk::data_const_ref, murk::data>::create([&](auto b) {
      murk::data a = ptext;
      a.insert(a.begin(), b.begin(), b.end());
      a.insert(a.begin(), v.begin(), v.end());
      return encrypt(a);
    });

    auto spam_res = murk::crypto::ecb_determine_spam(oracle, 16);

    auto normalised_oracle = murk::crypto::ecb_bypass_fixed_prefix_len(oracle, spam_res);

    auto res = murk::crypto::ecb_crack_prepend_oracle(normalised_oracle, 16);

    murk::alert("Success: {}", res == ptext);
  }

  // Hard mode
  {
    auto oracle = murk::threaded_flow_t<murk::data_const_ref, murk::data>::create([&](auto b) {
      murk::data a = ptext;
      a.insert(a.begin(), b.begin(), b.end());
      auto v = rng.random_vec(rng.next_byte());
      a.insert(a.begin(),v.begin(), v.end());
      return encrypt(a);
    });

    auto normalised_oracle = murk::threaded_flow_t<murk::data_const_ref, murk::data>::create(murk::crypto::ecb_bypass_random_prefix_len(oracle, 16));

    auto res = murk::crypto::ecb_crack_prepend_oracle(normalised_oracle, 16);

    murk::alert("Success: {}", res == ptext);
  }
}
