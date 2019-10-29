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
static std::vector<uint8_t> iv(16, 21);

murk::data processor(std::string_view b) {
  if (b.find_first_of(";&") != b.npos)
    throw std::invalid_argument("Oioi");

  std::string target = "comment1=cooking%20MCs;userdata=";
  target += b;
  target += ";comment2=%20like%20a%20pound%20of%20bacon";

  return murk::crypto::cbc_encrypt(murk::get_data_const_ref(target), iv, *bc);
}

bool validator(murk::data_const_ref b) {
  auto ptext_b = murk::crypto::cbc_decrypt(b, iv, *bc);
  auto ptext = murk::deserialise<std::string_view>(ptext_b);

  murk::log("{}", ptext);

  auto pos = ptext.find(";admin=true;");

  return pos != ptext.npos;
}

int main() {
  auto oracle =
      murk::in<murk::data_const_ref>()
   >> murk::deserialise<std::string_view>
   >> processor;

  auto spam = murk::crypto::cbc_determine_spam(oracle, 16);
  auto buf = spam.first;
  buf.resize(buf.size() + 32, 0);
  auto res = oracle(buf);

  murk::data msg = ";admin=true;"_b;
  murk::xor_bytes_inplace(murk::data_ref{res}.subspan(spam.second, static_cast<murk::data_const_ref::index_type>(msg.size())), msg);
  if (!validator(res))
    throw std::logic_error("Hack failed");
}
