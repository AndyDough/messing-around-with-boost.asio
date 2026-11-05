#include <boost/asio.hpp>
#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <cstdio>
#include <functional>
#include <print>

class Printer {
private:
  boost::asio::steady_timer m_timer;
  int m_count;

public:
  Printer(boost::asio::io_context &io)
      : m_timer{io, boost::asio::chrono::seconds(1)}, m_count{0} {
    m_timer.async_wait(std::bind(&Printer::print, this));
  }

  ~Printer() { std::print("Final count is {}\n", m_count); }

  void print() {
    if (m_count < 5) {
      std::print("{}", m_count);
      ++m_count;
      std::fflush(stdout);

      m_timer.expires_at(m_timer.expiry() + boost::asio::chrono::seconds(1));
      m_timer.async_wait(std::bind(&Printer::print, this));
    }
  }
};

int main() {
  boost::asio::io_context io;
  Printer p(io);
  io.run();

  return 0;
}
