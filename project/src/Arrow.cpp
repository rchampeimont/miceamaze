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



#include "Arrow.h"
#include "Program.h"

void Arrow::render(Maze *maze) {
	glLoadMatrixf(maze->modelMatrix);
	float x1 = maze->x0 + (j+0.5)*maze->cellWidth;
	float y1 = maze->y0 + (i+0.5)*maze->cellHeight;
	glTranslatef(x1, y1, 0);
	glScalef(maze->cellWidth*0.3, maze->cellHeight*0.3, 0);
	glRotatef(direction*90, 0, 0, 1);
	Program::getInstance()->playerColors[player].gl();
	glBegin(GL_TRIANGLES);
	glVertex2f(-1, 1);
	glVertex2f(1, 0);
	glVertex2f(-1, -1);
	glEnd();
}

void Arrow::render() {
	glScalef(0.1*0.3, 0.1*Program::getInstance()->screenRatio*0.3, 0);
	glRotatef(direction*90, 0, 0, 1);
	Program::getInstance()->playerColors[player].gl();
	glBegin(GL_TRIANGLES);
	glVertex2f(-1, 1);
	glVertex2f(1, 0);
	glVertex2f(-1, -1);
	glEnd();
}

