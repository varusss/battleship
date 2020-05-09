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
	static const char WALL      = '.';
	static const char SHIP      = '#';
	static const char EMPTY     = ' ';
	static const char ENEMY_HIT = 'O';
	static const char ENEMY_MISS= 'E';
	static const size_t SIZE = 20; //10X10 map 
	static const size_t DISPLAY = 20; //Show a 10x10 area at a time
	//Randomly generate map
	void init_map() {
		map.clear();
		map.resize(SIZE); 
		
		for (auto &v : map) v.resize(SIZE,'.'); //100 columns wide
		
		for (size_t i = 0; i < SIZE; i++) {
			for (size_t j = 0; j < SIZE; j++) {	
				if (j%2 and i%2) {
					map.at(i).at(j) = WALL;
				}
				else {
					map.at(i).at(j) = EMPTY;
				}
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
				else if (map.at(i).at(j) == SHIP) 
					color = 3;
				attron(COLOR_PAIR(color));
				mvaddch(i,j,map.at(i).at(j));
				attroff(COLOR_PAIR(color));
				//attroff(A_UNDERLINE | A_BOLD);
			}
		}
	}

	void draw_enemy_field(){
		for(size_t i = 0; i < SIZE; i++){
			for (size_t j =0 ; j< SIZE; j++){
				move(i,30+j);
				if (j%2 and i%2) printw(".");
				else printw(" ");
			}
		}
	}

	int check_for_ship(int x, int y){
		const int MISS = 0,
			  	  HIT = 1,
				  E = 2;
		if (map.at(x).at(y-30) == SHIP) {
			map.at(x).at(y-30) = ENEMY_HIT;
			move(x,y);
			printw("X");
			return HIT;
		}
		if (map.at(x).at(y-30) == WALL) {
			map.at(x).at(y-30) = ENEMY_MISS;
			move(x,y);
			printw("M");
			return MISS;
		}
		return E;
	}


	int change(int x, int y, int size, bool orient){
		const int SUCESS = 0, 
			  	  BAD_POS = 1, 
				  MAX_SHIP =2;
		if (size >=7) return MAX_SHIP ;
		if (x >= SIZE) return BAD_POS;
		if (y >= SIZE) return BAD_POS;
		if (map.at(x).at(y) == EMPTY) {
			if (y%2 == 0) {
				if (y <=0) return BAD_POS;
				y -=1;
			}
			if (x%2 == 0) {
				if (x <=0) return BAD_POS;
				x -=1;
			}
		}
		if (!orient) {
			for (int i = 0; i < size; i++){
				if (y+i*2 >= SIZE) return BAD_POS;
				if (map.at(x).at(y + i*2) == SHIP) return BAD_POS;
			}
			for (int i =0; i < size; i++)
				map.at(x).at(y +i*2) = SHIP;
		}
		else if (orient){
			for (int i = 0; i < size; i++){
				if (x+i*2 >= SIZE) return BAD_POS;
				if (map.at(x+i*2).at(y) == SHIP) return BAD_POS;
			}
			for (int i =0; i < size; i++)
				map.at(x+i*2).at(y) = SHIP;
		}
		return SUCESS;
	}
	Map() {
		init_map();
	}
};
