#pragma once

#include <murk/flow.hpp>
#include <murk/data.hpp>

#define TBB_PREVIEW_CONCURRENT_ORDERED_CONTAINERS true
#include <tbb/concurrent_map.h>

#include <botan/block_cipher.h>

#include <cppthings/safe_iter.hpp>
#include <cppthings/number.hpp>

#include <atomic>
#include <map>
#include <optional>
#include <thread>
#include <iostream>

namespace murk::crypto {
  /// A known-plaintext attack on a doubly applied cypher (maybe I'll do multi-dimensional later)
  ///
  /// O(2^(2n)) -> O(2^(n+1)) time
  /// O(2^n) space
  ///
  /// TODO: Check for collisions
  template<typename OuterKeyIter, typename InnerKeyIter = OuterKeyIter,
           typename OuterKey = typename std::iterator_traits<OuterKeyIter>::value_type,
           typename InnerKey = typename std::iterator_traits<InnerKeyIter>::value_type>
  inline std::pair<OuterKey, InnerKey> meet_in_the_middle_core(
      OuterKeyIter outer_begin, OuterKeyIter outer_end,
      InnerKeyIter inner_begin, InnerKeyIter inner_end,
      threaded_flow_t<const OuterKey&, std::optional<std::pair<OuterKey, InnerKey>>> dec_check_inter_res,
      threaded_flow_t<const InnerKey&, void> enc_store_inter_res) {

    // Manual thread pool gives us flexibility over skips
    auto conc = std::thread::hardware_concurrency();
    std::vector<std::thread> threads(conc);

    // First, we create the search table of keys
    for (decltype(conc) thread_id = 0; thread_id < conc; ++thread_id) {
      threads[thread_id] = std::thread{[&, thread_id]() -> void {
        auto iter = inner_begin;
        // Check that we don't have more threads than ids
        if (!cppthings::safe_iter::safe_forward_small(iter, inner_end, thread_id))
          return;
        do
          enc_store_inter_res(*iter);
        while (cppthings::safe_iter::safe_forward_small(iter, inner_end, conc));
      }};
    }
    // Wait for the results
    for (auto& i : threads)
      i.join();

    // Look at me, I'm being evil
    std::atomic_flag first;
    /*std::atomic<*/bool/*>*/ stop = false;
    std::optional<std::pair<OuterKey, InnerKey>> ret;

//    auto fast_conc_iter_diff = static_cast<typename std::iterator_traits<OuterKeyIter>::difference_type>(conc);

    // Now brute force the outer key
    for (decltype(conc) thread_id = 0; thread_id < conc; ++thread_id) {
      threads[thread_id] = std::thread{[&, thread_id]() {
        auto iter = outer_begin;
        // Check that we don't have more threads than ids
        if (!cppthings::safe_iter::safe_forward_small(iter, outer_end, thread_id))
          return;
        do {
          if (auto res = dec_check_inter_res(*iter); res && first.test_and_set()) {
            stop = true;
            ret = std::move(res);
          }
        }
        while(!stop && cppthings::safe_iter::safe_forward_small(iter, outer_end, conc));
      }};
    }

    // Wait for the results
    for (auto& i : threads)
      i.join();

    if (!ret)
      throw std::logic_error("Meet in the middle attack could not find key. Check the keyspace and functions");

    return *ret;
  }

  template<typename OuterPText, typename InnerCText,
           typename OuterKeyIter, typename InnerKeyIter = OuterKeyIter,
           typename OuterKey = typename std::iterator_traits<OuterKeyIter>::value_type,
           typename InnerKey = typename std::iterator_traits<InnerKeyIter>::value_type>
  inline std::pair<OuterKey, InnerKey> meet_in_the_middle_mem(
      OuterKeyIter outer_begin, OuterKeyIter outer_end,
      InnerKeyIter inner_begin, InnerKeyIter inner_end,
      threaded_flow_t<const OuterKey&, OuterPText> dec,
      threaded_flow_t<const InnerKey&, InnerCText> enc) {
    tbb::concurrent_map<InnerCText, InnerKey> inner_store;
    return meet_in_the_middle_core<OuterKeyIter, InnerKeyIter>(
           std::move(outer_begin), std::move(outer_end),
           std::move(inner_begin), std::move(inner_end),
           threaded_flow_t<const OuterKey&, std::optional<std::pair<OuterKey, InnerKey>>>::create([&](const OuterKey& k) -> std::optional<std::pair<OuterKey, InnerKey>> {
             if (auto iter = inner_store.find(dec(k)); iter != inner_store.end())
               return std::pair<OuterKey, InnerKey>{k, iter->second};
             else
               return std::nullopt;
           }),
           threaded_flow_t<const InnerKey&, void>::create([&](const InnerKey& k) {
              inner_store.emplace(enc(k), k);
           })
    );
  }

  template<typename... KeyT>
  inline void decrypt_n(murk::data_ref data, Botan::BlockCipher& bc,
                        data_const_ref k, KeyT... keys) {
    bc.set_key(k.data(), k.size());
    bc.decrypt_n(data.data(), data.data(), data.size() / bc.block_size());
    if constexpr (sizeof...(keys))
      decrypt_n(data, bc, keys...);
  }
}
