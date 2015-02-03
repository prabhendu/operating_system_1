#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gtthread.h"
#include <sys/time.h>

/*The "main" execution context */
static ucontext_t maincontext;

/*Count of present threads in progress
  Same is used as thread ID*/
static int readyThreads = 0;
static int finishThreads = 0;

queue ready_q, finish_t; /* queue to store threads - ready and finished */

static gtthread_t *main;
static gtthread_t *current;

/*Timer starting*/
static long period_t;
struct itimerval timer;
void start_time() {
	settimer(ITIMER_VIRTUAL,&timer,0);
}

void stop_time() {
	setitimer(ITIMER_VIRTUAL, 0, 0);
}

gtthread_t gtthread_self(void) {
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
	/*Getting user context */
	getcontext(&(thread->uc));
	if (thread->stack == 0) {
	printf("could not allocate stack memory\n");
	return -1;
	}
        /*Modifying context to point to new stack */
	thread->uc.uc_link = NULL;
	thread->uc.uc_stack.ss_sp = malloc(SIGSTKSZ);
	thread->uc.uc_stack.ss_size = SIGSTKSZ;
	thread->id = ++readyThreads;       
	/*Creating the context */
        makecontext(&(thread->uc), (void (*) ()) thread_run, 2, start_routine, arg);
	++numThreads;
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

	/*Setting up timer intervals*/
	timer.it_value.tv_sec=-0;
	timer.it_value.tv_usec=period;
	timer.it_interval.tv_sec=0;
	timer.it_interval.tv_usec=period;

	start_timer();
	/*Setting up signal handler*/
	signal(SIGVTALRM,scheduler);

	/*Saving context for main thread*/
	main->id = -1;
	getcontext(&(main->uc));	
	current = main;

}

/* Waiting for particular thread */
int  gtthread_join(gtthread_t thread, void **status) {
	printf("gtthread_join work in progress\n");
	return 0;
}

/*Cancelling the thread*/
int gtthread_cancel (gtthread_t thread) {

	/* Remove the thread from the ready queue. */
  int i = 0;
  int found = 0;
  gtthread_t* item;
  for (int i = 0; i < que_size(&ready_q); ++i) {
    item = (gtthread_t*) get_element_from_queue(&ready_queue, i);
    if (gtthread_equal(*item, thread)) {
      if (!queue_remove_element(&ready_queue, (void*) item)) {
        return 1;
      } else {
        found = 1;
        break;
      }
    }
  }
  if (gtthread_equal(thread, gtthread_self())) {
    /* If it is the current thread, reschedule. */
    cancel_current_thread = 1;
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
	printf("gtthread_exit work in progress\n");
}

/*Resource yielding by thread */
int gtthread_yield() {
	/* returns 0 on success */
	stop_time();
	start_time();
	scheduler();
}

/*To check if threads are same*/
int gtthread_equal (gtthread_t t1, gtthread_t t2) {
	return t1 == t2;
}

void scheduler () {
	gtthread_t *prev , *next = NULL;
	stop_time();
	prev = current;

	enqueue(&ready_q, prev); /*if not cancelled*/
	next = dequeue(&ready_q);
	if (next == NULL) return EXIT_SUCCESS; /*No thread present in queue*/
	current = next;
	start_timer();
	swapcontext(&(prev->uc), &(next->uc)); /*calling the next thread*/
}
