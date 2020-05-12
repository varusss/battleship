//NCURSES + ZMQ chat client
//by Kerney
#include <zmq.hpp>
#include <map>
#include <zhelpers.hpp>
#include <string>
#include <queue>
#include <sstream>
#include <iostream>
using namespace std;
using namespace zmq;

//My cool color header
#include "/public/colors.h"

//NCURSES headers
#include <locale.h>
#include <curses.h>
#include <unistd.h>

const int DEFAULT_PORT = 3030; //Change this to any port above 1024
string username; //Our username on the server
socket_t *server; //I don't use this except here
void bail_out(int s = 0) {
	s = s; //Disable warning
	clear();
	endwin(); //Turn off NCURSES
	cout << username << " has left the chat." << endl;
	if (server and username != "") {
		s_send(*server,username + ":LOGOUT");
		usleep(1'000'000); //Let the message time to go out
	}
	exit(EXIT_SUCCESS);
}

void showWelcomeScreen() {
	def_prog_mode();
	endwin();
	system("clear && figlet Welcome to Battlesloop 41! && sleep 0");
	printw("\n");
	reset_prog_mode();
	refresh();
}

void showPlayerMaps(vector<vector<char>> &MY_MAP, vector<vector<char>> &ENEMY_MAP) {
	// Print my map first.
	printw("        My Map       \n");
	printw("  A B C D E F G H I J\n");

	const char ZERO = '0';
	for (int i = 0; i < MY_MAP.size(); i++) {
		addch(i+ZERO);
		for (int j = 0; j < MY_MAP[i].size(); j++) {
			//			printw(int(i));
			addch(' ');
			addch(MY_MAP[i][j]);
		}
		printw("\n");
	}

	// Print enemy map
	move(1,25);
	printw("       Enemy Map        \n");
	move(2,25);
	printw("  A B C D E F G H I J\n");
	for (int i = 0; i < ENEMY_MAP.size(); i++) {
		move(i+3,25);
		addch(i+ZERO);
		for (int j = 0; j < ENEMY_MAP[i].size(); j++) {
			//			printw(int(i));
			addch(' ');
			addch(ENEMY_MAP[i][j]);
		}
		printw("\n");
	}
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
	const int SIZE = 10;
	const char EMPTY = '.', HIT = 'X', MISS = 'O', SHIP = 'S';
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
	usleep(1'000'000);

	//Set up NCURSES
	if (setlocale(LC_ALL, "") == NULL) {
		cout << "Couldn't set locale.\n";
		exit(EXIT_FAILURE);
	}

	initscr(); //Turns on NCURSES
	noecho(); //Turns off echoing to the screen
	cbreak(); //Disable input buffering since I will do this myself
	//	curs_set(0); //Hide the cursor
	keypad(stdscr,TRUE); //Enable extended input from NCURSES
	//TODO: Decouple the client frame rate from the server update rate
	const int MAXFPS = 60; //Cap framerate 
//	timeout(1'000'000 / MAXFPS); //Nonblocking IO - 1/60th of a second delay if they don't type
	timeout(0);
	mousemask(ALL_MOUSE_EVENTS,NULL); //Enable mouse support

	// Get current users terminal dimensions.
	int MAX_TERMINAL_ROWS = 40, MAX_TERMINAL_COLS = 20, FIRST_COL = 0, FIRST_ROW = 0, MY_MAP_FIRST_ROW = 3, MY_MAP_FIRST_COL = 2, MY_MAP_MAX_ROW = SIZE + MY_MAP_FIRST_ROW - 1, MY_MAP_MAX_COL = 21, ENEMY_MAP_FIRST_ROW = 3, ENEMY_MAP_FIRST_COL = 27, ENEMY_MAP_MAX_COL = 46, ENEMY_MAP_MAX_ROW = SIZE + ENEMY_MAP_FIRST_ROW - 1;
	getmaxyx(stdscr, MAX_TERMINAL_ROWS, MAX_TERMINAL_COLS);
	move(FIRST_ROW,FIRST_COL); // move ( ROW, COL) ;  

	// Initialize two player maps, a 10x10 grid filled with '.' 
	vector<vector<char>> my_map(SIZE, vector<char> (SIZE, EMPTY));
	vector<vector<char>> enemy_map(SIZE, vector<char> (SIZE, EMPTY));

	showWelcomeScreen();
	showPlayerMaps(my_map,enemy_map);
	refresh();
	string temppp = s_recv(socket);
	usleep(1'000'000);

	move(MY_MAP_MAX_ROW+2, 0);
	printw("Please pick a position for your ships using the arrow keys. \nPress v to place ships vertically, press h to place ships horizontally");
	queue<int> ship_sizes({2,3,3,4,5});
	map<int,char> ship_chars = {{2,'P'}, {3,'S'}, {4,'B'}, {5,'C'}};
	int ROW = 0, COL = 0;
	bool horizontal = true;
	move(MY_MAP_FIRST_ROW, MY_MAP_FIRST_COL);
	while (1) {
		bool sent_anything = false;
		getyx(stdscr, ROW, COL);
		int ch = wgetch(stdscr);
		if (ch == ERR) {;} //  DO NOTHING
		if (ch == 'q' or ch == 'Q') bail_out();

		// User placing ships or attacking a spot.
		if (ch == '\n') {
			// User wants to put down a ship on the board, check if in valid spot
			// If it's horizontal, check horizontally
			if (ship_sizes.size()) {
				bool cant_place_ship = false;
				if (horizontal) {
					if (COL + ship_sizes.front()*2-1 <= MY_MAP_MAX_COL) {
						// First check if theres already a ship in these positions...
						for (int i = 0; i < ship_sizes.size(); i++) {
							int map_column = COL % 2 == 0 ? COL / 2 - 1 : COL / 2;
							if (map_column < 0) map_column = 0;
							if (my_map[ROW-MY_MAP_FIRST_ROW][map_column+i] == SHIP) {
								mvprintw(MY_MAP_MAX_ROW+10,0,"Can't place the ship there. ");
								move(ROW,COL);
								cant_place_ship = true;
								break;
							}
						}
						// If cant_place_ship bool is true, that means there is already a
						// ship, so don't add it to the vector or update the screen..
						if (cant_place_ship) continue;

						for (int i = 0; i < ship_sizes.front(); i++) {
							int map_column = COL % 2 == 0 ? COL / 2 - 1 : COL / 2;
							if (map_column < 0) map_column = 0;
							my_map[ROW-MY_MAP_FIRST_ROW][map_column+i] = SHIP;
						}
						ship_sizes.pop();
						move(1,0);
						showPlayerMaps(my_map,enemy_map);
						if (COL >= MY_MAP_MAX_COL) COL = MY_MAP_MAX_COL - 1;
						move(ROW,COL);
					}
					else {
						mvprintw(MY_MAP_MAX_ROW+3,0,"Not a valid move!");
						move(ROW,COL);
					}
				}
				// Else it's vertical alignment of ships
				else {
					if (ROW + ship_sizes.front()-1 <= MY_MAP_MAX_ROW) {
						// First check if there's already a ship at this location..
						for (int i = 0; i < ship_sizes.front(); i++) {
							if (my_map[ROW-MY_MAP_FIRST_ROW+i][COL/2-MY_MAP_FIRST_COL+1] == SHIP) {
								mvprintw(MY_MAP_MAX_ROW+10,0,"Can't place the ship there. ");
								move(ROW,COL);
								cant_place_ship = true;
								break;
							}
						}

						if (cant_place_ship) {
							continue;
						}
						for (int i = 0; i < ship_sizes.front(); i++) {
							my_map[ROW-MY_MAP_FIRST_ROW+i][COL/2-MY_MAP_FIRST_COL+1] = SHIP; 
						}
						ship_sizes.pop();
						move(1,0);
						showPlayerMaps(my_map,enemy_map);
						if (ROW > MY_MAP_MAX_ROW) ROW = MY_MAP_MAX_ROW;
						move(ROW,COL);
					}
				}
			if (!ship_sizes.size()) { 
				mvprintw(MY_MAP_MAX_ROW+6,0,"Done placing ships. Waiting for enemy to place ships..\n");
				move(ENEMY_MAP_FIRST_ROW, ENEMY_MAP_FIRST_COL);	
			}
		}
			
		else {// Else, time to take hits at enemy ships
			move(ENEMY_MAP_FIRST_ROW, ENEMY_MAP_FIRST_COL);	
			if (ch == '\n' or ch == KEY_ENTER) {
				getyx(stdscr, ROW, COL);
				string attack;
				attack += username + ':' + "ATTACK" + ":" + to_string(ROW) + ":" + to_string(COL);
				s_send(socket,attack); 
				usleep(1'000'000);
			}
		}
		
	}

		else if (ch == KEY_RIGHT) {
			COL+=2;
			if (ship_sizes.size()) {
				if (COL >= MY_MAP_MAX_COL) COL = MY_MAP_MAX_COL - 1;
				move(ROW,COL);
			}
			else {
				if (COL >= ENEMY_MAP_MAX_COL) COL = ENEMY_MAP_MAX_COL - 1;
				move(ROW,COL);
			}
		}
		else if (ch == KEY_LEFT) {
			COL-=2;
			if (ship_sizes.size()) {
				if (COL < MY_MAP_FIRST_COL) COL = MY_MAP_FIRST_COL;
				move(ROW,COL);
			}
			else {
				if (COL < ENEMY_MAP_FIRST_COL) COL = ENEMY_MAP_FIRST_COL;
				move(ROW,COL);
			}
		}
		else if (ch == KEY_UP) {
			ROW--;
			if (ROW < MY_MAP_FIRST_ROW) ROW = MY_MAP_FIRST_ROW;
			move(ROW,COL);
		}
		else if (ch == KEY_DOWN) {
			ROW++;
			if (ROW > MY_MAP_MAX_ROW) ROW = MY_MAP_MAX_ROW;
			move(ROW,COL);
		}
		else if (ch == 'h' or ch == 'H') {
			horizontal = true;	
		}
		else if (ch == 'v' or ch == 'V') {
			horizontal = false;
		}
		// Else get an update from the server...
		else {
//			s_send(socket, username + ":");
//			usleep(5'000'000);
//			string tmp = s_recv(socket);
		}
		if (!sent_anything) {
			continue;
		}
		string response = s_recv(socket);
		if (response != "") {
			mvprintw(35,0,"Does it die here?");
			stringstream ss(response);
			string user;
			getline(ss,user,':');
			string command;
			getline(ss,command,':');
			bool is_it_us = false;
			if (user == username) is_it_us = true;
			if (command == "ATTACK" and user == username) {
				string c_row, c_col;
				getline(ss,c_row,':');
				getline(ss,c_col,':');
				if (my_map[stoi(c_row)-MY_MAP_FIRST_ROW][stoi(c_col)-MY_MAP_FIRST_COL] == SHIP) {
					string hit = username + ":" + "HIT" + ":" ;
					s_send(socket,hit);
				}
			}
		}
	}
	refresh();
	usleep(1'000);

	endwin(); //Turn off NCURSES
	return 0;
}
