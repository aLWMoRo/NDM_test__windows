cmake_minimum_required(VERSION 3.31)
project(NDM_test)

set(CMAKE_CXX_STANDARD 20)

add_executable(NDM_test main.cpp)

target_link_libraries(NDM_test ws2_32 iphlpapi)