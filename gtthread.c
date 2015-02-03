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
static int numThreads = 1;

queue ready_q; /* queue to store threads */

gtthread_t *main;
gtthread_t *current;

sigset_t sign_t;

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
	getcontext(&(main->uc));	
	current = main;


}

/* Waiting for particular thread */
int  gtthread_join(gtthread_t thread, void **status) {
	printf("gtthread_join work in progress\n");
	return 0;
}

/* Thread Exit */
void gtthread_exit(void *retval) {
	printf("gtthread_exit work in progress\n");
}

/*Resource yielding by thread */
int gtthread_yield() {
	/* returns 0 on success */
	return sched_yield();
}

void signal_block(){
	sigprocmask(SIG_BLOCK, &sign_t, NULL);
}

void signal_unblock(){
	sigprocmask(SIG_UNBLOCK, &sign_t, NULL);
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
