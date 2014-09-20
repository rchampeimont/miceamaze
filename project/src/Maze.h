// Copyright (c) 2012, Raphael CHAMPEIMONT
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef MAZE_H_
#define MAZE_H_

#include <iostream>
#include <vector>

#include "Wall.h"
#include "Mouse.h"
#include "Snake.h"
#include "Arrow.h"
#include "House.h"
#include "Spawner.h"
#include "IntXY.h"


using namespace std;

class Game;

class Maze {
	bool hasDisplayList;

public:
	int size, width, height;
	vector<Wall> vWalls, hWalls;
	vector<Mouse> mice;
	vector<Snake> snakes;
	vector<Arrow> arrows;
	vector<House> houses;
	vector<Spawner> spawners;
	float x0;
	float y0;
	float cellWidth;
	float cellHeight;
	GLfloat modelMatrix[16];
	GLuint fixedObjectsDisplayList;
	string name, description;
	int maxArrowsByPlayer;

	Maze();
	~Maze();
	void render(Game *);
	void runTick(Game *);
	int x2i(float x) {
		if (x >= 0)
			return (int) x;
		else
			return ((int) x) - 1;
	}
	IntXY nextCell(int,int,int);
	bool crossesWall(int,int,int,int);
	int addArrow(int,int,int,int);
	void save(string);
	void load(string);
	void prepareRender(Game *game);
	void randomizeHouses();
};

#endif /* MAZE_H_ */
