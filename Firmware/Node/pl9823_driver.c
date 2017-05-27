#include "pl9823_driver.h"
#include "stm8s.h"
#include "stm_util.h"

#define LED_PORT_DDR PC_DDR
#define LED_PORT_CR1 PC_CR1
#define LED_PORT_ODR PC_ODR
#define LED_BIT_MASK 0x80

#define NOP_3() {__asm__("nop\nnop\nnop");}
#define NOP_10() {__asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");}

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

void send1() {
  SETBIT(LED_PORT_ODR, LED_BIT_MASK);
  NOP_10();
  CLRBIT(LED_PORT_ODR, LED_BIT_MASK);
  NOP_3();
}

void send0() {
  SETBIT(LED_PORT_ODR, LED_BIT_MASK);
  NOP_3();
  CLRBIT(LED_PORT_ODR, LED_BIT_MASK);
  NOP_10();
}

void sendReset() {
  int i;
  CLRBIT(LED_PORT_ODR, LED_BIT_MASK);
  for(i = 0; i < 100; i++) {
    NOP_10();
  }

}

void sendByte(unsigned char byte) {
  (byte & 0x80) ? send1() : send0();
  (byte & 0x40) ? send1() : send0();
  (byte & 0x20) ? send1() : send0();
  (byte & 0x10) ? send1() : send0();
  (byte & 0x08) ? send1() : send0();
  (byte & 0x04) ? send1() : send0();
  (byte & 0x02) ? send1() : send0();
  (byte & 0x01) ? send1() : send0();
}

void pl9823_init() {
  int i;
  SETBIT(LED_PORT_DDR, LED_BIT_MASK);
  SETBIT(LED_PORT_CR1, LED_BIT_MASK);
  CLRBIT(LED_PORT_ODR, LED_BIT_MASK);

  sendReset();

  for(i = 0; i < 20; i++) {
    sendByte(0xFF);
    sendByte(0x00);
    sendByte(0x00);
  }

  sendReset();
}