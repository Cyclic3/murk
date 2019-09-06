#pragma once
//! A collection of awful crypto primatives used by idiots

#include "murk/data.hpp"
#include "murk/flows/seq.hpp"
#include "murk/crypto/analysis.hpp"

#include <map>

namespace murk::crypto {
  namespace xor_single {
    inline murk::data decrypt(uint8_t key, data_const_ref b) {
      murk::data ret;
      for (auto i : b)
        ret.push_back(i & key);
      return ret;
    }

    uint8_t crack(const dist_t& expected, murk::data_const_ref ctext);
  }

  namespace xor_vigenere {
    inline murk::data decrypt(murk::data_const_ref key, data_const_ref b) {
      murk::data ret(b.size());
      for (size_t i = 0; i < b.size(); ++i)
        ret[i] = b[i] ^ key[i % key.size()];

      return ret;
    }

    size_t calc_key_length(const dist_t& expected, data_const_ref ctext, size_t min = 2, size_t max = 40);
    /// @returns The key
    murk::data crack_with_known_len(const dist_t& expected, data_const_ref ctext, size_t key_len);

    inline murk::data crack(const dist_t& expected, data_const_ref ctext) {
      return crack_with_known_len(expected, ctext, calc_key_length(expected, ctext));
    }
  }
//  template<typename T>
//  std::map<T, T> caeser_gen_encrypt_table(gsl::span<const T> alphabet, size_t key) {
//    auto target = rotate_right(key, alphabet);
//    return zip<T, T>(alphabet, target);
//  }
//  template<typename T>
//  std::map<T, T> caeser_gen_decrypt_table(gsl::span<const T> alphabet, size_t key) {
//    auto target = rotate_right(key, alphabet);
//    return zip<T, T>(target, alphabet);
//  }

//  template<typename T>
//  inline size_t caesar_known_sub_get_key(gsl::span<const T> alphabet,
//                                         T known_ctext, T known_ptext,
//                                         gsl::span<const T> msg) {
//    auto pos_iter = std::find(alphabet.begin(), alphabet.end(), known_ctext);
//    auto known_iter = std::find(alphabet.begin(), alphabet.end(), known_ptext);

//    ssize_t rot = known_iter - pos_iter;

//    return rot >= 0 ? rot : alphabet.size() + rot;
//  }

//  template<typename T>
//  inline size_t caesar_known_elem_get_key(gsl::span<const T> alphabet,
//                                          T known_elem, size_t known_pos,
//                                          gsl::span<const T> msg) {
//    auto pos_iter = std::find(alphabet.begin(), alphabet.end(), msg.at(known_pos));
//    auto known_iter = std::find(alphabet.begin(), alphabet.end(), known_elem);

//    ssize_t rot = known_iter - pos_iter;

//    return rot >= 0 ? rot : alphabet.size() + rot;
//  }

//  template<typename T>
//  inline size_t caesar_known_dist_get_key(gsl::span<const T> alphabet,
//                                          const std::map<T, float>& dist,
//                                          gsl::span<const T> msg) {
//    if (msg.size() == 0)
//      throw std::invalid_argument("Need at least one element for distribution analysis");

//    auto in_dist =
//           in<gsl::span<const T>>()
//        >> freq_analysis<T>
//        >> normalise_freq<T>
//        << msg;
//    // TODO: actual stats

//    return caesar_known_sub_get_key(alphabet, *in_dist.begin(), *alphabet.begin(), msg);
//  }

//  template<typename In, typename Out>
//  inline std::map<In, Out> crack_substitution(std::map<Out, float> out_dist, gsl::span<const In> in_msg) {
//    using namespace flow_ops;

//    auto in_dist =
//        in<gsl::span<const In>>()
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
