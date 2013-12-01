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


#include "FPS.h"
#include "RenderFlatText.h"

FPS::FPS() {
	fps = -1;
	framesSinceLastSecond = 0;
	lastSecond = SDL_GetTicks();
	limitFPS = 0;
	frameDelay = 0;
}

FPS::~FPS() {

}

void FPS::decideLimitFPS() {
	// Config explicitly decides between powersave or max FPS?
	if (Program::getInstance()->config.fpsBehaviour == 1) {
		limitFPS = 0;
	} else if (Program::getInstance()->config.fpsBehaviour == -1) {
		limitFPS = 100;
	} else {
		limitFPS = 200;
	}
}

void FPS::renderInMenu() {
	if (!Program::getInstance()->config.showFPS) return;
	glLoadIdentity();
	glTranslatef(0.72, -0.99, 0);
	glScalef(0.04, 0.04, 1);
	if (fps < 20 && fps >= 0)
		glColor3f(1, 0, 0);
	else
		glColor3f(0.8, 0.7, 0.4);
	if (fps >= 0) {
		string s = "FPS: " + Functions::toString(fps) + " (5 ms)";
		RenderFlatText::render(s);
	}
}


void FPS::renderInGame() {
	if (!Program::getInstance()->config.showFPS) return;
	glLoadIdentity();
	glTranslatef(0.72, -0.99, 0);
	glScalef(0.04, 0.04, 1);
	if (fps < 20 && fps >= 0)
		glColor3f(1, 0, 0);
	else
		glColor3f(0.8, 0.7, 0.4);
	if (fps >= 0) {
		string s = "FPS: " + Functions::toString(fps);
		if (limitFPS) {
			//s += " (lim)";
			s += " (" + Functions::toString(frameDelay) + " ms)";
		}
		RenderFlatText::render(s);
	}
}

void FPS::waitInGame() {
	// Count FPS
	update();

	if (limitFPS != 0) {
		// Adjust frame delay
		if (framesSinceLastSecond == 0) {
			if (fps > limitFPS) {
				frameDelay++;
			} else if (fps < limitFPS) {
				frameDelay--;
			}
			if (frameDelay < 0)
				frameDelay = 0;
		}

		// Wait for the computed delay
		if (frameDelay > 0) {
			SDL_Delay(frameDelay);
		}
	}

}

// Wait between two frames in menu-like things
// that don't need dynamic delay adjustment.
// This is useful to prevent coil noise when FPS are very high
// (as menus are rendered very fast - like 1000 FPS),
// and it also saves power since we don't need thousands of FPS in a menu.
void FPS::waitInMenu() {
	update();
	SDL_Delay(5);
}
