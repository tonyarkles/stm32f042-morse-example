
#ifndef MORSE_H
#define MORSE_H

#include <stdint.h>

void morse_reset();

void morse_output_callback(void (*callback)(void));
void morse_output_set(uint8_t output, uint8_t ticks);
uint8_t morse_output_get(void);
void morse_output_tick(void);

void morse_letter_set(uint8_t letter);
void morse_letter_get_next_output(uint8_t* output, uint8_t* count);
void morse_letter_callback(void (*callback)(void));

void morse_stream_set(const char* stream);
uint8_t morse_stream_get(void);
uint8_t morse_stream_empty(void);

void morse_letter_output_glue(void);
void morse_stream_letter_glue(void);

#endif
