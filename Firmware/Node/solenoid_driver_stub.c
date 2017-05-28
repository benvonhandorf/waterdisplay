#include <stdint.h>
#include <stdio.h>

uint8_t solenoid_state = -1;

void solenoid_init() {
  fprintf(stdout, "solenoid: init\n");
  solenoid_state = 0;
}

void solenoid_write(uint8_t active) {
  solenoid_state = active;
  fprintf(stdout, "solenoid: write: %x\n", solenoid_state);
}