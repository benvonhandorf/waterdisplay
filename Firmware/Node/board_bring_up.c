#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"
#include "uart_driver.h"
#include "solenoid_driver.h"

void uart1_tx_isr_handler(void) __interrupt(UART1_TXE_vector) {
  uart1_tx_isr();
}

void uart1_rx_isr_handler(void) __interrupt(UART1_RXF_vector) {
  uart1_rx_isr();
}

void main() {
  char buffer[64];
  int i = 0;
  int j = 0;
  uint8_t solenoid_active = 0;
  uint8_t bytesReceived = 0;

  CLK_DIVR = 0x00;
  CLK_PCKENR1 = 0xFF;
  
  uart_init();
  solenoid_init();

  rmi();

  while(1) {
    if(!bytesReceived) {
      uart_write("Alpha Beta\n",11);
    }

    bytesReceived = 0;

    solenoid_active = !solenoid_active;
    solenoid_write(solenoid_active);

    for(i = 0 ; i < 0x7FFF; i++) {
      for(j = 0; j < 0x40; j++) {
        if(uart_bytes_available()) {
          int bytesRead = uart_read(buffer, 64);
          uart_write(buffer, bytesRead);
          bytesReceived = 1;
        } 
      }
    }
  }
}