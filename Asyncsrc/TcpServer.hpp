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

  const std::vector<std::shared_ptr<TcpConnection>> &getAllClients();

private:
  void startAccept();

  void handleAccept(TcpConnection::Sptr newConnection,
                    const boost::system::error_code &ec);
};
