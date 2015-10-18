#include"central_barrier.h"
#include"MPI_barrier.h"


int main(int argc, char** argv)
{
  int num_processes, no_of_threads, id;
  int number;
  double start, end;

// ***********************************************************************************************************
// ***********************************************************************************************************
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);   
  // num_processes = atoi(argv[0]); 
  ntbarrier_init(num_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  start = MPI_Wtime();
  printf("Started process id: %i at time %f \n",id, start);
  
// ***********************************************************************************************************
	no_of_threads = atoi(argv[2]);
	central_barrier_init(no_of_threads);
		
	#pragma omp parallel num_threads(no_of_threads) 
	{
		int my_sense = MPsense;
		printf("\n Thread %d of process %i : entering barrier >>>>>", omp_get_thread_num(), id);

		central_barrier(&my_sense);
/*		#pragma omp barrier*/
	
		printf("\n Thread %d of process %i : \t\t\t\t>>>>> leaving barrier", omp_get_thread_num(), id);
	
/*		#pragma omp barrier*/
	}
	
// ***********************************************************************************************************
  
  ntbarrier_barrier(id);
// ***********************************************************************************************************
// ***********************************************************************************************************

  end = MPI_Wtime();
  printf("\nFinished process id: %i at time %f \n",id, end);
  printf("Time taken by process id: %i is %f \n",id,(end - start));

  MPI_Finalize(); 
  ntbarrier_finalize(); 

  return(0);
}

/*int main(int argc, char **argv)*/
/*{*/
/*	int i, no_of_threads = -1, no_of_proc = -1, iterations = 1;*/
/*	no_of_threads = atoi(argv[1]);*/
/*	double start, end, time, total_time = 0;*/

/*	ntbarrier_init(no_of_proc);*/
/*	tree_barrier_init(no_of_threads);*/
/*	for(i=0; i<iterations; i++)*/
/*	{*/
/*		*/
/*		printf("\n -------------------------------------------------------------------------------------------");*/
/*		printf("\n NUMBER OF THREADS : %d\n", no_of_threads);*/
/*	*/
/*		#pragma omp parallel num_threads(no_of_threads) */
/*		{*/
/*			printf("\n Thread %d : entering barrier >>>>>", omp_get_thread_num());*/
/*		*/
/*		*/
/*			start = omp_get_wtime();*/
/*			tree_barrier(omp_get_thread_num());*/
/*			end = omp_get_wtime();*/
/*		*/
/*			printf("\n Thread %d : \t\t\t\t>>>>> leaving barrier", omp_get_thread_num());*/
/*		*/
/*			#pragma omp barrier*/
/*		}*/

/*		printf("\n -------------------------------------------------------------------------------------------");*/
/*	*/
/*		time = (end - start);*/
/*		total_time += time;*/
/*	}*/
/*	printf("\n -------------------------------------------------------------------------------------------");*/
/*	printf("\n\n *** Average Barrier Run Time = %f *** \n", total_time/iterations);*/
/*	printf("\n");*/
/*	*/
/*	printf("\n -------------------------------------------------------------------------------------------");*/
/*	printf("\n\n");*/
/*  	return 0;*/
/*}*/

