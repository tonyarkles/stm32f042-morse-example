
cmake_minimum_required(VERSION 2.8)

project(bare_example)

option(WITH_TARGET "with(out) target build")

if(WITH_TARGET)
  add_definitions(-DBUILD_FOR_TARGET)
endif(WITH_TARGET)

enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/inc")
add_subdirectory(src)
add_subdirectory(test)
if(WITH_TARGET)
  add_subdirectory(target)
endif(WITH_TARGET)
