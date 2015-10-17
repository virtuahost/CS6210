#include "gtthread.h"
/* Just an example. Nothing to see here. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static sigset_t vtalrm;

int counter = 1;

void run_thread(void *(*entry_routine)(void *),void* args)
{
	// printf("run Thread \n");
	void* ret_val = entry_routine(args);
	gtthread_exit(ret_val);
}

void schedule(int sig)
{
	// printf("Schedule Thread \n");
	ucontext_t* curr;
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	int found = -1;
	int start = (running_thread + 1 > MAX_LIMIT)?0:(running_thread+1);
	while(found < 0)
	{
		if(all_threads[start]!=NULL && all_threads[start]->running!=0)
		{
			found = start;
			break;
		}
		else if(start == running_thread)
		{
			break;
		}
		start = (start + 1 > MAX_LIMIT)?0:(start+1);
	}
	if(found < 0) exit(0);
	curr = &all_threads[running_thread]->context;
	gtthread_t* nxt = all_threads[found];
	running_thread = found;
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	swapcontext(curr,&nxt->context);
}

void gtthread_init(long period)
{
	// printf("Thread Started \n");
	gtthread_t* main_program = malloc(sizeof(gtthread_t));
	sigemptyset(&vtalrm);
	sigaddset(&vtalrm, SIGVTALRM);

	struct sigaction handler;
	memset(&handler, '\0',sizeof(handler));
	handler.sa_handler = &schedule;
	// printf("Thread Started \n");
	if(sigaction(SIGVTALRM,&handler,NULL) < 0)
	{
		printf("Error!!! \n");
		exit(1);
	}
	if(period > 0)
	{
		// printf("Setting timer \n");
		struct itimerval *T = (struct itimerval*) malloc(sizeof(struct itimerval));
	    T->it_value.tv_sec = T->it_interval.tv_sec = 0;
	    T->it_value.tv_usec = T->it_interval.tv_usec = period;

        setitimer(ITIMER_VIRTUAL, T, NULL);
	}
	main_program->running = 1;
	main_program->t_id = 0;
	main_program->j_t_id = -1;
	all_threads[main_program->t_id] = main_program;
	running_thread = main_program->t_id;
}

int gtthread_create(gtthread_t* thread,void* (*start_routine)(void *),void* arg)
{
	thread->running = 1;
	thread->t_id = counter++;
	thread->j_t_id = -1;
	// printf("Thread created id: %i \n",thread->t_id);
	if(thread->t_id >= MAX_LIMIT)
	{
		// printf('Max number of threads exceeded.');
		return -1;
	}

	if(getcontext(&thread->context)<0)
	{
		// printf("No context found.");
		return -1;
	}
	// printf("Thread Create \n");
	thread->context.uc_stack.ss_sp = (char*)malloc(SIGSTKSZ);
	thread->context.uc_stack.ss_size = SIGSTKSZ;
	thread->context.uc_link = 0;

	// printf("Thread Context \n");
	makecontext(&thread->context,run_thread,2,start_routine,arg);

	all_threads[thread->t_id] = thread;
	return 0;
}

int gtthread_join(gtthread_t thread,void **status)
{
	gtthread_t* temp;
	gtthread_t* mainT;
	if(gtthread_equal(thread,gtthread_self()))
	{
		return -1;
	}		
	temp = all_threads[thread.t_id];
	mainT = all_threads[running_thread];
	if(mainT->j_t_id == temp->t_id)
	{
		printf("Deadlock scenario \n");
		return -2;
	}
	temp->j_t_id = mainT->t_id;
	// printf("Yield: ");
	while(temp->running != 0)
	{
		gtthread_yield();
		// printf("Yield \n: %i", temp->running);
	}
	// printf("Status: ");
	// printf(status);
	if(status != NULL)
	{		
		*status = temp->retVal;
	}
	return 0;
}

void gtthread_yield(void)
{
	raise(SIGVTALRM);
}

int gtthread_equal(gtthread_t t1, gtthread_t t2)
{
	return (t1.t_id == t2.t_id);
}

int gtthread_cancel(gtthread_t thread)
{
	all_threads[thread.t_id]->running = 0;
	return 0;
}

gtthread_t gtthread_self(void)
{
	return *all_threads[running_thread];
}

void gtthread_exit(void* retVal)
{
	// printf("Exiting Thread");
	all_threads[running_thread]->retVal = retVal;
	all_threads[running_thread]->running = 0;
	raise(SIGVTALRM);
}