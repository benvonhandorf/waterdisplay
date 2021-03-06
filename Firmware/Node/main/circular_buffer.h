#include <stdint.h>

#define BUFFER_SIZE 32
typedef struct {
  char  buffer[BUFFER_SIZE];
  uint8_t pos;
  uint8_t bytes;
} CIRCULAR_BUFFER ;

void buffer_init(CIRCULAR_BUFFER* buffer);

uint8_t buffer_copy_from(CIRCULAR_BUFFER* targetBuffer, uint8_t *source, uint8_t len);

uint8_t buffer_copy_to(CIRCULAR_BUFFER* sourceBuffer, uint8_t *target, uint8_t maxLen);

uint8_t buffer_consume(CIRCULAR_BUFFER *buffer, uint8_t bytes);

uint8_t buffer_bytes(CIRCULAR_BUFFER *buffer);