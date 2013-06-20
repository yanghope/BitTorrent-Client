#ifndef _QUEUE_H
#define _QUEUE_H

#include "jhl_lib.h"

typedef struct queue_node {
	void *item;
	struct queue_node *next;	//points to next node. if node is last, points to NULL
	struct queue_node *prev;	//points to previous node. if node is start, points to last node on queue
} node;

typedef struct {
	// int size;
	// pthread_mutex_t queue_mutex;
	// pthread_mutex_t iterator_mutex;
	node *start;
} queue;

void *queue_init();

void queue_free(void *q);

void queue_add (void *q, void *obj);

void queue_remove_all_obj(void *q, void *obj, int (*compare_func)(void *, void *));

void *queue_remove(void *q);

void queue_print(void *q);

//returns 1 if queue is empty, 0 if not.
int queue_is_empty(void *q);

#endif
