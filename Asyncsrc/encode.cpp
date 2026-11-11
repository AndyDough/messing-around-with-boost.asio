#include "encode.hpp"
#include <cstring>

void serialize(std::array<unsigned char, 250> &dataArray,
               const MessageHeader &message) {
  std::memcpy(&dataArray[0], &message, sizeof(MessageHeader));
}

MessageHeader deserialize(std::array<unsigned char, 250> &serializedMessage) {
  MessageHeader msg;
  std::memcpy(&msg, &serializedMessage[0], sizeof(MessageHeader));

  return msg;
}
