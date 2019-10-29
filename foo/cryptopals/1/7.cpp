#include <murk/flows/fs.hpp>
#include <murk/ext/manager.hpp>
#include <murk/crypto/block.hpp>

#include <botan/block_cipher.h>

#include <murk/common.hpp>

int main() {
  auto ctext = murk::base64_decode(murk::fs::read_all_text("/tmp/set1_challenge7")); //murk::base64_decode(murk::fs::read_all_text(murk::ext::mgr.obtain("https://cryptopals.com/static/challenge-data/7.txt")));
  auto key = murk::serialise(u8"YELLOW SUBMARINE");

  auto cypher = Botan::BlockCipher::create_or_throw("AES-128");
  cypher->set_key(key);

  auto ptext = murk::crypto::ecb_decrypt(ctext, *cypher);

  murk::crypto::pkcs7_remove_inplace(ptext);
  murk::log("{}", murk::deserialise<std::string>(ptext));
}
