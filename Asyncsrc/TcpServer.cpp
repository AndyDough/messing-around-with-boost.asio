#pragma once
#include "TcpConnection.hpp"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

const std::vector<std::shared_ptr<TcpConnection>> &TcpServer::getAllClients() {
  return m_connectedClients;
}

void TcpServer::startAccept() {
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
}
;
