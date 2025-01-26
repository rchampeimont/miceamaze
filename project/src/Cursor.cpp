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


#include "Cursor.h"
#include "Program.h"

Cursor::Cursor(int p) {
	x = y = 0;
	player = p;
	hasDisplayList = false;
}

Cursor::~Cursor() {
	if (hasDisplayList)
		glDeleteLists(displayList, 1);
}

void Cursor::prepareRender() {
	displayList = glGenLists(1);
	hasDisplayList = true;
	glNewList(displayList, GL_COMPILE);

	float scaleFactor = 0.14;
	glColor3f(1, 1, 1);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, -1*scaleFactor);
	glVertex2f(0, 0);
	glVertex2f(0.2*scaleFactor, -0.5*scaleFactor);
	glVertex2f(0.7*scaleFactor, -0.7*scaleFactor);
	glEnd();

	glTranslatef(0.005, -0.015, 0);
	scaleFactor = 0.1;
	if (player >= 0) {
		Program::getInstance()->playerColors[player].gl();
	} else {
		glColor3f(1, 0.5, 0);
	}
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, -1*scaleFactor);
	glVertex2f(0, 0);
	glVertex2f(0.2*scaleFactor, -0.5*scaleFactor);
	glVertex2f(0.7*scaleFactor, -0.7*scaleFactor);
	glEnd();

	glEndList();
}

void Cursor::render() {
	glLoadIdentity();
	glTranslatef(x, y, 0);
	if (!glIsList(displayList)) {
		Functions::error("no list");
	}
	glCallList(displayList);
}

// Return -1 if cursor is not on a cell
int Cursor::getCellJ(Maze *maze) {
	int j = (x - maze->x0) / maze->cellWidth;
	if (j < 0 || j >= maze->width) {
		return -1;
	} else {
		return j;
	}
}

// Return -1 if cursor is not on a cell
int Cursor::getCellI(Maze *maze) {
	int i = (y - maze->y0) / maze->cellHeight;
	if (i < 0 || i >= maze->height) {
		return -1;
	} else {
		return i;
	}
}

void Cursor::setFromWindowXY(int x1, int y1) {
	Program::getInstance()->glCoordsFromWindowCoords(x1, y1, &x, &y);
}
