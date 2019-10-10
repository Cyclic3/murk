#include <murk/common.hpp>

#include <murk/crypto/block.hpp>

#include <botan/block_cipher.h>

int main() {
  auto cypher = Botan::BlockCipher::create_or_throw("AES-128");
  auto in = murk::serialise("1234567890ABCDEFGHIJKLMNOPQRSTUVAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
  cypher->set_key(murk::data(16, 0));
  auto iv = murk::data(16, 0);
  auto a = murk::crypto::cbc_encrypt(in, iv, *cypher);
  auto b = murk::crypto::cbc_decrypt(a, iv, *cypher);

  murk::log("{}", b == in);
}
