#pragma once

#include "murk/flow.hpp"

#include <map>
#include <memory>
#include <optional>
#include <string>

namespace murk::mod {
  struct module {
    /// Called when module is called from a prompt
    virtual std::optional<std::string> act(std::string command) = 0;

    static module& root();
  };
  class manager {
  public:
    using mods_t = std::map<std::string, std::shared_ptr<module>>;

  private:
    mods_t mods;

  public:
    inline mods_t::iterator register_module(std::shared_ptr<module> mod, std::string str) {
      auto a = mods.emplace(std::move(str), mod);
      if (!a.second)
        throw std::logic_error("Module already registered");
      return a.first;
    };

    template<typename... Names>
    inline void register_modules(const std::shared_ptr<module>& mod, std::string name, Names&&... tail) {
      register_module(mod, name);
      if constexpr (sizeof...(Names) != 0)
        register_modules(mod, std::forward<Names&&>(tail)...);
    }
    inline mods_t::iterator find(const std::string& s) {
      return mods.find(s);
    }
    inline mods_t::const_iterator find(const std::string& s) const {
      return mods.find(s);
    }
    inline mods_t::iterator begin() {
      return mods.begin();
    }
    inline mods_t::const_iterator begin() const {
      return mods.cbegin();
    }
    inline mods_t::const_iterator cbegin() const {
      return mods.cbegin();
    }
    inline mods_t::iterator end() {
      return mods.end();
    }
    inline mods_t::const_iterator end() const {
      return mods.cend();
    }
    inline mods_t::const_iterator cend() const {
      return mods.cend();
    }

  public:
    static manager& root();
    static void register_path(std::shared_ptr<module> mod, std::initializer_list<std::string> path);
  };

  struct shell : public module {
    manager mgr;

    virtual std::string_view get_prompt() const noexcept = 0;

    std::optional<std::string> act(std::string) override;

    static shell& root();

    shell();
  };

  inline std::shared_ptr<shell> make_shell(std::string prompt) {
    class shell_int : public shell {
    private:
      std::string p;
    public:
      std::string_view get_prompt() const noexcept override { return p; }
    public:
      shell_int(std::string p_) : p{std::move(p_)} {}
    };

    return std::make_shared<shell_int>(std::move(prompt));
  }

  template<typename F>
  inline std::shared_ptr<module> adapt_default_func_print(F f) {
    class mod_int : public module {
    private:
      decltype(f) flow;
    public:
      std::optional<std::string> act(std::string arg) override {
        if (arg.empty())
          return flow();
        else
          return flow(arg);
      }
    public:
      mod_int(decltype(f) flow_) : flow{std::move(flow_)} {}
    };

    return std::make_shared<mod_int>(std::move(f));
  }

  template<typename F>
  inline std::shared_ptr<module> adapt_simple_func_print(F f) {
    class mod_int : public module {
    private:
      decltype(f) flow;
    public:
      std::optional<std::string> act(std::string arg) override {
        return flow(arg);
      }
    public:
      mod_int(decltype(f) flow_) : flow{std::move(flow_)} {}
    };

    return std::make_shared<mod_int>(std::move(f));
  }
}
