#include "gtthread.h"
/* Just an example. Nothing to see here. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static sigset_t vtalrm;

void run_thread(void *(*entry_routine)(void *),void* args)
{
	void* ret_val = entry_routine(args);
	if(main_program.t_id == running_thread->t_id)
	{
		exit(*(int*)ret_val);
	}
	else
	{
		gtthread_exit(ret_val);
	}
}

int find_thread_id()
{
	int i = 0;
	for(i = 0; i < MAX_LIMIT; i++)
	{
		if(all_threads[i]->running == 0)
		{
			return i;
		}
	}
	return (MAX_LIMIT+1);
}

void schedule(int sig)
{
	ucontext_t* curr;
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	if(steque_isempty(&thread_queue))
	{
		if(running_thread->running == 0)
		{
			curr = &running_thread->context;
			running_thread = &main_program;
			sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
			swapcontext(curr,&main_program.context);
		}
		sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
		return;
	}

	gtthread_t* nxt_thrd = steque_pop(&thread_queue);
	if(running_thread->running != 0)
	{
		steque_enqueue(&thread_queue,running_thread);
	}

	if(running_thread != NULL)
	{
		curr = &running_thread->context;
	}
	else
	{
		curr = &main_program.context;	
	}
	running_thread = nxt_thrd;
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	swapcontext(curr,&running_thread->context);
}

void gtthread_init(long period)
{
	steque_init(&thread_queue);
	sigemptyset(&vtalrm);
	sigaddset(&vtalrm, SIGVTALRM);

	struct sigaction handler;
	memset(&handler, '\0',sizeof(handler));
	handler.sa_handler = &schedule;

	if(sigaction(SIGVTALRM,&handler,NULL) < 0)
	{
		exit(1);
	}
	if(period > 0)
	{
		struct itimerval *T = (struct itimerval*) malloc(sizeof(struct itimerval));
	    T->it_value.tv_sec = T->it_interval.tv_sec = 0;
	    T->it_value.tv_usec = T->it_interval.tv_usec = 1000;

        setitimer(ITIMER_VIRTUAL, T, NULL);
	}
	main_program.t_id = -100;
	running_thread = &main_program;
}

int gtthread_create(gtthread_t* thread,void* (*start_routine)(void *),void* arg)
{
	thread->running = 1;
	thread->t_id = find_thread_id();

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

	thread->context.uc_stack.ss_sp = (char*)malloc(SIGSTKSZ);
	thread->context.uc_stack.ss_size = SIGSTKSZ;
	thread->context.uc_link = &main_program.context;

	makecontext(&thread->context,run_thread,2,start_routine,arg);

	all_threads[thread->t_id] = thread;
	sigprocmask(SIG_BLOCK, &vtalrm, NULL);
	steque_enqueue(&thread_queue, thread);
	sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);

	return 0;
}

int gtthread_join(gtthread_t thread,void **status)
{
	if(gtthread_equal(thread,gtthread_self()))
	{
		return -1;
	}		
	if(running_thread->j_t_id != thread.t_id)
	{
		// printf('Deadlock scenario');
		return -2;
	}
	thread.j_t_id = running_thread->t_id;

	while(thread.running != 0)
	{
		gtthread_yield();
	}
	if(status != NULL)
	{
		*status = thread.retVal;
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
	return *running_thread;
}

void gtthread_exit(void* retVal)
{
	running_thread->retVal = retVal;
	running_thread->running = 0;
	raise(SIGVTALRM);
}