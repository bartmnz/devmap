
#ifndef QUEUE_H
 #define QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

#ifdef QUEUE_ARRAY

typedef struct {
	size_t head;
	size_t tail;
	size_t capacity;
	void **data;
} queue;

#else

#include "llist.h"

typedef struct {
	struct llist *head;
	struct llist *tail;
} queue;

#endif

queue *queue_create(void);
void queue_disassemble(queue *q);
void queue_destroy(queue *q);

bool queue_is_empty(queue *q);
size_t queue_size(queue *q);

bool queue_enqueue(queue *q,  void *data);
 void *queue_dequeue(queue *q);

void queue_flatten(queue **q);

#endif