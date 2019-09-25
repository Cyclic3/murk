#include "murk/ext/manager.hpp"

#include "murk/flows/file.hpp"

namespace murk::ext {
  manager mgr;

  manager::manager() {
    base_dir = file::home() / ".murk";
    std::filesystem::create_directory(base_dir);
    base_dir /= "ext";
    std::filesystem::create_directory(base_dir);
    std::filesystem::create_directory(base_dir/".dls");
  }

  std::filesystem::path manager::obtain(std::string_view file_name, murk::web::http::address addr) {
    auto out = base_dir/file_name;

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
      murk::file::write_all_text(base_dir/".dls"/file_name, fetched.body);
      std::filesystem::rename(base_dir/".dls"/file_name, out);
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
