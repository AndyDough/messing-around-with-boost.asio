#include "checkError.hpp"
#include "encode.hpp"
#include "messageHeader.hpp"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/impl/read_until.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

using boost::asio::ip::tcp;

class TcpClient : public std::enable_shared_from_this<TcpClient> {
private:
  boost::asio::io_context &m_IO;
  std::string_view m_hostIP;
  tcp::socket m_socket;
  boost::system::error_code ec;
  tcp::resolver m_resolver;
  bool m_connected{false};

  TcpClient(boost::asio::io_context &io_context, std::string_view hostIP)
      : m_IO(io_context), m_hostIP{hostIP}, m_socket{io_context},
        m_resolver(io_context) {}

  void start() {
    m_resolver.async_resolve(
        m_hostIP, "9001",
        std::bind(&TcpClient::onResolve, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
  }

public:
  static std::shared_ptr<TcpClient> create(boost::asio::io_context &io_context,
                                           std::string_view hostIP) {
    auto tcpClient =
        std::shared_ptr<TcpClient>(new TcpClient(io_context, hostIP));

    tcpClient->start();
    // tcpClient->startWrite("Hey I'm connected");

    return tcpClient;
  }

  void startWrite(std::string_view msg) {
    // std::cout << "Start Write is being called\n";
    auto writtingBuffer =
        std::make_shared<std::vector<unsigned char>>(8 + msg.size());

    MessageHeader m_header;
    m_header.m_id = 1;
    m_header.m_length = 8 + msg.size();

    serialize(*writtingBuffer, m_header);
    std::memcpy(&(*writtingBuffer)[8], msg.data(), msg.size());
    boost::asio::dispatch(m_IO, [self = shared_from_this(), writtingBuffer]() {
      boost::asio::async_write(
          self->socket(), boost::asio::buffer(*writtingBuffer),
          std::bind(&TcpClient::handleWrite, self, std::placeholders::_1,
                    std::placeholders::_2));
    });
  }

  void handleWrite(const boost::system::error_code &ec,
                   std::size_t bytesTransfered) {
    // std::cout << "Starting (handleWrite)\n";
    checkError(ec);
    std::cout << "I'm writting right now bytesTransfering:" << bytesTransfered
              << std::endl;
  };

  tcp::socket &socket() { return m_socket; }

  bool isConnected() { return m_connected; }

private:
  void onResolve(const boost::system::error_code &ec,
                 tcp::resolver::results_type endpointIterator) {
    checkError(ec);
    boost::asio::async_connect(
        m_socket, endpointIterator,
        std::bind(&TcpClient::onConnect, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
  }

  void onConnect(const boost::system::error_code &ec,
                 const boost::asio::ip::tcp::endpoint &endpoint) {
    // std::cout << "onConnect is being called\n";
    checkError(ec);
    m_connected = true;
  }
};

void uiThread(TcpClient &client) {
  std::string msg;

  while (true) {
    std::cout << "msg (enter 0 to exit): ";
    std::cin >> msg;
    if (msg == "0") {
      break;
    } else {
      client.startWrite(msg);
    }
  }
}

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: client <host ip>" << '\n';
      return 1;
    }
    std::string_view hostIP{argv[1]};
    boost::asio::io_context io_context;

    std::cout << "Before TcpClient is contructed\n";
    auto tcpClient = TcpClient::create(io_context, hostIP);

    std::jthread uiBackground(uiThread, std::ref(*tcpClient.get()));

    auto work_guard = boost::asio::make_work_guard(io_context);
    io_context.run();
    uiBackground.join();

  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
