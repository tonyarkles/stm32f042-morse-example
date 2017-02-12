
cmake_minimum_required(VERSION 2.8)

project(bare_example C ASM)

enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/inc")
add_subdirectory(src)
if(!CMAKE_CROSSCOMPILING)
  add_subdirectory(test)
endif(!CMAKE_CROSSCOMPILING)
if(CMAKE_CROSSCOMPILING)
  add_subdirectory(target)
endif(CMAKE_CROSSCOMPILING)
