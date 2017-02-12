
#include <hal.h>

volatile int i = 0;

void NMI_Handler() {
  while(1);
}

void HardFault_Handler() {
  while(1);
}

static uint32_t tick = 0;

void SysTick_Handler(void) {
  tick += 1;
  hardware_led_gpio((tick/1000) % 2);
}

int main() {
  hardware_init();
  while(1) {
  }
}

void _exit(int code) {
  for(;;) {}
}
