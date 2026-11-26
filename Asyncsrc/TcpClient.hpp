#pragma once
#include "messageHeader.hpp"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/impl/read_until.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>

using boost::asio::ip::tcp;

class TcpClient : public std::enable_shared_from_this<TcpClient> {
private:
  boost::asio::io_context &m_IO;
  std::string_view m_hostIP;
  tcp::socket m_socket;
  boost::system::error_code ec;
  tcp::resolver m_resolver;

  TcpClient(boost::asio::io_context &io_context, std::string_view hostIP)
      : m_IO(io_context), m_hostIP{hostIP}, m_socket{io_context},
        m_resolver(io_context) {}

  void start();

public:
  static std::shared_ptr<TcpClient> create(boost::asio::io_context &io_context,
                                           std::string_view hostIP);

  void disconnect();

  void startWrite(std::string_view msg, int messageID);

  void handleWrite(const boost::system::error_code &ec,
                   std::size_t bytesTransfered, MessageHeader header);

  tcp::socket &socket();

private:
  void onResolve(const boost::system::error_code &ec,
                 tcp::resolver::results_type endpointIterator);

  using sPtrVectorUC = std::shared_ptr<std::vector<unsigned char>>;
  void onConnect(const boost::system::error_code &ec,
                 const boost::asio::ip::tcp::endpoint &endpoint);

  void readMessageHeader();

  void handleRecieveMessage(sPtrVectorUC readBuffer,
                            const boost::system::error_code &ec,
                            std::size_t bytesTransfered);
};
