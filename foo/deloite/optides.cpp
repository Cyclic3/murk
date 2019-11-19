#include <murk/flows/fs.hpp>
#include <murk/plod/png_chunks.hpp>
#include <murk/plod/php.hpp>
#include <murk/crypto/construction_flaws.hpp>

#include <murk/web/http.hpp>

#include <botan/des.h>

#include <cppthings/iota_iter.hpp>

#include <murk/common.hpp>

using arr_t = std::array<uint8_t, 8>;

int main() {
//  Botan::DES d;

//  arr_t ptext;
//  arr_t ctext;
//  auto ptext_b = "abcdefg\1"_b;
//  auto ctext_b = "eeeb64d14bf313c7"_hex;
//  std::copy(ptext_b.begin(), ptext_b.end(), ptext.begin());
//  std::copy(ctext_b.begin(), ctext_b.end(), ctext.begin());

//  arr_t key1;
//  arr_t key2;
//  auto key1_b = "d88ae650e2"_hex;
//  auto key2_b = "b812390568"_hex;
//  std::copy(key1_b.begin(), key1_b.end(), key1.begin());
//  std::copy(key2_b.begin(), key2_b.end(), key2.begin());

//  // First, generate the encrypted table
//  std::map<arr_t, arr_t> enc_tab;

//  for (size_t i = 0; i < (1 << (3 * 8)); ++i) {
//    key1[5] = i;
//    key1[6] = i >> 8;
//    key1[7] = i >> 16;

//    d.set_key(key1.data(), key1.size());
//    auto candidate = ptext;
//    d.encrypt(candidate.data());

//    enc_tab[candidate] = key1;
//  }

//  murk::log("made enc tab {}", enc_tab.size());

//  // Now we brute force
//  for (size_t i = 0; i < 1 << (3 * 8); ++i) {
//    key2[5] = i;
//    key2[6] = i >> 8;
//    key2[7] = i >> 16;

//    d.set_key(key2.data(), key2.size());
//    auto candidate = ctext;
//    d.decrypt(candidate.data());

//    if (auto iter = enc_tab.find(candidate); iter != enc_tab.end()) {
//      murk::log("{}, {}", murk::hex_encode(iter->second), murk::hex_encode(key2));
//      return 0;
//    }
//  }

  Botan::DES d;

  arr_t ptext;
  arr_t ctext;
  auto ptext_b = "abcdefg\1"_b;
  auto ctext_b = "eeeb64d14bf313c7"_hex;
  std::copy(ptext_b.begin(), ptext_b.end(), ptext.begin());
  std::copy(ctext_b.begin(), ctext_b.end(), ctext.begin());

  auto res = murk::crypto::meet_in_the_middle_mem(
        cppthings::iota_iter{0xb812390568000000uLL}, cppthings::iota_iter{0xb812390568FFFFFFuLL + 1},
        cppthings::iota_iter{0xd88ae650e2000000uLL}, cppthings::iota_iter{0xd88ae650e2FFFFFFuLL + 1},
        murk::threaded_flow_t<const uint64_t&, arr_t>::create([&](const uint64_t& k) -> arr_t {
          thread_local Botan::DES d;
          std::array<uint8_t, 8> arr;
          const auto* ptr = reinterpret_cast<const uint8_t*>(&k);
          std::reverse_copy(ptr, ptr + 8, arr.begin());
          d.set_key(arr.data(), arr.size());
          arr_t res;
          d.decrypt(ctext.data(), res.data());
          return res;

        }),
        murk::threaded_flow_t<const uint64_t&, arr_t>::create([&](const uint64_t& k) -> arr_t {
          thread_local Botan::DES d;
          std::array<uint8_t, 8> arr;
          const auto* ptr = reinterpret_cast<const uint8_t*>(&k);
          std::reverse_copy(ptr, ptr + 8, arr.begin());
          d.set_key(arr.data(), arr.size());
          arr_t res;
          d.encrypt(ptext.data(), res.data());
          return res;
        }));

  return 0;
//  Botan::DES d;
//  auto key1 = "d88ae650e295bb0f"_hex, key2 = "b812390568b07aa8"_hex;

//  auto res = "ba0ce7285e64723cff3075b45ccef63c6d667432833617aa"_hex;

//  d.set_key(key2);
//  d.decrypt_n(res.data(), res.data(), res.size() / 8);
//  d.set_key(key1);
//  d.decrypt_n(res.data(), res.data(), res.size() / 8);
//  murk::alert("{}", murk::deserialise<std::string>(res));
}
