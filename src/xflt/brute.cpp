#include "murk/xflt/brute.hpp"

#include "murk/span.hpp"

#include <cppthings/defer.hpp>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace murk::xflt {
  std::string brute(nonstd::span<const std::string> options, murk::flow_t<std::string_view, bool> oracle, std::chrono::seconds log_interval) {
    thread_local boost::asio::thread_pool pool;
    std::thread logger;
    bool stop_logging = false;
    std::condition_variable stop_logging_condvar;
    std::mutex stop_logging_mutex;
    std::atomic<size_t> count = 0;

    if (log_interval.count()) {
      logger = std::thread{[&]() {
        size_t last_count = 0;
        while (true) {
          std::unique_lock lock{stop_logging_mutex};
          if (stop_logging_condvar.wait_for(lock, log_interval,
                                            [&](){ return stop_logging; }))
            break;
          murk::log("{} candidates checked ({} candidates / s)", count, static_cast<float>(count - last_count) / log_interval.count());
          last_count = count;
        }
      }};
    }

    std::atomic<bool> found = false;

    std::string res;

    for (auto& i : options) {
      if (found)
        break;
      boost::asio::post(pool, [&, i]() {
        if (found)
          return;

        ++count;

        if (!oracle(i))
          return;

        // See if we did it
        if (!found.exchange(true))
          res = i;
      });
    }

    pool.join();

    if (log_interval.count()){
      {
        std::lock_guard lock{stop_logging_mutex};
        stop_logging = true;
      }
      stop_logging_condvar.notify_all();
      logger.join();
    }

    // Gotos for once induce code duplication, so this is probably better
    //
    // *oh no* 1 atomic read op after a massive brute force. mY nAnOsEcOnDs!!!1!
    if (!found)
      throw std::runtime_error("Search space exhausted");

    return res;
  }
}
