#include <stdint.h>

void solenoid_init();

void solenoid_write(uint8_t active);

uint8_t solenoid_state();