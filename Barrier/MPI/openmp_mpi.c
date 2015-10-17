#include <stdio.h>
#include <omp.h>
#include "mpi.h"

#define NUM_THREADS 3


int main(int argc, char **argv)
{
  int my_id, num_processes, thread_num, num_threads;
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  omp_set_num_threads(NUM_THREADS);

#pragma omp parallel private(thread_num, num_threads)
  {
    num_threads = omp_get_num_threads();
    thread_num = omp_get_thread_num();

    printf("Hello World from thread %d:%d of %d procs and %d threads.\n",
        my_id, thread_num, num_processes, num_threads);
  } // implied barrier

  MPI_Finalize();
  return 0;
}

