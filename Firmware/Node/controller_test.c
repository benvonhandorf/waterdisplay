#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "controller.h"
#include "solenoid_driver.h"
#include "led_driver.h"

extern uint8_t ledCount;
extern LED_DATA_T *ledData;
extern FADE_DATA_T fadeData;

void printLed(LED_DATA_T *testLed) {
  fprintf(stdout, "Current: %2x %2x %2x %2x\nSource:%2x %2x %2x %2x\n Target: %2x %2x %2x %2x\n", 
    testLed->current[0], testLed->current[1], testLed->current[2], testLed->current[3],
    testLed->source[0], testLed->source[1], testLed->source[2], testLed->source[3],
    testLed->target[0], testLed->target[1], testLed->target[2], testLed->target[3]
    );
}

int main(void) {
  led_init(1);
  solenoid_init();
  controller_init();

  controller_add_bytes((uint8_t *)"D", 1);

  printf("State dumped\n");

  controller_add_bytes((uint8_t *)"S\x01", 2);

  if(solenoid_state() != 0x01) {
    fprintf(stderr, "Solenoid state was not correct after atomic command: %02x\n", solenoid_state());
    exit(-1);
  }

  controller_add_bytes((uint8_t *)"S", 1);

  if(solenoid_state() != 0x01) {
    fprintf(stderr, "Solenoid state was not correct after partial command: %02x\n", solenoid_state());
    exit(-1);
  }

  controller_add_bytes((uint8_t *)"\x00", 1);

  if(solenoid_state() != 0x00) {
    fprintf(stderr, "Solenoid state was not correct after finishing command: %02x\n", solenoid_state());
    exit(-1);
  }

  controller_add_bytes((uint8_t *)"S\x01", 2);
  controller_add_bytes((uint8_t *)"F", 1);

  if(solenoid_state() != 0x00) {
    fprintf(stderr, "Solenoid state was not correct after OFF command: %02x\n", solenoid_state());
    exit(-1);
  }

  controller_add_bytes((uint8_t *)"FS\x01", 3);
  
  if(solenoid_state() != 0x01) {
    fprintf(stderr, "Solenoid state was not correct after multiple commands: %02x\n", solenoid_state());
    exit(-1);
  }

  controller_add_bytes((uint8_t *)"L\x01\x7F\x7F\x7F\x7F", 6);
  
  if(ledData[0].current[0] != 0xFE
    && ledData[0].current[1] != 0xFE
    && ledData[0].current[2] != 0xFE
    && ledData[0].current[3] != 0xFE) {
    fprintf(stderr, "LED not in correct state after write\n");
    printLed(ledData);
    exit(-1);
  }

  controller_add_bytes((uint8_t *)"l\x50\x01\x7F\x7F\x7F\x7F", 7);
  
  if(fadeData.fadeTotal != 0x50) {
    fprintf(stderr, "LED fade not in correct state after write\n");
    printLed(ledData);
    exit(-1);
  }

  if(ledData[0].target[0] != 0xFE
    && ledData[0].target[1] != 0xFE
    && ledData[0].target[2] != 0xFE
    && ledData[0].target[3] != 0xFE) {
    fprintf(stderr, "LED target not in correct state after write\n");
    printLed(ledData);
    exit(-1);
  }

  controller_add_bytes((uint8_t *)"F", 1);

  if(fadeData.fadeTotal != 0) {
    fprintf(stderr, "LED fade not in correct state after OFF\n");
    printLed(ledData);
    exit(-1);
  }

  if(ledData[0].current[0] != 0
    && ledData[0].current[1] != 0
    && ledData[0].current[2] != 0
    && ledData[0].current[3] != 0) {
    fprintf(stderr, "LED current not in correct state after OFF\n");
    printLed(ledData);
    exit(-1);
  }  

  controller_add_bytes((uint8_t *)"L\x01\x7F\x7F\x7F\x7F", 6);

  controller_add_bytes((uint8_t *)"l\x01\x01\x00\x00\x00\x00", 7);

  if(ledData[0].target[0] != 0x00
    && ledData[0].target[1] != 0x00
    && ledData[0].target[2] != 0x00
    && ledData[0].target[3] != 0x00) {
    fprintf(stderr, "LED target not in correct state after fade to black\n");
    printLed(ledData);
    exit(-1);
  }

  led_fade_single(ledData, 0xFF);

  if(ledData[0].current[0] != 0x00
    && ledData[0].current[1] != 0x00
    && ledData[0].current[2] != 0x00
    && ledData[0].current[3] != 0x00) {
    fprintf(stderr, "LED current not in correct state after fade to black\n");
    printLed(ledData);
    exit(-1);
  }
}