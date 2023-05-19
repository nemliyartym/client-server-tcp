cmake_minimum_required(VERSION 2.6)
PROJECT(server-tcp)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT /usr/include/boost)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system thread log_setup log REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
add_executable(server-tcp main.cpp client_handler.cpp ../log.cpp)
target_link_libraries(server-tcp ${Boost_LIBRARIES})
