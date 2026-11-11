#pragma once
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using namespace std::placeholders;
struct MessageHeader {
  std::uint32_t m_length;
  std::uint32_t m_id;
};

class TcpServer;

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

  static Sptr create(boost::asio::io_context &io_context, TcpServer *server);

  tcp::socket &socket();

  void start(); // Writing messages back to our clients

  void handleReadMessageHeader(const boost::system::error_code &ec,
                               std::size_t bytesTransfered);

  void handleHelloFromClient();

  void handleReqMessage(const boost::system::error_code &ec,
                        std::size_t bytesTransfered);

  void broadCastMessage();

  void handleWrite();
};

using Sptr = std::shared_ptr<TcpConnection>;
