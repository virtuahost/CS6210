#include <omp.h>
#include <stdio.h>


int main(int argc, char **argv)
{
  int i, x = 0;

#pragma omp parallel for shared(x) lastprivate(i)
  for (i = 1; i <= 1000; ++i)
  {
#pragma omp atomic
    x += i;
  }

  printf("sum(1 - %d) = %d\n", i - 1, x);
  return 0;
}

