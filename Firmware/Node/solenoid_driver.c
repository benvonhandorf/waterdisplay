#include "solenoid_driver.h"
#include "stm8s.h"

//Board v1 runs solenoid driver off PC3
#define SOLENOID_BIT_MASK 0x08

//Dev board uses PB5

void solenoid_init() {
	PC_DDR = SOLENOID_BIT_MASK;
	PC_CR1 = SOLENOID_BIT_MASK;
}

void solenoid_write(uint8_t active) {
	if(active) {
		PC_ODR |= SOLENOID_BIT_MASK;
	} else {
		PC_ODR &= ~SOLENOID_BIT_MASK;
	}
}