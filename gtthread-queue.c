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
