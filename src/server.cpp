#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>

using boost::asio::ip::tcp;

void checkError(boost::system::error_code &ec) {
  if (ec) {
    std::cout << ec.what() << '\n';
  }
}

int main() {
  try {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor{io_context, tcp::endpoint(tcp::v4(), 9001)};
    std::string serverPassword{"Ilovenetworking:)"};

    while (true) {
      tcp::socket socket{io_context};
      acceptor.accept(socket);
      int passAttemps{3};

      std::string message{"Please provide us a password: \r\n"};
      boost::system::error_code ec;

      boost::asio::write(socket, boost::asio::buffer(message), ec);

      while (true) {
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, '\n');

        std::string sResponse;
        std::istream is(&response);
        is >> sResponse;
        if (passAttemps > 0) {
          if (sResponse != serverPassword) {
            std::string WPmessage;
            std::format_to(std::back_inserter(WPmessage),
                           "Wrong Password, attempt(s) remaining: {}\n",
                           passAttemps);
            WPmessage.append(message);
            boost::asio::write(socket, boost::asio::buffer(WPmessage), ec);
            checkError(ec);
            --passAttemps;
          } else {
            boost::asio::write(socket, boost::asio::buffer("I'm in\r\n"), ec);
            checkError(ec);
            socket.shutdown(boost::asio::socket_base::shutdown_both);
            socket.close();
            break;
          }
        } else {
          boost::asio::write(
              socket, boost::asio::buffer("No more attemps bye bye\r\n"), ec);
          checkError(ec);
          socket.shutdown(boost::asio::socket_base::shutdown_both);
          socket.close();
          break;
        }
      }
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
