#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <cstddef>
#include <iostream>

using boost::asio::ip::udp;

std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

std::string ask_for_password() { return "PassWord Plz: "; }

int main() {
  try {
    boost::asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), 9001));
    std::string password{"123"};

    while (true) {
      std::array<char, 1> recv_buf;
      std::array<char, 20> recv_pass_buf;
      boost::asio::streambuf sb;
      udp::endpoint remote_endpoint;

      socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);
      std::string message = ask_for_password();

      boost::system::error_code ignored_error;
      socket.send_to(boost::asio::buffer(message), remote_endpoint, 0,
                     ignored_error);

      size_t len = socket.receive_from(boost::asio::buffer(recv_pass_buf),
                                       remote_endpoint);

      boost::asio::read_until(socket, sb, '\n');
      std::string recv_password;
      std::istream is(&sb);
      is >> recv_password;

      if (password == recv_password) {
        std::cout << "You have successfully login\n";
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
