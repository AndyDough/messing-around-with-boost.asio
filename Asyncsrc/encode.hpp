#pragma once
#include "messageHeader.hpp"
#include <cstring>

template <typename T>
void serialize(T &dataArray, const MessageHeader &message) {
  std::memcpy(&dataArray[0], &message, sizeof(MessageHeader));
}

template <typename T> MessageHeader deserialize(T &serializedMessage) {
  MessageHeader msg;
  std::memcpy(&msg, &serializedMessage[0], sizeof(MessageHeader));

  return msg;
}
