#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RingBuffer.h"

RingBuffer *RingBuffer_create(int length)
{
    RingBuffer *buffer = calloc(1, sizeof(RingBuffer));
    buffer->length  = length + 1;
    buffer->start = 0;
    buffer->end = 0;
    buffer->buffer = calloc(buffer->length, 1);

    return buffer;
}

void RingBuffer_destroy(RingBuffer *buffer)
{
    if(buffer) {
        free(buffer->buffer);
        free(buffer);
    }
}

int RingBuffer_write(RingBuffer *buffer, char *data, int length)
{
    void *result;
		
		if(RingBuffer_available_data(buffer) == 0) {
        buffer->start = buffer->end = 0;
    }

    if(check(length <= RingBuffer_available_space(buffer)) == -1)
    {
        return -1;
    }

    result = memcpy(RingBuffer_ends_at(buffer), data, length);

    if(check(result != NULL) == -1)
    {
        return -1;
    }

    RingBuffer_commit_write(buffer, length);

    return length;
}

int RingBuffer_read(RingBuffer *buffer, char *target, int amount)
{
    void *result;
	
    if(check(amount <= RingBuffer_available_data(buffer)) == -1)
    {
        return -1;
    }

    result = memcpy(target, RingBuffer_starts_at(buffer), amount);

    if(check(result != NULL) == -1)
    {
        return -1;
    }

    RingBuffer_commit_read(buffer, amount);

    if(buffer->end == buffer->start) {
        buffer->start = buffer->end = 0;
    }

    return amount;
}
