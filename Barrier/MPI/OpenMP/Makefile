OMPFLAGS = -fopenmp
OMPLIBS = -lgomp

CC = gcc
CPPFLAGS = -g -Wall $(OMPFLAGS)
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

all: hello_world priv_vars shared_vars parallel_for

hello_world: hello_world.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

priv_vars: priv_vars.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

shared_vars: shared_vars.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

parallel_for: parallel_for.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o hello_world priv_vars shared_vars parallel_for
