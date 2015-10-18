#include"tree_barrier.h"


int main(int argc, char **argv)
{
	int i, no_of_threads = -1, iterations = 50;
	no_of_threads = atoi(argv[1]);
	double start, end, time, total_time = 0;
	tree_barrier_init(no_of_threads);
	for(i=0; i<iterations; i++)
	{
		
/*		printf("\n -------------------------------------------------------------------------------------------");*/
/*		printf("\n NUMBER OF THREADS : %d\n", no_of_threads);*/
	
		#pragma omp parallel num_threads(no_of_threads) 
		{
/*			printf("\n Thread %d : entering barrier >>>>>", omp_get_thread_num());*/
		
		
			start = omp_get_wtime();
			tree_barrier(omp_get_thread_num());
			end = omp_get_wtime();
		
/*			printf("\n Thread %d : \t\t\t\t>>>>> leaving barrier", omp_get_thread_num());*/
		
/*			#pragma omp barrier*/
		}

/*		printf("\n -------------------------------------------------------------------------------------------");*/
	
		time = (end - start);
		total_time += time;
	}
	printf("\n -------------------------------------------------------------------------------------------");
	printf("\n\n *** Average Barrier Run Time = %f *** \n", total_time/iterations);
	printf("\n");
	
	printf("\n -------------------------------------------------------------------------------------------");
	printf("\n\n");
  	return 0;
}

