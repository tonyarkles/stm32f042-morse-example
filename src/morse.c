
#include <morse.h>
#include <stddef.h>

/* To keep each individual piece of functionality simple and
   independently testable, this module is broken up into three
   submodules:

   - Output Engine 
     - this takes (output state, count) as input
     - receives tick notifications
     - notifies the caller when done
   - Letter Converter
     - this takes a single letter as input
     - converts it into a series of on-off states
     - notifies the caller when done
   - Stream processor
     - this takes a string as input
     - and gives it to the letter converter one character at a time

   All functionality is to be implemented in a call-to-completion
   fashion.
*/

static const char *alpha[] = {
  ".-",   //A
  "-...", //B
  "-.-.", //C
  "-..",  //D
  ".",    //E
  "..-.", //F
  "--.",  //G
  "....", //H
  "..",   //I
  ".---", //J
  "-.-",  //K
  ".-..", //L
  "--",   //M
  "-.",   //N
  "---",  //O
  ".--.", //P
  "--.-", //Q
  ".-.",  //R
  "...",  //S
  "-",    //T
  "..-",  //U
  "...-", //V
  ".--",  //W
  "-..-", //X
  "-.--", //Y
  "--..", //Z
  "-----", //0
  ".----", //1
  "..---", //2
  "...--", //3
  "....-", //4
  ".....", //5
  "-....", //6
  "--...", //7
  "---..", //8
  "----.", //9
};

static uint8_t output_state;
static uint8_t output_count; 

static uint8_t current_letter_in_table;
static uint8_t letter_morse_idx;
static uint8_t letter_or_space;
static uint8_t letter_idle = 1;

void morse_reset() {
  output_state = 0;
  output_count = 0;

  current_letter_in_table = 0;
  letter_morse_idx = 0;
  letter_or_space = 0;
  letter_idle = 1;
}

/*******************************************************************************/
/* Glue */

void morse_stream_letter_glue(void) {
  /* when we're done a letter, grab the next one from the stream */
  uint8_t next_letter = morse_stream_get();
  morse_letter_set(next_letter);
}

void morse_letter_output_glue(void) {
  /* when we're done a symbol, grab the next symbol */
  uint8_t output;
  uint8_t count;
  morse_letter_get_next_output(&output, &count);
  morse_output_set(output, count);
}

/*******************************************************************************/
/* Streamer */

static uint8_t* stream_str;
static uint8_t stream_offset;

void morse_stream_set(const char* stream) {
  stream_str = (uint8_t*)stream;
  stream_offset = 0;
}

uint8_t morse_stream_get(void) {
  if (stream_str[stream_offset] == 0) {
    return 0;
  }
  uint8_t out = stream_str[stream_offset];
  stream_offset++;
  return out;
}


/*******************************************************************************/
/* Letter Conversion */

static void (*letter_callback)(void) = NULL;

void morse_letter_callback(void (*callback)(void)) {
  letter_callback = callback;
}

void morse_letter_set(uint8_t letter) {
  current_letter_in_table = letter - 'A';
  letter_morse_idx = 0;
  letter_or_space = 0;
  letter_idle = 0;
}

void morse_letter_get_next_output(uint8_t* output, uint8_t* count) {
  if (letter_idle) {
    /* try the callback and check again */
    if (letter_callback != NULL) {
      letter_callback();
    }
    if (letter_idle) {
      *output = 0;
      *count = 0;
      return;
    }
  }
  if (letter_or_space == 0) {
    if (alpha[current_letter_in_table][letter_morse_idx] == '.') {
      *output = 1;
      *count = 1;
    }
    if (alpha[current_letter_in_table][letter_morse_idx] == '-') {
      *output = 1;
      *count = 3;
    }
    letter_morse_idx++;
    letter_or_space = 1;
  } else {
    /* look at the next letter to decide */
    if (alpha[current_letter_in_table][letter_morse_idx] != '\0') {
      *output = 0;
      *count = 1;
    } else {
      *output = 0;
      *count = 3;
      letter_idle = 1;
      if (letter_callback != NULL) {
	letter_callback();
      }
    }
    letter_or_space = 0;
  }
}


/*******************************************************************************/
/* Output functionality */

static void (*output_callback)(void) = NULL;

void morse_output_callback(void (*callback)(void)) {
  output_callback = callback;
}

void morse_output_set(uint8_t output, uint8_t ticks) {
  output_state = output;
  output_count = ticks;
}

uint8_t morse_output_get(void) {
  return output_state;
}

void morse_output_tick(void) {
  if (output_count > 0) {
    output_count--;
  }
  /* if we're empty, try to get another one */
  if ((output_count == 0) && (output_callback != NULL)) {
    output_callback();
  }
}
