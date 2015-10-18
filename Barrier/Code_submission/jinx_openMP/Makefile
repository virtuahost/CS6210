OMPFLAGS = -fopenmp
OMPLIBS = -lgomp

CC = gcc
CPPFLAGS = -g -w -Wall $(OMPFLAGS)
LDFLAGS = -g -w -Wall
LDLIBS = $(OMPLIBS)

all: central_barrier tree_barrier

central_barrier: central_barrier.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lm

tree_barrier: tree_barrier.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lm

clean:
	rm -f *.o central_barrier tree_barrier
