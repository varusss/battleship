#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <ncurses.h>
using namespace std; //Boo hiss

class Map {
	vector<vector<char>> map;
	default_random_engine gen;
	public:
	static const char WALL     = '.';
	static const char SHIP     = '#';
	static const size_t SIZE = 20; //10X10 map 
	static const size_t DISPLAY = 20; //Show a 10x10 area at a time

	//Randomly generate map
	void init_map() {
		map.clear();
		map.resize(SIZE); 
		for (auto &v : map) v.resize(SIZE,'.'); //100 columns wide
		for (size_t i = 0; i < SIZE; i++) {
			for (size_t j = 0; j < SIZE; j++) {	
				if (j%2 and i%2)map.at(i).at(j) = WALL;
				else map.at(i).at(j) = ' ';
			} 
		}
	}
	//Draw the DISPLAY tiles around coordinate (x,y)
	void draw(int x, int y) {
		//Now draw the map using NCURSES
		for (size_t i = 0; i < SIZE; i++) {
			for (size_t j = 0; j < SIZE; j++) {
				//if (i == cursor_x && j == cursor_y)
				//	attron(A_UNDERLINE | A_BOLD);
				int color = 1;
				if (map.at(i).at(j) == WALL){
					color = 2;
				}
				attron(COLOR_PAIR(color));
				mvaddch(i,j,map.at(i).at(j));
				attroff(COLOR_PAIR(color));
				//attroff(A_UNDERLINE | A_BOLD);
			}
		}
	}
	void change(int x, int y, int size, int orient){
		if (x%2) x/=2 *2;
		if (y%2) y/=2 *2;
		if (orient == 0) {
			for (int i =0; i < size; i++)
				map.at(x + i*2).at(y +i*2) = SHIP;
		}
	}
	Map() {
		init_map();
	}
};
