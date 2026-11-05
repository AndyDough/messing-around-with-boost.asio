#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <exception>
#include <iostream>

int main() {
  try {
    boost::asio::io_context io_context;
    tcp_server server{io_context};
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
