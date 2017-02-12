
#include <hal.h>
#include <morse.h>

volatile int i = 0;

void NMI_Handler() {
  while(1);
}

void HardFault_Handler() {
  while(1);
}

static volatile uint32_t tick = 0;
static volatile uint8_t morse_ready = 0;

void SysTick_Handler(void) {
  tick += 1;
  if (!morse_ready) { return; }

  if ((tick % 100) == 0) {
    if (morse_stream_empty()) {
      morse_stream_set("HELLO WORLD");
    }
    morse_output_tick();
    hardware_led_gpio(morse_output_get());
  }
}

int main() {
  hardware_init();
  morse_reset();
  morse_output_callback(morse_letter_output_glue);
  morse_letter_callback(morse_stream_letter_glue);
  morse_ready = 1;
  
  while(1) {
  }
}

void _exit(int code) {
  for(;;) {}
}
