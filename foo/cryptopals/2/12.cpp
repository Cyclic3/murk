#include <murk/crypto/block.hpp>

#include <botan/block_cipher.h>

#include <murk/common.hpp>

auto make_bc() {
  murk::data key = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  auto bc = Botan::BlockCipher::create_or_throw("AES-128");
  bc->set_key(key);
  return bc;
}

murk::data encrypt(murk::data_const_ref b) {
  static auto bc = make_bc();
  auto a = murk::crypto::pkcs7_add(b, 16);
  return murk::crypto::ecb_encrypt(a, *bc);
}

int main() {
  murk::data ptext = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK"_b64;
  auto oracle = murk::threaded_flow_t<murk::data_const_ref, murk::data>::create([&](auto b) {
    murk::data a = ptext;
    a.insert(a.begin(), b.begin(), b.end());
    return encrypt(a);
  });

  auto res = murk::crypto::ecb_crack_prepend_oracle(oracle, 16);

  murk::alert("Success: {}", res == ptext);
}
