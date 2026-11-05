#include <SFML/Network.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <optional>
#include <print>

int main(int argc, char **argv) {

  char buffer[100];
  std::size_t received;
  std::string msg;

  sf::UdpSocket socket;
  if (socket.bind(7333) != sf::Socket::Status::Done) {
    std::print("Could not create udp socket\n");
    return -1;
  }

  auto sender = std::make_optional<sf::IpAddress>(127, 0, 0, 1);
  unsigned short port;
  while (true) {
    if (socket.receive(buffer, 100u, received, sender, port) !=
        sf::Socket::Status::Done) {
      std::print("Failed to recieve message on server\n");
      return -1;
    }
    msg = std::string(buffer);
    std::cout << "Received " << msg << " from " << sender.value() << " on port "
              << port << std::endl;
  }

  return 0;
}
