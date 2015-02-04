#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gtthread.h"
#include <sys/time.h>

/*The "main" execution context */
ucontext_t maincontext;

/*Count of present threads in progress
  Same is used as thread ID*/
int cancel_current = 0;

queue ready_q, finish_q; /* queue to store threads - ready and finished */

gtthread_t main_t;
gtthread_t *current;

/*Timer starting*/
long period_t;
struct itimerval timer;
void start_time() {
	setitimer(ITIMER_VIRTUAL,&timer,0);
}

void stop_time() {
	setitimer(ITIMER_VIRTUAL, 0, 0);
}

/*Queue functions' definition*/
void queue_init (queue *que) {
        que->front = que->rear = NULL;
	que->count = 0;
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
	++(que->count);
        return 1;
}

gtthread_t *dequeue (queue *que) {
        if (que->front == NULL) return NULL;
        node *temp = (node *) malloc(sizeof(node));
        temp = que->front;
        que->front = que->front->next;
        gtthread_t *ret = NULL;
	ret = temp->data;
        free(temp);
	--(que->count);
        return ret;
}

/*returns the count of threads in queue*/
int que_size (queue *que) {
        node *temp = que->front;
	if (temp == NULL) return 0;
        int size = 0;
	/*size = 0;*/
        while (temp == que->rear) {
                ++size;
                temp = temp->next;
        }
        return size;
}

/*Remove an element from queue which matches thread
  Returns 1 on success */
int remove_q_ele (queue *que, gtthread_t *thread) {
        node *curr = que->front;
        node *prev = NULL;
        if (curr == NULL) return 0;

        while (curr) {
                if (curr->data == thread) {
                        if (prev == NULL) {
                                que->front = curr->next;
                        } else {
                                prev->next = curr->next;
                        }

                        if (curr == que->rear) {
                                que->rear = prev;
                        }
                        free(curr);
			--(que->count);
                        return 1;
                }
        prev = curr;
        curr = curr->next;
        }

        return 0;
}

/*Find and return thread from the queue*/
gtthread_t *get_q_ele (queue *que, gtthread_t thread) {
        node *temp = que->front;
	gtthread_t *data = NULL;
        if (temp == NULL) return NULL; /*Empty queue*/
        while (temp) {
                if (temp->data->id == thread.id) {
                        data = temp->data;
			return data;
                }
                temp = temp->next;
        }

        return NULL;
}

/*queue functions end here*/

void scheduler () {
        gtthread_t *prev , *next = NULL;
        stop_time();
	getcontext(&maincontext);
        prev = current;

        if (cancel_current == 0) {
                enqueue(&ready_q, prev);
        } else {
                cancel_current = 0;
        }

        next = dequeue(&ready_q);
        if (next == NULL) exit(EXIT_SUCCESS); /*No thread present in queue*/
        current = next;
        start_time();
        swapcontext(&(prev->uc), &(next->uc)); /*calling the next thread*/
}

gtthread_t gtthread_self() {
  return *current;
}

void thread_run (void* (*func)(void*), void* arg) {
  /* Run the function and gtthread_exit it. */
  gtthread_exit(func(arg));
  return;
}

/*Executing the start function for thread */

/*Creating new context for a thread */
int gtthread_create (gtthread_t *thread, void *(*start_routine)(void *), void *arg)  {
	thread->id = que_size(&ready_q);
	/*Getting user context */
	getcontext(&(thread->uc));
        /*Modifying context to point to new stack */
	thread->uc.uc_link = &maincontext;
	thread->uc.uc_stack.ss_sp = malloc(SIGSTKSZ);
	thread->uc.uc_stack.ss_size = SIGSTKSZ;
	/*Creating the context */
        makecontext(&(thread->uc), (void (*) ()) thread_run, 2, start_routine, arg);
	/*Queue the thread for execution*/
	enqueue(&ready_q, thread);
	/*Start the context 
	swapcontext(&maincontext,&(thread->uc)); */
	return 0 ; /* for now */
}

void gtthread_init (long period) {

	period_t = period;
	/*Initializing thread queue*/
	queue_init(&ready_q);
	queue_init(&finish_q);

	/*Setting up timer intervals*/
	timer.it_value.tv_sec=period_t;
	timer.it_value.tv_usec=0;
	timer.it_interval.tv_sec=period_t;
	timer.it_interval.tv_usec=0;
	start_time();
	/*Setting up signal handler*/
	signal(SIGVTALRM,scheduler);

	/*Saving context for main thread*/
	main_t.id = -1;
	getcontext(&(main_t.uc));	
	current = &main_t;

}

/* Waiting for particular thread */
int  gtthread_join(gtthread_t thread, void **status) {

	/*Locating thread from queue*/
	gtthread_t* thr;
	thr = get_q_ele(&finish_q, thread);
	if (thr == NULL) return 1; /*NULL returned from queue*/

	/*Looping to check if thread is finished*/
	while (1) {
		if (!remove_q_ele(&finish_q, thr)) {
			if (status) *status = 0;
			return 1;
		} else {
			if (status) *status = thr->retval;
			return 0;
		}
	}
	return gtthread_yield();

}

/*Cancelling the thread*/
int gtthread_cancel (gtthread_t thread) {

	/* Remove the thread from the ready queue. */
	int found = 0;
	gtthread_t* thr;
	thr = get_q_ele(&ready_q, thread);
	if (thr == NULL) return 1; /*NULL returned from queue*/

	if (!remove_q_ele(&ready_q, thr)) {
		return 1;
	} else {
		found = 1;
	}

	if (gtthread_equal(thread, gtthread_self())) {
	/* If it is the current thread, reschedule. */
		cancel_current = 1;
		scheduler();
		return 0;
	}

	if (!found) {
		return 1;
	}

	return 0;

}

/* Thread Exit */
void gtthread_exit(void *retval) {
	current->retval = retval;
	enqueue(&finish_q, current);
	gtthread_cancel(gtthread_self());
	return;
}

/*Resource yielding by thread */
int gtthread_yield() {
	/* returns 0 on success */
	stop_time();
	start_time();
	scheduler();
	return 0;
}

/*To check if threads are same*/
int gtthread_equal (gtthread_t t1, gtthread_t t2) {
	return t1.id == t2.id;
}

/*Functions to manipulate mutexes
  Trying to use spin lock mechanism :D */

int gtthread_mutex_init (gtthread_mutex_t *mutex) {
	mutex->lock = 0; /*Initially it is unlocked*/
	return 0;
}

int gtthread_mutex_lock (gtthread_mutex_t *mutex) {
	stop_time();
	if (mutex->lock == 0) {
		start_time();
		mutex->lock = 1;
	} else {
		while (mutex->lock == 1) {
			start_time();
			gtthread_yield();
			stop_time();
		}
	}
	return 0;
}

int gtthread_mutex_unlock(gtthread_mutex_t *mutex) {
	mutex->lock = 0;
	return 0;
}
