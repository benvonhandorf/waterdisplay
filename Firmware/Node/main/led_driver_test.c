#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "led_driver.h"

void printLed(LED_DATA_T *testLed) {
  fprintf(stdout, "Current: %2x %2x %2x %2x\nSource:%2x %2x %2x %2x\n Target: %2x %2x %2x %2x\n", 
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

uint8_t TEST_DATA[] = {0xFF, 0x00, 0xAA, 0x55,
  0x00, 0xAA, 0x55, 0xFF, 
  0xAA, 0x55, 0xFF, 0x00
};

uint8_t HALFWAY_DATA[] = {128, 127, 0, 0};  


extern LED_DATA_T *ledData;
extern FADE_DATA_T fadeData;
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
  
  for(i = 0; i < 255; i++) {
    led_fade_single(&testLed, i);
    printLed(&testLed);
  }

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

  if(fadeData.fadeTotal != 0) {
    fprintf(stderr, "After initialization, LED driver thinks a fade is ongoing\n");
    exit(-1);
  }  

  led_write_values(1, TEST_DATA);

  LED_DATA_T *led = ledData;
  uint8_t *ledExpectedValues = TEST_DATA;

  if(led->current[0] != ledExpectedValues[0]
    || led->current[1] != ledExpectedValues[1]
    || led->current[2] != ledExpectedValues[2]
    || led->current[3] != ledExpectedValues[3]) {
    fprintf(stderr, "LED values do not match expected after partial write\n");
    exit(-1);    
  }

  led = ledData + 1;

  if(led->current[0] != 0x00
    || led->current[1] != 0x00
    || led->current[2] != 0x00
    || led->current[3] != 0x00) {
    fprintf(stderr, "Too many LEDs written by partial write\n");
    printLed(led);
    exit(-1);    
  }

  led_write_values(3, TEST_DATA);

  for(i = 0; i < ledCount; i++) {
    led = ledData + i;
    ledExpectedValues = TEST_DATA + (i * 4);

    if(led->current[0] != ledExpectedValues[0]
      || led->current[1] != ledExpectedValues[1]
      || led->current[2] != ledExpectedValues[2]
      || led->current[3] != ledExpectedValues[3]) {
      fprintf(stderr, "LED values do not match expected after full write\n");
      exit(-1);    
    }
  }

  if(fadeData.fadeTotal != 0) {
    fprintf(stderr, "After full write, LED driver thinks a fade is ongoing\n");
    exit(-1);
  }  


  led_off();

  for(i = 0; i < ledCount; i++) {
    led = ledData + i;

    if(led->current[0] != 0x00
      || led->current[1] != 0x00
      || led->current[2] != 0x00
      || led->current[3] != 0x00) {
      fprintf(stderr, "LED values do not match expected after off\n");
    exit(-1);    
  }
  }

  if(fadeData.fadeTotal != 0) {
    fprintf(stderr, "After off, LED driver thinks a fade is ongoing\n");
    exit(-1);
  }  

  led_write_fade_target(10, 3, TEST_DATA);

  for(i = 0; i < ledCount; i++) {
    led = ledData + i;
    ledExpectedValues = TEST_DATA + (i * 4);

    if(led->target[0] != ledExpectedValues[0]
      || led->target[1] != ledExpectedValues[1]
      || led->target[2] != ledExpectedValues[2]
      || led->target[3] != ledExpectedValues[3]) {
      fprintf(stderr, "LED values do not match expected after full write\n");
    exit(-1);    
  }
  }

  if(fadeData.fadeTotal != 10) {
    fprintf(stderr, "After write_fade_target, fade duration is wrong\n");
    exit(-1);
  }  

  if(fadeData.fadeComplete != 1) {
    fprintf(stderr, "After write_fade_target, fade current is wrong\n");
    exit(-1);
  }  

}