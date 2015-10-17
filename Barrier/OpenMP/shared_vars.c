#include <omp.h>
#include <stdio.h>


int main(int argc, char **argv)
{
  int thread_num = -1, priv = 0, pub = 0;

#pragma omp parallel num_threads(5) firstprivate(thread_num, priv) shared(pub)
  {
    thread_num = omp_get_thread_num();
    printf("thread %d: hello\n", thread_num);

#pragma omp critical
    {
      priv += thread_num;
      pub += thread_num;
    }

    printf("thread %d: before barrier value of pub = %d\n", thread_num, pub);

#pragma omp barrier

    printf("thread %d: final value of pub = %d\n", thread_num, pub);

#pragma omp barrier

    printf("thread %d: final value of priv = %d\n", thread_num, priv);
  }

  return 0;
}

