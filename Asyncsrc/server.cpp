#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
private:
  tcp::socket m_socket;

  TcpConnection(boost::asio::io_context &io_context) : m_socket(io_context) {}

  void handleWrite();

public:
  using Sptr = std::shared_ptr<TcpConnection>;

  static Sptr create(boost::asio::io_context &io_context) {
    return Sptr(new TcpConnection(io_context));
  }

  tcp::socket &socket() { return m_socket; }

  void start(); // Writing messages back to our clients
};

class TcpServer {
private:
  boost::asio::io_context &m_IO;
  tcp::acceptor m_acceptor;

public:
  TcpServer(boost::asio::io_context &io_context)
      : m_IO(io_context),
        m_acceptor(io_context, tcp::endpoint(tcp::v4(), 9001)) {
    startAccept();
  }

private:
  void startAccept() {
    TcpConnection::Sptr newConnection{TcpConnection::create(m_IO)};

    m_acceptor.async_accept(newConnection->socket(),
                            std::bind(&TcpServer::handleAccept, this,
                                      newConnection,
                                      boost::asio::placeholders::error));
  }

  void handleAccept(TcpConnection::Sptr newConnection,
                    const boost::system::error_code &ec) {
    if (!ec) {
      newConnection->start();
    }
    startAccept();
  }
};

int main() {

  try {
    boost::asio::io_context io_context;

  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
