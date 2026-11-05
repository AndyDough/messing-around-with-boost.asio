#include <array>
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <exception>
#include <iostream>

using boost::asio::ip::udp;

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: client <host>\n";
      return 1;
    }
    boost::asio::io_context io_context;

    udp::resolver resolver(io_context);
    udp::endpoint reciever_endpoint =
        *resolver.resolve(udp::v4(), argv[1], "9001").begin();

    udp::socket socket(io_context);
    socket.open(udp::v4());

    std::array<char, 1> send_buf = {{0}};
    socket.send_to(boost::asio::buffer(send_buf), reciever_endpoint);

    while (true) {
      std::array<char, 128> revc_buf;
      udp::endpoint sender_endpoint;
      size_t len =
          socket.receive_from(boost::asio::buffer(revc_buf), sender_endpoint);

      std::cout.write(revc_buf.data(), len);

      std::string password;
      std::cin >> password;
      password.append("\n");

      socket.send_to(boost::asio::buffer(password.c_str(), password.size()),
                     reciever_endpoint);
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
