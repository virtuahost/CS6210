#include "gtthread.h"
/* Just an example. Nothing to see here. */

#include <stdio.h>
#include <stdlib.h>

int gtthread_mutex_init(gtthread_mutex_t* mutex)
{
	mutex->locked = 0;
	mutex->t_id = -1;
	return 0;
}

int gtthread_mutex_lock(gtthread_mutex_t* mutex)
{
	// printf("Checking Mutex \n");
	if(mutex->locked == 0)
	{
		mutex->locked = 1;
		mutex->t_id = all_threads[running_thread]->t_id;
		return 0;		
	}	
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
			mutex->locked = 1;
			mutex->t_id = all_threads[running_thread]->t_id;
			return 0;
		}
	}

	//If it gets to this point it is a problem
	return -1;
}

int gtthread_mutex_unlock(gtthread_mutex_t* mutex)
{
	// printf("Unlocking \n");
	if(mutex->t_id != all_threads[running_thread]->t_id)
	{
		mutex->locked = 0;
		mutex->t_id = -1;
	}
	return 0;
}

