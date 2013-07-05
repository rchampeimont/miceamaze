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


#include "Snake.h"
#include "Program.h"

unsigned int Snake::snakeTexture = 0;

Snake::Snake(Maze *m, int x0, int y0, int dir): Animal(m, x0, y0, dir) {
	color.r = 1;
	color.g = 0.5;
	color.b = 1;
	speed = 0.015;
}

void Snake::loadTexture() {
	snakeTexture = Program::getInstance()->loadTexture(Program::getInstance()->dataPath + "/images/snake.png");
}

bool Snake::reachedHouse(Game *game, int player) {
	if (game->eagleOwner == player) {
		// the eagle kills the snake
		dying = true;
		return false;
	} else {
		// snake reaches house
		game->scores[player] -= 10;
		return true;
	}
}


void Snake::render() {
	glLoadMatrixf(maze->modelMatrix);
	float x1 = maze->x0 + x*maze->cellWidth;
	float y1 = maze->y0 + y*maze->cellHeight;
	glTranslatef(x1, y1, 0);
	glRotatef(angle - 90, 0, 0, 1);
	glScalef(maze->cellWidth*0.6, maze->cellHeight*0.6, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, snakeTexture);
	glBegin(GL_QUADS);
	adjustColor(color).gl();
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glTexCoord2f(0, 1); glVertex2f(-1, 1);
	glTexCoord2f(1, 1); glVertex2f(1, 1);
	glTexCoord2f(1, 0); glVertex2f(1, -1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}


Color Snake::adjustColor(Color c) {
	c.r = c.r*life;
	c.g = c.g+(1-c.g)*(1-life);
	if (inHouse > 0) {
		c.a = 1 - inHouse;
	} else {
		c.r = c.r*dieAnim;
		c.g = c.g+(1-c.g)*(1-dieAnim);
		c.a = dieAnim;
	}
	return c;
}


