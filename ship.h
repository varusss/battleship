#include <iostream>
#include <ostream>
#include <string>
#include <vector>

struct Coordinate {
	int x,y;
};


struct Ship {
	std::string type;
	std::vector<Coordinate> coordinates;
	Ship() {};
	Ship(std::string type, std::vector<Coordinate> coordinates) : type(type), coordinates(coordinates){}
};

std::ostream& operator<<(std::ostream &outs, Coordinate &coordinate) {
	outs << "(" << std::to_string(coordinate.x) << "," << std::to_string(coordinate.y) << ")";
	return outs;
}

std::ostream& operator<<(std::ostream &outs, Ship &ship) {
	outs << "This ship is a " << ship.type << " with coordinates ";
	for (size_t i = 0; i < ship.coordinates.size(); i++) {
		std::cout << ship.coordinates[i] << " "; 
	} std::cout << std::endl;
	return outs;
}
