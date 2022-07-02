#include <stdint.h>
#include "stm8s.h"

void uart_init(uint8_t address);

uint8_t uart_address();

void uart1_tx_isr();

void uart1_rx_isr();

uint8_t uart_write(char *buffer, uint8_t size);

uint8_t uart_bytes_available();

uint8_t uart_read(char *buffer, uint8_t size);