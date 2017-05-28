#include "controller.h"
#include "solenoid_driver.h"
#include "led_driver.h"
#include "circular_buffer.h"

typedef struct {
	
} 

CIRCULAR_BUFFER commandBuffer ;

void controller_init() {
	circular_buffer_init(&commandBuffer);
}

void controller_add_bytes(char *bytes, uint8_t length) {
	uint8_t bytes;
	buffer_copy_from(&commandBuffer, bytes, length);

	bytes = buffer_bytes(&commandBuffer);

	if(bytes > 0) {

	}
}