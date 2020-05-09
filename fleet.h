#include <vector>
#include "ship.h"

class Fleet {
	std::vector<Ship> carrier;	
	std::vector<Ship> battleship;	
	std::vector<Ship> cruiser;	
	std::vector<Ship> submarine;
	std::vector<Ship> destroyer;
	public:
		Fleet();
		Fleet(std::vector<Ship> carrier, 
			  std::vector<Ship> battleship,
			  std::vector<Ship> cruiser,
			  std::vector<Ship> submarine,
			  std::vector<Ship> destroyer);
};
