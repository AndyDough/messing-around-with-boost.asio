#include "encode.hpp"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/impl/read_until.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;

class TcpClient : public std::enable_shared_from_this<TcpClient> {
private:
  boost::asio::io_context &m_IO;
  tcp::socket m_socket;
  boost::system::error_code ec;

public:
  TcpClient(boost::asio::io_context &io_context, std::string_view hostIP)
      : m_IO(io_context), m_socket{io_context} {
    tcp::resolver resolver(io_context);

    resolver.async_resolve(hostIP, "9001",
                           std::bind(&TcpClient::onResolve, this,
                                     std::placeholders::_1,
                                     std::placeholders::_2));
  }

private:
  void onResolve(const boost::system::error_code &ec,
                 tcp::resolver::results_type endpointIterator) {

    boost::asio::async_connect(m_socket, endpointIterator,
                               std::bind(&TcpClient::onConnect, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }

  void onConnect(const boost::system::error_code &ec,
                 const boost::asio::ip::tcp::endpoint &endpoint) {}
};

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: client <host ip>" << '\n';
      return 1;
    }
    std::string_view hostIP{argv[1]};
    boost::asio::io_context io_context;

    TcpClient tcpClient(io_context, hostIP);

    while (true) {
      std::array<unsigned char, 250> streamb;
      boost::system::error_code ec;

      // serialize(streamb, MessageHeader{13, 1});
      // std::memcpy(&streamb[8], "Hello", 5);

      // boost::asio::write(socket, boost::asio::const_buffer(&streamb[0], 13),
      //                    ec);
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
