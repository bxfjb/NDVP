cmake_minimum_required(VERSION 3.10)
project(NetworkSystem)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_executable(NetworkSystem main.cpp ndvp/ndvp.cpp ndvp/util.cpp)

target_link_libraries(NetworkSystem PRIVATE Threads::Threads)
