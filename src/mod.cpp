#include "murk/mod.hpp"

#include "murk/flow.hpp"

#include <sstream>

namespace murk::mod {
  struct base_shell : shell {
    std::string_view get_prompt() const noexcept override { return "murk"; }
  };

  shell& shell::root() {
    static base_shell s;
    return s;
  }

  module& module::root() {
    return shell::root();
  }

  manager& manager::root() {
    return shell::root().mgr;
  }

  std::pair<std::string, std::string> parse_cmd(std::string s) {
    auto split_pt = s.find(' ');
    std::pair<std::string, std::string> ret;
    std::string& cmd = ret.first,& args = ret.second;
    if (split_pt == s.npos)
      cmd = s;
    else {
      cmd = std::string{s.begin(), s.begin() + split_pt};
      while (true) {
        if (split_pt >= s.size()) {
          cmd = s;
          break;
        }
        else if (s[split_pt] == ' ') {
          ++split_pt;
          continue;
        }
        else {
          args = std::string{s.begin() + split_pt, s.end()};
          break;
        }
      }
    }
    return ret;
  }

  std::optional<std::string> shell::act(std::string s) {
    if (!s.empty()) {
      auto c = parse_cmd(std::move(s));
      if (auto iter = mgr.find(c.first); iter != mgr.end())
        return iter->second->act(c.second);
      else {
        fmt::print(stderr, "Unknown command \"{}\"\n", c.first);
        return std::nullopt;
      }
    }

    auto p = get_prompt();
    murk::prompt([this](std::string s) {
      auto c = parse_cmd(std::move(s));

      if (c.first == "exit" || c.first == "quit")
        return false;
      else if (auto iter = mgr.find(c.first); iter != mgr.end()) {
        if (auto res = iter->second->act(std::move(c.second)))
          fmt::print("{}\n", *res);
      }
      else
        fmt::print(stderr, "Unknown command \"{}\"\n", c.first);
      return true;
    }, p);
    return std::nullopt;
  }

  shell::shell() {
    mgr.register_modules(adapt_default_func_print([this](auto... x) -> std::string {
      if constexpr (sizeof...(x) == 0) {
        std::string ret;
        for (const auto& i : mgr) {
          ret += i.first;
          ret += '\n';
        }
        ret.pop_back();
        return ret;
      }
      else {
        return {};
      // TODO
      }
    }), "help", "?");
  }

  void manager::register_path(std::shared_ptr<module> mod, std::initializer_list<std::string> path) {
    if (path.size() == 0)
      throw std::invalid_argument("Cannot register to empty path");
    manager* mgr = &root();
    std::string pretty_path{shell::root().get_prompt()};
    auto iter = path.begin();
    auto end = path.end() - 1;
    for (; iter != end; ++iter) {
      auto& i = *iter;
      pretty_path.push_back('/');
      pretty_path.append(i);
      if (auto a = mgr->find(i); a != mgr->end()) {
        auto sh = dynamic_cast<shell*>(mgr->mods.at(i).get());
        if (!sh)
          throw std::logic_error("register_path hit non-shell");
        mgr = &sh->mgr;
      }
      else {
        auto sh = make_shell(pretty_path);
        mgr->register_module(sh, std::move(i));
        mgr = &sh->mgr;
      }
    }
    mgr->register_module(mod, *iter);
  }
}
