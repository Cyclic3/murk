#include <murk/flows/fs.hpp>
#include <murk/crypto/awful.hpp>
#include <murk/crypto/aes.hpp>
#include <murk/ext/manager.hpp>

#include <murk/common.hpp>

int main() {
//  {
//    auto ctext = "8fd2c6939bc7d8f49fd998f4fb9b9e9e9a93c798f4df9b9b8a8a8a"_hex;
//    auto res = murk::crypto::xor_single::crypt(ctext, 0xAB);

//    murk::log("2.2: {}", murk::deserialise<std::string_view>(res));
//  }

//  {
//    auto ctext = "feedfaf1d7fbedebfdfaed"_hex;
//    murk::data res;
//    for (int i = 0; i <= 255; ++i) {
//      res = murk::crypto::xor_single::crypt(ctext, i);
//      if (std::all_of(res.begin(), res.end(), ::isprint))
//        murk::log("2.3?: {}", murk::deserialise<std::string_view>(res));
//    }
//  }

//  {
//    auto ctext = "SMJ YEOYVLUO UOOUWRJGJWS ZI SMJ UKJD ZI SMJ XEIIJOJWYJ JWREWJ OZVWX LUORJ YJWSOUL HMJJLD LJX SZ SMJ GZDS JKSJWXJX POZDPJYSD. "
//                 "SMJ HMZLJ ZI UOESMGJSEY WZH UPPJUOJX HESMEW SMJ ROUDP ZI GJYMUWEDG. "
//                 "U CURVJ RLEGPDJ JCJW ZI UW UWULTSEYUL JWREWJ US LJWRSM ZPJWJX ZVS, UWX E PVODVJX HESM JWSMVDEUDG SMJ DMUXZHT CEDEZW. "
//                 "SMJ XOUHEWRD UWX SMJ JKPJOEGJWSD HJOJ ZI SMJ GZDS YZDSLT FEWX. "
//                 "XOUISDGJW ZI SMJ MERMJDS ZOXJO HJOJ WJYJDDUOT SZ JYZWZGEQJ SMJ LUAZVO ZI GT ZHW MJUX; "
//                 "HMELDS DFELLJX HZOFGJW HJOJ OJNVEOJX SZ JKJYVSJ SMJ JKPJOEGJWSUL GUYMEWJOT SZ HMEYM E HUD ZALERJX YZWDSUWSLT SZ MUCJ OJYZVODJ. EW ZOXJO SZ YUOOT ZVS GT PVODVESD DVYYJDDIVLLT, E MUX PVOYMUDJX U MZVDJ HESM UAZCJ U NVUOSJO ZI UW UYOJ ZI ROZVWX EW U CJOT NVEJS LZYULEST. GT YZUYM-MZVDJ HUD WZH YZWCJOSJX EWSZ U IZORJ UWX U IZVWXOT, HMELDS GT DSUALJD HJOJ SOUWDIZOGJX EWSZ U HZOFDMZP. E AVELS ZSMJO JKSJWDECJ HZOFDMZPD GTDJLI, UWX MUX U IEOJ-POZZI AVELXEWR IZO GT XOUHEWRD UWX XOUISDGJW. MUCEWR GTDJLI HZOFJX HESM U CUOEJST ZI SZZLD, UWX MUCEWR DSVXEJX SMJ UOS ZI YZWDSOVYSEWR JUYM ZI SMJG, E US LJWRSM LUEX ES XZHW UD U POEWYEPLJ—SMUS, JKYJPS EW OUOJ YUDJD, E HZVLX WJCJO XZ UWTSMEWR GTDJLI EI E YZVLX UIIZOX SZ MEOJ UWZSMJO PJODZW HMZ YZVLX XZ ES IZO GJ."_b;

//    auto key = murk::crypto::subsitution::crack_with_known(ctext, murk::crypto::dist_conv(murk::crypto::english_letter_dist),
//    {{'S', 't'}, {'M', 'h'}, {'J', 'e'}, {'D', 's'}, {'Z', 'o'}, {'O', 'r'}, {'W', 'n'}}, ::isalpha);

//    auto inv = murk::crypto::subsitution::invert_key(key);

//    std::cout << inv['i'] << std::endl;
////    auto h = std::find(key.begin(), key.end(), [](auto& i){return i.second == 'h';});
////    key['S'] = 't';
////    key['M'] = 'h';
////    key['U'] = 'a';
////    key['U'] = 'a';
////    std::swap(key['a'] = 'h', h->second);

//    auto res = murk::crypto::subsitution::crypt(ctext, key);

//    murk::log("2.4: {}", murk::deserialise<std::string_view>(res));
//  }

//  {
//    murk::crypto::aes::table_t state = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//    murk::crypto::aes::shift_rows(state);
//    std::string arr = "[";
//    for (auto i : state)
//      arr += fmt::format("{}, ", i);

//    arr.back() = ']';

//    fmt::print("3.1: {}\n", arr);
//  }

//  {
//    murk::crypto::aes::table_t state = {0x75};
//    murk::crypto::aes::sub_bytes(state);
//    fmt::print("3.2: 0x{:x}\n", state[0]);
//  }

//  {
//    for (size_t i = 0; i <= 255; ++i) {
//      fmt::print("0x{:02x}, ", std::find(murk::crypto::aes::sbox, murk::crypto::aes::sbox+256, i) - murk::crypto::aes::sbox);
//    }
//    fmt::print("\n");
//  }
  {
    murk::crypto::aes::table_t rk = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    murk::crypto::aes::table_t ptext;
    std::fill(ptext.begin(), ptext.end(), 1);

    murk::crypto::aes::sub_bytes(ptext);
    murk::crypto::aes::shift_rows(ptext);
    murk::crypto::aes::add_round_key(ptext, rk);

    murk::crypto::aes::unshift_rows(ptext);
    murk::crypto::aes::unsub_bytes(ptext);

    // This leaves us with a polyalphabetic repeated cipher, so we can use a similar tatic to xor_vigenere!
  }

  murk::crypto::vigenere::key_t key;
  const auto ctexts = murk::map<std::string, murk::data>(murk::fs::read_all_lines("/tmp/ciphertexts.txt"),
                                                         murk::flow_t<std::string, murk::data>(murk::hex_decode));


  auto squish = [](murk::crypto::aes::table_ref_t tab) {
    murk::crypto::aes::unshift_rows(tab);
    murk::crypto::aes::unsub_bytes(tab);
  };

  std::vector<murk::data> squished = ctexts;

  {

    for (auto& i : squished)
      squish(i);

    auto b = "67a2401f0f36c3b680abb775ecedf311"_hex;
    squish(b);

    std::string text = murk::fs::read_all_text(murk::ext::mgr.obtain("http://historyofeconomicthought.mcmaster.ca/babbage/babb1"));
//    text += murk::fs::read_all_text(murk::ext::mgr.obtain("http://historyofeconomicthought.mcmaster.ca/babbage/babb2"));
    text += murk::fs::read_all_text(murk::ext::mgr.obtain("http://historyofeconomicthought.mcmaster.ca/babbage/babb3"));
    std::vector<murk::crypto::token_t> dist_tokens;
    for (auto i : text) {
      if (::isupper(i))
        i = ::tolower(i);
      if (i == ' ' || ::islower(i))
        dist_tokens.push_back(i);
    }

    auto filtered_text = murk::filter<char>(text, [](auto i) { return i == ' ' || ::islower(i); });
    auto mod_dist = murk::crypto::normalise_freq(murk::count<murk::crypto::token_t>(dist_tokens));

    mod_dist = murk::crypto::renormalise_freq(mod_dist);

    murk::data pile;
    for (auto& i : squished)
      pile.insert(pile.end(), i.begin(), i.end());
    key = murk::crypto::vigenere::crack(pile, mod_dist, 16);

    auto res = murk::crypto::vigenere::crypt(b, key);
    murk::log("4.1: {}", murk::deserialise<std::string_view>(res));
  }

  {
    {
      murk::crypto::aes::round_constant_t rc;
      for (auto i = 1; i <= 10; ++i) {
        murk::crypto::aes::update_round_constant(i, rc);
        murk::log("{:x}", rc[0]);
      }
    }

    auto ctext_1 = "67a2401f0f36c3b680abb775ecedf311"_hex, ptext_1 = "rijndael is king"_b;
    auto ctext_2 = "0c9e246cb8a1bfa3b0e947a1a94c8d11"_hex, ptext_2 = "the economy of m"_b;

    squish(ctext_1); squish(ctext_2);
//    murk::log("4.3: {}", murk::deserialise<std::string_view>(res));
  }
}
