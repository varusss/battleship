//NCURSES + ZMQ chat client
//by Kerney
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <iostream>
#include "map.h"
using namespace std;
using namespace zmq;

//My cool color header
#include "/public/colors.h"

//NCURSES headers
#include <locale.h>
#include <curses.h>
#include <unistd.h>

string username;
socket_t *server;
void bail_out(int s = 0) {
	s = s; //Disable warning
	clear();
	endwin(); //Turn off NCURSES

	cout << username << " has left the chat." << endl;
	if (server and username != "") {
		s_send(*server,username + ":LOGOUT");
		usleep(500'000); //Let the message time to go out
	}
	exit(EXIT_SUCCESS);
}

//To make a client using ZMQ
//Step 1. Make a socket
//Step 2. Connect to server (hostname+port)
//In a loop:
	//Step 3. Send to server
	//Step 4. Receive from server

//I decided to use colons as field separators, so no colons can be used in strings
bool hascolon(const string &s) {
	for (auto &c : s) if (c == ':') return true;
	return false;
}

//Run the program like this: "client hostname portname"
//Example "client 55.22.11.78 2001"
//Or "client 2001" to use localhost as the host
int main (int argc, char **argv)
{
	//Step 1. Make a socket
	context_t context (1);
	socket_t socket (context, ZMQ_REQ);
	socket.setsockopt(ZMQ_IPV6,1);
	server = &socket; //Get pointer for signal handler
	signal(SIGINT,bail_out); //Disconnect nicely
	//atexit(bail_out); 

	//Step 2. Connect to server
	string hostname = "localhost";
	if (argc > 2) hostname = argv[1];
	const int DEFAULT_PORT = 2020; //Change this to any port above 1024
	int port = DEFAULT_PORT;
	//You can run it like this: client 2000 to bind it to a different port
	try {
		if (argc == 2) port = stoi(argv[1]);
		else if (argc > 2) port = stoi(argv[2]);
	} catch (...) { port = DEFAULT_PORT; }

	cout << "Connecting to chat server..." << endl;
	socket.connect ("tcp://"s + hostname + ":" + to_string(port));
	cout << "Please enter your name:\n";
	getline(cin,username);
	if (hascolon(username)) {
		cout << "No username can have colons.\n";
		exit(EXIT_FAILURE);
	}

	//Step 3. Send data to the server - login
	string str = username + ":LOGIN";
	s_send(socket,str);

	//Set up NCURSES
	if (setlocale(LC_ALL, "") == NULL) {
		cout << "Couldn't set locale.\n";
		exit(EXIT_FAILURE);
	}

	Map map;
	initscr();
	noecho();
	cbreak(); //Disable input buffering since I will do this myself
	//curs_set(0); //Hide the cursor
	keypad(stdscr,TRUE); //Enable extended input from NCURSES
	timeout(0); //Nonblocking IO
	const int MAXFPS = 60; //Cap framerate at 30fps
	mousemask(ALL_MOUSE_EVENTS,NULL); //Enable mouse support
	timeout(10);
	int x = 5, y=5;

	while(true){
		int ch = wgetch(stdscr);
		if (ch == 'q' || ch == 'Q') break;
		map.draw(x,y);
		//mvprintw(11,11 ,"x: %i y: %i\n", x,y);

        if (ch == ERR) { //No keystroke
            ; //Do nothing
        }
		//TODO: Add mouse support, I'm commenting this out for now
		else if(ch == KEY_MOUSE) { //Handle mouse events
			MEVENT mouse;
			if (OK == getmouse(&mouse)) {
				move(24,0);
				printw("Cursor Location: %i %i", mouse.y, mouse.x);
				int ship_size = 3;
				map.change(mouse.y,mouse.x,ship_size, 0);
			}
			else printw("BAD EVENT");
			refresh();
			usleep(4000);
		}
		refresh();
		usleep(5000);
	}


	endwin(); //Turn off NCURSES
	return 0;
}
