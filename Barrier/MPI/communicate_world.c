#include <stdio.h>
#include "mpi.h"


int main(int argc, char **argv)
{
  int my_id, my_dst, my_src, num_processes;
  int tag = 1;
  int my_msg[2];
  MPI_Status mpi_result;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  my_dst = (my_id + 1) % num_processes;
  my_src = (my_id - 1) % num_processes;
  while (my_src < 0)
    my_src += num_processes;

  printf("%i\n", my_id);
  printf("%i\n", my_dst);
  printf("%i\n", my_src);
  my_msg[0] = my_id;
  my_msg[1] = num_processes;

  // Be careful of deadlock when using blocking sends and receives!
  MPI_Send(&my_msg, 2, MPI_INT, my_dst, tag, MPI_COMM_WORLD);
  MPI_Recv(&my_msg, 2, MPI_INT, my_src, tag, MPI_COMM_WORLD, &mpi_result);
  
  printf("proc %d: received message from proc %d of %d\n", my_id, my_msg[0], my_msg[1]);

  MPI_Finalize();
  return 0;
}

