// Copyright (c) 2013, Raphael CHAMPEIMONT
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


#include "HowToPlay.h"

#include "Program.h"
#include "RenderFlatText.h"


HowToPlay::HowToPlay() : cursor(-1), snake(NULL, 0, 0, 0) {
	hasDisplayList = false;
	fixedObjectsDisplayList = 0;
	initVideoCounter = -1;
}

HowToPlay::~HowToPlay() {
	if (hasDisplayList)
		glDeleteLists(fixedObjectsDisplayList, 1);
}

void HowToPlay::prepareRender() {
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
	RenderFlatText::render("How to play", 0);

	// render version
	vector<string> lines;
	lines.push_back("Mice will appear continuously.");
	lines.push_back("Your goal is to bring more mice to your house than other players before the time is over.");
	lines.push_back("");
	lines.push_back("Arrows: If you play using your mouse, you have 2 ways to place arrows:");
	lines.push_back(" - either use left-click/right-click/wheel-up/wheel-down,");
	lines.push_back(" - or click and maintain pressed then move in one direction and release.");
	lines.push_back("If you play using the keyboard, use the arrows to move the cursor and I,J,K,L to place arrows.");
	lines.push_back("In any case, you cannot have more than 3 arrows at the same time.");
	lines.push_back("");
	lines.push_back("Snakes will eat mice they meet until they get sick of eating too much and die.");
	lines.push_back("If a snake reaches your house, you loose 10 points.");
	lines.push_back("");
	lines.push_back("The golden mouse will trigger a special event for 10 seconds. These are:");
	lines.push_back(" - mouse mania (much more mice appear),");
	lines.push_back(" - snake attack (many snakes appear),");
	lines.push_back(" - color madness (new mice are randomly colored),");
	lines.push_back(" - eagle (you get the eagle which protects you from snakes).");
	for (unsigned int i=0; i<lines.size(); i++) {
		glLoadIdentity();
		glTranslatef(-0.83, 0.57-0.06*i, 0);
		glScalef(0.05, 0.05, 1);
		RenderFlatText::render(lines[i], -1);
	}

	glEndList();

	initVideoCounter = Program::getInstance()->initVideoCounter;
}

void HowToPlay::run() {
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

		// render mouse
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(-0.9, 0.57, 0);
		glScalef(0.05, 0.05f*4.0f/3.0f, 0);
		glRotatef(-90, 0, 0, 1);
		glBindTexture(GL_TEXTURE_2D, Mouse::mouseTexture);
		glBegin(GL_QUADS);
		float color = (SDL_GetTicks() % 2000)/1000.0f;
		if (color > 1) color = 2 - color;
		glColor3f(color, color, color);
		glTexCoord2f(0, 0); glVertex2f(-1, -1);
		glTexCoord2f(0, 1); glVertex2f(-1, 1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, -1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		// render golden mouse
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(-0.9, -0.15, 0);
		glScalef(0.05, 0.05f*4.0f/3.0f, 0);
		glRotatef(-90, 0, 0, 1);
		glBindTexture(GL_TEXTURE_2D, Mouse::mouseTexture);
		glBegin(GL_QUADS);
		glColor3f(1, 0.7, 0);
		glTexCoord2f(0, 0); glVertex2f(-1, -1);
		glTexCoord2f(0, 1); glVertex2f(-1, 1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, -1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		// render snake
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(-0.9, 0, 0);
		glScalef(0.05, 0.05f*4.0f/3.0f, 0);
		glRotatef(-90, 0, 0, 1);
		glBindTexture(GL_TEXTURE_2D, Snake::snakeTexture);
		glBegin(GL_QUADS);
		float life = (SDL_GetTicks() % 2000)/1000.0f;
		if (life > 1) life = 2 - life;
		snake.life = life;
		snake.adjustColor(snake.color).gl();
		glTexCoord2f(0, 0); glVertex2f(-1, -1);
		glTexCoord2f(0, 1); glVertex2f(-1, 1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, -1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		// render arrow
		glLoadIdentity();
		glTranslatef(-0.9, 0.4, 0);
		arrow.player = (SDL_GetTicks() / 1000) % 4;
		arrow.render();

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



