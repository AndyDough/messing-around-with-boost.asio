#pragma once
#include "TcpConnection.hpp"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
class TcpServer {
private:
  boost::asio::io_context &m_IO;
  tcp::acceptor m_acceptor;
  std::vector<std::shared_ptr<TcpConnection>> m_connectedClients;

public:
  TcpServer(boost::asio::io_context &io_context)
      : m_IO(io_context),
        m_acceptor(io_context, tcp::endpoint(tcp::v4(), 9001)) {
    startAccept();
  }

  const std::vector<std::shared_ptr<TcpConnection>> &getAllClients() {
    return m_connectedClients;
  }

private:
  void startAccept() {
    TcpConnection::Sptr newConnection{TcpConnection::create(m_IO, this)};

    m_acceptor.async_accept(newConnection->socket(),
                            std::bind(&TcpServer::handleAccept, this,
                                      newConnection,
                                      boost::asio::placeholders::error));
  }

  void handleAccept(TcpConnection::Sptr newConnection,
                    const boost::system::error_code &ec) {
    if (!ec) {
      newConnection->start();
      m_connectedClients.push_back(newConnection);
    }
    startAccept();
  }
};
