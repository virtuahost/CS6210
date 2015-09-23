
#include "gtthread.h"
#include <stdio.h>
#include <stdlib.h>

gtthread_t a,b,c,d;
int flag = 0;
void *funct_d(void *arg)
{
	printf("printing d \n");
	while(1) if(count>20) break; 	
	printf("\n \t\t\t\t\t thread %i leaving ...  \n", gtthread_self());
	return (void*) 40;
}


void *funct_a(void *arg)
{
	printf("Printing a; \n");
	while(1)
	{
		if(count>12 && count<16) 
		{
			gtthread_create(&d, funct_d, NULL);
			flag = 1;
			break;
		}	
	}	
	while(1);
	return (void*) 10;
}

void *funct_b(void *arg)
{
	printf("Printing b; \n");
	void *x;
	gtthread_join(c, &x);
	printf("\n Thread 3 returned : %i \n", (int) x);

	while(1);
	return (void*) 20;
}

void *funct_c(void *arg)
{
	printf("Printing c; \n");
	while(1)
	{
		if(count>3 && count<8) gtthread_exit((void*)35);
	}
	return (void*) 30;
}

int main(void)
{
	void *x;
	count = 0;
	flag = 0;
	printf("Started \n");
	gtthread_init(1);
	gtthread_create(&a, funct_a, NULL);
	gtthread_create(&b, funct_b, NULL);
	gtthread_create(&c, funct_c, NULL);

	while(!flag);
	gtthread_join(d, &x);
	printf("\n Thread 4 finished with return value : %i \n", (int) x);
	while(1);
	return 0;
}