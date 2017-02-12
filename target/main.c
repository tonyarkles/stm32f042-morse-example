
#include <hal.h>

volatile int i = 0;

void NMI_Handler() {
  while(1);
}

void HardFault_Handler() {
  while(1);
}

int main() {
  hardware_init();
  int j = 0;
  while(1) {
    hardware_led_gpio(j % 2);
    j++;
    for(i = 0; i < 100000; i++) {
    }
  }
}

void _exit(int code) {
  for(;;) {}
}
