#ifndef __GTTHREAD_H
#define __GTTHREAD_H
#include <ucontext.h>
/* Declaring structure for gtthread_t */
typedef struct {
	int id; /* thread id to be used for equality */
	ucontext_t uc; /* variable to store context */
	void *retval; /*to store return value of thread*/
} gtthread_t;

typedef struct node {
	struct node *next;
	gtthread_t *data;
} node; /*Node for queue*/

typedef struct queue {
	struct node *front;
	struct node *rear;
	int count;
} queue; /*Structure of queue*/

/*Functions to modify thread queue*/
void queue_init (queue *que); /*queue initialization*/

int queue_size(queue *que); /* returns size of queue*/

int enqueue (queue *que, gtthread_t *thread); /*adds a node to rear*/

gtthread_t * dequeue (queue *que); /*removes a node from front*/

int remove_q_ele (queue *que, gtthread_t *thread); /* removes a particular node*/

gtthread_t * get_element_from_queue(queue *que, int num); /* gets element from queue*/

/* Must be called before any of the below functions. Failure to do so may
* result in undefined behavior. 'period' is the scheduling quantum (interval)
* in microseconds (i.e., 1/1000000 sec.). */
void gtthread_init(long period);

/* see man pthread_create(3); the attr parameter is omitted, and this should
* behave as if attr was NULL (i.e., default attributes) */
int gtthread_create(gtthread_t *thread,
void *(*start_routine)(void *),
void *arg);

/* see man pthread_join(3) */
int gtthread_join(gtthread_t thread, void **status);
/* gtthread_detach() does not need to be implemented; all threads should be
* joinable */

/* see man pthread_exit(3) */
void gtthread_exit(void *retval);

/* see man sched_yield(2) */
int gtthread_yield(void);

/* see man pthread_equal(3) */
int gtthread_equal(gtthread_t t1, gtthread_t t2);

/* see man pthread_cancel(3); but deferred cancelation does not need to be
* implemented; all threads are canceled immediately */
int gtthread_cancel(gtthread_t thread);
/* see man pthread_self(3) */

gtthread_t gtthread_self(void);
/* see man pthread_mutex(3); except init does not have the mutexattr parameter,
* and should behave as if mutexattr is NULL (i.e., default attributes); also,
* static initializers do not need to be implemented */
/*Structure for mutex*/

typedef struct {
	int lock; /*stores 1 if locked else 0*/
} gtthread_mutex_t;

int gtthread_mutex_init(gtthread_mutex_t *mutex);

int gtthread_mutex_lock(gtthread_mutex_t *mutex);

int gtthread_mutex_unlock(gtthread_mutex_t *mutex);
/* gtthread_mutex_destroy() and gtthread_mutex_trylock() do not need to be
* implemented */
#endif /* __GTTHREAD_H */
