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


#include "MenuPlayers.h"
#include "Program.h"
#include "RenderFlatText.h"

int MenuPlayers::playerControls[4] = {0, 1, 2, 2};

MenuPlayers::MenuPlayers() : cursor(-1) {
	hasDisplayList = false;
	fixedObjectsDisplayList = 0;
	initVideoCounter = -1;
}

MenuPlayers::~MenuPlayers() {
	if (hasDisplayList)
		glDeleteLists(fixedObjectsDisplayList, 1);
}

void MenuPlayers::prepareRender() {
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
	RenderFlatText::render("Select players", 0);

	// Render buttons background and text
	for (int i=0; i<4; i++) {
		glLoadIdentity();
		Program::getInstance()->playerColors[i].gl();
		Functions::drawSquare(-1, buttons[firstControlButton()+i*4].y-0.05, 2, 0.2);

		glLoadIdentity();
		glTranslatef(-0.9, buttons[firstControlButton()+i*4].y+0.01, 0);
		glScalef(0.08, 0.08, 1);
		glColor3f(0, 0, 0);
		RenderFlatText::render(string("Player ") + Functions::toString(i+1));
	}

	glEndList();

	initVideoCounter = Program::getInstance()->initVideoCounter;
}

void MenuPlayers::run() {
	buttons.push_back(Button(-0.8, -0.6, 1.6, 0.15, "OK"));

	const float textSizeFactor = 0.5;
	for (int i=0; i<4; i++) {
		buttons.push_back(Button(-0.6, 0.4-0.2*i, 0.2, 0.10, "Mouse", textSizeFactor));
		buttons.push_back(Button(-0.3, 0.4-0.2*i, 0.5, 0.10, "Keyboard (IJKL+arrows)", textSizeFactor));
		buttons.push_back(Button(0.3, 0.4-0.2*i, 0.2, 0.10, "AI", textSizeFactor));
		buttons.push_back(Button(0.6, 0.4-0.2*i, 0.2, 0.10, "None", textSizeFactor));
	}

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
		Program::getInstance()->fps->render();

		// render buttons
		for (int b=0; b<(int) buttons.size(); b++) {
			glLoadIdentity();
			bool selected = false;
			if (b >= firstControlButton() && b < firstControlButton() + 4*4) {
				int pl = (b-firstControlButton()) / 4;
				if (playerControls[pl] == (b-firstControlButton()) % 4) {
					selected = true;
				}
			}
			if (selected) {
				buttons[b].select(true);
			} else {
				buttons[b].select(false);
			}
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
								} else 	if (pressedButton >= firstControlButton() && pressedButton < firstControlButton() + 4*4) {
									int pl = (pressedButton-firstControlButton()) / 4;
									selectControls(pl, (pressedButton-firstControlButton()) % 4);
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
		Program::getInstance()->fps->update();
		Program::getInstance()->fps->waitStandard();

	}
}


void MenuPlayers::selectControls(int player, int control) {
	// Mouse: only one player can use it
	if (control == 0) {
		for (int p=0; p<4; p++) {
			if (playerControls[p] == 0 && p != player) {
				playerControls[p] = 3; // None
			}
		}
	}
	// Keyboard: only one player can use it
	if (control == 1) {
		for (int p=0; p<4; p++) {
			if (playerControls[p] == 1 && p != player) {
				playerControls[p] = 3; // None
			}
		}
	}

	playerControls[player] = control;
}


