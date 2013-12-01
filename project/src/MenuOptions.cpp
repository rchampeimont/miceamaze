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


#include "MenuOptions.h"

#include "Program.h"
#include "RenderFlatText.h"


MenuOptions::MenuOptions() : cursor(-1) {
	hasDisplayList = false;
	fixedObjectsDisplayList = 0;
	initVideoCounter = -1;
}

MenuOptions::~MenuOptions() {
	if (hasDisplayList)
		glDeleteLists(fixedObjectsDisplayList, 1);
}

void MenuOptions::prepareRender() {
	glLoadIdentity();
	cursor.prepareRender();
	glClearColor(0.2, 0.1, 0, 0);

	// Precompile a list of operations to draw the fixed part of the maze
	fixedObjectsDisplayList = glGenLists(1);
	hasDisplayList = true;
	glNewList(fixedObjectsDisplayList, GL_COMPILE);

	// render title
	glLoadIdentity();
	glTranslatef(0, .8, 0);
	glColor3f(1, 1, 1);
	glScalef(0.15, 0.15, 1);
	RenderFlatText::render("Options", 0);

	string optionsNames[] = {"Display mode", "FPS in game", "Show FPS count"};

	for (int i=0; i<3; i++) {
		float y = 0.4-0.2*i;
		glLoadIdentity();
		if (i == 0) {
			glColor3f(0, 0, 0.3);
		} else if (i == 1) {
			glColor3f(0, 0.3, 0);
		} else if (i == 2) {
			glColor3f(0.3, 0, 0);
		}
		Functions::drawSquare(-1, y-0.05, 2, 0.2);

		glLoadIdentity();
		glTranslatef(-0.9, y+0.01, 0);
		glScalef(0.06, 0.06, 1);
		glColor3f(1, 1, 1);
		RenderFlatText::render(optionsNames[i]);
	}


	glEndList();

	initVideoCounter = Program::getInstance()->initVideoCounter;
}

void MenuOptions::selectOption(int option) {
	switch (option) {
	case 1:
		if (!Program::getInstance()->fullscreen) {
			Program::getInstance()->toggleFullscreen();
		}
		break;
	case 2:
		if (Program::getInstance()->fullscreen) {
			Program::getInstance()->toggleFullscreen();
		}
		break;
	case 3:
		Program::getInstance()->config.fpsBehaviour = -1;
		Program::getInstance()->fps->decideLimitFPS();
		break;
	case 4:
		Program::getInstance()->config.fpsBehaviour = 0;
		Program::getInstance()->fps->decideLimitFPS();
		break;
	case 5:
		Program::getInstance()->config.fpsBehaviour = 1;
		Program::getInstance()->fps->decideLimitFPS();
		break;
	case 6:
		Program::getInstance()->config.showFPS = true;
		break;
	case 7:
		Program::getInstance()->config.showFPS = false;
		break;
	default:
		return;
	}
	Program::getInstance()->config.save();
}

void MenuOptions::run() {
	buttons.push_back(Button(-0.8, -0.9, 1.6, 0.15, "OK"));
	const float textSizeFactor = 0.5;
	int i = 0;
	buttons.push_back(Button(-0.4, 0.4-0.2*i, 0.3, 0.10, "Fullscreen", textSizeFactor));
	buttons.push_back(Button(-0.0, 0.4-0.2*i, 0.3, 0.10, "Window", textSizeFactor));

	i=1;
	buttons.push_back(Button(-0.4, 0.4-0.2*i, 0.3, 0.10, "100 (low)", textSizeFactor));
	buttons.push_back(Button(-0.0, 0.4-0.2*i, 0.3, 0.10, "200 (default)", textSizeFactor));
	buttons.push_back(Button(+0.4, 0.4-0.2*i, 0.3, 0.10, "MAX", textSizeFactor));

	i=2;
	buttons.push_back(Button(-0.4, 0.4-0.2*i, 0.3, 0.10, "Yes", textSizeFactor));
	buttons.push_back(Button(-0.0, 0.4-0.2*i, 0.3, 0.10, "No", textSizeFactor));

#ifndef _WIN32
	// This option only works on Windows,
	// so don't display it on other OSs.
	buttons[5].shown = false;
#endif

	int pressedButton = -1;

	// place cursor at correct position
	cursor.setFromMouse();

	// Prepare rendering
	prepareRender();

	while (true) {
		if (initVideoCounter != Program::getInstance()->initVideoCounter) {
			// Video was reinitialized, so we need to resend display lists
			prepareRender();
		}

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT);

		// Render fixed parts of the maze
		glCallList(fixedObjectsDisplayList);

		// show FPS counter
		Program::getInstance()->fps->renderInMenu();

		// which buttons are selected
		buttons[1].select(Program::getInstance()->fullscreen);
		buttons[2].select(!Program::getInstance()->fullscreen);
		buttons[3].select(Program::getInstance()->config.fpsBehaviour == -1);
		buttons[4].select(Program::getInstance()->config.fpsBehaviour == 0);
		buttons[5].select(Program::getInstance()->config.fpsBehaviour == 1);
		buttons[6].select(Program::getInstance()->config.showFPS);
		buttons[7].select(!Program::getInstance()->config.showFPS);

		// render buttons
		for (int b=0; b<(int) buttons.size(); b++) {
			glLoadIdentity();
			if (pressedButton == b) {
				buttons[b].render(2);
			} else {
				buttons[b].render(buttons[b].over(cursor.x, cursor.y));
			}
		}

		// render cursor
		cursor.render();

		SDL_GL_SwapBuffers();


		// Event handling
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			Program::getInstance()->generalEventHandler(&event);
			switch (event.type) {
				case SDL_MOUSEMOTION:
					cursor.setFromWindowXY(event.motion.x, event.motion.y);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
						for (int b=0; b<(int) buttons.size(); b++) {
							if (buttons[b].over(cursor.x, cursor.y)) {
								pressedButton = b;
							}
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
						if (pressedButton >= 0) {
							if (buttons[pressedButton].over(cursor.x, cursor.y)) {
								if (pressedButton == 0) {
									Program::getInstance()->scene = 0;
									return;
								} else {
									selectOption(pressedButton);
								}
							}
							pressedButton = -1;
						}
					}
					break;
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						Program::getInstance()->scene = 0;
						return;
					}
					break;
			}

		}

		// Count FPS and wait some time before rendering next frame
		Program::getInstance()->fps->waitInMenu();

	}
}



