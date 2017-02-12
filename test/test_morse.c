
#include "unity_fixture.h"
#include <morse.h>

TEST_GROUP(morse);

static uint32_t done_count = 0;

static void dummy_done(void) {
  done_count++;
}

TEST_SETUP(morse) {
  done_count = 0;
  
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

  TEST_ASSERT_EQUAL(0, done_count);
  
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  TEST_ASSERT_EQUAL(1, done_count);

  morse_output_set(0, 1);
  morse_output_tick();
  TEST_ASSERT_EQUAL(0, morse_output_get());
  TEST_ASSERT_EQUAL(2, done_count);
}

TEST(morse, letter_converter_produces_correct_output_for_A) {
  uint8_t output;
  uint8_t count;

  morse_letter_set('A');

  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(1, output);
  TEST_ASSERT_EQUAL(1, count);

  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(0, output);
  TEST_ASSERT_EQUAL(1, count);

  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(1, output);
  TEST_ASSERT_EQUAL(3, count);

  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(0, output);
  TEST_ASSERT_EQUAL(3, count);
}

TEST(morse, letter_converter_produces_correct_output_for_E) {
  uint8_t output;
  uint8_t count;

  morse_letter_set('E');

  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(1, output);
  TEST_ASSERT_EQUAL(1, count);

  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(0, output);
  TEST_ASSERT_EQUAL(3, count);  
}

TEST(morse, stream_output) {
  morse_stream_set("FOO BAR BAZ");

  TEST_ASSERT_EQUAL('F', morse_stream_get());
  TEST_ASSERT_EQUAL('O', morse_stream_get());
  TEST_ASSERT_EQUAL('O', morse_stream_get());
  TEST_ASSERT_EQUAL(' ', morse_stream_get());
  TEST_ASSERT_EQUAL('B', morse_stream_get());
  TEST_ASSERT_EQUAL('A', morse_stream_get());
  TEST_ASSERT_EQUAL('R', morse_stream_get());
  TEST_ASSERT_EQUAL(' ', morse_stream_get());
  TEST_ASSERT_EQUAL('B', morse_stream_get());
  TEST_ASSERT_EQUAL('A', morse_stream_get());
  TEST_ASSERT_EQUAL('Z', morse_stream_get());
  TEST_ASSERT_EQUAL('\0', morse_stream_get());
}

TEST(morse, stream_letter_glued_together) {
  morse_letter_callback(morse_stream_letter_glue);
  morse_stream_set("AET");

  uint8_t output;
  uint8_t count;
  
  /* A = .-, E = ., T = - */

  /* A */
  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(1, output);
  TEST_ASSERT_EQUAL(1, count);
  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(0, output);
  TEST_ASSERT_EQUAL(1, count);
  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(1, output);
  TEST_ASSERT_EQUAL(3, count);

  /* inter-letter space */
  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(0, output);
  TEST_ASSERT_EQUAL(3, count);

  /* E */
  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(1, output);
  TEST_ASSERT_EQUAL(1, count);

  /* inter-letter space */
  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(0, output);
  TEST_ASSERT_EQUAL(3, count);

  /* T */
  morse_letter_get_next_output(&output, &count);
  TEST_ASSERT_EQUAL(1, output);
  TEST_ASSERT_EQUAL(3, count);  
}

TEST(morse, letter_and_output_glued_together) {
  morse_output_callback(morse_letter_output_glue);
  morse_letter_set('X');

  /* X = -..- = 3 on, 1 off, 1 on, 1 off, 1 on, 1 off, 3 on */
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());

  morse_output_tick();
  TEST_ASSERT_EQUAL(0, morse_output_get());
  
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  
  morse_output_tick();
  TEST_ASSERT_EQUAL(0, morse_output_get());
  
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  
  morse_output_tick();
  TEST_ASSERT_EQUAL(0, morse_output_get());
  
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
  morse_output_tick();
  TEST_ASSERT_EQUAL(1, morse_output_get());
}

TEST(morse, all_tied_together) {
  morse_output_callback(morse_letter_output_glue);
  morse_letter_callback(morse_stream_letter_glue);
  morse_stream_set("HELLO");

  uint8_t expected_output = 1;
  /* these will toggle every time - S = ... = 1 (on), 1 (off), 1 (on), 1 (off), 1 (on), 3 (off) */
  uint8_t expected_durations[] = { 1, 1, 1, 1, 1, 1, 1, 3, // H
				   1, 3, // E
				   1, 1, 3, 1, 1, 1, 1, 3, // L
				   1, 1, 3, 1, 1, 1, 1, 3, // L
				   3, 1, 3, 1, 3, 3, // O
				   0 };
  for(int i = 0; expected_durations[i] != 0; i++) {
    for(int j = 0; j < expected_durations[i]; j++) {
      morse_output_tick();
      TEST_ASSERT_EQUAL(expected_output, morse_output_get());
    }
    expected_output = !expected_output;
  }
}

TEST_GROUP_RUNNER(morse) {
  RUN_TEST_CASE(morse, output_engine_sets_state_for_count);
  RUN_TEST_CASE(morse, output_engine_toggles_state);
  RUN_TEST_CASE(morse, letter_converter_produces_correct_output_for_A);
  RUN_TEST_CASE(morse, letter_converter_produces_correct_output_for_E);
  RUN_TEST_CASE(morse, stream_output);
  RUN_TEST_CASE(morse, stream_letter_glued_together);
  RUN_TEST_CASE(morse, letter_and_output_glued_together);
  RUN_TEST_CASE(morse, all_tied_together);
}
