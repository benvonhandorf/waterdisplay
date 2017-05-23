#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "circular_buffer.h"

extern void buffer_dump(CIRCULAR_BUFFER *buffer);

int main(int argc, char **argv) {
  CIRCULAR_BUFFER circular_buffer;
  char buffer[64];
  char secondBuffer[64];

  memset(buffer, 0x00, 64);
  memset(secondBuffer, 0x00, 64);

  buffer_init(&circular_buffer);

  if(circular_buffer.pos != 0
    || circular_buffer.bytes != 0) {
    fprintf(stderr, "Buffer not initialized - pos %d - end %d\n", circular_buffer.pos, circular_buffer.bytes);
    exit(-1);
  }

  strncpy(buffer, "hello, world!", 64);

  int sourceLen = strlen(buffer);

  int result = buffer_copy_from(&circular_buffer, buffer, sourceLen);

  if(result != sourceLen) {
    fprintf(stderr, "Copied fewer bytes than provided - sourceLen %d - result %d\n", sourceLen, result);
    exit(-1);
  }

  if(circular_buffer.pos != 0) {
    fprintf(stderr, "Position moved when copying data into buffer\n");
    exit(-1);
  }

  if(circular_buffer.bytes != sourceLen) {
    fprintf(stderr, "Bytes of data did not move when copying data into buffer- %d\n", circular_buffer.bytes);
    exit(-1);
  }

  int result2 = buffer_copy_to(&circular_buffer, secondBuffer, 64);

  if(result != result2) {
    fprintf(stderr, "Bytes copied into and out of buffer are not equal - %d - %d\n", result, result2);
    fprintf(stderr, "Source - %s\n", buffer);
    fprintf(stderr, "Target - %s\n", secondBuffer);
    exit(-1);   
  }

  if(circular_buffer.bytes != sourceLen) {
    fprintf(stderr, "End of data moved when reading from buffer- %d\n", circular_buffer.bytes);
    exit(-1);
  }

  if(circular_buffer.pos != 0) {
    fprintf(stderr, "Start of data moved before bytes were consumed - %d\n", circular_buffer.pos);
    exit(-1);
  }

  int result3 = buffer_consume(&circular_buffer, 64);

  if(result3 != result2) {
    fprintf(stderr, "Bytes consumed were different than expected - %d - %d\n", result3, result2);
    buffer_dump(&circular_buffer);
    exit(-1);
  }

  int result_copy_to_when_consumed = buffer_copy_to(&circular_buffer, buffer, 64);

  if(result_copy_to_when_consumed != 0) {
    fprintf(stderr, "Bytes were copied after all should have been consumed - %d\n", result_copy_to_when_consumed);
    buffer_dump(&circular_buffer);
    exit(-1);
  } 

  memset(buffer, 0x00, 64);
  memset(secondBuffer, 0x00, 64);
  strncpy(buffer, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 64);

  int result_copy_with_wrap = buffer_copy_from(&circular_buffer, buffer, 64);

  if(result_copy_with_wrap != 32) {
    fprintf(stderr, "Bytes were copied even after buffer was full - %d\n", result_copy_with_wrap);
    buffer_dump(&circular_buffer);
    exit(-1);
  }

  int result_copy_to_when_full = buffer_copy_to(&circular_buffer, secondBuffer, 64);

  if(result_copy_to_when_full != result_copy_with_wrap) {
    fprintf(stderr, "Fewer bytes copied out than in - %d - %d\n", result_copy_to_when_full, result_copy_with_wrap);
    fprintf(stderr, "Source - %s\n", buffer);
    fprintf(stderr, "Target - %s\n", secondBuffer);
    buffer_dump(&circular_buffer);
    exit(-1); 
  }

  int consume_less_than_full = buffer_consume(&circular_buffer, 5);

  if(consume_less_than_full != 5) {
    fprintf(stderr, "Wrong amount consumed when extra data available - %d\n", consume_less_than_full);
    buffer_dump(&circular_buffer);
    exit(-1);   
  }

  int result_copy_to_when_slightly_drained = buffer_copy_to(&circular_buffer, secondBuffer, 64);

  if(result_copy_to_when_slightly_drained != result_copy_with_wrap - 5) {
    fprintf(stderr, "Wrong bytes copied out after partial consumption - %d - %d\n", result_copy_to_when_slightly_drained, result_copy_with_wrap - 5);
    fprintf(stderr, "Source - %s\n", buffer);
    fprintf(stderr, "Target - %s\n", secondBuffer);
    buffer_dump(&circular_buffer);
    exit(-1); 
  }

  exit(0);
}