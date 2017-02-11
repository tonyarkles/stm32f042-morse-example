
set(SRCS
  led_driver.c)

add_library (bare_example SHARED ${SRCS} )

install (TARGETS bare_example DESTINATION lib)
