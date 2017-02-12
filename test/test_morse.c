
#include "unity_fixture.h"
#include <morse.h>

TEST_GROUP(morse);

static uint32_t done_count = 0;

static void dummy_done(void) {
  done_count++;
}

TEST_SETUP(morse) {
  morse_reset();
  morse_output_callback(dummy_done);
}

TEST_TEAR_DOWN(morse) {
}


TEST(morse, output_engine_sets_state_for_count) {
  morse_output_set(1, 3); /* output 1 for 3 ticks */
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  TEST_ASSERT_EQUAL(1, done_count);
}

TEST(morse, output_engine_toggles_state) {
  morse_output_set(1, 1);
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());

  morse_output_set(0, 1);
  morse_output_tick();
  TEST_ASSERT_EQUAL(0, morse_output_get());

  TEST_ASSERT_EQUAL(2, done_count);
}

TEST_GROUP_RUNNER(morse) {
  RUN_TEST_CASE(morse, output_engine_sets_state_for_count);
}
