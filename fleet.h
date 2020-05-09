#include "ship.h"

class Fleet {
	Ship carrier;	
	Ship battleship;	
	Ship cruiser;	
	Ship submarine;
	Ship destroyer;
	public:
		Fleet();
		Fleet(Ship carrier, 
			  Ship battleship,
			  Ship cruiser,
			  Ship submarine,
			  Ship destroyer);
};
