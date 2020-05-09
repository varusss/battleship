#include "player.h"

using namespace std;
Player::Player() {};
Player::Player(std::string name) : name(name){};
Player::Player(std::string name, std::vector<Ship> ships) : name(name), ships(ships){};

