#include "murk/crypto/analysis.hpp"

namespace murk::crypto {
  double score_dist_match(const dist_t& expected, const dist_t& measured) {
    double chi_squared = 0.;

    // TODO: thread
    for (auto& i : measured) {
      auto p_expected_iter = expected.find(i.first);
      double p_expected = p_expected_iter != expected.end() ? p_expected_iter->second : 0.;
      double stat = (p_expected - i.second);
      chi_squared += (stat * stat) / p_expected;
    }

    return chi_squared;
  }

  double score_dist_compare(const dist_t& expected, const dist_t& measured) {
    // Check both ways!
    const auto expected_sorted = murk::crypto::sort_freq(expected);
    const auto measured_sorted = murk::crypto::sort_freq(measured);

    // I'm sure this has been done before, I just don't know the name
    double pseudo_chi_squared = 0.;

    if (expected_sorted.size() < measured_sorted.size()) {
      size_t i = 0;
      for (; i < expected_sorted.size(); ++i) {
        double stat = (expected_sorted[i].second - measured_sorted[i].second);
        pseudo_chi_squared += (stat * stat) / expected_sorted[i].second;
      }
      for (; i < measured_sorted.size(); ++i) {
        double stat = (measured_sorted[i].second);
        pseudo_chi_squared += (stat * stat) / expected_sorted[i].second;
      }
    }
    else {
      size_t i = 0;
      for (; i < measured_sorted.size(); ++i) {
        double stat = (expected_sorted[i].second - measured_sorted[i].second);
        pseudo_chi_squared += (stat * stat) / expected_sorted[i].second;
      }
      for (; i < expected_sorted.size(); ++i) {
        // (f_e - 0)^2 / f_e == f_e
        pseudo_chi_squared += expected_sorted[i].second;
      }
    }

    return pseudo_chi_squared;
  }

  size_t hamming_distance(murk::data_const_ref a, murk::data_const_ref b) {
    size_t ret = 0;
    size_t i = 0;

    if (a.size() < b.size()) {
      for (; i < a.size(); ++i) {
        // Show which bits differ
        uint8_t c = a[i] ^ b[i];
        ret += count_set_bits(c);
      }
      for (; i < b.size(); ++i) {
        ret += count_set_bits(a[i]);
      }
    }
    else {
      for (; i < b.size(); ++i) {
        // Show which bits differ
        uint8_t c = a[i] ^ b[i];
        ret += count_set_bits(c);
      }
      for (; i < a.size(); ++i) {
        ret += count_set_bits(a[i]);
      }
    }

    return ret;
  }
}
