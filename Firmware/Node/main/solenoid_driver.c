#include "solenoid_driver.h"
#include "stm8s.h"
#include "stm_util.h"

#ifndef DEVBOARD

//Solenoid driver board v1 runs solenoid driver off PC3
#define SOLENOID_PORT_DDR PC_DDR
#define SOLENOID_PORT_CR1 PC_CR1
#define SOLENOID_PORT_ODR PC_ODR
#define SOLENOID_BIT_MASK 0x08

#else

//Dev board uses PB5
#define SOLENOID_PORT_DDR PB_DDR
#define SOLENOID_PORT_CR1 PB_CR1
#define SOLENOID_PORT_ODR PB_ODR
#define SOLENOID_BIT_MASK 0x20

#endif

uint8_t solenoid_state_internal;

uint8_t solenoid_state() {
  return solenoid_state_internal;
}

void solenoid_init() {
  CLRBIT(SOLENOID_PORT_ODR, SOLENOID_BIT_MASK);
  SETBIT(SOLENOID_PORT_DDR, SOLENOID_BIT_MASK);
  SETBIT(SOLENOID_PORT_CR1, SOLENOID_BIT_MASK);
}

void solenoid_write(uint8_t active) {
  solenoid_state_internal = active;
  if(active) {
    SETBIT(SOLENOID_PORT_ODR, SOLENOID_BIT_MASK);
  } else {
    CLRBIT(SOLENOID_PORT_ODR, SOLENOID_BIT_MASK);
  }
}