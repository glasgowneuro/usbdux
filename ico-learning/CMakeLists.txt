cmake_minimum_required(VERSION 3.1.0)

project(icorobot VERSION 1.0.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

include(GNUInstallDirs)

add_executable (main main.cpp)
target_link_libraries(main icolearning comedi ncurses)
