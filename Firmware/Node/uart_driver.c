#include "stm8s.h"
#include "stm_util.h"
#include "uart_driver.h"
#include "circular_buffer.h"

#include <stdlib.h>

static CIRCULAR_BUFFER receiveBuffer;
static CIRCULAR_BUFFER transmitBuffer;
uint8_t nodeAddress;

void uart_init(uint8_t address) {
  CLRBIT(UART1_CR1, UART_CR1_UARTD);
  CLRBIT(UART1_CR1, UART_CR1_M);
  
  // 1 stop bit
  UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2);

  //9600 baud
  UART1_BRR2 = 0x03;
  UART1_BRR1 = 0x68;

  UART1_CR4 &= 0xF0;
  UART1_CR4 |= (address & 0x0F);

  SETBIT(UART1_CR2, UART_CR2_TEN);
  SETBIT(UART1_CR2, UART_CR2_REN);
  SETBIT(UART1_CR2, UART_CR2_RIEN);

  nodeAddress = address;

  if(address > 0) {
    SETBIT(UART1_CR1, UART_CR1_WAKE);
    SETBIT(UART1_CR2, UART_CR2_RWU);
  }

  buffer_init(&receiveBuffer);
  buffer_init(&transmitBuffer);
}

uint8_t txByte() {
  char dataByte;
  uint8_t bytesCopied = buffer_copy_to(&transmitBuffer, &dataByte, 1);

  if (bytesCopied) {
    buffer_consume(&transmitBuffer, bytesCopied);
    UART1_DR = dataByte;
  }

  return bytesCopied;
}

void uart1_tx_isr() {
  // The vector is used for multiple purposes, check that TXE is indeed set.
  if (UART1_SR & UART_SR_TXE) {
    if (!txByte()) {
      // Transfer complete. Disable tx interrupts.
      CLRBIT(UART1_CR2, UART_CR2_TIEN);
    }
  }
}

void uart1_rx_isr() {
  if (UART1_SR & UART_SR_RXNE) {
    char dataByte = UART1_DR;
    uint8_t bytesCopied;

    if(nodeAddress == 0
        || dataByte != (0x80 | nodeAddress)) {
      bytesCopied = buffer_copy_from(&receiveBuffer, &dataByte, 1);
    }
  }
}

uint8_t uart_write(char *buffer, uint8_t size) {
  uint8_t bytesWritten ;

	bytesWritten = buffer_copy_from(&transmitBuffer, buffer, size);

  SETBIT(UART1_CR2, UART_CR2_TIEN);

  return bytesWritten;
}

uint8_t uart_write_batch(char *buffer, uint8_t size) {
  uint8_t bytesWritten ;

  bytesWritten = buffer_copy_from(&transmitBuffer, buffer, size);

  return bytesWritten;
}

uint8_t uart_flush_batch() {
  SETBIT(UART1_CR2, UART_CR2_TIEN);

  return buffer_bytes(&transmitBuffer);
}


uint8_t uart_bytes_available() {
    return buffer_bytes(&receiveBuffer);
}

uint8_t uart_read(char *buffer, uint8_t size) {
    uint8_t bytesRead = buffer_copy_to(&receiveBuffer, buffer, size);
    buffer_consume(&receiveBuffer, bytesRead);

    return bytesRead;
}