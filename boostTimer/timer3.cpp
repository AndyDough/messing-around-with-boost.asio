#include <boost/asio.hpp>
#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <functional>
#include <print>

void print(const boost::system::error_code & /*e*/,
           boost::asio::steady_timer *t, int *count) {
  if (*count < 5) {
    std::print("{}\n", *count);
    ++(*count);

    t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
    t->async_wait(std::bind(print, boost::asio::placeholders::error, t, count));
  }
}

int main() {
  boost::asio::io_context io;
  int count = 0;
  boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));

  t.async_wait(std::bind(print, boost::system::error_code(), &t, &count));

  io.run();

  std::print("Final count is {}\n", count);

  return 0;
}
