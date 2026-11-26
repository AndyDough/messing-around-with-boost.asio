#include "TcpClient.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <exception>
#include <functional>
#include <iostream>
#include <thread>

void uiThread(TcpClient &client) {
  std::string msg;

  while (true) {
    std::cout << "msg (enter 0 to exit): ";
    std::getline(std::cin, msg);
    if (client.socket().is_open()) {
      if (msg == "0") {
        client.disconnect();
        break;
      } else {
        client.startWrite(msg, 1);
      }
    } else {
      std::cout << "Still Trying to connect to server\n";
    }
  }
}

bool runTest(tgui::BackendGui &gui) { return true; }

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: client <host ip>" << '\n';
      return 1;
    }
    std::string_view hostIP{argv[1]};
    boost::asio::io_context io_context;

    sf::RenderWindow window{sf::VideoMode{{800, 600}}, "Test Window"};

    tgui::Gui gui{window};
    if (runTest(gui)) {
      gui.mainLoop();
    }

    // std::cout << "Before TcpClient is contructed\n";
    auto tcpClient = TcpClient::create(io_context, hostIP);

    std::jthread uiBackground(uiThread, std::ref(*tcpClient.get()));

    auto work_guard = boost::asio::make_work_guard(io_context);
    io_context.run();
    uiBackground.join();

  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
