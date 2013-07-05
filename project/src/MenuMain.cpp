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


#include "MenuMain.h"
#include "Program.h"
#include "RenderFlatText.h"


MenuMain::MenuMain() : cursor(-1) {
	hasDisplayList = false;
	fixedObjectsDisplayList = 0;
	initVideoCounter = -1;
}

MenuMain::~MenuMain() {
	if (hasDisplayList)
		glDeleteLists(fixedObjectsDisplayList, 1);
}

void MenuMain::prepareRender() {
	glLoadIdentity();
	cursor.prepareRender();
	glClearColor(0.2, 0.1, 0, 0);

	// Precompile a list of operations to draw the fixed part of the maze
	fixedObjectsDisplayList = glGenLists(1);
	hasDisplayList = true;
	glNewList(fixedObjectsDisplayList, GL_COMPILE);

	// render title
	glLoadIdentity();
	glTranslatef(0, .55, 0);
	glColor3f(1, 1, 1);
	glScalef(0.25, 0.2, 1);
	RenderFlatText::render(Functions::getAppName(), 0);

	// render copyright
	glLoadIdentity();
	glTranslatef(0, -0.96, 0);
	glScalef(0.06, 0.05, 1);
	RenderFlatText::render(Functions::getAppName() + " " + Functions::getVersion() + " " + Functions::getCopyright(), 0);

	glEndList();

	initVideoCounter = Program::getInstance()->initVideoCounter;
}

void MenuMain::run() {
	vector<Button> buttons;
	float y0 = 0.05;
	float delta = 0.18;
	buttons.push_back(Button(-0.8, y0, 1.6, 0.15, "Start game"));
	buttons.push_back(Button(-0.8, y0-4*delta, 1.6, 0.15, "About"));
	buttons.push_back(Button(-0.8, y0-5*delta, 1.6, 0.15, "Exit"));
	buttons.push_back(Button(-0.8, y0-delta, 1.6, 0.15, "Select players"));
	buttons.push_back(Button(-0.8, y0-2*delta, 1.6, 0.15, "Options"));
	buttons.push_back(Button(-0.8, y0-3*delta, 1.6, 0.15, "How to play"));
	int pressedButton = -1;

	int decoMice[4] = {0, 300, 1000, 1500};
	float decoMiceColors[4] = {1, 0.5, 0.3, 0.7};

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
			if (pressedButton == b) {
				buttons[b].render(2);
			} else {
				buttons[b].render(buttons[b].over(cursor.x, cursor.y));
			}
		}

		// render eagle
		glLoadIdentity();
		glTranslatef(0, 0.85, 0);
		glScalef(0.10, 0.10, 0);
		float r = ((SDL_GetTicks()/2) % 3600)/10.0f;
		glRotatef(r, 0, 1, 0);
		glTranslatef(5, 0, 0);
		glRotatef(90, 0, 1, 0);
		glScalef(0.5, 0.5, 1);
		glColor3f(1, 1, 1);
		Images::renderImage(1);

		// render mouse
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		for (int k=0; k<4; k++) {
			glLoadIdentity();
			glTranslatef(((SDL_GetTicks()/5 + decoMice[k]) % 2200)/1000.0f - 1.1, 0.4, 0);
			glScalef(0.05, 0.05f*4.0f/3.0f, 0);
			glRotatef(-90, 0, 0, 1);
			glBindTexture(GL_TEXTURE_2D, Mouse::mouseTexture);
			glBegin(GL_QUADS);
			glColor3f(decoMiceColors[k], decoMiceColors[k], decoMiceColors[k]);
			glTexCoord2f(0, 0); glVertex2f(-1, -1);
			glTexCoord2f(0, 1); glVertex2f(-1, 1);
			glTexCoord2f(1, 1); glVertex2f(1, 1);
			glTexCoord2f(1, 0); glVertex2f(1, -1);
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

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
									Program::getInstance()->scene = 1;
									return;
								} else if (pressedButton == 1) {
									Program::getInstance()->scene = 3;
									return;
								} else if (pressedButton == 2) {
									exit(0);
								} else if (pressedButton == 3) {
									Program::getInstance()->scene = 4;
									return;
								} else if (pressedButton == 4) {
									Program::getInstance()->scene = 5;
									return;
								} else if (pressedButton == 5) {
									Program::getInstance()->scene = 6;
									return;
								}
							}
							pressedButton = -1;
						}
					}
					break;
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						exit(0);
					}
					break;
			}

		}

		// Count FPS and wait some time before rendering next frame
		Program::getInstance()->fps->update();
		Program::getInstance()->fps->waitStandard();

	}
}



