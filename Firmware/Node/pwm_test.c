#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"

volatile uint8_t fade_cycles = 0;
volatile uint8_t timer_counter = 0;
volatile uint8_t pwm_cycle = 0;
volatile int8_t delta = 5;

void tim4_isr(void) __interrupt(TIM4_vector) {
  if(TIM4_SR & TIM_SR1_UIF){
    timer_counter++;
    if(timer_counter == 25) {
      timer_counter = 0;
      fade_cycles++;
    }
    CLRBIT(TIM4_SR, TIM_SR1_UIF);
  }
}

void tim4_init() {
  //Targeting a 50ms timer
  TIM4_PSCR = 0x07; //Prescaler = 128

  SETBIT(TIM4_IER, TIM_IER_UIE);
  SETBIT(TIM4_CR1, TIM_CR1_CEN);
}

void tim2_init() {
  SETBIT(PD_CR1, 0x10);

  TIM2_PSCR = 0x03; //Prescaler = 2

  TIM2_ARRH = 0x00;
  TIM2_ARRL = 0xFF;

  TIM2_CCER1 = 0x01; //CCR1 enabled
  TIM2_CCMR1 = 0x78; //PWM Mode 111, Preload register enabled

  TIM2_CCR1H = 0x00;
  TIM2_CCR1L = pwm_cycle;

  SETBIT(TIM2_CR1, 0x80);
  SETBIT(TIM2_CR1, TIM_CR1_CEN);
}

void main() {
  smi();

  CLK_DIVR = 0x00;
  CLK_PCKENR1 = 0xFF;

  tim4_init();

  SETBIT(PB_DDR, 0x20);
  SETBIT(PB_CR1, 0x20);
  CLRBIT(PB_ODR, 0x20);
  tim2_init();

  rmi();  

  while(1) {
    uint8_t i = 0;

    for(i = 0; i < fade_cycles; i++) {


      if(pwm_cycle > 240) {
        delta = -5;
        SETBIT(PB_ODR, 0x20);
      } else if( pwm_cycle < 10) {
        delta = 5;
        CLRBIT(PB_ODR, 0x20);
      }
      pwm_cycle +=delta;

      TIM2_CCR1L = pwm_cycle;
    }

    fade_cycles = 0;
  }
}