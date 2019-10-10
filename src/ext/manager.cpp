#include "murk/ext/manager.hpp"

#include "murk/flows/fs.hpp"

namespace murk::ext {
  manager mgr;

  manager::manager() {
    base_dir = fs::home() / ".murk";
    std::filesystem::create_directory(base_dir);
    base_dir /= "ext";
    std::filesystem::create_directory(base_dir);
    std::filesystem::create_directory(base_dir/".dls");
    tmp_dir = fs::temp();
    tmp_dir /= "murk";
    std::filesystem::create_directory(tmp_dir);
  }

  std::filesystem::path manager::obtain(std::string_view file_name, murk::web::http::address addr, bool perm) {
    auto tmp_file = tmp_dir/file_name;
    auto out = perm ? base_dir/file_name : tmp_file;

    if (std::filesystem::exists(file_name))
      return out;

    bool created;
    {
      std::unique_lock lock{dls_mutex};
      created = dls.insert(file_name).second;
    }

    // Check if someone else is downloading
    if (created) {
      auto fetched = murk::web::http::get(addr);
      // Need to do this to keep it atomic
      murk::fs::write_all_text(tmp_file, fetched.body);
      if (perm)
        std::filesystem::rename(tmp_file, out);
      {
        std::unique_lock lock{dls_mutex};
        dls.erase(file_name);
      }
    }
    else {
      while (true) {
        std::this_thread::yield();
        std::shared_lock lock{dls_mutex};
        if (dls.count(file_name))
          break;
      }
    }

    return out;
  }
}
