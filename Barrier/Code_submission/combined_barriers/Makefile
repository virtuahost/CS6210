OBJS = MPI_barrier.h central_barrier.h combined_barriers.c

OMPFLAGS = -fopenmp
OMPLIBS = -lgomp
MATH = -lm
	
CC = gcc
CPPFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)
	
MPICC = mpicc
MPICH = /usr/lib64/openmpi/bin/mpicc
	
CFLAGS = -I$(MPICH)

combined_barriers: 
			$(MPICC) $(OMPFLAGS) $(OMPLIBS) $(OBJS) $(MATH) -o combined_barriers

clean:
		rm -f *.o combined_barriers
