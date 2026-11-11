#pragma once
#include "messageHeader.hpp"
#include <array>

void serialize(std::array<unsigned char, 250> &dataArray,
               const MessageHeader &message);

MessageHeader deserialize(std::array<unsigned char, 250> &serializedMessage);
