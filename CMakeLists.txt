
cmake_minimum_required(VERSION 2.8)

project(dumb_example)

enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/inc")
add_subdirectory(src)
add_subdirectory(test)
