OMPFLAGS = -fopenmp -lm

CC = gcc
CPPFLAGS = -g -Wall
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

MPICC = mpicc
MPICH = /opt/openmpi-1.4.3-gcc44/bin/mpicc
CFLAGS = -I$(MPICH)

all: tb ntb

tb: tournament_barrier.c
	$(MPICH) -o $@ $^ $(OMPLIBS)

ntb: new_treebased_barrier.c
	$(MPICH) -o $@ $^ $(OMPLIBS)
clean:
	rm -f *.o tb ntb