#pragma once

#include <cstdint>
struct MessageHeader {
  std::uint32_t m_length;
  std::uint32_t m_id;
};
