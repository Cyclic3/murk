#pragma once
//! A collection of awful crypto primatives used by idiots

#include "murk/data.hpp"
#include "murk/flows/seq.hpp"
#include "murk/crypto/analysis.hpp"

#include <map>

namespace murk::crypto {
  template<typename T>
  std::map<T, T> caeser_gen_encrypt_table(gsl::span<const T> alphabet, size_t key) {
    auto target = rotate_right(key, alphabet);
    return zip<T, T>(alphabet, target);
  }
  template<typename T>
  std::map<T, T> caeser_gen_decrypt_table(gsl::span<const T> alphabet, size_t key) {
    auto target = rotate_right(key, alphabet);
    return zip<T, T>(target, alphabet);
  }

  template<typename T>
  inline size_t caesar_known_sub_get_key(gsl::span<const T> alphabet,
                                         T known_ctext, T known_ptext,
                                         gsl::span<const T> msg) {
    auto pos_iter = std::find(alphabet.begin(), alphabet.end(), known_ctext);
    auto known_iter = std::find(alphabet.begin(), alphabet.end(), known_ptext);

    ssize_t rot = known_iter - pos_iter;

    return rot >= 0 ? rot : alphabet.size() + rot;
  }

//  template<typename T>
//  inline size_t caesar_known_elem_get_key(gsl::span<const T> alphabet,
//                                          T known_elem, size_t known_pos,
//                                          gsl::span<const T> msg) {
//    auto pos_iter = std::find(alphabet.begin(), alphabet.end(), msg.at(known_pos));
//    auto known_iter = std::find(alphabet.begin(), alphabet.end(), known_elem);

//    ssize_t rot = known_iter - pos_iter;

//    return rot >= 0 ? rot : alphabet.size() + rot;
//  }

  template<typename T>
  inline size_t caesar_known_dist_get_key(gsl::span<const T> alphabet,
                                          const std::map<T, float>& dist,
                                          gsl::span<const T> msg) {
    if (msg.size() == 0)
      throw std::invalid_argument("Need at least one element for distribution analysis");

    auto in_dist =
           in<gsl::span<const T>>()
        >> freq_analysis<T>
        >> normalise_freq<T>
        << msg;
    // TODO: actual stats

    return caesar_known_sub_get_key(alphabet, *in_dist.begin(), *alphabet.begin(), msg);
  }

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
