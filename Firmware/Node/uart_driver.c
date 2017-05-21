#include "stm8s.h"
#include "uart_driver.h"
#include "circular_buffer.h"

static CIRCULAR_BUFFER receiveBuffer;
static CIRCULAR_BUFFER transmitBuffer;

void uart_init() {
  UART1_CR2 = UART_CR2_TEN;
  // 1 stop bit
  UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2);

  //9600 baud
  UART1_BRR2 = 0x03;
  UART1_BRR1 = 0x68;
}


void uart_write(char *buffer, uint8_t size) {
	buffer_copy_from(&transmitBuffer, buffer, size);

	
}