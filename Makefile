CC = g++
CCFLAGS = -Wall -Wextra -std=c++2a -g -O3 -lzmq -lncursesw

all: server client fleet.o 

server: server.cc
	 $(CC) $(CCFLAGS) -o server server.cc 

client: client.cc fleet.o
	 $(CC) $(CCFLAGS) -o client client.cc fleet.o

fleet.o: fleet.h fleet.cc
	$(CC) $(CCFLAGS) -c fleet.cc

clean:
	rm -f *.o core a.out server client
