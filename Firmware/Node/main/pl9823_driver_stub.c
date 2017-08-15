#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "pl9823_driver.h"

void pl9823_init() {
  fprintf(stdout, "pl9823: init\n");
}

void pl9823_write_led(uint8_t *bytes) {
  fprintf(stdout, "pl9823: write_led: %2x %2x %2x (%2x)\n", bytes[0], bytes[1], bytes[2], bytes[3]);
}

void pl9823_latch() {
  fprintf(stdout, "pl9823: latch\n");
}
