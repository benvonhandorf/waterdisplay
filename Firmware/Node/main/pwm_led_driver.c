#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"

void pwmled_init() {
  TIM2_PSCR = 0x03; //Prescaler

  TIM2_ARRH = 0x00;
  TIM2_ARRL = 0xFF;

  //PWM 0 - R Channel
  TIM2_CCMR3 = 0x68; //PWM Mode 111, Preload register enabled

  TIM2_CCR3H = 0x00;
  TIM2_CCR3L = 0x00;

  //PWM 1  - G Channel
  TIM2_CCMR1 = 0x68; //PWM Mode 111, Preload register enabled

  TIM2_CCR1H = 0x00;
  TIM2_CCR1L = 0x00;

  //PWM 2 - B Channel
  TIM2_CCMR2 = 0x68; //PWM Mode 111, Preload register enabled

  TIM2_CCR2H = 0x00;
  TIM2_CCR2L = 0x00;


  TIM2_CCER1 = 0x01 | 0x10; //CC1 and 2 enabled
  TIM2_CCER2 = 0x01; //CC3 enabled

  SETBIT(TIM2_CR1, 0x80);
  SETBIT(TIM2_CR1, TIM_CR1_CEN);
}

void pwmled_write_led(uint8_t *bytes) {
	//R Channel
    TIM2_CCR3H = 0x00;
    TIM2_CCR3L = bytes[0];

    //G Channel
    TIM2_CCR1H = 0x00;
    TIM2_CCR1L = bytes[1];

    //B Channel
    TIM2_CCR2H = 0x00;
    TIM2_CCR2L = bytes[2];
}

void pwmled_latch() {
	//Stub, does nothing with PWM data, it's latched in as soon as it's received
}