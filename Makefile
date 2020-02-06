CC=g++
CFLAGS=-c -std=c++11 -g -Wall
LDFLAGS=
SOURCES=server.cpp client.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLES=client server

all: client server
    
server.o: server.cpp
	$(CC) $(CFLAGS) server.cpp -o $@

server: server.o
	$(CC) $(LDFLAGS) server.o -o $@

client.o: client.cpp
	$(CC) $(CFLAGS) client.cpp -o $@

client: client.o
	$(CC) $(LDFLAGS) client.o -o $@

clean: 
	rm $(OBJECTS) $(EXECUTABLES)
