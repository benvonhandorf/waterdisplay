#include <stdint.h>
#include <stdio.h>

void uart_init(uint8_t address) {
	fprintf(stdout, "uart: init - address %d\n", address);
}

uint8_t uart_address() {
  return 1;
}

void uart1_tx_isr() {

}

void uart1_rx_isr() {

}

uint8_t uart_write(char *buffer, uint8_t size) {
	int i = 0;
	fprintf(stdout, "uart: write \'");

	for(i = 0; i < size; i++) {
		fprintf(stdout, "%c", buffer[i]);
	}

	fprintf(stdout, "\'\n");
	return 0;
}

uint8_t uart_bytes_available() {
	return 0;
}

uint8_t uart_read(char *buffer, uint8_t size) {
	return 0;
}