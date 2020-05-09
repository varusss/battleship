#include <iostream>
#include "fleet.h"
Fleet::Fleet() {}; 

Fleet::Fleet(std::vector<Ship> carrier,std::vector<Ship> battleship,std::vector<Ship> cruiser,std::vector<Ship> submarine,std::vector<Ship> destroyer){
	carrier = carrier;
	battleship = battleship;
	cruiser = cruiser;
	submarine = submarine;
	destroyer = destroyer;	
};
