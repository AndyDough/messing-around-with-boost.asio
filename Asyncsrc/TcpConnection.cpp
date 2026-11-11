#include "TcpConnection.hpp"
#include "TcpServer.hpp"
#include "encode.hpp"
#include <boost/asio/completion_condition.hpp>
#include <iostream>

Sptr TcpConnection::create(boost::asio::io_context &io_context,
                           TcpServer *server) {
  return Sptr(new TcpConnection(io_context, server));
}

tcp::socket &TcpConnection::socket() { return m_socket; }

void TcpConnection::start() {
  boost::asio::async_read(
      m_socket, boost::asio::buffer(m_data), boost::asio::transfer_exactly(8),
      std::bind(&TcpConnection::handleReadMessageHeader, this, _1, _2));

  // boost::asio::async_write(
  //     m_socket, boost::asio::buffer(m_messages),
  //     [this](const boost::system::error_code &ec, size_t) {});
} // Writing messages back to our clients

void TcpConnection::handleReadMessageHeader(const boost::system::error_code &ec,
                                            std::size_t bytesTransfered) {
  // m_data: first 4 bytes length, next 4 bytes id, the rest datos
  m_header = deserialize(m_data);

  switch (m_header.m_id) {
  case 1:
    handleHelloFromClient();
  case 2:
    broadCastMessage();
  }
}

void TcpConnection::handleHelloFromClient() {
  boost::asio::async_read(
      m_socket, boost::asio::buffer(m_data),
      boost::asio::transfer_exactly(m_header.m_length - 8),
      std::bind(&TcpConnection::handleReqMessage, this, _1, _2));
}

void TcpConnection::handleReqMessage(const boost::system::error_code &ec,
                                     std::size_t bytesTransfered) {

  std::cout << "Recieved this message from Client\n";
  for (auto e : m_data) {
    std::cout << std::hex << static_cast<int>(e) << ' ';
  }
  std::cout << std::endl;

  m_server->getAllClients();
  for (const auto &e : m_server->getAllClients()) {
    if (this == e.get()) {
      continue;
    }
    boost::asio::async_write(
        e->socket(),
        boost::asio::const_buffer(&m_data[8], m_header.m_length - 8),
        boost::asio::transfer_exactly(m_header.m_length),
        std::bind(&TcpConnection::handleWrite, shared_from_this()));
  }
}

void TcpConnection::broadCastMessage() {
  for (const auto &e : m_server->getAllClients()) {
    if (this == e.get()) {
      continue;
    }
    boost::asio::async_write(
        e->socket(),
        boost::asio::const_buffer(&m_data[8], m_header.m_length - 8),
        boost::asio::transfer_exactly(m_header.m_length),
        std::bind(&TcpConnection::handleWrite, shared_from_this()));
  }
}

void TcpConnection::handleWrite() {}
