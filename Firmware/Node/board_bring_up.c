#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"
#include "uart_driver.h"
#include "solenoid_driver.h"
#include "led_driver.h"
#include "controller.h"
#include "configuration.h"

volatile uint8_t solenoid_active = 0;

#ifndef DEFAULT_NODE_ADDRESS
#define DEFAULT_NODE_ADDRESS 0x01
#endif

void uart1_tx_isr_handler(void) __interrupt(UART1_TXE_vector) {
  uart1_tx_isr();
}

void uart1_rx_isr_handler(void) __interrupt(UART1_RXF_vector) {
  uart1_rx_isr();
}

void tim2_isr(void) __interrupt(TIM2_OVF_vector) {
  if(TIM2_SR1 & TIM_SR1_UIF){
    led_fade();
    CLRBIT(TIM2_SR1, TIM_SR1_UIF);
  }
}

void tim2_init() {
  //Targeting a 50ms timer
  TIM2_PSCR = 0x0F; //Prescaler = 2 ^ 15

  TIM2_ARRH = 0x00;
  TIM2_ARRL = 24;

  SETBIT(TIM2_IER, TIM_IER_UIE);
  SETBIT(TIM2_CR1, TIM_CR1_CEN);
}

uint8_t RED[] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00};

void main() {
  char buffer[32];
  uint8_t bytesReceived = 0;
  uint8_t nodeAddress = configuration_get(CNF_ADDR);

  CLK_DIVR = 0x00;
  CLK_PCKENR1 = 0xFF;

  if(nodeAddress == 0
    || nodeAddress > 0x0F) {
    nodeAddress = DEFAULT_NODE_ADDRESS;
  }
  
  uart_init(nodeAddress);
  solenoid_init();
  led_init(1);
  controller_init();

  tim2_init();

  rmi();

  while(1) {
    int bytesReceived = uart_read(buffer, 32);
    if(bytesReceived > 0) {
      solenoid_write(1);
    }
    controller_add_bytes(buffer, bytesReceived);
  }
}