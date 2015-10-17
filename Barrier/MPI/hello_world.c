#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv)
{
  int my_id, num_processes;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  printf("Hello World from processes %d of %d\n", my_id, num_processes);

  MPI_Finalize();
  return 0;
}

