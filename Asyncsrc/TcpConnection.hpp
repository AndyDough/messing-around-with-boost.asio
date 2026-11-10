#pragma once
#include "TcpServer.hpp"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using namespace std::placeholders;
struct MessageHeader {
  std::uint32_t m_length;
  std::uint32_t m_id;
};

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
private:
  tcp::socket m_socket;
  MessageHeader m_header;
  std::array<unsigned char, 250> m_data;
  boost::system::error_code m_ec;
  TcpServer *m_server;

  TcpConnection(boost::asio::io_context &io_context, TcpServer *server)
      : m_socket(io_context), m_server(server) {}

public:
  using Sptr = std::shared_ptr<TcpConnection>;

  static Sptr create(boost::asio::io_context &io_context, TcpServer *server) {
    return Sptr(new TcpConnection(io_context, server));
  }

  tcp::socket &socket() { return m_socket; }

  void start() {
    boost::asio::async_read(
        m_socket, boost::asio::buffer(m_data), boost::asio::transfer_exactly(8),
        std::bind(&TcpConnection::handleReadMessageHeader, this, _1, _2));

    // boost::asio::async_write(
    //     m_socket, boost::asio::buffer(m_messages),
    //     [this](const boost::system::error_code &ec, size_t) {});
  } // Writing messages back to our clients

  void handleReadMessageHeader(const boost::system::error_code &ec,
                               std::size_t bytesTransfered) {
    // m_data: first 4 bytes length, next 4 bytes id, the rest datos
    uint32_t *messageLength{new (&m_data[0]) uint32_t};
    uint32_t *messageID{new (&m_data[4]) uint32_t};
    m_header.m_length = *messageLength;
    m_header.m_id = *messageID;
    switch (m_header.m_id) {
    case 1:
      handleHelloFromClient();
    case 2:
      broadCastMessage();
    }
  }

  void handleHelloFromClient() {
    boost::asio::async_read(
        m_socket, boost::asio::buffer(m_data),
        boost::asio::transfer_exactly(m_header.m_length - 8),
        std::bind(&TcpConnection::handleReqMessage, this, _1, _2));
  }

  void handleReqMessage(const boost::system::error_code &ec,
                        std::size_t bytesTransfered) {
    m_server->getAllClients();
  }

  void broadCastMessage() {
    for (const auto &e : m_server->getAllClients()) {
    }
  }
};
