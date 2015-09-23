#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"
long period = 0; //100;
gtthread_t t[100];
gtthread_mutex_t m[100];
int hold[100];
void *status = (void*)100;
void *thrR(void *in)
{
    int ndx;
    int th = (int)(long)in;
    printf("thrR: enter\n"); fflush(stdout);
    for(ndx=0; ndx<100; ++ndx) {
        gtthread_mutex_lock(&m[0]);
printf("thrR: lock %d\n",th); fflush(stdout);
        usleep(100*1000);
printf("thrR: unlock %d\n",th); fflush(stdout);
        gtthread_mutex_unlock(&m[0]);
    }
    printf("thrR: exit\n"); fflush(stdout);
    return in;
}
void *thrX(void *in)
{
    int ndx;
    int th = (int)(long)in;
    int dur = (int)hold[th];
    printf("thrX: enter\n"); fflush(stdout);
    for(ndx=0; ndx<10; ++ndx) {
        gtthread_mutex_lock(&m[0]);
printf("thrX: lock %d\n",th); fflush(stdout);
        gtthread_yield();
        usleep(dur*1000);
printf("thrX: unlock %d\n",th); fflush(stdout);
        gtthread_mutex_unlock(&m[0]);
        gtthread_yield();
    }
    printf("thrX: exit\n"); fflush(stdout);
    gtthread_exit(in);
    return in;
}
int main()
{
    int rc;
    int rep;
    int ndx;
    printf("test mutex init, lock, unlock, destroy\n");
    for( ndx=0; ndx<100; ++ndx ) {
        hold[ndx] = 100+(ndx%10)*10;
    }
    printf("main: enter\n");
    gtthread_init(period);
    for( ndx=0; ndx<10; ++ndx ) {
        rc = gtthread_mutex_init(&m[ndx]);
        printf("init %d\n",ndx); fflush(stdout);
        if(rc)printf("rc = %d\n",rc); fflush(stdout);
    }
    for( rep=0; rep<10; ++rep ) {
      for( ndx=0; ndx<10; ++ndx ) {
        rc = gtthread_mutex_lock(&m[ndx]);
        printf("lock(%d)\n",ndx); fflush(stdout);
        if(rc)printf("rc = %d\n",rc); fflush(stdout);
        usleep(10*1000);
        rc = gtthread_mutex_unlock(&m[ndx]);
        printf("unlock(%d)\n",ndx); fflush(stdout);
        if(rc)printf("rc = %d\n",rc); fflush(stdout);
      }
    }
    for( ndx=0; ndx<10; ++ndx ) {
        rc = gtthread_mutex_destroy(&m[ndx]);
        printf("destroy(%d)\n",ndx); fflush(stdout);
        if(rc)printf("rc = %d\n",rc); fflush(stdout);
    }
    printf("main: exit\n");
    fflush(stdout);
    return EXIT_SUCCESS;
}