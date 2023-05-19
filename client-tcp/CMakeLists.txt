cmake_minimum_required(VERSION 2.6)
PROJECT(client-tcp)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT /usr/include/boost)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system thread log_setup log REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
add_executable(client-tcp main.cpp ../log.cpp)
target_link_libraries(client-tcp ${Boost_LIBRARIES} pthread)
