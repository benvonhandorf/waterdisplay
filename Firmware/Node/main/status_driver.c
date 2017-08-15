#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"
#include "status_driver.h"

// //PB4 - CH2
// #define STATUS_ONE_PORT_DDR PB_DDR
// #define STATUS_ONE_PORT_CR1 PB_CR1
// #define STATUS_ONE_PORT_CR2 PB_CR2
// #define STATUS_ONE_PORT_ODR PB_ODR
// #define STATUS_ONE_BIT_MASK 0x10

// //PB5 - CH4
// #define STATUS_TWO_PORT_DDR PB_DDR
// #define STATUS_TWO_PORT_CR1 PB_CR1
// #define STATUS_TWO_PORT_CR2 PB_CR2
// #define STATUS_TWO_PORT_ODR PB_ODR
// #define STATUS_TWO_BIT_MASK 0x20

//PB4 - CH2
#define STATUS_ONE_PORT_DDR PA_DDR
#define STATUS_ONE_PORT_CR1 PA_CR1
#define STATUS_ONE_PORT_CR2 PA_CR2
#define STATUS_ONE_PORT_ODR PA_ODR
#define STATUS_ONE_BIT_MASK 0x02

//PA5 - CH4
#define STATUS_TWO_PORT_DDR PA_DDR
#define STATUS_TWO_PORT_CR1 PA_CR1
#define STATUS_TWO_PORT_CR2 PA_CR2
#define STATUS_TWO_PORT_ODR PA_ODR
#define STATUS_TWO_BIT_MASK 0x04

void status_init() {
  SETBIT(STATUS_ONE_PORT_DDR, STATUS_ONE_BIT_MASK);
  SETBIT(STATUS_ONE_PORT_CR1, STATUS_ONE_BIT_MASK);
  SETBIT(STATUS_ONE_PORT_CR2, STATUS_ONE_BIT_MASK);
  CLRBIT(STATUS_ONE_PORT_ODR, STATUS_ONE_BIT_MASK);

  SETBIT(STATUS_TWO_PORT_DDR, STATUS_TWO_BIT_MASK);
  SETBIT(STATUS_TWO_PORT_CR1, STATUS_TWO_BIT_MASK);
  SETBIT(STATUS_TWO_PORT_CR2, STATUS_TWO_BIT_MASK);
  CLRBIT(STATUS_TWO_PORT_ODR, STATUS_TWO_BIT_MASK);
}

void status_set(uint8_t status_bit) {
  switch(status_bit){
    case STATUS_ONE:
      SETBIT(STATUS_ONE_PORT_ODR, STATUS_ONE_BIT_MASK);
      break;
    case STATUS_TWO:
      SETBIT(STATUS_TWO_PORT_ODR, STATUS_TWO_BIT_MASK);
      break;
  }
}

void status_clear(uint8_t status_bit) {
  switch(status_bit){
    case STATUS_ONE:
      CLRBIT(STATUS_ONE_PORT_ODR, STATUS_ONE_BIT_MASK);
      break;
    case STATUS_TWO:
      CLRBIT(STATUS_TWO_PORT_ODR, STATUS_TWO_BIT_MASK);
      break;
  }
}

void status_toggle(uint8_t status_bit) {
  uint8_t portValue = 0;
  uint8_t mask = 0;

  switch(status_bit){
    case STATUS_ONE:
      portValue = STATUS_ONE_PORT_ODR;
      mask = STATUS_ONE_BIT_MASK;
      break;
    case STATUS_TWO:
      portValue = STATUS_TWO_PORT_ODR;
      mask = STATUS_TWO_BIT_MASK;
      break;
  }

  if(portValue & mask == mask) {
    status_clear(status_bit);
  } else {
    status_set(status_bit);
  }
}