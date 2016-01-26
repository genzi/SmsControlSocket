#ifndef Queue_h
#define Queue_h

#include <stdlib.h>
#include "RingBuffer.h"

typedef struct {
    RingBuffer *data;
    RingBuffer *sizes;
    int amount;
    int length;
    int currentAmount;
} Queue;

Queue *Queue_create(int amount, int length);
void Queue_destroy(Queue *queue);
int Queue_write(Queue *queue, char *data, int length);
int Queue_read(Queue *queue, char *data);

#endif
