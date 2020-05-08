CC = g++
CCFLAGS = -Wall -Wextra -std=c++2a -g -O3 -lzmq -lncursesw

<<<<<<< HEAD
all: server client blockingclient
=======
all: server client fleet.o player.o
>>>>>>> 43cc84802d51a82b6799aaaff760a5f1b4663637

server: server.cc
	 $(CC) $(CCFLAGS) -o server server.cc 

<<<<<<< HEAD
client: client.cc
	 $(CC) $(CCFLAGS) -o client client.cc

blockingclient: blockingclient.cc
	 $(CC) $(CCFLAGS) -o blockingclient blockingclient.cc
=======
client: client.cc fleet.o player.o
	 $(CC) $(CCFLAGS) -o client client.cc fleet.o player.o

fleet.o: fleet.h fleet.cc
	g++ -c fleet.cc
player.o: player.h player.cc
	g++ -c player.cc
>>>>>>> 43cc84802d51a82b6799aaaff760a5f1b4663637

clean:
	rm -f *.o core a.out server client
