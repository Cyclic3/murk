#include "murk/crypto/analysis.hpp"

namespace murk::crypto {
  const std::array<char, 26> english_lcase = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z'
  };

  const std::array<char, 26> english_ucase {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z'
  };

  const std::array<char, 10> arabic_numerals {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
  };

  // TODO: use uppercase
  const std::map<char, freq_t> english_letter_dist {
    { 'e', 0.12702 },
    { 't', 0.09056 },
    { 'a', 0.08167 },
    { 'o', 0.07507 },
    { 'i', 0.06966 },
    { 'n', 0.06749 },
    { 's', 0.06327 },
    { 'h', 0.06094 },
    { 'r', 0.05987 },
    { 'd', 0.04253 },
    { 'l', 0.04025 },
    { 'c', 0.02782 },
    { 'u', 0.02758 },
    { 'm', 0.02406 },
    { 'w', 0.0236  },
    { 'f', 0.02228 },
    { 'g', 0.02015 },
    { 'y', 0.01974 },
    { 'p', 0.01929 },
    { 'b', 0.01492 },
    { 'v', 0.00978 },
    { 'k', 0.00772 },
    { 'j', 0.00153 },
    { 'x', 0.0015  },
    { 'q', 0.00095 },
    { 'z', 0.00074 },
  };

  const std::map<char, freq_t> twist_char_dist {
    { '\n', 0.0205083545297384 },
    { ' ', 0.160632133483887 },
    { '!', 0.00158888415899128 },
    { '"', 9.88248648354784e-05 },
    { '\'', 0.0123366368934512 },
    { '(', 8.23540540295653e-05 },
    { ')', 8.23540540295653e-05 },
    { '*', 1.64708108059131e-05 },
    { ',', 0.0176896508783102 },
    { '-', 0.00287690153345466 },
    { '.', 0.00884262938052416 },
    { ':', 0.000707146769855171 },
    { ';', 0.00289337243884802 },
    { '?', 0.00111342675518245 },
    { 'A', 0.00118809449486434 },
    { 'B', 0.00143735273741186 },
    { 'C', 0.000820246350485831 },
    { 'D', 0.000566595874261111 },
    { 'E', 0.000600635539740324 },
    { 'F', 0.000566595874261111 },
    { 'G', 0.000451300205895677 },
    { 'H', 0.0012408010661602 },
    { 'I', 0.00309321819804609 },
    { 'J', 0.000408476102165878 },
    { 'K', 5.38046479050536e-05 },
    { 'L', 0.000437025504652411 },
    { 'M', 0.00213132286444306 },
    { 'N', 0.00104315136559308 },
    { 'O', 0.00149225536733866 },
    { 'P', 0.000272317411145195 },
    { 'Q', 2.96474590868456e-05 },
    { 'R', 0.000557811465114355 },
    { 'S', 0.00129460566677153 },
    { 'T', 0.00217085285112262 },
    { 'U', 0.000130668428028002 },
    { 'V', 0.000116393726784736 },
    { 'W', 0.000987150589935482 },
    { 'X', 8.7844324298203e-05 },
    { 'Y', 0.000478751549962908 },
    { 'Z', 1.09805398551543e-06 },
    { '[', 1.09805398551543e-06 },
    { ']', 1.09805398551543e-06 },
    { '_', 0.000250356330070645 },
    { 'a', 0.0572942644357681 },
    { 'b', 0.0107477530837059 },
    { 'c', 0.0165915954858065 },
    { 'd', 0.0349993743002415 },
    { 'e', 0.0932280793786049 },
    { 'f', 0.0152234211564064 },
    { 'g', 0.0158899389207363 },
    { 'h', 0.0484088100492954 },
    { 'i', 0.04863391071558 },
    { 'j', 0.000621498562395573 },
    { 'k', 0.00671350210905075 },
    { 'l', 0.0301558580249548 },
    { 'm', 0.0176501199603081 },
    { 'n', 0.049803338944912 },
    { 'o', 0.0561951100826263 },
    { 'p', 0.0129142133519053 },
    { 'q', 0.00072251952951774 },
    { 'r', 0.0439770631492138 },
    { 's', 0.0435466282069683 },
    { 't', 0.0638386681675911 },
    { 'u', 0.0203008223325014 },
    { 'v', 0.0070330360904336 },
    { 'w', 0.0174766276031733 },
    { 'x', 0.000930051784962416 },
    { 'y', 0.0150422425940633 },
    { 'z', 0.000173492531757802 },
  };

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
    size_t ret;
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
