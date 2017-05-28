// #include <stdio.h>
// #include <string.h>
#include "controller.h"
#include "solenoid_driver.h"
#include "led_driver.h"
#include "circular_buffer.h"
#include "uart_driver.h"

CIRCULAR_BUFFER commandBuffer ;

void controller_init() {
  buffer_init(&commandBuffer);
}

char parseBuffer[32];

void controller_dump_state() {
  uart_write("FW: 0.0.1\n", 10);
}

void controller_off() {
  solenoid_write(0x00);
  led_off();
}

void controller_add_bytes(char *bytes, uint8_t length) {
  uint8_t availableBytes;
  uint8_t continueParsing = 1;
  buffer_copy_from(&commandBuffer, bytes, length);

  availableBytes = buffer_bytes(&commandBuffer);

  if(availableBytes > 0) {
    char *commandStart = parseBuffer;
    availableBytes = buffer_copy_to(&commandBuffer, parseBuffer, 32);

    while(availableBytes > 0 && continueParsing) {
      uint8_t leds = 1; //We will have AT LEAST 1 LED

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
          availableBytes--;
          commandStart++;
          break;
      }
    }
  }
}