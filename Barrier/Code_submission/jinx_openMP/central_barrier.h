#include<omp.h>
#include<stdio.h>
#include<time.h>

// **************************************************************************************************************
// Global Variables
// **************************************************************************************************************
unsigned int total_threads = 0;
unsigned int count = 0;
unsigned int sense = 1;

// **************************************************************************************************************
// Function Prototypes
// **************************************************************************************************************
void central_barrier_init(unsigned int);
void central_barrier(unsigned int *);

// **************************************************************************************************************
// Function Implementations
// **************************************************************************************************************
void central_barrier_init(unsigned int threads)
{
	count = threads;
	total_threads = threads;
}

void central_barrier(unsigned int *local_sense)
{
	#pragma omp critical
	{
		count--;
		*local_sense = !(*local_sense);
	}

	while(sense != *local_sense)
	{
		#pragma omp critical
		{
			if(count == 0)
			{
				count = total_threads;
				sense = *local_sense; 
			}
		}	
			
	}
}

