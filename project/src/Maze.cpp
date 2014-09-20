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


#include "Maze.h"
#include "RenderFlatText.h"
#include "Program.h"


using namespace std;

Maze::Maze() {
	// Maze size (some parts of the code ASSUME it is a square)
	size = 14;
	width = height = size;
	x0 = -0.95;
	cellWidth = 0.1;
	cellHeight = cellWidth * Program::getInstance()->screenRatio;
	maxArrowsByPlayer = 3;
	y0 = 0.96 - cellHeight*height;
	name = "";
	description = "";
	hasDisplayList = false;
}

Maze::~Maze() {
	if (hasDisplayList)
		glDeleteLists(fixedObjectsDisplayList, 1);
}

void Maze::load(string path) {
	// Read the file
	vector<string> A;
	ifstream f;
	f.open(path.c_str(), ios::in | ios::binary);
	if (!f.is_open()) {
		Functions::error(string("load maze: could not read file ") + path);
		return;
	}
	char c;
	string line = "";
	int lineCount = 0;
	bool loop = true;
	while (loop) {
		bool endline = false;
		f.read(&c, 1);
		if (f.eof()) {
			endline = true;
			loop = false;
		} else {
			endline = (c == '\n');
		}
		if (endline) {
			if (lineCount < 2*height+1) {
				A.insert(A.begin(), line);
				//cout << line << endl;
				if (line.length() < 2*(unsigned)width+1) {
					Functions::error(string("load maze: incomplete line"));
					return;
				}
			} else if (lineCount == 2*height+1) {
				name = string(line);
			} else if (lineCount == 2*height+2) {
				description = string(line);
			}
			line = "";
			lineCount++;
		} else if (c != '\r') {
			line += c;
		}
	}
	f.close();
	if (A.size() < 2*(unsigned)height+1) {
		Functions::error(string("load maze: incomplete maze"));
		return;
	}

	// walls

	for (int i=0; i<height; i++) {
		for (int j=0; j<=width; j++) {
			if (A[i*2+1][j*2] == '|') {
				vWalls.push_back(Wall(i, j));
			}
		}
	}

	for (int i=0; i<=height; i++) {
		for (int j=0; j<width; j++) {
			if (A[i*2][j*2+1] == '_') {
				hWalls.push_back(Wall(i, j));
			}
		}
	}

	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			char c = A[i*2+1][j*2+1];
			if (Spawner::isSpawnerLetterCode(c)) {
				spawners.push_back(Spawner(this, i, j, Spawner::directionFromLetterCode(c)));
			}
		}
	}

	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			if (A[i*2+1][j*2+1] == '0') {
				houses.push_back(House(i, j, 0));
			} else if (A[i*2+1][j*2+1] == '1') {
				houses.push_back(House(i, j, 1));
			} else if (A[i*2+1][j*2+1] == '2') {
				houses.push_back(House(i, j, 2));
			} else if (A[i*2+1][j*2+1] == '3') {
				houses.push_back(House(i, j, 3));
			}
		}
	}
}

void Maze::save(string path) {
	// Translate the maze into text format
	vector<string> A;
	for (int i=0; i<2*height+1; i++) {
		A.push_back(string(2*width+1, ' '));
	}
	for (int i=0; i<2*height+1; i+=2) {
		for (int j=0; j<2*width+1; j+=2) {
			A[i][j] = '.';
		}
	}

	// walls
	for (vector<Wall>::iterator wall = vWalls.begin(); wall != vWalls.end(); ++wall) {
		A[wall->i*2+1][wall->j*2] = '|';
	}
	for (vector<Wall>::iterator wall = hWalls.begin(); wall != hWalls.end(); ++wall) {
		A[wall->i*2][wall->j*2+1] = '_';
	}

	// houses
	for (vector<House>::iterator house = houses.begin(); house != houses.end(); ++house) {
		A[house->i*2+1][house->j*2+1] = Functions::toString(house->player)[0];
	}

	// spawners
	for (vector<Spawner>::iterator spawner = spawners.begin(); spawner != spawners.end(); ++spawner) {
		A[spawner->i*2+1][spawner->j*2+1] = spawner->getLetterCode();
	}


	// Actually write the file
	ofstream f;
	f.open(path.c_str(), ios::trunc | ios::out | ios::binary);
	if (!f.is_open()) {
		Functions::error(string("could not write to file ") + path);
		return;
	}
	for (int i=2*height; i>=0; i--) {
		f << A[i].c_str() << "\r\n";
	}
	f << name << "\r\n";
	f << description << "\r\n";
	f.close();
}
// Call this function in the same OpenGL context as you will do the rendering,
// but only once.
void Maze::prepareRender(Game *game) {
	// Save current matrix as our reference matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);

	// Precompile a list of operations to draw the fixed part of the maze
	fixedObjectsDisplayList = glGenLists(1);
	hasDisplayList = true;
	glNewList(fixedObjectsDisplayList, GL_COMPILE);

	// draw background
	glLoadMatrixf(modelMatrix);
	glBegin(GL_QUADS);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if ((i + j) % 2 == 0) {
				glColor3f(1, 0.9, 0.5);
			} else {
				glColor3f(0.8, 0.7, 0.4);
			}
			// (x1,y1) is the top left corner of the cell
			float x1 = x0 + j*cellWidth;
			float y1 = y0 + (i+1)*cellHeight;
			glVertex2f(x1, y1);
			glVertex2f(x1, y1-cellHeight);
			glVertex2f(x1+cellWidth, y1-cellHeight);
			glVertex2f(x1+cellWidth, y1);
		}
	}
	glEnd();

	// draw walls
	glColor3f(0.8, 0.5, 0.1);
	glBegin(GL_QUADS);
	{
		const float w = cellWidth*0.1;
		const float h = cellHeight*0.6;
		for (unsigned k=0; k<vWalls.size(); k++) {
			Wall *wall = &vWalls[k];
			float x1 = x0 + wall->j*cellWidth;
			float y1 = y0 + (wall->i+0.5)*cellHeight;
			glVertex2f(x1-w, y1-h);
			glVertex2f(x1-w, y1+h);
			glVertex2f(x1+w, y1+h);
			glVertex2f(x1+w, y1-h);
		}
	}
	{
		const float w = cellWidth*0.6;
		const float h = cellHeight*0.1;
		for (unsigned k=0; k<hWalls.size(); k++) {
			Wall *wall = &hWalls[k];
			float x1 = x0 + (wall->j+0.5)*cellWidth;
			float y1 = y0 + wall->i*cellHeight;
			glVertex2f(x1-w, y1-h);
			glVertex2f(x1-w, y1+h);
			glVertex2f(x1+w, y1+h);
			glVertex2f(x1+w, y1-h);
		}
	}
	glEnd();


	// draw spawners
	for (vector<Spawner>::iterator spawner = spawners.begin(); spawner != spawners.end(); ++spawner) {
		spawner->render();
	}

	// show maze credit
	if (game != NULL) {
		glLoadMatrixf(modelMatrix);
		glTranslatef(-0.2, -0.98, 0);
		glScalef(0.06, 0.05, 1);
		glColor3f(0.8, 0.7, 0.4);
		if (description.length() == 0) {
			RenderFlatText::render(name, 0);
		} else {
			RenderFlatText::render(name + " - " + description, 0);
		}
	}


	glEndList();
}

// game argument may be NULL
void Maze::render(Game *game) {
	glLoadMatrixf(modelMatrix);

	// Render fixed parts of the maze
	glCallList(fixedObjectsDisplayList);

	// draw arrows
	for (vector<Arrow>::iterator arrow = arrows.begin(); arrow != arrows.end(); ++arrow) {
		arrow->render(this);
	}

	// draw houses
	for (vector<House>::iterator house = houses.begin(); house != houses.end(); ++house) {
		house->render(this, game);
	}

	// draw mice
	// rendered in reverse order so mice appear "below" previous mice
	for (int k=mice.size()-1; k>=0; k--) {
		mice[k].render();
	}

	// draw snakes (above mice)
	for (int k=snakes.size()-1; k>=0; k--) {
		snakes[k].render();
	}

}

void Maze::runTick(Game *game) {
	for (unsigned int k=0; k<mice.size(); k++) {
		int state = mice[k].walk(game);
		if (state == 1 || state == 2) {
			mice.erase(mice.begin() + k);
			// Now element k is the next one, so we mustn't skip it
			k--;
		}
	}
	for (unsigned int k=0; k<snakes.size(); k++) {
		int state = snakes[k].walk(game);
		if (state == 1 || state == 2) {
			snakes.erase(snakes.begin() + k);
			// Now element k is the next one, so we mustn't skip it
			k--;
		}
	}
	for (unsigned int k=0; k<spawners.size(); k++) {
		spawners[k].tick(game);
	}
}

// Returns whether going from position (x,y) to (nx,ny) crosses a wall
bool Maze::crossesWall(int x, int y, int nx, int ny) {
	if (x != nx) {
		for (vector<Wall>::iterator wall = vWalls.begin(); wall != vWalls.end(); ++wall) {
			if (y == wall->i) {
				if ((nx == wall->j && x < nx) || (x == wall->j && x > nx)) {
					return true;
				}
			}
		}
		return false;
	} else if (y != ny) {
		for (vector<Wall>::iterator wall = hWalls.begin(); wall != hWalls.end(); ++wall) {
			if (x == wall->j) {
				if ((ny == wall->i && y < ny) || (y == wall->i && y > ny)) {
					return true;
				}
			}
		}
		return false;
	} else {
		return false;
	}
}


// Returns:
// 0 if no arrow added
// 1 if modified an existing arrow
// 2 if a new arrow is placed
int Maze::addArrow(int i, int j, int player, int direction) {
	//Program::verify(i >= 0 && j >= 0 && j < width && i < height,
	//		"addArrow() called with invalid parameters: " + Program::toString(i) + " " + Program::toString(j));
	//Program::verify(direction >= 0 && direction <= 3,
	//		"addArrow() called with invalid direction: " + Program::toString(direction));
	if (! (i >= 0 && j >= 0 && j < width && i < height && direction >= 0 && direction <= 3))
		return 0;

	for (unsigned int a=0; a<houses.size(); a++) {
		if (i == houses[a].i && j == houses[a].j) {
			// Placing an arrow on a house is forbidden.
			return 0;
		}
	}

	for (unsigned int a=0; a<spawners.size(); a++) {
		if (i == spawners[a].i && j == spawners[a].j) {
			// Placing an arrow on a spawners is forbidden.
			return 0;
		}
	}

	// Search if an arrow is already on that cell
	// and count how many arrows the player has.
	int playerArrows = 0;
	for (unsigned int a=0; a<arrows.size(); a++) {
		if (arrows[a].player == player) {
			playerArrows++;
		}
		if (i == arrows[a].i && j == arrows[a].j) {
			// There already is an arrow on that cell.
			// Is it own by the same player?
			if (player == arrows[a].player) {
				if (arrows[a].direction == direction) {
					// This is the SAME arrow, so do nothing.
					return 0;
				} else {
					// Change arrow direction
					arrows[a].direction = direction;
					return 1;
				}
			} else {
				// Not allowed to place an arrow here.
				return 0;
			}
		}
	}

	// No arrow on this cell, so place one.
	arrows.push_back(Arrow(i, j, player, direction));
	playerArrows++;

	// Does the player have too much arrows?
	while (playerArrows > maxArrowsByPlayer) {
		// Delete oldest arrow of this player
		for (unsigned int a=0; a<arrows.size(); a++) {
			if (arrows[a].player == player) {
				arrows.erase(arrows.begin() + a);
				playerArrows--;
				break;
			}
		}
	}

	return 2;
}

// If we go in direction from (x,y), on which cell do we arrive?
IntXY Maze::nextCell(int x, int y, int direction) {
	IntXY p = IntXY(x, y);
	p.x += direction == 0 ? 1 : (direction == 2 ? -1 : 0);
	p.y += direction == 1 ? 1 : (direction == 3 ? -1 : 0);
	return p;
}

void Maze::randomizeHouses() {
	int perm = rand() % 4;
	for (unsigned k=0; k<houses.size(); k++) {
		houses[k].player = (houses[k].player + perm) % 4;
	}
}

