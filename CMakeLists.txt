cmake_minimum_required(VERSION 3.9)
project(brainfuck_interpreter)

set(CMAKE_CXX_STANDARD 17)

# Set the default build type to Release if none is provided
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()


set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")


add_executable(brainfuck_interpreter main.cpp)