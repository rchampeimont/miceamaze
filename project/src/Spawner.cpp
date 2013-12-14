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



#include "Spawner.h"
#include "Program.h"

unsigned int Spawner::texture = 0;

Spawner::Spawner(Maze *m, int i0, int j0, int dir) {
	maze = m;
	i = i0;
	j = j0;
	counter = rand() % 100;
	direction = dir;
}

void Spawner::render() {
	glLoadMatrixf(maze->modelMatrix);
	float x1 = maze->x0 + (j+0.5)*maze->cellWidth;
	float y1 = maze->y0 + (i+0.5)*maze->cellHeight;
	glTranslatef(x1, y1, -0.0005);
	glScalef(maze->cellWidth*0.4, maze->cellHeight*0.4, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glTexCoord2f(0, 1); glVertex2f(-1, 1);
	glTexCoord2f(1, 1); glVertex2f(1, 1);
	glTexCoord2f(1, 0); glVertex2f(1, -1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void Spawner::loadTexture() {
	texture = Program::getInstance()->loadTexture(Program::getInstance()->dataPath + "/images/spawner.png");
}

void Spawner::tick(Game *game) {
	if (game->specialMode == 1) {
		// Mouse mania
		if (counter % 20 == 0) {
			maze->mice.push_back(Mouse(maze, j, i, getNextDir()));
		}
	} else if (game->specialMode == 2) {
		// Snake attack
		if (counter % 500 == 0) {
			maze->snakes.push_back(Snake(maze, j, i, getNextDir()));
		}
	} else {
		// Normal mode
		if (counter % 100 == 0) {
			if (rand() % 100 == 0) {
				maze->snakes.push_back(Snake(maze, j, i, getNextDir()));
			} else {
				Mouse mouse = Mouse(maze, j, i, getNextDir());
				if (rand() % 100 == 0) {
					mouse.makeMagic();
				}
				if (!mouse.isMagic()) {
					if (game->specialMode == 3) {
						mouse.color.g = (rand() % 1000) / 1000.0;
						mouse.color.b = (rand() % 1000) / 1000.0;
					} else if (game->specialMode == 4) {
						mouse.makeSick();
					}
				}
				maze->mice.push_back(mouse);
			}
		}
	}

	if (counter == 100000000) {
		counter = 0;
	}
	counter++;
}

char Spawner::getLetterCode() {
	switch (direction) {
	case 0:
		return 'R';
	case 1:
		return 'U';
	case 2:
		return 'L';
	case 3:
		return 'D';
	default:
		return 'S';
	}
}

int Spawner::directionFromLetterCode(char code) {
	switch (code) {
	case 'R':
		return 0;
	case 'U':
		return 1;
	case 'L':
		return 2;
	case 'D':
		return 3;
	case 'S':
		return -1;
	default:
		return -1;
	}
}

bool Spawner::isSpawnerLetterCode(char code) {
	switch (code) {
	case 'R':
	case 'U':
	case 'L':
	case 'D':
	case 'S':
		return true;
	default:
		return false;
	}
}
