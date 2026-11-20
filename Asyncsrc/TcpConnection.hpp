#pragma once
#include "messageHeader.hpp"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using namespace std::placeholders;

class TcpServer;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
private:
  tcp::socket m_socket;
  // MessageHeader m_header;
  // std::array<unsigned char, 250> m_data;
  boost::system::error_code m_ec;
  TcpServer *m_server;

  TcpConnection(boost::asio::io_context &io_context, TcpServer *server)
      : m_socket(io_context), m_server(server) {}

public:
  using Sptr = std::shared_ptr<TcpConnection>;

  static Sptr create(boost::asio::io_context &io_context, TcpServer *server);

  tcp::socket &socket();

  void start(); // Writing messages back to our clients

  using sPtrVectorUC = std::shared_ptr<std::vector<unsigned char>>;
  void handleReadMessageHeader(sPtrVectorUC readBuffer,
                               const boost::system::error_code &ec,
                               std::size_t bytesTransfered);

  void handleMsgFromClient(sPtrVectorUC readBuffer, MessageHeader &header);

  void handleReqMessage(sPtrVectorUC readBuffer, MessageHeader &header,
                        const boost::system::error_code &ec,
                        std::size_t bytesTransfered);

  // void broadCastMessage();

  void handleWrite(const boost::system::error_code &ec,
                   std::size_t bytesTransfered);
};

using Sptr = std::shared_ptr<TcpConnection>;
