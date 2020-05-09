#pragma once
#include "ship.h"
#include <string>

class Player {
	std::string name;
	std::vector<Ship> ships;	
	public:
		Player();
		Player(std::string name);
		Player(std::string name, std::vector<Ship> ships);
};
