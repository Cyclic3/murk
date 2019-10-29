#include <murk/crypto/block.hpp>
#include <murk/web/form.hpp>

#include <botan/block_cipher.h>

#include <murk/common.hpp>

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

std::string profile_for(std::string email) {
  static size_t uid = 0;

  if (email.find('&') != email.npos || email.find('=') != email.npos)
    throw std::invalid_argument("Nice try");

  murk::web::form_t form {
    {"email", email},
//    {"uid", std::to_string(uid++)},
    {"role", "user"}
  };

  return murk::web::form_url_encode_dodgy(form);
}

murk::data get_cookie(std::string email) {
  auto profile = profile_for(email);
  return encrypt(murk::serialise(profile));
}

static auto is_admin = murk::threaded_flow_t<murk::data_const_ref, bool>::create(
    murk::in<murk::data_const_ref>()
     >> decrypt
     >> murk::deserialise<std::string>
     >> (murk::log<std::string> < "{}"s)
     >> murk::web::form_url_decode
     >> [](const murk::web::form_t& a) -> bool {
          if (auto iter = a.find("role"); iter != a.end())
            return iter->second == "admin";
          else return false;
        });

int main() {
  murk::flow_t<murk::data_const_ref, murk::data> insert_oracle =
      murk::in<murk::data_const_ref>()
   >> murk::deserialise<std::string>
   >> (murk::log<std::string> < "Inserting: {}"s)
   >> get_cookie;

  auto spam = murk::crypto::ecb_determine_spam(insert_oracle, 16);
  auto admin_tail = murk::serialise("admin");
  murk::crypto::pkcs7_add_inplace(admin_tail, 16);
  auto encryptor = murk::crypto::ecb_make_block_enc_oracle(insert_oracle, 16, spam);
  auto enc_tail = encryptor(admin_tail);

  std::string b = {};
  while(true) {
    auto cookie = get_cookie(b);
    std::copy(enc_tail.begin(), enc_tail.end(), cookie.end() - 16);
    try {
      if (is_admin(cookie))
        break;
    }
    catch(...) {}
    b.push_back('A');
  }

  murk::alert("Success!");

  return 0;

}
