// #include <stdio.h>
// #include <string.h>
#include "controller.h"
#include "solenoid_driver.h"
#include "led_driver.h"
#include "circular_buffer.h"
#include "uart_driver.h"
#include <stdio.h> 
#include <string.h> 

#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "0.0.2"
#endif

extern uint8_t ledCount;
extern LED_DATA_T *ledData;
extern FADE_DATA_T fadeData;

CIRCULAR_BUFFER commandBuffer ;


void controller_dump_state() {
  char buffer[10];
  uart_write("V:", 2);
  uart_write(FIRMWARE_VERSION, strlen(FIRMWARE_VERSION));
  uart_write("\n", 1);
  sprintf(buffer, "A:%2x\n", uart_address());
  uart_write(buffer, strlen(buffer));
  sprintf(buffer, "S:%2x\n", solenoid_state());
  uart_write(buffer, strlen(buffer));
  sprintf(buffer, "L:%u\n", ledCount);
  uart_write(buffer, strlen(buffer));
  if(ledCount > 0) {
    sprintf(buffer, "E:%02x%02x%02x\n", ledData[0].current[0], ledData[0].current[1], ledData[0].current[2]);
    uart_write(buffer, strlen(buffer));
  }
}

void controller_init() {
  buffer_init(&commandBuffer);

  controller_dump_state();
}

uint8_t parseBuffer[32];
void controller_off() {
  solenoid_write(0x00);
  led_off();
}

void controller_add_bytes(uint8_t *bytes, uint8_t length) {
  uint8_t availableBytes;
  uint8_t continueParsing = 1;
  buffer_copy_from(&commandBuffer, bytes, length);

  availableBytes = buffer_bytes(&commandBuffer);

  if(availableBytes > 0) {
    uint8_t *commandStart = parseBuffer;
    availableBytes = buffer_copy_to(&commandBuffer, parseBuffer, 32);

    while(availableBytes > 0 && continueParsing) {
      uint8_t leds = 1; //We will have AT LEAST 1 LED for any LED command

      switch(*commandStart) {
        case 'D':
          controller_dump_state();
          continueParsing = 1;
          buffer_consume(&commandBuffer, 1);
          commandStart++;
          availableBytes--;
          break;
        case 'F':
          controller_off();
          continueParsing = 1;
          buffer_consume(&commandBuffer, 1);
          commandStart++;
          availableBytes--;
          break;
        case 'S':
          if(availableBytes >= 2) {
            uint8_t solenoidCommand ;
            
            //Consume the command
            commandStart++;
            availableBytes--;
            buffer_consume(&commandBuffer, 1);

            solenoidCommand = *commandStart;

            solenoid_write(solenoidCommand);

            //Consume the parameter
            commandStart++;
            availableBytes--;
            buffer_consume(&commandBuffer, 1);
          } else {
            continueParsing = 0;
          }
          break;
        case 'L':
          if(availableBytes > 2) {
            leds = *(commandStart + 1);
          } 

          if(availableBytes >= 2 + (4 * leds)) {
            int bytesInCommand = 2 + (4 * leds);
            uint8_t i = 0;

            for(i = 2 ; i < bytesInCommand; i++ ) {
              //Left shift bytes of LED data by 1 bit to handle MSB issues.
              commandStart[i] = commandStart[i] << 1;
            }
            //We actually have the necessary number of bytes.  
            led_write_values(leds, commandStart + 2);

            //Consume the command
            availableBytes -= bytesInCommand;
            commandStart += bytesInCommand;
            buffer_consume(&commandBuffer, bytesInCommand);
          } else {
            continueParsing = 0;
          }

          break;
        case 'l':
          if(availableBytes > 3) {
            leds = *(commandStart + 2);
          } 

          if(availableBytes >= 3 + (4 * leds)) {
            int bytesInCommand = 3 + (4 * leds);
            int fadeDuration = *(commandStart + 1);
            uint8_t i = 0;

            for(i = 3 ; i < bytesInCommand; i++ ) {
              //Left shift bytes of LED data by 1 bit to handle MSB issues.
              commandStart[i] = commandStart[i] << 1;
            }

            //We actually have the necessary number of bytes.  
            led_write_fade_target(fadeDuration, leds, commandStart + 3);
            
            //Consume the command
            availableBytes -= bytesInCommand;
            commandStart += bytesInCommand;
            buffer_consume(&commandBuffer, bytesInCommand);
          } else {
            continueParsing = 0;
          }

          break;
        default:
          //Start of the buffer is a command we don't recognize or a random byte.  We need to move on.
          uart_write_batch("Unknown:", 8);
          uart_write_batch(commandStart, 1);
          uart_write_batch("\n", 1);
          uart_flush_batch();

          availableBytes--;
          commandStart++;
          buffer_consume(&commandBuffer, 1);

          break;
      }
    }
  }
}