#include<stdio.h>
#include<math.h>
#include<omp.h>
#include<time.h>

// *******************************************************************************************************
// Data Structures
// *******************************************************************************************************
typedef struct Data
{
	int count;
	int k;
	int max_count;
	int sense1;
	int sense2;
} Data;

int number_of_proc, levels;
Data *d;

// *******************************************************************************************************
// Function Prototypes
// *******************************************************************************************************
void tree_barrier_init(int);
void compute_max_counts(void);
void compute_levels(void);
void tree_barrier(int);
void wait_on_up(int);
void wait_on_down(int);

// *******************************************************************************************************
// Function Implementations
// *******************************************************************************************************
void tree_barrier_init(int proc)
{
//	printf("\n Reached init MP \n");
	int i;
	d = malloc(sizeof(Data) * proc);	
	number_of_proc = proc;
	for(i=0; i<proc; i++)
	{
		d[i].k = 0;
		d[i].count = 0;
		d[i].sense1 = 0;
		d[i].sense2 = !d[i].sense1;
	}
	compute_levels();
	compute_max_counts();
	
////	 test
//	for(i=0; i<number_of_proc; i++)
//	{
//		printf("\n Max count for Proc %i is %i", i, d[i].max_count);
//	}
}

void compute_max_counts()
{
	int i, k = 0, count = 0;
	while(k < levels)
	{
		for(i=0; i<number_of_proc; i++)
		{	
			if(i % (int)(pow(2,k)) == 0) d[i].max_count = count;
		}
		count++;
		k++;
	}
//	d[0].max_count = levels + 1;
}

void compute_levels()
{
	levels = (int)ceil(log2(number_of_proc));
//	printf("\n\n Levels : %i\n\n", levels);
}

void tree_barrier(int proc_id)
{
	#pragma omp critical	
	d[proc_id].sense2 = !d[proc_id].sense1;
//	printf("\n \t\t -----------------> Proc %i before UP \n", proc_id);
	wait_on_up(proc_id);
//	printf("\n \t\t -----------------> Proc %i before DOWN \n", proc_id);
	wait_on_down(proc_id);	
//	d[proc_id].leave_barrier = 0;
	#pragma omp critical
	{
		d[proc_id].sense2 = !d[proc_id].sense1;
		d[proc_id].count = 0;
		d[proc_id].k = 0;
	}
		
	
}

void wait_on_up(int i)
{
	while(d[i].k <= d[i].max_count)
	{
		while(d[i].count == d[i].k)
		{
			if((i % (int)pow(2, d[i].k+1)) != 0)
			{
				#pragma omp critical
				d[i - (int)pow(2, d[i].k)].count++;
				break;
			}
			else
			{
				int index = i + (int)pow(2, d[i].k);
				if(index >= number_of_proc)
					#pragma omp critical
					d[i].count++;
			}
		}
		d[i].k++;
	}		
}

void wait_on_down(int i)
{
	while((d[i].sense1 != d[i].sense2) && (i != 0));
	while(d[i].k > 0)
	{
		int index = i + (int)pow(2, d[i].k-1);
		if(index < number_of_proc)
		{
			#pragma omp critical
			d[index].sense2 = d[index].sense1;
		}
		d[i].k--;
	}
}



















