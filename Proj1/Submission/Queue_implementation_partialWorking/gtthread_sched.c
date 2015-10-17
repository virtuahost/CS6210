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
	// printf("run Thread \n");
	void* ret_val = entry_routine(args);
	if((running_thread->t_id == 0)&&(running_thread->running == 0)&&(steque_size(&thread_queue)==0))
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
	int i = 1;
	for(i = 1; i < MAX_LIMIT; i++)
	{
		if(all_threads[i] != NULL && all_threads[i]->running == 0)
		{
			return i;
		}
		else if(all_threads[i] == NULL)
		{
			return i;
		}
	}
	return (MAX_LIMIT+1);
}

void schedule(int sig)
{
	// printf("Schedule Thread \n");
	ucontext_t* curr;
	sigprocmask(SIG_BLOCK,&vtalrm,NULL);
	if(steque_isempty(&thread_queue))
	{
		// printf("Switching to Main. \n");
		if(running_thread->running == 0)
		{
			if(running_thread->t_id == 0)
			{
				exit(0);
			}
			curr = &running_thread->context;
			running_thread = &main_program;
			sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
			count++;
			swapcontext(curr,&main_program.context);
		}
		// printf(running_thread->running);
		sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
		return;
	}
	// printf("Creating context");
	gtthread_t* nxt_thrd = steque_pop(&thread_queue);
	// if((nxt_thrd->t_id == 0)&&(steque_size(&thread_queue)==0)&& (main_program.running == 0))
	// {	
	// 	//nxt_thrd = steque_pop(&thread_queue);
	// 	exit(0);
	// }
	// else if((nxt_thrd->t_id == 0)&&(nxt_thrd->running == 0)&&(steque_size(&thread_queue)>0))
	// {
	// 	steque_enqueue(&thread_queue,nxt_thrd);
	// 	nxt_thrd = steque_pop(&thread_queue);
	// }
	// else if(nxt_thrd->running == 0)
	// {

	// }

	if(running_thread != NULL && running_thread->running != 0)
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
	// printf("%i %i \n", nxt_thrd->t_id, nxt_thrd->running);
	sigprocmask(SIG_UNBLOCK,&vtalrm,NULL);
	count++;
	swapcontext(curr,&running_thread->context);
}

void gtthread_init(long period)
{
	// printf("Thread Started \n");
	steque_init(&thread_queue);
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
	    T->it_value.tv_usec = T->it_interval.tv_usec = period*1000;

        setitimer(ITIMER_VIRTUAL, T, NULL);
	}
	main_program.running = 1;
	main_program.t_id = 0;
	main_program.j_t_id = -1;
	all_threads[main_program.t_id] = &main_program;
	steque_enqueue(&thread_queue, &main_program);
	running_thread = &main_program;
}

int gtthread_create(gtthread_t* thread,void* (*start_routine)(void *),void* arg)
{
	thread->running = 1;
	thread->t_id = find_thread_id();
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
	thread->context.uc_link = &main_program.context;

	// printf("Thread Context \n");
	makecontext(&thread->context,run_thread,2,start_routine,arg);

	all_threads[thread->t_id] = thread;
	sigprocmask(SIG_BLOCK, &vtalrm, NULL);
	steque_enqueue(&thread_queue, thread);
	sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
	return 0;
}

int gtthread_join(gtthread_t thread,void **status)
{
	gtthread_t* temp;
	if(gtthread_equal(thread,gtthread_self()))
	{
		return -1;
	}		
	temp = all_threads[thread.t_id];
	if(running_thread->j_t_id == temp->t_id)
	{
		printf("Deadlock scenario \n");
		return -2;
	}
	temp->j_t_id = running_thread->t_id;
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

int gtthread_yield(void)
{
	raise(SIGVTALRM);
	return 0;
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
	// printf("Exiting Thread");
	running_thread->retVal = retVal;
	running_thread->running = 0;
	raise(SIGVTALRM);
}