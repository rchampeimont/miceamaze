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


#include "MenuAbout.h"

#include "Program.h"
#include "RenderFlatText.h"


MenuAbout::MenuAbout() : cursor(-1) {
	hasDisplayList = false;
	fixedObjectsDisplayList = 0;
	initVideoCounter = -1;
}

MenuAbout::~MenuAbout() {
	if (hasDisplayList)
		glDeleteLists(fixedObjectsDisplayList, 1);
}

void MenuAbout::prepareRender() {
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
	RenderFlatText::render("About " + Functions::getAppName(), 0);

	// render version
	vector<string> lines;
	lines.push_back(Functions::getAppName() + " " + Functions::getVersion());
	lines.push_back(Functions::getCopyright());
	string license = Functions::getLicense();
	int a = 0, b = 0;
	for (int i=0; i<(int) license.length(); i++) {
		if (license[i] == '\n') {
			b = i;
			lines.push_back(license.substr(a, b-a));
			a = i+1;
		}
	}
	for (unsigned int i=0; i<lines.size(); i++) {
		glLoadIdentity();
		glTranslatef(-0.935, 0.57-0.05*i, 0);
		glScalef(0.04, 0.04, 1);
		RenderFlatText::render(lines[i], -1);
	}

	glEndList();

	initVideoCounter = Program::getInstance()->initVideoCounter;
}

void MenuAbout::run() {
	vector<Button> buttons;
	buttons.push_back(Button(-0.8, -0.9, 1.6, 0.15, "OK"));
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


