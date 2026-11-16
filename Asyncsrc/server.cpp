#include "TcpConnection.hpp"
#include "TcpServer.hpp"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>

void checkError(
    const boost::system::error_code &ec,
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

int main() {

  try {
    boost::asio::io_context io_context;
    TcpServer asyncTCP(io_context);

    io_context.run();

  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
