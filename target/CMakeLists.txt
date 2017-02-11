
set(SRCS
  main.c)

add_executable(target.elf ${SRCS})
target_link_libraries(target.elf bare_example)
