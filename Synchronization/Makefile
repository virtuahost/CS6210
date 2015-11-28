CC = g++
CPP = -std=c++11
DHAVE = -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
CPPFLAGS = -g -Wall
THRIFT = -I/usr/local/include/thrift
LOC = -L/usr/local/	lib
LFLAGS = -lthrift -lcurl

all: servertest url_processing gen-cpp/URLproxy	gen-cpp/server_types gen-cpp/server_constants 

server: server.o
	$(CC) $(CPP) $(DHAVE) $(CPPFLAGS) $(THRIFT) -o $@ $^ $(LOC) $(LFLAGS)

clean:
	rm -f *.o servertest url_processing gen-cpp/URLproxy gen-cpp/server_types gen-cpp/server_constants