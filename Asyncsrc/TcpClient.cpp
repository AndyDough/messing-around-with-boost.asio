#include "TcpClient.hpp"
#include "checkError.hpp"
#include "encode.hpp"
#include <netinet/tcp.h>

void TcpClient::start() {
  m_resolver.async_resolve(m_hostIP, "9001",
                           std::bind(&TcpClient::onResolve, shared_from_this(),
                                     std::placeholders::_1,
                                     std::placeholders::_2));
}

std::shared_ptr<TcpClient>
TcpClient::create(boost::asio::io_context &io_context,
                  std::string_view hostIP) {
  auto tcpClient =
      std::shared_ptr<TcpClient>(new TcpClient(io_context, hostIP));

  tcpClient->start();
  // tcpClient->startWrite("Hey I'm connected");

  return tcpClient;
}

void TcpClient::disconnect() {
  std::string_view goodByeMsg{"bye"};
  startWrite(goodByeMsg, 3);
}

void TcpClient::startWrite(std::string_view msg, int messageID) {
  // std::cout << "Start Write is being called\n";
  auto writtingBuffer =
      std::make_shared<std::vector<unsigned char>>(8 + msg.size());

  MessageHeader m_header;
  m_header.m_id = messageID;
  m_header.m_length = 8 + msg.size();

  serialize(*writtingBuffer, m_header);
  std::memcpy(&(*writtingBuffer)[8], msg.data(), msg.size());

  boost::asio::dispatch(m_IO, [self = shared_from_this(), writtingBuffer,
                               m_header]() {
    boost::asio::async_write(
        self->socket(), boost::asio::buffer(*writtingBuffer),
        [iself = self, buffer = writtingBuffer, header = m_header](
            const boost::system::error_code &ec, std::size_t bytesTransfered) {
          iself->handleWrite(ec, bytesTransfered, header);
        });
  });
}

void TcpClient::handleWrite(const boost::system::error_code &ec,
                            std::size_t bytesTransfered, MessageHeader header) {
  // std::cout << "Starting (handleWrite)\n";
  checkError(ec);
  if (header.m_id == 3) {
    m_IO.stop();
  }
}

tcp::socket &TcpClient::socket() { return m_socket; }

void TcpClient::onResolve(const boost::system::error_code &ec,
                          tcp::resolver::results_type endpointIterator) {
  checkError(ec);
  boost::asio::async_connect(
      m_socket, endpointIterator,
      std::bind(&TcpClient::onConnect, shared_from_this(),
                std::placeholders::_1, std::placeholders::_2));
}

void TcpClient::onConnect(const boost::system::error_code &ec,
                          const boost::asio::ip::tcp::endpoint &endpoint) {
  checkError(ec);
  readMessageHeader();
}

void TcpClient::readMessageHeader() {

  auto readBuffer = std::make_shared<std::vector<unsigned char>>();
  readBuffer->resize(8);

  boost::asio::async_read(
      m_socket, boost::asio::buffer(*readBuffer),
      boost::asio::transfer_exactly(8),
      [self = shared_from_this(), buffer = readBuffer](
          const boost::system::error_code ec, std::size_t bytesTransfered) {
        self->handleRecieveMessage(buffer, ec, bytesTransfered);
      });
}

void TcpClient::handleRecieveMessage(sPtrVectorUC readBuffer,
                                     const boost::system::error_code &ec,
                                     std::size_t bytesTransfered) {
  checkError(ec);
  MessageHeader header{deserialize(*readBuffer)};
  readBuffer->resize(header.m_length);

  boost::asio::async_read(
      m_socket, boost::asio::buffer(&((*readBuffer)[8]), header.m_length - 8),
      boost::asio::transfer_exactly(header.m_length - 8),
      [self = shared_from_this(), buffer = readBuffer](
          const boost::system::error_code &ec, std::size_t bytesTransfered) {
        checkError(ec);

        std::cout << "Recieved this message from Client\n";
        for (auto e : *buffer) {
          std::cout << std::hex << e << ' ';
        }
        std::cout << std::endl;
      });

  readMessageHeader();
}
