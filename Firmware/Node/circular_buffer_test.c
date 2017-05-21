#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "circular_buffer.h"

void buffer_dump(CIRCULAR_BUFFER *buffer) {
	printf("Pos: %d End:%d Data:\'%s\'\n", buffer-> pos, buffer->end, buffer->buffer);
}

int main(int argc, char **argv) {
	CIRCULAR_BUFFER circular_buffer;
	char buffer[64];
	char secondBuffer[64];

	memset(buffer, 0x00, 64);
	memset(secondBuffer, 0x00, 64);

	buffer_init(&circular_buffer);

	if(circular_buffer.pos != 0
		|| circular_buffer.end != 0) {
		fprintf(stderr, "Buffer not initialized - pos %d - end %d\n", circular_buffer.pos, circular_buffer.end);
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

	if(circular_buffer.end != sourceLen) {
		fprintf(stderr, "End of data did not move when copying data into buffer- %d\n", circular_buffer.end);
		exit(-1);
	}

	int result2 = buffer_copy_to(&circular_buffer, secondBuffer, 64);

	if(result != result2) {
		fprintf(stderr, "Bytes copied into and out of buffer are not equal - %d - %d\n", result, result2);
		fprintf(stderr, "Source - %s\n", buffer);
		fprintf(stderr, "Target - %s\n", secondBuffer);
		exit(-1);		
	}

	if(circular_buffer.end != sourceLen) {
		fprintf(stderr, "End of data moved when reading from buffer- %d\n", circular_buffer.end);
		exit(-1);
	}

	if(circular_buffer.pos != 0) {
		fprintf(stderr, "Start of data moved before bytes were consumed - %d\n", circular_buffer.pos);
		exit(-1);
	}

	buffer_dump(&circular_buffer);

	int result3 = buffer_consume(&circular_buffer, result2);

	if(result3 != result2) {
		fprintf(stderr, "Bytes consumed were different than requested - %d - %d\n", result3, result2);
		exit(-1);
	}

	buffer_dump(&circular_buffer);

	int result4 = buffer_copy_to(&circular_buffer, buffer, 64);

	if(result4 != 0) {
		fprintf(stderr, "Bytes were copied after all should have been consumed - %d\n", result4);
		buffer_dump(&circular_buffer);
		exit(-1);
	}	

	exit(0);
}