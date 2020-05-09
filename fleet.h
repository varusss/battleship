#pragma once
#include <vector>
#include "ship.h"

class Fleet {
	std::vector<Ship> fleet;
	public:
		Fleet();
		Fleet(std::vector<Ship> fleet);
};
