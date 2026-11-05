#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <iostream>
#include <print>
#include <string>

int main(int argc, char **argv) {

  sf::UdpSocket udp;
  if (udp.bind(6000) != sf::Socket::Status::Done) {
    std::print("Could not create udp socket\n");
    return -1;
  }
  sf::IpAddress server{sf::IpAddress::LocalHost};
  unsigned short port{7333};

  while (true) {
    std::string msg;
    std::print("Message for server: ");
    std::getline(std::cin, msg);

    if (udp.send(msg.c_str(), msg.size() + 1, server, port) !=
        sf::Socket::Status::Done) {
      std::print("Failed to send to UDP server\n");
      return -1;
    }
  }

  return 0;
}
