
volatile int i = 0;

void NMI_Handler() {
  while(1);
}

void HardFault_Handler() {
  while(1);
}


int main() {
  while(1) {
    for(i = 0; i < 100000; i++) {
    }
  }
}
