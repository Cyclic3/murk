#include <murk/flows/fs.hpp>
#include <murk/plod/png_chunks.hpp>
#include <murk/plod/php.hpp>
#include <murk/crypto/mitm.hpp>
#include <murk/crypto/padding.hpp>
#include <murk/web/http.hpp>

#include <botan/des.h>

#include <cppthings/iota_iter.hpp>

#include <murk/common.hpp>

using arr_t = std::array<uint8_t, 8>;

int main() {
  arr_t ptext;
  arr_t ctext;
  auto ptext_b = "abcdefg\1"_b;
  auto ctext_b = "eeeb64d14bf313c7"_hex;
  std::copy(ptext_b.begin(), ptext_b.end(), ptext.begin());
  std::copy(ctext_b.begin(), ctext_b.end(), ctext.begin());

  using iter_t = cppthings::iota_iter_arr_be<uint8_t, 8>;

  iter_t outer_begin{std::in_place, 0xb8, 0x12, 0x39, 0x05, 0x68},
         outer_end  {std::in_place, 0xb8, 0x12, 0x39, 0x05, 0x69},
         inner_begin{std::in_place, 0xd8, 0x8a, 0xe6, 0x50, 0xe2},
         inner_end  {std::in_place, 0xd8, 0x8a, 0xe6, 0x50, 0xe3};

  auto res = murk::crypto::meet_in_the_middle_mem(
        outer_begin, outer_end,
        inner_begin, inner_end,
        murk::threaded_flow_t<const arr_t&, arr_t>::create([&](const arr_t& k) -> arr_t {
          thread_local Botan::DES d;
          d.set_key(k.data(), k.size());
          arr_t res;
          d.decrypt(ctext.data(), res.data());
          return res;
        }),
        murk::threaded_flow_t<const arr_t&, arr_t>::create([&](const arr_t& k) -> arr_t {
          thread_local Botan::DES d;
          d.set_key(k.data(), k.size());
          arr_t res;
          d.encrypt(ptext.data(), res.data());
          return res;
        }));

  auto target = "ba0ce7285e64723cff3075b45ccef63c6d667432833617aa"_hex;

  Botan::DES d;
  murk::crypto::decrypt_n(target, d, res.first, res.second);
  murk::crypto::pkcs7_remove_inplace(target);
  murk::alert("{}", murk::deserialise<std::string>(target));
}
