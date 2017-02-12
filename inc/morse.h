
#ifndef MORSE_H
#define MORSE_H

#include <stdint.h>

void morse_reset();

void morse_output_callback(void (*callback)(void));
void morse_output_set(uint8_t output, uint8_t ticks);
uint8_t morse_output_get(void);
void morse_output_tick(void);

#endif
