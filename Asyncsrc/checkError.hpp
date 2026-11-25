#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <source_location>
inline void
checkError(const boost::system::error_code &ec,
           std::source_location location = std::source_location::current()) {
  if (ec == boost::asio::error::eof) {
    std::cout << "\nfile: " << location.file_name() << '(' << location.line()
              << ':' << location.column() << ") `" << location.function_name()
              << std::endl;
    throw boost::system::system_error(ec);
  } else if (ec) {
    std::cout << "\nfile: " << location.file_name() << '(' << location.line()
              << ':' << location.column() << ") `" << location.function_name()
              << std::endl;
    throw boost::system::system_error(ec);
  }
}

template <typename EOFHandler>
inline void
checkError(EOFHandler handler, const boost::system::error_code &ec,
           std::source_location location = std::source_location::current()) {
  if (ec == boost::asio::error::eof) {
    handler();
  } else if (ec) {
    std::cout << "\nfile: " << location.file_name() << '(' << location.line()
              << ':' << location.column() << ") `" << location.function_name()
              << std::endl;
    throw boost::system::system_error(ec);
  }
}
