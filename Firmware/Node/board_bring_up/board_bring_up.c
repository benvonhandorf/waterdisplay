#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"

volatile uint8_t state = 0;
volatile uint8_t timer_counter = 0;
volatile uint8_t pwm_cycle = 0;

void tim4_isr(void) __interrupt(TIM4_vector) {
  if(TIM4_SR & TIM_SR1_UIF){
    timer_counter++;
    if(timer_counter % 10 ==0) {
      pwm_cycle++;
    }
    if(timer_counter == 50) {
      timer_counter = 0;
      state++;
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
  TIM2_PSCR = 0x03; //Prescaler

  TIM2_ARRH = 0x00;
  TIM2_ARRL = 0xFF;

  TIM2_CCMR1 = 0x68; //PWM Mode 111, Preload register enabled

  TIM2_CCR1H = 0x00;
  TIM2_CCR1L = 0xFF;

  TIM2_CCMR2 = 0x78; //PWM Mode 111, Preload register enabled

  TIM2_CCR2H = 0x00;
  TIM2_CCR2L = 0x80;

  TIM2_CCMR3 = 0x78; //PWM Mode 111, Preload register enabled

  TIM2_CCR3H = 0x00;
  TIM2_CCR3L = 0x7F;

  TIM2_CCER1 = 0x01 | 0x10; //CC1 and 2 enabled
  TIM2_CCER2 = 0x01; //CC3 enabled



  SETBIT(TIM2_CR1, 0x80);
  SETBIT(TIM2_CR1, TIM_CR1_CEN);
}

void main() {

  smi();

  CLK_DIVR = 0x00;
  CLK_PCKENR1 = 0xFF;

  //Main Control 
  SETBIT(PC_DDR, 0x08);
  SETBIT(PC_CR1, 0x08);
  CLRBIT(PC_ODR, 0x08);

  tim4_init();
  tim2_init();

  rmi();  

  while(1) {
    //Main Control 
    CLRBIT(PC_ODR, 0x08);

    TIM2_CCR1H = 0x00;
    TIM2_CCR1L = pwm_cycle;

    TIM2_CCR2H = 0x00;
    TIM2_CCR2L = pwm_cycle;

    TIM2_CCR3H = 0x00;
    TIM2_CCR3L = pwm_cycle;

    if((state + 0) % 4 == 3) {
      // Main Control 
      SETBIT(PC_ODR, 0x08);
    }
  }
}