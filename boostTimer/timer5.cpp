#include <boost/asio.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/detail/chrono.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <cstdio>
#include <functional>
#include <print>
#include <thread>

class Printer {
private:
  boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
  boost::asio::steady_timer m_timer1;
  boost::asio::steady_timer m_timer2;
  int m_count;

public:
  Printer(boost::asio::io_context &io)
      : m_strand{boost::asio::make_strand(io)},
        m_timer1{io, boost::asio::chrono::seconds(1)},
        m_timer2{io, boost::asio::chrono::seconds(1)}, m_count{0} {
    m_timer1.async_wait(boost::asio::bind_executor(
        m_strand, std::bind(&Printer::print1, this)));
    m_timer2.async_wait(boost::asio::bind_executor(
        m_strand, std::bind(&Printer::print2, this)));
  }

  ~Printer() { std::print("The final count is {}\n", m_count); }

  void print1() {
    if (m_count < 10) {
      std::print("Timer 1: {}\n", m_count);
      ++m_count;

      m_timer1.expires_at(m_timer1.expiry() + boost::asio::chrono::seconds(1));

      m_timer1.async_wait(boost::asio::bind_executor(
          m_strand, std::bind(&Printer::print1, this)));
    }
  }

  void print2() {
    if (m_count < 10) {
      std::print("Timer 2: {}\n", m_count);
      ++m_count;

      m_timer2.expires_at(m_timer2.expiry() + boost::asio::chrono::seconds(1));

      m_timer2.async_wait(boost::asio::bind_executor(
          m_strand, std::bind(&Printer::print2, this)));
    }
  }
};

int main() {
  boost::asio::io_context io;
  Printer p(io);

  std::thread t([&] { io.run(); });
  io.run();
  t.join();

  return 0;
}
