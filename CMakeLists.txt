cmake_minimum_required(VERSION 3.16)
project(networkingTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find_package(SFML 3 COMPONENTS Network REQUIRED)
#
# add_executable(SERVER src/server.cpp)
# target_link_libraries(SERVER PRIVATE SFML::Network)
#
# add_executable(CLIENT src/client.cpp)
# target_link_libraries(CLIENT PRIVATE SFML::Network)

find_package(Boost REQUIRED COMPONENTS asio)
find_package(Boost REQUIRED COMPONENTS system)

add_executable(BOOST_CLIENT src/client.cpp)
target_link_libraries(BOOST_CLIENT PRIVATE Boost::asio Boost::system)

add_executable(BOOST_SERVER src/server.cpp)
target_link_libraries(BOOST_SERVER PRIVATE Boost::asio Boost::system)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)
