#include <hal.h>

#define rcc_base (0x40021000)
#define rcc_ahbenr ((uint32_t*)(rcc_base + 0x14))

#define gpioa_base (0x48000000)
#define gpioa_moder ((uint32_t*)(gpioa_base + 0x00))
#define gpioa_otyper ((uint32_t*)(gpioa_base + 0x04))
#define gpioa_ospeedr ((uint32_t*)(gpioa_base + 0x08))
#define gpioa_pupdr ((uint32_t*)(gpioa_base + 0x0c))
#define gpioa_idr ((uint32_t*)(gpioa_base + 0x10))
#define gpioa_odr ((uint32_t*)(gpioa_base + 0x14))

#define gpiob_base (0x48000400)
#define gpiob_moder ((uint32_t*)(gpiob_base + 0x00))
#define gpiob_otyper ((uint32_t*)(gpiob_base + 0x04))
#define gpiob_ospeedr ((uint32_t*)(gpiob_base + 0x08))
#define gpiob_pupdr ((uint32_t*)(gpiob_base + 0x0c))
#define gpiob_idr ((uint32_t*)(gpiob_base + 0x10))
#define gpiob_odr ((uint32_t*)(gpiob_base + 0x14))

#define systick_base (0xe000E010)
#define systick_csr ((uint32_t*)(systick_base + 0x00))
#define systick_rvr ((uint32_t*)(systick_base + 0x04))
#define systick_cvr ((uint32_t*)(systick_base + 0x08))
#define systick_calib ((uint32_t*)(systick_base + 0x0c))


void hardware_init() {
  /* set up the LED port */

  /* Each GPIO port has 4 config registers:
     GPIOx_MODER
     GPIOx_OTYPER
     GPIOx_OSPEEDR
     GPIOx_PUPDR
     And two data registers:
     GPIOx_IDR
     GPIOx_ODR
     And a set/reset register GPIOx_BSRR
  */

  /* For LED output, want output push-pull with no pull-up or
     pull-down. LED is on PB3 */

  uint32_t portb_clock = 1 << 18;
  *rcc_ahbenr |= portb_clock;
  
  uint32_t led_moder = 0x1 << 6; /* (0b01 = GP out, LED is on PB3) */
  uint32_t led_otyper = 0x0 << 3; /* 0b0 = push-pull */
  uint32_t led_ospeedr = 0x0 << 6; /* 0bx0 = low speed */
  uint32_t led_pupdr = 0x0 << 6; /* 0b00 = no pull-up or down */

  *gpiob_moder |= led_moder;
  *gpiob_otyper |= led_otyper;
  *gpiob_ospeedr |= led_ospeedr;
  *gpiob_pupdr |= led_pupdr;

  /* SysTick */
  *systick_rvr = 8000; // systick runs at 8MHz (processor clock) ->
		       // want a tick every 1ms -> 8e6/s * 1/0.001s =
		       // 8000
  *systick_cvr = 0; // reset the counter
  *systick_csr |= (1 << 2) | (1 << 1) | (1 << 0); /* run from processor clock, enable tick interrupt, and enable counter */
}

void hardware_led_gpio(uint8_t val) {
  uint32_t led_bit = 1 << 3;
  if (val) {
    *gpiob_odr |= led_bit;
  } else {
    *gpiob_odr &= ~led_bit;
  }
}
