#include <vector>
struct Coordinate{
	int x, y;
};

class Fleet {
	std::vector<Coordinate> carrier;	
	std::vector<Coordinate> battleship;	
	std::vector<Coordinate> cruiser;	
	std::vector<Coordinate> submarine;
	std::vector<Coordinate> destroyer;
	public:
		Fleet();
		Fleet(std::vector<Coordinate> carrier, 
			  std::vector<Coordinate> battleship,
			  std::vector<Coordinate> cruiser,
			  std::vector<Coordinate> submarine,
			  std::vector<Coordinate> destroyer);
};
