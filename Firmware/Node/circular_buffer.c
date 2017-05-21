#include "circular_buffer.h"

void buffer_init(CIRCULAR_BUFFER* buffer) {
	buffer->end = (uint8_t)0;
	buffer->pos = (uint8_t)0;
}

uint8_t buffer_copy_from(CIRCULAR_BUFFER* targetBuffer, char* source, uint8_t len) {
  uint8_t i;
  for(i = 0; i < len ; i++) {
  	if(source[i] == 0x00) {
  		//Don't bother copying null
  		return i;
  	}

    targetBuffer->buffer[targetBuffer->end] = source[i];
    targetBuffer->end++;

    if(targetBuffer->end >= BUFFER_SIZE) {
      targetBuffer->end = 0;
    }

    if(targetBuffer->end == targetBuffer->pos) {
      //We've wrapped around and can't continue writing to the buffer.
      return i;
    }
  }

  return i;
}

uint8_t buffer_copy_to(CIRCULAR_BUFFER* sourceBuffer, char* target, uint8_t maxLen) {
  uint8_t i;
  uint8_t sourceOffset = sourceBuffer->pos;

  for(i = 0; i < maxLen ; i++) {
  	if((sourceOffset + sourceBuffer->pos) == sourceBuffer->end) {
      return i;
    }

    target[i] = sourceBuffer->buffer[sourceOffset];

    sourceOffset++;

    if(sourceOffset >= BUFFER_SIZE) {
      sourceOffset = 0;
    }
  }

  return i;
}

uint8_t buffer_consume(CIRCULAR_BUFFER* buffer, uint8_t bytes) {
	for(int i = 0; i < bytes; i++) {
		if(buffer->pos == buffer->end) {
			return i;
		}

		buffer->pos++;

		if(buffer->pos >= BUFFER_SIZE) {
      		buffer->pos = 0;
    	}
	}

	return bytes;
}