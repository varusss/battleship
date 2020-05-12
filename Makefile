CC = g++
CCFLAGS = -Wall -Wextra -std=c++2a -g -O3 -lzmq -lncursesw

all: server client player.o

server: server.cc
	 $(CC) $(CCFLAGS) -o server server.cc 

blockingclient: blockingclient.cc
	 $(CC) $(CCFLAGS) -o blockingclient blockingclient.cc

client: client2.cc player.o
	 $(CC) $(CCFLAGS) -o client client2.cc player.o

player.o: player.h player.cc
	g++ -c player.cc

clean:
	rm -f *.o core a.out server client
