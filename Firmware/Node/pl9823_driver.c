#include <stdint.h>
#include <stdlib.h>
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

uint8_t ledCount;
LED_DATA_T *ledData = NULL;
FADE_DATA_T fadeData;

void send1() {
  SETBIT(LED_PORT_ODR, LED_BIT_MASK);
  NOP_3();
  NOP_3();
  CLRBIT(LED_PORT_ODR, LED_BIT_MASK);
  NOP_3();
}

void send0() {
  SETBIT(LED_PORT_ODR, LED_BIT_MASK);
  {__asm__("nop");}
  CLRBIT(LED_PORT_ODR, LED_BIT_MASK);
  {__asm__("nop");}
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

void sendRGB(unsigned char *bytes) {
  sendByte(bytes[0]);
  sendByte(bytes[1]);
  sendByte(bytes[2]);
}

void sendGRB(unsigned char *bytes) {
  sendByte(bytes[0]);
  sendByte(bytes[1]);
  sendByte(bytes[2]);
}

#define send sendGRB

unsigned char OFF[] = {0x00, 0x00, 0x00, 0x00};

void sendLedString() {
  uint8_t i;

  for(i = 0; i < ledCount; i++) {
    LED_DATA_T *led = ledData + i;

    unsigned char *bytes = ledData->current + (4 * i);
    send(bytes);
  }

  sendReset();  
}

void pl9823_init(uint8_t led_count) {
  uint8_t i;

  ledCount = led_count;

  ledData = (LED_DATA_T *) malloc(sizeof(LED_DATA_T) * ledCount);

  for(i = 0 ; i < led_count; i++) {
    LED_DATA_T *led = ledData + i;
    led->current[0] = 0x00;
    led->current[1] = 0x00;
    led->current[2] = 0x00;
    led->current[3] = 0x00;

    led->source[0] = 0xFF;
    led->source[1] = 0x00;
    led->source[2] = 0x00;
    led->source[3] = 0x00;

    led->target[0] = 0x00;
    led->target[1] = 0xFF;
    led->target[2] = 0x00;
    led->target[3] = 0x00;
  }

  fadeData.fadeTotal = 10;
  fadeData.fadeComplete = 0;


  SETBIT(LED_PORT_DDR, LED_BIT_MASK);
  SETBIT(LED_PORT_CR1, LED_BIT_MASK);
  CLRBIT(LED_PORT_ODR, LED_BIT_MASK);

  sendReset();

  sendLedString();
}

#define FADEVAL(sourceValue, targetValue, portion) (uint8_t) (sourceValue + ((((uint16_t) targetValue) - sourceValue) *portion )/255)

void fadeLed(LED_DATA_T * led, uint8_t fadePortion) {
    led->current[0] = FADEVAL(led->source[0], led->target[0], fadePortion);
    led->current[1] = FADEVAL(led->source[1], led->target[1], fadePortion);
    led->current[2] = FADEVAL(led->source[2], led->target[2], fadePortion);
    led->current[3] = FADEVAL(led->source[3], led->target[3], fadePortion);
}

void pl9823_fade() {
  uint8_t fadePortion, i ;

  if(fadeData.fadeTotal ==0) {
    return;
  }

  fadeData.fadeComplete += 1; //Expect this to be called every 50 ms

  fadePortion = (((uint16_t) fadeData.fadeTotal) * 255) / fadeData.fadeComplete;

  for(i = 0 ; i < ledCount; i++) {
    LED_DATA_T *led = ledData + i;

    fadeLed(led, fadePortion);
  }

  sendLedString();
}