cmake_minimum_required(VERSION 3.10.0)
set (CMAKE_CXX_STANDARD 11)
project(cppUSBDUXexample)
set(CMAKE_BUILD_TYPE "Debug")

add_executable (print2screen_demo print2screen_demo.cpp)

target_link_libraries(print2screen_demo comedi)
