
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

#if 0
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
#endif

void morse_reset() {

}

/*******************************************************************************/
/* Output functionality */

static void (*output_callback)(void) = NULL;
static uint8_t output_state;
static uint8_t output_count;

void morse_output_callback(void (*callback)(void)) {
  output_callback = callback;
}

void morse_output_set(uint8_t output, uint8_t ticks) {
  output_state = output;
  output_count = ticks;
}

uint8_t morse_output_get(void) {
  return 1;
}

void morse_output_tick(void) {
  if (output_count >= 0) {
    output_count--;
  }
  if ((output_count == 0) && (output_callback != NULL)) {
    output_callback();
  }
}
