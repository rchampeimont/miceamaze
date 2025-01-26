// Copyright (c) 2012, River CHAMPEIMONT
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


#include "House.h"
#include "Program.h"

unsigned int House::texture = 0;
unsigned int House::textureFirst = 0;

// game is optional (may be NULL)
void House::render(Maze *maze, Game *game) {
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
	Program::getInstance()->playerColors[player].gl();
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glTexCoord2f(0, 1); glVertex2f(-1, 1);
	glTexCoord2f(1, 1); glVertex2f(1, 1);
	glTexCoord2f(1, 0); glVertex2f(1, -1);
	glEnd();

	if (game != NULL && game->eagleOwner == player) {
		glLoadMatrixf(maze->modelMatrix);
		glTranslatef(x1, y1, 0);
		glScalef(maze->cellWidth*0.4, maze->cellHeight*0.4, 0);
		glTranslatef(0, 1, 0);
		float r = game->time % 360;
		glRotatef(r, 0, 1, 0);
		glTranslatef(1, 0, 0);
		glRotatef(90, 0, 1, 0);
		glScalef(0.5, 0.5, 1);
		glBindTexture(GL_TEXTURE_2D, Images::getTexture(1));
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 0); glVertex2f(-1, -1);
		glTexCoord2f(0, 1); glVertex2f(-1, 1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, -1);
		glEnd();
	}

	if (game != NULL && player == game->getLeader()) {
		// This is the leader's house
		glLoadMatrixf(maze->modelMatrix);
		glTranslatef(x1, y1, 0);
		glScalef(maze->cellWidth*0.4, maze->cellHeight*0.4, 0);
		glTranslatef(-0.5, 0.3, 0);
		float f = 0.5 + 0.3*(1+sin(game->time/10.0f))/2;
		glScalef(f, f, 1);
		glBindTexture(GL_TEXTURE_2D, textureFirst);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 0); glVertex2f(-1, -1);
		glTexCoord2f(0, 1); glVertex2f(-1, 1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, -1);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void House::loadTextures() {
	texture = Program::getInstance()->loadTexture(Program::getInstance()->dataPath + "/images/house.png");
	textureFirst = Program::getInstance()->loadTexture(Program::getInstance()->dataPath + "/images/first.png");
}
