#include <omp.h>
#include <stdio.h>

#define NUM_THREADS 3


int main(int argc, char **argv)
{
  // Serial code
  printf("This is the serial section\n");
  omp_set_num_threads(NUM_THREADS);

#pragma omp parallel
  {
    // Now we're in the parallel section
    int num_threads = omp_get_num_threads();
    int thread_num = omp_get_thread_num();
    printf("Hello World from thread %d of %d.\n", thread_num, num_threads);
  } // implied barrier

  // Resume serial code
  printf("Back in the serial section again\n");
  return 0;
}

