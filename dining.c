#include <stdio.h>
#include "gtthread.h"

gtthread_mutex_t m1, m2, m3, m4, m5;
gtthread_t t1,t2,t3,t4,t5;

void* philosopher(char c)
{
	printf("bla\n");
	while(1)
	{
		if ( c == 'A')
		{
			printf("Philosopher A is hungry\n");
			printf("Philosopher A tries to acquire m #1\n");
			gtthread_mutex_lock(&m1);
			printf("Philosopher A tries to acquire m #5\n");
			gtthread_mutex_lock(&m5);
			printf("Philosopher A is eating.\n");
			gtthread_yield();
			sleep(1);
			gtthread_mutex_unlock(&m5);
			printf("Philosopher A releases m #5\n");
			gtthread_mutex_unlock(&m1);
			printf("Philosopher A releases m #1\n");
			printf("Philosopher A is resting.\n");
			gtthread_yield();
			sleep(1);
		}
		else if ( c == 'B')
		{
			printf("Philosopher B is hungry\n");
			printf("Philosopher B tries to acquire m #1\n");
			gtthread_mutex_lock(&m1);
			printf("Philosopher B tries to acquire m #2\n");
			gtthread_mutex_lock(&m2);
			printf("Philosopher B is eating.\n");
			gtthread_yield();
			sleep(1);
			gtthread_mutex_unlock(&m2);
			printf("Philosopher B releases m #2\n");
			gtthread_mutex_unlock(&m1);
			printf("Philosopher B releases m #1\n");
			printf("Philosopher B is resting.\n");
			gtthread_yield();
			sleep(1);
		}
		else if ( c == 'C')
		{
			printf("Philosopher C is hungry\n");
			printf("Philosopher C tries to acquire m #2\n");
			gtthread_mutex_lock(&m2);
			printf("Philosopher C tries to acquire m #3\n");
			gtthread_mutex_lock(&m3);
			printf("Philosopher C is eating.\n");
			gtthread_yield();
			sleep(1);
			gtthread_mutex_unlock(&m3);
			printf("Philosopher C releases m #5\n");
			gtthread_mutex_unlock(&m2);
			printf("Philosopher C releases m #1\n");
			printf("Philosopher C is resting.\n");
			gtthread_yield();
			sleep(1);
		}
		else if ( c == 'D')
		{
			printf("Philosopher D is hungry\n");
			printf("Philosopher D tries to acquire m #3\n");
			gtthread_mutex_lock(&m3);
			printf("Philosopher D tries to acquire m #4\n");
			gtthread_mutex_lock(&m4);
			printf("Philosopher D is eating.\n");
			gtthread_yield();
			sleep(1);
			gtthread_mutex_unlock(&m4);
			printf("Philosopher D releases m #4\n");
			gtthread_mutex_unlock(&m3);
			printf("Philosopher D releases m #3\n");
			printf("Philosopher D is resting.\n");
			gtthread_yield();
			sleep(1);
		}
		else
		{
			printf("Philosopher E is hungry\n");
			printf("Philosopher E tries to acquire m #4\n");
			gtthread_mutex_lock(&m4);
			printf("Philosopher E tries to acquire m #5\n");
			gtthread_mutex_lock(&m5);
			printf("Philosopher E is eating.\n");
			gtthread_yield();
			sleep(1);
			gtthread_mutex_unlock(&m5);
			printf("Philosopher E releases m #5\n");
			gtthread_mutex_unlock(&m4);
			printf("Philosopher E releases m #4\n");
			printf("Philosopher E is resting.\n");
			gtthread_yield();
			sleep(1);
		}
	}
return;
}
void main()
{
	gtthread_init(10);
	gtthread_mutex_init(&m1);
	gtthread_mutex_init(&m2);
	gtthread_mutex_init(&m3);
	gtthread_mutex_init(&m4);
	gtthread_mutex_init(&m5);
	printf("hello\n");
	gtthread_create(&t1,philosopher, (void *)'A');
	gtthread_create(&t2,philosopher, (void *)'B');
	gtthread_create(&t3,philosopher, (void *)'C');
	gtthread_create(&t4,philosopher, (void *)'D');
	gtthread_create(&t5,philosopher, (void *)'E');
	while(1);
	gtthread_exit(NULL);
}
