
#include "unity_fixture.h"
#include "led_driver.h"

TEST_GROUP(led_driver);

TEST_SETUP(led_driver) {
}

TEST_TEAR_DOWN(led_driver) {
}


TEST(led_driver, first_test) {
  TEST_ASSERT_EQUAL_HEX16(0, 0);
}


TEST_GROUP_RUNNER(led_driver) {
  RUN_TEST_CASE(led_driver, first_test);
}
