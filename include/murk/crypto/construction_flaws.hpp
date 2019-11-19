#pragma once

#include <murk/flow.hpp>
#include <murk/data.hpp>

#include <cppthings/safe_iter.hpp>

#include <atomic>
#include <map>
#include <optional>
#include <thread>

#include <iostream>

#define TBB_PREVIEW_CONCURRENT_ORDERED_CONTAINERS true
#include <tbb/concurrent_map.h>

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
    for (unsigned int thread_id = 0; thread_id < conc; ++thread_id) {
      threads[thread_id] = std::thread{[&, thread_id]() -> void {
        auto iter = inner_begin;
        // Check that we don't have more threads than ids
        if (!cppthings::safe_iter::safe_forward_small(iter, inner_end, thread_id))
          return;
        do
          enc_store_inter_res(*iter);
        while(cppthings::safe_iter::safe_forward_small(iter, inner_end, conc));
      }};
    }
    // Wait for the results
    for (auto& i : threads)
      i.join();

    std::atomic<bool> stop = false;
    std::optional<std::pair<OuterKey, InnerKey>> ret;

    // Now brute force the outer key
    for (unsigned int thread_id = 0; thread_id < conc; ++thread_id) {
      threads[thread_id] = std::thread{[&, thread_id]() {
        auto iter = outer_begin;
        // Check that we don't have more threads than ids
        if (!cppthings::safe_iter::safe_forward_small(iter, outer_end, thread_id))
          return;
        do {
          if (auto res = dec_check_inter_res(*iter)) {
            // Stop race conditions
            if (!stop.exchange(true))
              ret = res;
            return;
          }
        }
        while(!stop && cppthings::safe_iter::safe_forward_small(iter, outer_end, conc));
      }};
    }

    // Wait for the results
    for (auto& i : threads)
      i.join();

    [[unlikely]]
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

//  template<size_t BlockSize>
//  struct block_iter_t {
//    std::array<uint8_t, BlockSize> block;
//    bool end;

//    inline block_iter_t& operator++(int) {
//      auto i = block.rbegin();
//      while (!*i++) {
//        if (i == block.rend()) {
//          end = true;
//          break;
//        }
//      }

//      return *this;
//    }

//    template<typename Int>
//    inline block_iter_t& operator+=(Int n) {
//      auto i = block.rbegin();
//      while (!*i++) {
//        if (i == block.rend()) {
//          end = true;
//          break;
//        }
//      }

//      return *this;
//    }
//  };
}
