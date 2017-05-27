#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "led_driver.h"

void printLed(LED_DATA_T *testLed) {
  fprintf(stdout, "Current: %d %d %d %d\nSource:%d %d %d %d\n Target: %d %d %d %d\n", 
    testLed->current[0], testLed->current[1], testLed->current[2], testLed->current[3],
    testLed->source[0], testLed->source[1], testLed->source[2], testLed->source[3],
    testLed->target[0], testLed->target[1], testLed->target[2], testLed->target[3]
    );
}

uint8_t compareColors(uint8_t *c1, uint8_t *c2) {
  if(c1[0] == c2[0]
    && c1[1] == c2[1]
    && c1[2] == c2[2]
    && c1[3] == c2[3]) {
    return 0;
} else {
  return -1;
}
}

uint8_t HALFWAY_DATA[] = {128, 127, 0, 0};

extern LED_DATA_T *ledData;
extern uint8_t ledCount;

#define LED_COUNT 3

int main(void) {
  uint8_t i;
  LED_DATA_T testLed;

  testLed.current[0] = 0x00;
  testLed.current[1] = 0x00;
  testLed.current[2] = 0x00;
  testLed.current[3] = 0x00;

  testLed.source[0] = 0xFF;
  testLed.source[1] = 0x00;
  testLed.source[2] = 0x00;
  testLed.source[3] = 0x00;

  testLed.target[0] = 0x00;
  testLed.target[1] = 0xFF;
  testLed.target[2] = 0x00;
  testLed.target[3] = 0x00;

  led_fade_single(&testLed, 0);

  if(compareColors(testLed.current, testLed.source)) {
    fprintf(stderr, "Current color was not source when fadePortion was 0\n");
    printLed(&testLed);
    exit(-1);
  }

  led_fade_single(&testLed, 255);

  if(compareColors(testLed.current, testLed.target)) {
    fprintf(stderr, "Current color was not target when fadePortion was 255\n");
    printLed(&testLed);
    exit(-1);
  }

    led_fade_single(&testLed, 127);

  if(compareColors(testLed.current, HALFWAY_DATA)) {
    fprintf(stderr, "Current color was not middle when fadePortion was 127\n");
    printLed(&testLed);
    exit(-1);
  }

  led_init(LED_COUNT);

  if(ledCount != LED_COUNT) {
    fprintf(stderr, "LED driver led count is not correct: %d\n", ledCount);
    exit(-1);
  }  

  for(i = 0 ; i < ledCount ; i++) {
    LED_DATA_T *led = ledData + i;

    fprintf(stdout, "LED: %x\n", (unsigned int) led);

    printLed(led);

    led_fade_single(led, 25);

    printLed(led);
  }

  led_fade();

  led_fade();

  led_fade();

  led_fade();
}