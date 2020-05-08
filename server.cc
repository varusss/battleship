#include <zmq.hpp>
#include <zhelpers.hpp>
#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include <unistd.h>
#include <unordered_map>
using namespace std;
using namespace zmq;

//To create an internet server (using ZMQ)
//1. Create a socket
//2. Bind it to a port 
//Loop:
	//3. Read from the socket
	//4. Write to the socket

const int DEFAULT_PORT = 2020; //Change this to any port above 1024
void pick_port(socket_t &sock, char *argv1);

struct User {
	string name;
	size_t last_read; //Holds last line of chat they read
};

unordered_map<string,User> users;

int main (int argc, char **argv) {
	/*
	cout << "ARGC: " << argc << endl;
	for (int i = 0; i < argc; i++) {
		cout << "ARGV[" << i << "]: " << argv[i] << endl;
	}
	*/
	vector<string> moves; //Holds a transcript of all the moves 

	//Step 1. Create a socket
	//Set up Zero MQ library to do networking for us
	context_t context (1); //Initializes ZMQ
	socket_t socket (context, ZMQ_REP); //Server-style socket
	socket.setsockopt(ZMQ_IPV6,1); //Alow IPV6

	//Step 2. Bind a socket
	//This function picks a port to open on the server
	if (argc == 2) pick_port(socket,argv[1]);
	else pick_port(socket,nullptr);

	//Steps 3-4 read and write to the socket.
	// NOTE: ZMQ mandates alternating reads and sends
	while (true) {
		//This will block until a read is made. TODO: Make this nonblocking
		string read = s_recv(socket); //s_recv is the simplest way of reading from a socket
		//Split apart this packet from the client
		stringstream sts(read); 
		string username;
		getline(sts,username,':');
		string data;
		getline(sts,data,':');
#ifdef DEBUG
		//Echo everything except heartbeats
		if (data != "")
			cout << "SERVER READ:" << read << endl;
#endif

		if (data == "LOGIN") {
			moves.push_back(username + " joined the chat.");
			if (users.count(username) == 0) { // New user
				//Get all chat
				//users.insert({username,{username,0}}); //Save their name and set their last_read to 0
				//Get last 10 chat
				size_t last = 0;
				if (moves.size() > 10) last = moves.size() - 10;
				users.insert({username,{username,last}}); //Save their name and set their last_read to 0
			}
		}
		//Handle a user disconnecting
		else if (data == "LOGOUT") {
			moves.push_back(username + " left the chat.");
		}
		else if (data == "") { //The client sent a heartbeat so update their chat
		}
		else {
			moves.push_back(read);
		}
		//No matter what command they give, update the client with all lines they've missed
		size_t last_read = users[username].last_read;
		//Build a string of all the lines they've missed from the chat log
		string temp;
		for (size_t i = last_read; i < moves.size(); i++) temp += moves.at(i) + '\n'; 
		s_send(socket,temp); //Send it to the client
		users[username].last_read = moves.size(); //Record where they last read
	}
	return 0;
}

void pick_port(socket_t& socket, char *argv1) {
	int port = DEFAULT_PORT;
	try { 
		if (argv1) port = stoi(argv1);
	} catch (...) { port = DEFAULT_PORT; }
	while (true) {
		//You can run it like this: server 2000 to bind it to a different port
		try {
			socket.bind ("tcp://*:"s + to_string(port)); //Listen on port port
		} catch (...) {
			cout << "Couldn't open a server on port " << port << ". Maybe another server is using it?\n";
			cout << "Please enter a new port number to try:\n";
			cin >> port;
			if (!cin) exit(1);
			continue;
		}
		break;
	}
}
