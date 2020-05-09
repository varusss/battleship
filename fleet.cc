#include "fleet.h"
Fleet::Fleet() {}; 
Fleet::Fleet(Ship carrier, Ship battleship, Ship cruiser, Ship submarine, Ship destroyer) :
carrier(carrier), battleship(battleship), cruiser(cruiser), submarine(submarine), destroyer(destroyer)	{};
