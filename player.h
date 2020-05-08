#include <locale.h>
#include <curses.h>
#include <unistd.h>
#include "fleet.h"
#include <string>

class Player {
	std::string name;
	Fleet fleet;	
	public:
		Player(std::string name, Fleet fleet);

};
