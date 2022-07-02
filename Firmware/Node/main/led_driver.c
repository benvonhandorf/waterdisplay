#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "led_driver.h"

#if 0

#include "pl9823_driver.h"

void(*impl_init)() = &pl9823_init;
void(*impl_write)(uint8_t *) = &pl9823_write_led;
void(*impl_latch)() = &pl9823_latch;

#else

#include "pwm_led_driver.h"

void(*impl_init)() = &pwmled_init;
void(*impl_write)(uint8_t *) = &pwmled_write_led;
void(*impl_latch)() = &pwmled_latch;

#endif

#define min(a, b) (a > b ? b : a)

uint8_t OFF[] = {0x00, 0x00, 0x00, 0x00};

uint8_t ledCount;
LED_DATA_T *ledData = NULL;
FADE_DATA_T fadeData;

void led_send_string() {
  uint8_t i;

  for(i = 0; i < ledCount; i++) {
    LED_DATA_T *led = ledData + i;

    uint8_t *bytes = led->current;
    impl_write(bytes);
  }

  impl_latch(); 
}

void led_init(uint8_t led_count) {
  ledCount = led_count;

  ledData = (LED_DATA_T *) malloc(sizeof(LED_DATA_T) * ledCount);

  fadeData.fadeTotal = 0;
  fadeData.fadeComplete = 0;

  impl_init();

  impl_latch(); 

  led_off();
}

#define FADEVAL(sourceValue, targetValue, portion) (uint8_t) (sourceValue + ((((uint16_t) targetValue) - sourceValue) *portion )/255)

void led_fade_single(LED_DATA_T * led, uint8_t fadePortion) {
    led->current[0] = FADEVAL(led->source[0], led->target[0], fadePortion);
    led->current[1] = FADEVAL(led->source[1], led->target[1], fadePortion);
    led->current[2] = FADEVAL(led->source[2], led->target[2], fadePortion);
    led->current[3] = FADEVAL(led->source[3], led->target[3], fadePortion);
}

void led_fade() {
  uint8_t fadePortion;
  uint8_t i ;

  if(fadeData.fadeTotal == 0) {
    return;
  }

  fadeData.fadeComplete += 1; //Expect this to be called every ~50 ms

  fadePortion = (((uint16_t) fadeData.fadeComplete) * 255) / fadeData.fadeTotal;

  if(fadeData.fadeComplete >= fadeData.fadeTotal) {
    fadeData.fadeTotal = 0;
    for(i = 0 ; i < ledCount; i++) {
      LED_DATA_T *led = ledData + i;
      
      memcpy(&led->current, &led->target, 4);
    }
  } else {
    for(i = 0 ; i < ledCount; i++) {
      LED_DATA_T *led = ledData + i;

      led_fade_single(led, fadePortion);
    }
  }

  led_send_string();
}

void led_write_values(uint8_t leds, uint8_t *led_bytes) {
  uint8_t i;

  leds = min(leds, ledCount);
  
  for(i = 0 ; i < leds ; i++) {
    uint8_t *bytes = led_bytes + (4 * i);
    LED_DATA_T *led = ledData + i;
    memcpy(&led->current, bytes, 4);
  }

  fadeData.fadeTotal = 0;
  led_send_string();
}

void led_write_fade_target(uint8_t fade_cycles, uint8_t leds, uint8_t *led_bytes) {
  uint8_t i;
  
  leds = min(leds, ledCount);

  for(i = 0 ; i < leds ; i++) {
    uint8_t *bytes = led_bytes + (4 * i);
    LED_DATA_T *led = ledData + i;

    memcpy(&led->source, &led->current, 4);
    memcpy(&led->target, bytes, 4);
  }

  fadeData.fadeTotal = fade_cycles;
  fadeData.fadeComplete = 0;
  led_fade();
}

void led_off() {
  uint8_t i;
  for(i = 0 ; i < ledCount ; i++) {
    LED_DATA_T *led = ledData + i;
    memcpy(&led->current, OFF, 4);
  }

  fadeData.fadeTotal = 0;
  led_send_string();
}