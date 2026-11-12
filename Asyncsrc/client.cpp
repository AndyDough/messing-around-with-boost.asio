#include "encode.hpp"
#include "messageHeader.hpp"
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

void checkError(const boost::system::error_code &ec) {
  if (ec == boost::asio::error::eof) {
    throw boost::system::system_error(ec);
  } else if (ec) {
    throw boost::system::system_error(ec);
  }
}

class TcpClient : public std::enable_shared_from_this<TcpClient> {
private:
  boost::asio::io_context &m_IO;
  tcp::socket m_socket;
  boost::system::error_code ec;
  std::array<unsigned char, 250> m_data;
  MessageHeader m_header;
  bool m_connected{false};

public:
  TcpClient(boost::asio::io_context &io_context, std::string_view hostIP)
      : m_IO(io_context), m_socket{io_context} {
    tcp::resolver resolver(io_context);

    resolver.async_resolve(hostIP, "9001",
                           std::bind(&TcpClient::onResolve, this,
                                     std::placeholders::_1,
                                     std::placeholders::_2));
  }

  void startWrite(std::string_view msg) {
    serialize(m_data, MessageHeader{13, 1});
    std::memcpy(&m_data[8], "Hello", 5);
    boost::asio::async_write(
        m_socket, boost::asio::const_buffer(&m_data[0], m_header.m_length),
        boost::asio::transfer_exactly(m_header.m_length),
        std::bind(&TcpClient::handleWrite, this, std::placeholders::_1,
                  std::placeholders::_2));
  }

  void handleWrite(const boost::system::error_code &ec,
                   std::size_t bytesTransfered) {
    checkError(ec);
    std::cout << "I'm writting right now\n";
  };

  tcp::socket &socket() { return m_socket; }

  bool isConnected() { return m_connected; }

private:
  void onResolve(const boost::system::error_code &ec,
                 tcp::resolver::results_type endpointIterator) {

    checkError(ec);
    boost::asio::async_connect(m_socket, endpointIterator,
                               std::bind(&TcpClient::onConnect, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }

  void onConnect(const boost::system::error_code &ec,
                 const boost::asio::ip::tcp::endpoint &endpoint) {
    checkError(ec);
    m_connected = true;
  }
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

    auto workGuard = boost::asio::make_work_guard(io_context);

    while (true) {
      if (tcpClient.isConnected()) {
        tcpClient.startWrite("Hello");
      }
      io_context.run_one();
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
