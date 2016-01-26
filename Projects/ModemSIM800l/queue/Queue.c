#include "Queue.h"

Queue *Queue_create(int amount, int length)
{
    Queue *queue = calloc(1, sizeof(Queue));

    queue->data = RingBuffer_create(amount * length);
    queue->sizes = RingBuffer_create(amount);
    queue->amount = amount;
    queue->length = length;
    queue->currentAmount = 0;

    return queue;
}

void Queue_destroy(Queue *queue)
{
    if(queue) {
        RingBuffer_destroy(queue->data);
        RingBuffer_destroy(queue->sizes);

        free(queue);
    }
}

int Queue_write(Queue *queue, char *data, int length)
{
    if(queue->currentAmount == queue->amount)
    {
        return -1;
    }

    if(length > queue->length || length > 255)
    {
        return -1;
    }

    if(RingBuffer_write(queue->data, data, length) == -1)
    {
        return -1;
    }

    if(RingBuffer_write(queue->sizes, (char *)&length, 1) == -1)
    {
        return -1;
    }

    queue->currentAmount++;
    return 0;
}

int Queue_read(Queue *queue, char *data)
{
    char size;

    if(queue->currentAmount == 0)
    {
        return -1;
    }

    queue->currentAmount--;

    RingBuffer_read(queue->sizes, &size, 1);

    return RingBuffer_read(queue->data, data, size);
}
