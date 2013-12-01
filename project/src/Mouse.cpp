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


#include "Mouse.h"
#include "Program.h"

unsigned int Mouse::mouseTexture = 0;

Mouse::Mouse(Maze *m, int x0, int y0, int dir): Animal(m, x0, y0, dir) {
	color.r = (rand() % 1000) / 1000.0;
	color.g = color.r;
	color.b = color.r;
	magic = false;
}

void Mouse::makeMagic() {
	magic = true;
}

void Mouse::loadTexture() {
	mouseTexture = Program::getInstance()->loadTexture(Program::getInstance()->dataPath + "/images/mouse.png");
}

bool Mouse::reachedHouse(Game *game, int player) {
	game->scores[player]++;
	if (magic) {
		game->magicHappens(player);
	}
	return true;
}

int Mouse::isKilled() {
	for (unsigned int k=0; k<maze->snakes.size(); k++) {
		Snake *snake = &maze->snakes[k];
		int snake_dxu = snake->direction == 0 ? 1 : (snake->direction == 2 ? -1 : 0);
		int snake_dyu = snake->direction == 1 ? 1 : (snake->direction == 3 ? -1 : 0);
		float snakeMouthX = snake->x + snake_dxu*0.5;
		float snakeMouthY = snake->y + snake_dyu*0.5;
		float dist = Functions::fabs(snakeMouthX - x) + Functions::fabs(snakeMouthY - y);
		if (dist < 0.2) {
			// The snake eats the mouse.
			// A snake becomes sick after eating too much mice.
			snake->life -= 0.05;
			if (snake->life < 0) {
				snake->life = 0;
			}
			return 1;
		}
	}
	return 0;
}

void Mouse::render() {
	glLoadMatrixf(maze->modelMatrix);
	float x1 = maze->x0 + x*maze->cellWidth;
	float y1 = maze->y0 + y*maze->cellHeight;
	glTranslatef(x1, y1, 0);
	glScalef(maze->cellWidth*0.5, maze->cellHeight*0.5, 0);
	glRotatef(angle - 90, 0, 0, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mouseTexture);
	glBegin(GL_QUADS);
	if (magic) {
		adjustColor(Color(1, 0.7, 0)).gl();
		glTexCoord2f(0, 0); glVertex2f(-1, -1);
		glTexCoord2f(0, 1); glVertex2f(-1, 1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, -1);
	} else {
		adjustColor(color).gl();
		glTexCoord2f(0, 0); glVertex2f(-1, -1);
		glTexCoord2f(0, 1); glVertex2f(-1, 1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, -1);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

Color Mouse::adjustColor(Color c) {
	if (inHouse > 0) {
		c.a = 1 - inHouse;
	} else {
		c.r = c.r+(1-c.r)*(1-dieAnim);
		c.g = c.g*dieAnim;
		c.b = c.b*dieAnim;
		c.a = dieAnim;
	}
	return c;
}



