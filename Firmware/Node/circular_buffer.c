#include "circular_buffer.h"

#ifdef DEBUG
#include <stdio.h>

void buffer_dump(CIRCULAR_BUFFER *buffer) {
  printf("Pos: %d Bytes:%d Data:\'%s\'\n", buffer-> pos, buffer->bytes, buffer->buffer);
}

#endif

void buffer_init(CIRCULAR_BUFFER* buffer) {
	buffer->bytes = (uint8_t)0;
	buffer->pos = (uint8_t)0;
}

uint8_t buffer_copy_from(CIRCULAR_BUFFER* targetBuffer, char* source, uint8_t len) {
  uint8_t i;
  uint8_t bytesToCopy = len;

  if(bytesToCopy > BUFFER_SIZE - targetBuffer->bytes) {
    bytesToCopy = BUFFER_SIZE - targetBuffer->bytes;
  }

  for(i = 0; i < bytesToCopy ; i++) {
    uint8_t offset = targetBuffer->pos + i;

  	if(source[i] == 0x00) {
  		//Don't bother copying the null
  		return i;
  	}

    offset = offset % BUFFER_SIZE;

    targetBuffer->buffer[offset] = source[i];
    targetBuffer->bytes++;
  }

  return i;
}

uint8_t buffer_copy_to(CIRCULAR_BUFFER* sourceBuffer, char* target, uint8_t maxLen) {
  uint8_t i;

  uint8_t bytesToCopy = maxLen;

  uint8_t sourceOffset = 0;

  if(bytesToCopy > sourceBuffer->bytes) {
    bytesToCopy = sourceBuffer->bytes;
  }

  for(i = 0; i < bytesToCopy ; i++) {
    sourceOffset = sourceBuffer->pos + i;

  	sourceOffset = sourceOffset % BUFFER_SIZE;

    target[i] = sourceBuffer->buffer[sourceOffset];
  }

  return i;
}

uint8_t buffer_consume(CIRCULAR_BUFFER* buffer, uint8_t bytes) {
  uint8_t bytesToConsume = bytes;

  if(bytesToConsume > buffer->bytes) {
    bytesToConsume = buffer->bytes;
  }

	buffer->pos = (buffer->pos + bytesToConsume) % BUFFER_SIZE;
  buffer->bytes = buffer->bytes - bytesToConsume;

	return bytesToConsume;
}

uint8_t buffer_bytes(CIRCULAR_BUFFER *buffer) {
  return buffer->bytes;
}