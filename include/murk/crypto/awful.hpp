#pragma once

#include "murk/data.hpp"
#include "murk/flow.hpp"
#include "murk/flows/seq.hpp"
#include "murk/crypto/analysis.hpp"

#include <map>
#include <set>

namespace murk::crypto {
  /// The simple xor substitution cipher
  namespace xor_single {
    /// Encrypts or decrypts an encrypted plaintext with given key
    ///
    /// @param b The target plaintext or cyphertext
    /// @param key The encryption key
    inline murk::data crypt(data_const_ref b, uint8_t key) {
      murk::data ret;
      for (auto i : b)
        ret.push_back(i ^ key);
      return ret;
    }

    /// Attempts to crack an encrypted plaintext with a known distribution
    ///
    /// @param ctext The target ciphertext
    /// @param expected The distribution of the plaintext
    uint8_t crack(murk::data_const_ref ctext, const dist_t& expected);

    /// Rates the likelihood that the given ciphertext was encrypted with an xor_single cipher
    ///
    /// @param expected The distribution of the plaintext
    /// @param ctext The target ciphertext
    inline double score(murk::data_const_ref ctext, const dist_t& expected) {
      using namespace murk::flow_ops;

      static const auto measure =
          in<murk::data_const_ref>()
       >> murk::cast_span<uint8_t, murk::crypto::token_t>
       >> count<murk::crypto::token_t>
       >> normalise_freq;
      // The closer to zero, the higher the score
      return -score_dist_compare(expected, measure(ctext));
    }
  }

  /// The xor vigenere cipher
  ///
  /// Whilst secure as a one-time-pad, the repetition makes this any fractionally
  /// harder to break than an xor_single cipher
  namespace xor_vigenere {
    /// Encrypts or decrypts the data with the given key and data
    ///
    /// @param b The target data
    /// @param key The key
    inline murk::data crypt(data_const_ref b, murk::data_const_ref key) {
      data ret(b.size());
      for (size_t i = 0; i < b.size(); ++i) {
        ret[i] = b[i] ^ key[i % key.size()];
      }
      return ret;
    }

    /// Attempts to determine the length of the key used to encrypt a given ciphertext
    ///
    /// @param ctext The target ciphertext
    /// @param min The minimum size of the key to check
    /// @param max The maxmimum size of the key to check
    size_t calc_key_length(data_const_ref ctext, size_t min = 2, size_t max = 40);
    /// Attempts to crack the given ciphertext with the key length and plaintext distribution provided
    ///
    /// @param ctext The target ciphertext
    /// @param expected The distribution of the plaintext
    /// @param key_len The length of the key
    ///
    /// @returns The key for the ciphertext
    murk::data crack_with_known_len(data_const_ref ctext, const dist_t& expected, size_t key_len);

    /// A convenience function that performs calc_key_length, followed by crack_with_known_len
    ///
    /// @param ctext The target ciphertext
    /// @param expected The distribution of the plaintext
    ///
    /// @returns The key for the ciphertext
    inline murk::data crack(data_const_ref ctext, const dist_t& expected) {
      return crack_with_known_len(ctext, expected, calc_key_length(ctext));
    }
  }

  namespace subsitution {
    using key_t = std::map<uint8_t, uint8_t>;

    inline double score(murk::data_const_ref ctext, const dist_t& expected) {
      using namespace murk::flow_ops;

      static const auto measure =
          in<murk::data_const_ref>()
       >> murk::cast_span<uint8_t, murk::crypto::token_t>
       >> count<murk::crypto::token_t>
       >> normalise_freq;
      // The closer to zero, the higher the score
      return -score_dist_compare(expected, measure(ctext));
    }

    inline key_t invert_key(const key_t& target) {
      key_t ret;
      for (auto i : target)
        ret[i.second] = i.first;
      return ret;
    }

    inline data crypt(murk::data_const_ref target, const key_t& key) {
      return murk::map<uint8_t, uint8_t>(target, [&](uint8_t in) -> uint8_t {
        if (auto iter = key.find(in); iter != key.end())
          return iter->second;
        else return in;
      });
    }

    key_t crack(murk::data_const_ref ctext, const dist_t& expected, flow_t<uint8_t, bool> filter = [](auto) { return true; });

    inline key_t crack_with_known(murk::data_const_ref ctext, const dist_t& expected,
                                  key_t&& known, flow_t<uint8_t, bool> filter = [](auto) { return true; }) {
      dist_t new_dist = expected;
      for (auto i : known)
        new_dist.erase(i.second); // Skip known values
      key_t res = crack(ctext, new_dist, [&](auto i){ return !known.contains(i) && filter(i); });
      res.merge(std::move(known));
      return res;
    }
  }

  namespace vigenere {
    using key_t = std::vector<subsitution::key_t>;

    inline key_t invert_key(const key_t& target) {
      key_t ret(target.size());
      for (size_t i = 0; i < target.size(); ++i)
        ret[i] = subsitution::invert_key(target[i]);
      return ret;
    }

    inline data crypt(murk::data_const_ref target, const key_t& key) {
      data ret(target.size());

      for (size_t i = 0; i < target.size(); ++i) {
        auto& key_i = key[i % key.size()];
        if (auto iter = key_i.find(target[i]); iter != key_i.end())
          ret[i] = iter->second;
        else
          ret[i] = target[i];
      }
      return ret;
    }

    inline key_t crack(murk::data_const_ref ctext, const dist_t& expected, size_t key_len, flow_t<uint8_t, bool> filter = [](auto) { return true; }) {
      key_t ret(key_len);

      for (size_t i = 0; i < key_len; ++i) {
        data amalg;
        amalg.reserve(ctext.size() / key_len + 1);
        for (size_t pos = i; pos < ctext.size(); pos += key_len)
          amalg.push_back(ctext[pos]);
        ret[i] = subsitution::crack(amalg, expected, filter);
      }

      return ret;
    }
  }
//  template<typename T>
//  std::map<T, T> caeser_gen_encrypt_table(nonstd::span<const T> alphabet, size_t key) {
//    auto target = rotate_right(key, alphabet);
//    return zip<T, T>(alphabet, target);
//  }
//  template<typename T>
//  std::map<T, T> caeser_gen_decrypt_table(nonstd::span<const T> alphabet, size_t key) {
//    auto target = rotate_right(key, alphabet);
//    return zip<T, T>(target, alphabet);
//  }

//  template<typename T>
//  inline size_t caesar_known_sub_get_key(nonstd::span<const T> alphabet,
//                                         T known_ctext, T known_ptext,
//                                         nonstd::span<const T> msg) {
//    auto pos_iter = std::find(alphabet.begin(), alphabet.end(), known_ctext);
//    auto known_iter = std::find(alphabet.begin(), alphabet.end(), known_ptext);

//    ssize_t rot = known_iter - pos_iter;

//    return rot >= 0 ? rot : alphabet.size() + rot;
//  }

//  template<typename T>
//  inline size_t caesar_known_elem_get_key(nonstd::span<const T> alphabet,
//                                          T known_elem, size_t known_pos,
//                                          nonstd::span<const T> msg) {
//    auto pos_iter = std::find(alphabet.begin(), alphabet.end(), msg.at(known_pos));
//    auto known_iter = std::find(alphabet.begin(), alphabet.end(), known_elem);

//    ssize_t rot = known_iter - pos_iter;

//    return rot >= 0 ? rot : alphabet.size() + rot;
//  }

//  template<typename T>
//  inline size_t caesar_known_dist_get_key(nonstd::span<const T> alphabet,
//                                          const std::map<T, float>& dist,
//                                          nonstd::span<const T> msg) {
//    if (msg.size() == 0)
//      throw std::invalid_argument("Need at least one element for distribution analysis");

//    auto in_dist =
//           in<nonstd::span<const T>>()
//        >> freq_analysis<T>
//        >> normalise_freq<T>
//        << msg;
//    // TODO: actual stats

//    return caesar_known_sub_get_key(alphabet, *in_dist.begin(), *alphabet.begin(), msg);
//  }

//  template<typename In, typename Out>
//  inline std::map<In, Out> crack_substitution(std::map<Out, float> out_dist, nonstd::span<const In> in_msg) {
//    using namespace flow_ops;

//    auto in_dist =
//        in<nonstd::span<const In>>()
//     >> freq_analysis<In>
//     >> normalise_freq<In>
//     << in_msg;

//    std::vector<In> in_indexes;
//    std::vector<Out> out_indexes;
//    std::map<size_t, float> in_indexed;
//    std::map<size_t, float> out_indexed;

//    for (const auto& i : in_dist) {
//      in_indexed[in_indexes.size()] = i.second;
//      in_indexes.push_back(i.first);
//    }

//    for (const auto& i : out_dist) {
//      out_indexed[out_indexes.size()] = i.second;
//      out_indexes.push_back(i.first);
//    }

//    std::map<size_t, float> freqs_indexed;

//    auto index_subs = match_distribution(in_indexed, out_indexed);

//    std::map<In, Out> ret;

//    for (const auto& i : index_subs)
//      ret[in_indexes[i.first]] = out_indexes[i.second];

//    return ret;
//  }
}
