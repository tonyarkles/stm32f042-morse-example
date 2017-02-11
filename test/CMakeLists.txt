
set(UNITY_SRCS
  "${CMAKE_SOURCE_DIR}/external/unity/unity.c")

set(UNITY_FIXTURE_SRCS
  ${UNITY_SRCS}
  "${CMAKE_SOURCE_DIR}/external/unity/unity_fixture.c")

set(TEST_RUNNER_SRCS
  test_led_driver.c
  testrunner.c)

include_directories("${CMAKE_SOURCE_DIR}/external/unity/")

add_executable(testrunner ${TEST_RUNNER_SRCS} ${UNITY_FIXTURE_SRCS})
target_link_libraries(testrunner bare_example)
add_test(testrunner testrunner "-v")
