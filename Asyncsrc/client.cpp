#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/impl/read_until.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <exception>
#include <iostream>
#include <streambuf>

using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: client <host ip>" << '\n';
      return 1;
    }
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);

    tcp::resolver::results_type endpoints{resolver.resolve(argv[1], "9001")};

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    while (true) {
      boost::asio::streambuf streamb;
      boost::system::error_code ec;

      boost::asio::read_until(socket, streamb, "\r\n", ec);
      if (ec == boost::asio::error::eof) {
        break;
      } else if (ec) {
        throw boost::system::system_error(ec);
      }

      
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
