#include "fleet.h"
#include <iostream>

Fleet::Fleet(std::vector<Coordinate> carrier, std::vector<Coordinate> battleship, std::vector<Coordinate> cruiser, std::vector<Coordinate> submarine, std::vector<Coordinate> destroyer) {
	carrier = carrier;
	battleship = battleship;
	cruiser = cruiser;
	submarine = submarine;
	destroyer = destroyer;	
};
