
set(SRCS
  startup_stm32f0xx.s
  main.c)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/target/linker/stm32f042.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT}")

add_executable(target.elf ${SRCS})
target_link_libraries(target.elf bare_example)
