#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"
#include "uart_driver.h"
#include "solenoid_driver.h"
#include "led_driver.h"
#include "controller.h"
#include "configuration.h"
#include "status_driver.h"

#ifndef DEFAULT_NODE_ADDRESS
#define DEFAULT_NODE_ADDRESS 0x01
#endif

volatile uint8_t fade_cycles = 0;
volatile uint8_t timer_counter = 0;

void uart1_tx_isr_handler(void) __interrupt(UART1_TXE_vector) {
  uart1_tx_isr();
}

void uart1_rx_isr_handler(void) __interrupt(UART1_RXF_vector) {
  uart1_rx_isr();
}

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

uint8_t RED[] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00};

void main() {
  char buffer[32];
  uint8_t bytesReceived = 0;
  uint8_t nodeAddress = configuration_get(CNF_ADDR);

  smi();

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
  status_init();

  tim4_init();

  IWDG_KR = 0xCC;
  IWDG_PR = 0x02;

  rmi();  

  status_set(STATUS_ONE);

  while(1) {
    uint8_t i ;
    int bytesReceived = uart_read(buffer, 32);
    controller_add_bytes(buffer, bytesReceived);

    for(i = 0; i < fade_cycles; i++) {
       led_fade();
    }

    IWDG_KR = 0xAA;

    fade_cycles = 0;

    if(bytesReceived) {
      status_set(STATUS_ONE);
    } else {
      status_clear(STATUS_ONE);
    }
  }
}