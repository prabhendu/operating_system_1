#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"

void queue_init (queue *que) {
	que->front = que->rear = NULL;
}

int enqueue (queue *que, gtthread_t *thread) {
	node *temp = (node *) malloc(sizeof(node));
	if (temp == NULL) return 0;

	if (que->rear == NULL) {
		que->rear = temp;
		if (que->rear == NULL) return 0;
		que->rear->data = thread;
		que->rear->next = NULL;
		que->front = que->rear;
	} 	
	else
	{
		que->rear->next = temp;
		temp->data = thread;
		temp->next = NULL;
		que->rear = temp;
	}

	return 1;
}

gtthread_t * dequeue (queue *que) {
	if (que->front == NULL) return NULL;
	temp = front;
	front = front->next;
	gtthread_t *ret = temp->data;
	free(temp);
	return ret;
}

/*returns the count of threads in queue*/
int que_size (queue *que) {
	node *temp = que->front;
	int size = 0;
	while (temp == que->last) {
		++size;
		temp = temp->next;
	}	
	return size;
}

/*Remove an element from queue which matches thread
  Returns 1 on success */
void remove_q_ele (queue *que, gtthread_t *thread) {
	node *curr = que->first;
	node *prev = NULL;
	if (curr == NULL) return 0;

	while (curr) {
		if (curr->data == thread) {	
			if (prev == NULL) {
				que->first = curr->next;
			} else {
				prev->next = curr->next;
			}

	 		if (curr == que->last) {
				que->last = prev;
			}
			free(curr);
			return 1;
		}
	prev = curr;
	curr = curr->next;
	}

	return 0;
}
