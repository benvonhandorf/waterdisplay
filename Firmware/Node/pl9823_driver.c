#include "pl9823_driver.h"
#include "stm8s.h"
#include "stm_util.h"

#define SOLENOID_PORT_DDR PC_DDR
#define SOLENOID_PORT_CR1 PC_CR1
#define SOLENOID_PORT_ODR PC_ODR
#define SOLENOID_BIT_MASK 0x80

/*
PL9823 Timing Characteristics

Reset: 50us low

Data pulses are high then low.  All are  +/- 150ns

T0 high: 0.35us
T0 low: 1.36us

T1 high: 1.36us
T1 low: 0.35us
*/

/* 
Candidate Timers:
## TIM1 ##
- 16 bit
- Up/down
- Prescaler: Any integer from 1 to 65536
- 4 C/C channels

## TIM2/TIM3/TIM5 ##
- 16 bit
- Up only
- Prescaler: Any power of 2 from 1 to 32768
- 2 C/C channels (2 and 5 both have 3)
*/

void pl9823_init() {
  SETBIT(SOLENOID_PORT_DDR, SOLENOID_BIT_MASK);
  SETBIT(SOLENOID_PORT_CR1, SOLENOID_BIT_MASK);
}