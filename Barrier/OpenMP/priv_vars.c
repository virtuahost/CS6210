#include <omp.h>
#include <stdio.h>


int main(int argc, char **argv)
{
  int thread_num = -1, x = 100;

  printf("Serial thread: thread_num = %d; x = %d\n", thread_num, x);

#pragma omp parallel num_threads(3) private(thread_num) firstprivate(x)
  {
    // value of thread_num is indeterminate here
    printf("thread %d: before initialization\n", thread_num);

    thread_num = omp_get_thread_num(); // private variable initialization
    printf("thread %d: after initialization\n", thread_num);

    // however, value of x is determined
    printf("thread %d: x = %d\n", thread_num, x);

    // proving that x is private:
    int i; // i is private because it is declared here
    for (i = 0; i < 3; ++i)
    {
      x += thread_num + 1;
      printf("thread %d: x = %d\n", thread_num, x);
    }

#pragma omp master
    {
      // you can use "omp master" when you want only one thread to do something
      printf("thread %d: I am the master.\n", thread_num);
    }
  }

  // i is out of scope
  // values of thread_num and x are indeterminate
  printf("Serial thread: thread_num = %d; x = %d\n", thread_num, x);
  return 0;
}

