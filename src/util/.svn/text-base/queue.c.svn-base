#include "queue.h"

void *queue_init() {
	void *q = (void *)malloc(sizeof(queue));
	queue *qp = (queue *)q;
	qp->start = NULL;
	return q;
}

void queue_free(void *q) {
	//current: assume that objects have been removed
	// queue *qp = (queue *)q;
	free(q);
	q = NULL;
}

void queue_add (void *q, void *obj) {
	queue *qp = (queue *)q;
	
	node *n = (node *)malloc(sizeof(node));
	n->item = obj;
	
	if (qp->start == NULL) {
		//n->next = n;
		n->next = NULL;
		n->prev = n;
		qp->start = n;
	}
	else {
		node *end = qp->start->prev;
		end->next = n;
		// n->next = q->start;
		n->next = NULL;
		n->prev = end;
		qp->start->prev = n;
	}
}

void queue_remove_all_obj(void *q, void *obj, int (*compare_func)(void *, void *)) {
	queue *qp = (queue *)q;
	node *curr=qp->start;

	while (curr != NULL) {
		if ((*compare_func) (curr->item, obj) == 1) {
			//splice it out of queue
			node *toremove = curr;
			free(toremove->item);
			curr = toremove->next;

			if (toremove->next != NULL)
				toremove->next->prev = toremove->prev;
			
			if (toremove!=qp->start)
				toremove->prev->next = toremove->next;
			else
				qp->start = curr;
		}
		else {
			curr = curr->next;
		}
	}
}

void *queue_remove(void *q) {
	queue *qp = (queue *)q;
	void *obj = NULL;
	node *toremove = qp->start;
	if (toremove != NULL) {
		qp->start = toremove->next;
		if (qp->start != NULL)
			qp->start->prev = toremove->prev;
		obj = toremove->item;
	}
	/* for a stack. haha.
	if (qp->start != NULL) {
		node *last = qp->start->prev;
		obj = last->item;
		if (last == qp->start) {
			qp->start = NULL;
		}
		else {
			node *secondlast = last->prev;
			qp->start->prev = secondlast;
			secondlast->next = NULL;
		}
		free(last);
	}*/
	else {
		printf("queue error: no items in queue. cannot remove an item\n");
	}
	return obj;
}

void queue_print(void *q){
	queue *qp = q;
	// pthread_mutex_lock(&(sp->queue_mutex));
	// printf("queue size: %d\n", sp->size);
	node *curr = qp->start;
	while (curr != NULL){
		printf("%d\n", *(int *)(curr->item));
		curr = curr->next;
	}
	// pthread_mutex_unlock(&(sp->queue_mutex));
}

//returns 1 if queue is empty, 0 if not.
int queue_is_empty(void *q) {
	queue *qp = (queue *)q;
	return (qp->start == NULL) ? 1 : 0;
}