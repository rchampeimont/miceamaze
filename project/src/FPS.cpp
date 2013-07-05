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
	limitFPS = false;
}

FPS::~FPS() {

}

void FPS::decideLimitFPS() {
	// Config explicitly decides between powersave or max FPS?
	if (Program::getInstance()->config.fpsBehaviour == 1) {
		limitFPS = false;
		return;
	} else if (Program::getInstance()->config.fpsBehaviour == -1) {
		limitFPS = true;
		return;
	}

	// Max FPS unless we have a reason to set otherwise
	limitFPS = false;
	// Detect if we are on battery
#ifdef _WIN32
	SYSTEM_POWER_STATUS powerStatus;
	if (GetSystemPowerStatus(&powerStatus)) {
		if (powerStatus.ACLineStatus == 0) {
			// We are on battery, so limit FPS to save energy
			limitFPS = true;
		}
	}
#endif
}

void FPS::render() {
	if (!Program::getInstance()->config.showFPS) return;
	glLoadIdentity();
	glTranslatef(0.72, -0.99, 0);
	glScalef(0.04, 0.04, 1);
	if (fps < 20 && fps >= 0)
		glColor3f(1, 0, 0);
	else
		glColor3f(0.8, 0.7, 0.4);
	if (fps >= 0) {
		string s = "FPS: " + Functions::toString(fps) + " (lim)";
		RenderFlatText::render(s);
	} else {
		RenderFlatText::render("FPS: ");
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
			s += " (lim)";
		}
		RenderFlatText::render(s);
	} else {
		RenderFlatText::render("FPS: ");
	}
}
