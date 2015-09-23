#include "gtthread.h"
/* Just an example. Nothing to see here. */

#include <stdio.h>
#include <stdlib.h>

int gtthread_mutex_init(gtthread_mutex_t* mutex)
{
	steque_init(&mutex->q);
	mutex->locked = 0;
	mutex->t_id = -1;
	return 0;
}

int gtthread_mutex_lock(gtthread_mutex_t* mutex)
{
	if(mutex->locked == 0)
	{
		mutex->locked = 1;
		mutex->t_id = running_thread->t_id;
		return 0;		
	}
	//Queue up data for processing
	steque_enqueue(&mutex->q,&running_thread->t_id);
	//Loop till mutex is available
	while(1)
	{
		if(mutex->locked != 0)
		{
			gtthread_yield();
			continue;
		}
		else
		{
			int* process_thread_id = (int*)steque_front(&mutex->q);
			if(*process_thread_id != running_thread->t_id)
			{
				steque_pop(&mutex->q);
				mutex->locked = 1;
				mutex->t_id = running_thread->t_id;
				return 0;
			}
			else
			{
				gtthread_yield();
				continue;
			}
		}
	}

	//If it gets to this point it is a problem
	return -1;
}

int gtthread_mutex_unlock(gtthread_mutex_t* mutex)
{
	mutex->locked = 0;
	mutex->t_id = -1;
	return 0;
}

int gtthread_mutex_destroy(gtthread_mutex_t* mutex)
{
	steque_destroy(&mutex->q);
	return 0;
}

