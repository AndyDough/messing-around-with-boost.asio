#include "TcpConnection.hpp"
#include "TcpServer.hpp"
#include "checkError.hpp"
#include "encode.hpp"
#include "messageHeader.hpp"
#include <boost/asio/completion_condition.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>

Sptr TcpConnection::create(boost::asio::io_context &io_context,
                           TcpServer *server) {
  return Sptr(new TcpConnection(io_context, server));
}

tcp::socket &TcpConnection::socket() { return m_socket; }

void TcpConnection::start() {
  auto readBuffer = std::make_shared<std::vector<unsigned char>>();
  readBuffer->resize(8);

  std::cout << readBuffer.get() << std::endl;
  boost::asio::async_read(m_socket, boost::asio::buffer(*readBuffer),
                          boost::asio::transfer_exactly(8),
                          [self = shared_from_this(), buffer = readBuffer](
                              const boost::system::error_code &ec,
                              std::size_t bytesTransfered) mutable {
                            std::cout << buffer.get() << std::endl;
                            self->handleReadMessageHeader(buffer, ec,
                                                          bytesTransfered);
                          });
}

void TcpConnection::handleReadMessageHeader(sPtrVectorUC readBuffer,
                                            const boost::system::error_code &ec,
                                            std::size_t bytesTransfered) {
  checkError(ec);
  std::cout << readBuffer.get() << std::endl;
  // m_data: first 4 bytes length, next 4 bytes id, the rest datos
  MessageHeader header;
  header = deserialize(*readBuffer);

  switch (header.m_id) {
  case 1:
    handleMsgFromClient(readBuffer, header);
    break;
  case 3:
    handleDisconnectionFromClient(readBuffer, header);
    break;
  }
}

void TcpConnection::handleDisconnectionFromClient(sPtrVectorUC readBuffer,
                                                  MessageHeader &header) {
  auto sPtr = shared_from_this();
  m_server->handleDisconnect(sPtr);
}

void TcpConnection::handleMsgFromClient(sPtrVectorUC readBuffer,
                                        MessageHeader &header) {
  readBuffer->resize(header.m_length); // m_length sizeof(content + header)
  // std::vector<unsigned char> *vecBuff{readBuffer.get()};
  // std::vector<unsigned char> &vecBuffRef = *vecBuff;
  // unsigned char &the8thByte = vecBuffRef[8];
  // unsigned char *the8thBytePtr = &the8thByte;

  boost::asio::async_read(
      m_socket, boost::asio::buffer(&((*readBuffer)[8]), header.m_length - 8),
      boost::asio::transfer_exactly(header.m_length - 8),
      [self = shared_from_this(), buffer = readBuffer,
       head = header](const boost::system::error_code &ec,
                      std::size_t bytesTransfered) mutable {
        self->handleReqMessage(buffer, head, ec, bytesTransfered);
      });
}

void TcpConnection::handleReqMessage(sPtrVectorUC readBuffer,
                                     MessageHeader &header,
                                     const boost::system::error_code &ec,
                                     std::size_t bytesTransfered) {
  checkError(ec);
  std::cout << "Recieved this message from Client\n";
  for (auto e : *readBuffer) {
    std::cout << std::hex << e << ' ';
  }
  std::cout << std::endl;

  serialize(*readBuffer, header);
  // BroadCast to all connected clients
  m_server->getAllClients();
  for (const auto &e : m_server->getAllClients()) {
    if (this == e.get()) {
      continue;
    }
    boost::asio::async_write(
        e->socket(),
        boost::asio::const_buffer(readBuffer->data(), header.m_length),
        boost::asio::transfer_exactly(header.m_length),
        [self = shared_from_this(), buffer = readBuffer](
            const boost::system::error_code &ec, std::size_t bytesTransfered) {
          self->handleWrite(buffer, ec, bytesTransfered);
          std::cout
              << "Buffer that is being broadcasted to all connected Clients:";
          for (auto e : *buffer) {
            std::cout << std::hex << e << ' ';
          }
          std::cout << std::endl;
        });
  }

  start();
}

// void TcpConnection::broadCastMessage() {
//   for (const auto &e : m_server->getAllClients()) {
//     if (this == e.get()) {
//       continue;
//     }
//     boost::asio::async_write(
//         e->socket(),
//         boost::asio::const_buffer(&m_data[8], m_header.m_length - 8),
//         boost::asio::transfer_exactly(m_header.m_length),
//         std::bind(&TcpConnection::handleWrite, shared_from_this()));
//   }
// }

void TcpConnection::handleWrite(sPtrVectorUC readBuffer,
                                const boost::system::error_code &ec,
                                std::size_t bytesTransfered) {
  checkError(ec);
  std::cout << "Transfering from server to clients:" << std::dec
            << bytesTransfered << std::endl;
}
