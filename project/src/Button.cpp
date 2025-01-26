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



#include "RenderFlatText.h"
#include "Program.h"

// State:
// 0 = normal
// 1 = mouse over
// 2 = pressed
void Button::render(int state) {
	if (!shown) return;

	float borderY = h/15;
	float borderX = borderY/4*3;

	glBegin(GL_QUADS);

	if (state != 2) {
		glColor3f(color.r*1, color.g*1, color.b*1);
	} else {
		glColor3f(color.r*0.4, color.g*0.4, color.b*0.4);
	}
	glVertex2f(x, y);
	glVertex2f(x, y+h);
	glVertex2f(x+w, y+h);
	glVertex2f(x+w, y);

	if (state == 1) {
		glColor3f(color.r*0.7, color.g*0.7, color.b*0.7);
	} else {
		glColor3f(color.r*0.6, color.g*0.6, color.b*0.6);
	}
	glVertex2f(x+borderX, y+borderY);
	glVertex2f(x+borderX, y+h-borderY);
	glVertex2f(x+w-borderX, y+h-borderY);
	glVertex2f(x+w-borderX, y+borderY);

	glEnd();

	if (state != 2) {
		glColor3f(color.r*0.4, color.g*0.4, color.b*0.4);
	} else {
		glColor3f(color.r*1, color.g*1, color.b*1);
	}
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x+borderX, y+borderY);
	glVertex2f(x+w-borderX, y+borderY);
	glVertex2f(x+w, y);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(x+w-borderX, y+borderY);
	glVertex2f(x+w-borderX, y+h-borderY);
	glVertex2f(x+w, y+h);
	glVertex2f(x+w, y);
	glEnd();

	glTranslatef(x+w/2, y+h/2, 0);
	if (state == 2) {
		glTranslatef(borderX/2, -borderY/2, 0);
	}
	glScalef(0.09*textSizeFactor, 0.09*textSizeFactor, 1);
	glTranslatef(0, -0.4, 0);
	glColor3f(1, 1, 1);
	RenderFlatText::render(text, 0);
}


