#include <stdint.h>
#include <stdio.h> 
#include <string.h>
#include "controller.h"
#include "solenoid_driver.h"
#include "led_driver.h"
#include "circular_buffer.h"
#include "uart_driver.h"
#include "configuration.h" 
#include "status_driver.h"

#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "0.0.3"
#endif

extern uint8_t ledCount;
extern LED_DATA_T *ledData;
extern FADE_DATA_T fadeData;

CIRCULAR_BUFFER commandBuffer ;

void controller_ack(char cmd) {
  uart_write("ACK-", 4);
  uart_write(&cmd, 1);
  uart_write("\n",1);
}


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

uint8_t controller_D(uint8_t *commandBuffer, uint8_t availableBytes) {
  int bytes_required = 1;
  if(availableBytes >= bytes_required) {
    controller_dump_state();

    controller_ack('D');

    return bytes_required;
  } else {
    return 0;
  }
}

uint8_t controller_A(uint8_t *commandBuffer, uint8_t availableBytes) {
  int bytes_required = 2;
  if(availableBytes >= bytes_required) {
    uint8_t proposedAddress = commandBuffer[1] ;

    if(proposedAddress == 0
      || proposedAddress > 0x0F) {
      uart_write("Invalid address\n", 16);
    } else {
      proposedAddress = configuration_set(CNF_ADDR, proposedAddress);
      uart_write("New Addr: ", 10);
      uart_write(&proposedAddress, 1);
      uart_write("\n", 1);
      controller_ack('A');
    }

    return bytes_required;
  } else {
    return 0;
  }
}

uint8_t controller_F(uint8_t *commandBuffer, uint8_t availableBytes) {
  int bytes_required = 1;
  if(availableBytes >= bytes_required) {
    controller_off();

    controller_ack('F');

    return bytes_required;
  } else {
    return 0;
  }
}

uint8_t controller_S(uint8_t *commandBuffer, uint8_t availableBytes) {
  int bytes_required = 2;
  if(availableBytes >= bytes_required) {
    uint8_t solenoidCommand ;

    solenoidCommand = commandBuffer[1];

    solenoid_write(solenoidCommand);

    controller_ack('S');

    return bytes_required;
  } else {
    return 0;
  }
}

uint8_t controller_L(uint8_t *commandBuffer, uint8_t availableBytes) {
  int bytes_required = 2; //Minimum
  int leds = 1; //Minimum
  int led_offset = 2;
  if(availableBytes >= bytes_required) {
    leds = *(commandBuffer+1);

    bytes_required = led_offset + (4 * leds);
  }

  if(availableBytes >= bytes_required) {
    uint8_t i = 0;

    for(i = led_offset ; i < bytes_required; i++ ) {
      //Left shift bytes of LED data by 1 bit to handle MSB issues.
      commandBuffer[i] = commandBuffer[i] << 1;
    }    

    led_write_values(leds, commandBuffer + led_offset);

    controller_ack('L');

    return bytes_required;
  } else {
    return 0;
  }
}

uint8_t controller_l(uint8_t *commandBuffer, uint8_t availableBytes) {
  int bytes_required = 3; //Minimum
  int leds = 1; //Minimum
  int led_offset = 3;
  if(availableBytes >= bytes_required) {
    leds = commandBuffer[2];

    bytes_required = led_offset + (4 * leds);
  }

  if(availableBytes >= bytes_required) {
    uint8_t i = 0;
    uint8_t delay = commandBuffer[1];

    for(i = led_offset ; i < bytes_required; i++ ) {
      //Left shift bytes of LED data by 1 bit to handle MSB issues.
      commandBuffer[i] = commandBuffer[i] << 1;
    }    

    led_write_fade_target(delay, leds, commandBuffer + led_offset);

    controller_ack('l');

    return bytes_required;
  } else {
    return 0;
  }
}

void controller_add_bytes(uint8_t *bytes, uint8_t length) {
  uint8_t availableBytes;
  uint8_t continueParsing = 1;
  buffer_copy_from(&commandBuffer, bytes, length);

  availableBytes = buffer_bytes(&commandBuffer);

  status_set(STATUS_TWO);

  if(availableBytes > 0) {
    uint8_t *commandStart = parseBuffer;
    availableBytes = buffer_copy_to(&commandBuffer, parseBuffer, 32);

    while(availableBytes > 0 && continueParsing) {
      uint8_t leds = 1; //We will have AT LEAST 1 LED for any LED command
      uint8_t bytes_consumed = 0;

      switch(*commandStart) {
        case 'D':
          bytes_consumed = controller_D(commandStart, availableBytes);
          break;
        case 'F':
          bytes_consumed = controller_F(commandStart, availableBytes);
          break;
        case 'S':
          bytes_consumed = controller_S(commandStart, availableBytes);
          break;
        case 'L':
          bytes_consumed = controller_L(commandStart, availableBytes);
          break;
        case 'l':
          bytes_consumed = controller_l(commandStart, availableBytes);
          break;
        case 'A':
          bytes_consumed = controller_A(commandStart, availableBytes);
          break;
        default:
          //Start of the buffer is a command we don't recognize or a random byte.  We need to move on.
          uart_write("U:", 2);
          uart_write(commandStart, 1);
          uart_write("\n", 1);

          bytes_consumed = 1;

          break;
      }

      if(bytes_consumed > 0) {
        commandStart += bytes_consumed;
        availableBytes -= bytes_consumed;
        buffer_consume(&commandBuffer, bytes_consumed);
        uart_write("N:", 2);
        uart_write(&bytes_consumed, 1);
        uart_write(&availableBytes, 1);
        uart_write("\n", 1);
      } else {
        continueParsing = 0;
      }
    }
  }

  status_clear(STATUS_TWO);
}