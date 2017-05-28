#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"
#include "uart_driver.h"
#include "solenoid_driver.h"
#include "led_driver.h"
#include "controller.h"

void uart1_tx_isr_handler(void) __interrupt(UART1_TXE_vector) {
  uart1_tx_isr();
}

void uart1_rx_isr_handler(void) __interrupt(UART1_RXF_vector) {
  uart1_rx_isr();
}

void tim2_capcom(void) __interrupt(TIM2_CAPCOM_vector) {
  led_fade();
}

void tim2_init() {
  //Targeting a 50ms timer

  // TIM2_CCR1H = 0x00;
  // TIM2_CCR1L = 24;
  
  // TIM2_PSCR = 
}

void main() {
  char buffer[32];
  int i = 0;
  int j = 0;
  uint8_t solenoid_active = 0;
  uint8_t bytesReceived = 0;

  CLK_DIVR = 0x00;
  CLK_PCKENR1 = 0xFF;
  
  uart_init();
  solenoid_init();
  led_init(1);
  controller_init();

  rmi();

  while(1) {
    int bytesReceived = uart_read(buffer, 32);
    controller_add_bytes(buffer, bytesReceived);
  }
}