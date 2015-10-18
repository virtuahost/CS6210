#include"central_barrier.h"

int main(int argc, char **argv)
{
	int i, no_of_threads = -1, iterations = 50; 
	no_of_threads = atoi(argv[1]);
	double start, end, time, total_time = 0;
	
	central_barrier_init(no_of_threads);
/*	printf("\n -------------------------------------------------------------------------------------------");*/
/*	printf("\n NUMBER OF THREADS : %d\n", no_of_threads);*/
	
	for(i=0; i<iterations; i++)
	{	
		#pragma omp parallel num_threads(no_of_threads)
		{
			int my_sense = sense;
/*			printf("\n Thread %d : entering barrier >>>>>", omp_get_thread_num());*/

			start = omp_get_wtime();
			central_barrier(&my_sense);
			end = omp_get_wtime();
			
/*			printf("\n Thread %d : \t\t\t\t>>>>> leaving barrier ", omp_get_thread_num());*/
		
/*			printf("\n Thread %d : entering barrier 2 >>>>>", omp_get_thread_num());*/

	/*		start2 = clock();*/
	/*		central_barrier(&my_sense);*/
	/*		end2 = clock();*/
	/*		*/
/*			printf("\n Thread %d : \t\t\t\t>>>>> leaving barrier 2", omp_get_thread_num());*/
		}
/*		printf("\n -------------------------------------------------------------------------------------------");*/
		
		time = (end - start);
	/*	float time2 = (float)(end2 - start2) / CLOCKS_PER_SEC;	*/
		total_time += time;
	}
		
	printf("\n\n *** Average Barrier Run Time = %f*** \n", total_time/iterations);
/*	printf("\n\t - Barrier 2 : %f", time2);*/
	printf("\n");
	
	printf("\n -------------------------------------------------------------------------------------------");
	printf("\n\n");
  	return 0;
}

