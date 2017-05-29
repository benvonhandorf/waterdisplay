#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"
#include "uart_driver.h"
#include "solenoid_driver.h"
#include "led_driver.h"
#include "controller.h"

volatile uint8_t solenoid_active = 0;

void uart1_tx_isr_handler(void) __interrupt(UART1_TXE_vector) {
  uart1_tx_isr();
}

void uart1_rx_isr_handler(void) __interrupt(UART1_RXF_vector) {
  uart1_rx_isr();
}

void tim2_isr(void) __interrupt(TIM2_OVF_vector) {
  //if(TIM2_SR1 & TIM_SR1_UIF){
    //led_fade();
    solenoid_active = !solenoid_active;
  //}
}

void tim2_init() {
  //Targeting a 50ms timer

  TIM2_ARRH = 0x00;
  TIM2_ARRL = 200;//////24;
  
  TIM2_PSCR = 0x08; //Prescaler = 2 ^ 15

  SETBIT(TIM2_IER, TIM_IER_UIE);

  SETBIT(TIM2_CR1, TIM_CR1_CEN);
}

void main() {
  char buffer[32];
  int i = 0;
  int j = 0;
  uint8_t solenoid_active = 0;
  uint8_t bytesReceived = 0;

  CLK_DIVR = 0x00;
  CLK_PCKENR1 = 0xFF;
  
  uart_init(0x01);
  solenoid_init();
  led_init(1);
  controller_init();

  solenoid_active = 1;
  solenoid_write(solenoid_active);

  tim2_init();

  rmi();

  while(1) {
    __asm__("nop");
    solenoid_write(solenoid_active);
    // int bytesReceived = uart_read(buffer, 32);
    // controller_add_bytes(buffer, bytesReceived);
  }
}