#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "led_driver.h"

void printLed(LED_DATA_T *ledData) {
  fprintf(stderr, "Current: %d %d %d %d\nSource:%d %d %d %d\n Target: %d %d %d %d\n", 
    ledData->current[0], ledData->current[1], ledData->current[2], ledData->current[3],
    ledData->source[0], ledData->source[1], ledData->source[2], ledData->source[3],
    ledData->target[0], ledData->target[1], ledData->target[2], ledData->target[3]
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

int main(void) {
  LED_DATA_T ledData;

  ledData.current[0] = 0x00;
  ledData.current[1] = 0x00;
  ledData.current[2] = 0x00;
  ledData.current[3] = 0x00;

  ledData.source[0] = 0xFF;
  ledData.source[1] = 0x00;
  ledData.source[2] = 0x00;
  ledData.source[3] = 0x00;

  ledData.target[0] = 0x00;
  ledData.target[1] = 0xFF;
  ledData.target[2] = 0x00;
  ledData.target[3] = 0x00;

  led_fade_single(&ledData, 0);

  if(compareColors(ledData.current, ledData.source)) {
    fprintf(stderr, "Current color was not source when fadePortion was 0\n");
    printLed(&ledData);
    exit(-1);
  }

  led_fade_single(&ledData, 255);

  if(compareColors(ledData.current, ledData.target)) {
    fprintf(stderr, "Current color was not target when fadePortion was 255\n");
    printLed(&ledData);
    exit(-1);
  }

    led_fade_single(&ledData, 127);

  if(compareColors(ledData.current, HALFWAY_DATA)) {
    fprintf(stderr, "Current color was not middle when fadePortion was 127\n");
    printLed(&ledData);
    exit(-1);
  }
}