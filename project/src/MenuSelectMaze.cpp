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


#include "MenuSelectMaze.h"
#include "Program.h"
#include "RenderFlatText.h"

using namespace std;

int MenuSelectMaze::page = 0;

MenuSelectMaze::MenuSelectMaze() : cursor(-1) {

}

MenuSelectMaze::~MenuSelectMaze() {
	for (unsigned k=0; k<mazes.size(); k++) {
		delete mazes[k];
	}
	mazes.clear();
}

void MenuSelectMaze::addMaze(string path) {
	Maze *maze = new Maze();
	mazes.push_back(maze);
	mazePaths.push_back(path);
	maze->load(path);
}

void MenuSelectMaze::translateForMazeRender(int i, int j) {
	glLoadIdentity();
	glTranslatef(-0.52 + 0.6*j, 0.4-0.8*i, 0);
	glScalef(0.35, 0.35, 1);
}

void MenuSelectMaze::prepareMazesRender() {
	glLoadIdentity();
	int m = 0;
	for (int i=0; i<2; i++) {
		for (int j=0; j<3; j++) {
			if (m < (int) mazes.size()) {
				translateForMazeRender(i, j);
				glTranslatef(0, -0.03, 1);
				mazes[m]->prepareRender(NULL);
			}
			m++;
		}
	}
}

void MenuSelectMaze::prepareRender() {
	glLoadIdentity();
	cursor.prepareRender();
	glClearColor(0.2, 0.1, 0, 0);

	prepareMazesRender();

	initVideoCounter = Program::getInstance()->initVideoCounter;
}

void MenuSelectMaze::searchMazes() {
	vector<string> files = Functions::listFiles(Program::getInstance()->dataPath + "/mazes");
	sort(files.begin(), files.end());
	for (unsigned k=0; k<files.size(); k++) {
		string file = files[k];
		if (file.length() < 5) continue; // need at least 1 char + ".txt"
		bool ok = true;
		ok = ok && (file[file.length()-1] == 't' || file[file.length()-1] == 'T');
		ok = ok && (file[file.length()-2] == 'x' || file[file.length()-2] == 'X');
		ok = ok && (file[file.length()-3] == 't' || file[file.length()-3] == 'T');
		ok = ok && (file[file.length()-4] == '.');
		if (ok) {
			availableMazes.push_back(Program::getInstance()->dataPath + "/mazes/" + file);
		}
	}
	if (page*6 >= (int) availableMazes.size()) {
		page = 0;
	}
}

// Load mazed  but don't prepare render
void MenuSelectMaze::loadMazes() {
	// Clear currently loaded mazed
	for (unsigned k=0; k<mazes.size(); k++) {
		delete mazes[k];
	}
	mazes.clear();
	mazePaths.clear();

	// Load 6 mazes
	int firstMaze, lastMaze;
	firstMaze = page * 6;
	lastMaze = firstMaze + 5;
	if (lastMaze >= (int) availableMazes.size()) {
		lastMaze = availableMazes.size() - 1;
	}
	for (int m=firstMaze; m<=lastMaze; m++) {
		addMaze(availableMazes[m]);
	}
}

// Differs from loadMazes by the fact that we need to prepare render again
void MenuSelectMaze::loadNewMazes() {
	loadMazes();
	prepareMazesRender();
}

void MenuSelectMaze::updatePagesButtons() {
	for (int b=0; b<2; b++) {
		buttons[b].shown = ((b == 0 && page > 0) || (b == 1 && (page+1)*6 < (int) availableMazes.size()));
	}
}

void MenuSelectMaze::run() {
	buttons.push_back(Button(-0.98, -0.07, 0.10, 0.15, "<"));
	buttons.push_back(Button(0.87, -0.07, 0.10, 0.15, ">"));
	buttons.push_back(Button(-0.6, -0.97, 1.2, 0.12, "Back", 0.7));
	int pressedButton = -1;

	// place cursor at correct position
	cursor.setFromMouse();

	// search for available mazes
	searchMazes();

	// load 6 first mazes
	loadMazes();

	// Prepare rendering
	prepareRender();

	while (true) {
		if (initVideoCounter != Program::getInstance()->initVideoCounter) {
			// Video was reinitialized, so we need to resend display lists
			prepareRender();
		}

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT);

		// show FPS counter
		Program::getInstance()->fps->renderInMenu();

		// render mazes
		int sel = -1;
		{
			const float x0 = -0.888672 + 0.03;
			const float y0 =  0.729167;
			const float w  = -0.390625 - (-0.888672);
			const float h  =  0.0703125 - y0;
			const float dx = -0.285156 - (-0.888672);
			const float dy = -0.0729166 - y0;
			int m = 0;
			for (int i=0; i<2; i++) {
				for (int j=0; j<3; j++) {
					if (cursor.x > x0 + j*dx && cursor.x < x0 + j*dx + w
							&& cursor.y < y0 + i*dy && cursor.y > y0 + i*dy + h) {
						if (m < (int) mazes.size()) {
							sel = m;
						}
					}
					m++;
				}
			}
		}
		{
			int m = 0;
			for (int i=0; i<2; i++) {
				for (int j=0; j<3; j++) {
					if (sel == m) {
						translateForMazeRender(i, j);
						glTranslatef(-0.2475, 0, 0);
						glScalef(0.8, 1, 1);
						glColor3f(.3, 0, .7);
						Functions::drawSquare();
					}
					if (m < (int) mazes.size()) {
						// show maze
						mazes[m]->render(NULL);

						// show maze name
						translateForMazeRender(i, j);
						glTranslatef(-0.2475, -1.2, 0);
						glScalef(0.2, 0.2, 1);
						glColor3f(1, 1, 1);
						RenderFlatText::render(mazes[m]->name, 0);
					}
					m++;
				}
			}
		}

		// render title
		glLoadIdentity();
		glTranslatef(0, .8, 0);
		glColor3f(1, 1, 1);
		glScalef(0.15, 0.15, 1);
		RenderFlatText::render("Select maze", 0);

		// make next page and previous page button exist only if needed
		updatePagesButtons();
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
						if (sel >= 0) {
							Program::getInstance()->scene = 2;
							Program::getInstance()->mazeToLoad = mazePaths[sel];
							return;
						}
						if (pressedButton >= 0) {
							if (buttons[pressedButton].over(cursor.x, cursor.y)) {
								if (pressedButton == 0) {
									page--;
									loadNewMazes();
									// we need to go directly to verify if <> buttons should exist
									updatePagesButtons();
								} else if (pressedButton == 1) {
									page++;
									loadNewMazes();
									updatePagesButtons();
								} else if (pressedButton == 2) {
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
		Program::getInstance()->fps->waitInMenu();

	}
}
