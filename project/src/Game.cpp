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


#include <iostream>
#include "Game.h"
#include "RenderFlatText.h"
#include "Button.h"

using namespace std;

Game::Game() : cursor(-1) {
	players = 4;

	for (int i=0; i<players; i++) {
		Cursor *cur = new Cursor(i);
		cur->x = 0.9;
		cur->y = -1 + (i+1)/10.0;
		cursors.push_back(cur);
		scores.push_back(0);
	}

	time = 0;
	timeLimit = 3*60*100;
	//timeLimit = 5*100; //debug
	eagleOwner = -1;
	pause = false;
	specialMode = 0;
	specialModeStart = 0;
	message = "Go!";
	messageTime = 0;
	ended = false;
}

Game::~Game() {
	for (unsigned i=0; i<cursors.size(); i++) {
		delete cursors[i];
	}
}

void Game::showMessage(string m) {
	message = m;
	messageTime = time;
}

void Game::togglePause() {
	if (time > timeLimit) {
		return; // Game has already ended: no point to pause
	}
	if (pause) {
		pause = false;
		// resume game
		running = true;
		lastGameTick = SDL_GetTicks();
		ticksSinceLastSecond = 100;
	} else {
		// pause game
		pause = true;
		running = false;
	}
}

void Game::magicHappens(int player) {
	switch (rand() % 4) {
	case 0:
		eagleOwner = player;
		showMessage("Got the eagle");
		break;
	case 1:
		specialModeStart = time;
		specialMode = 1;
		showMessage("Mouse mania");
		// remove all snakes
		for (unsigned int k=0; k<maze.snakes.size(); k++) {
			maze.snakes[k].dying = true;
		}
		break;
	case 2:
		specialModeStart = time;
		specialMode = 2;
		showMessage("Snake attack");
		break;
	case 3:
		specialModeStart = time;
		specialMode = 3;
		showMessage("Color madness");
		break;
	}

}

// Get the leading player, or -1 if tie
int Game::getLeader() {
	int index = 0;
	int max = scores[0];
	for (unsigned int i=1; i<scores.size(); i++) {
		if (scores[i] > max) {
			max = scores[i];
			index = i;
		}
	}
	int occ=0;
	for (unsigned int i=0; i<scores.size(); i++) {
		if (scores[i] == max) {
			occ++;
		}
	}
	if (occ > 1) {
		return -1; // tie
	} else {
		return index;
	}
}

void Game::prepareRender() {
	glLoadIdentity();
	maze.prepareRender(this);
	for (int i=0; i<players; i++) {
		cursors[i]->prepareRender();
	}
	cursor.prepareRender();
	glClearColor(0.2, 0.1, 0, 0);

	initVideoCounter = Program::getInstance()->initVideoCounter;
}

void Game::run() {
	vector<Button> buttons;
	int pressedButton = -1;

	buttons.push_back(Button(-0.7, -0.6, 1.4, 0.15, "Play the same maze again"));
	buttons[buttons.size()-1].shown = false; // hidden before game end

	buttons.push_back(Button(-0.7, -0.8, 1.4, 0.15, "Back to main menu"));
	buttons[buttons.size()-1].shown = false; // hidden before game end

	buttons.push_back(Button(0.6, -0.4, 0.3, 0.10, "Pause", 0.5));
	buttons.push_back(Button(0.6, -0.55, 0.3, 0.10, "Stop game", 0.5));


	cursor.setFromMouse();

	Program::getInstance()->fps->decideLimitFPS();

	// Load maze
	maze.load(Program::getInstance()->mazeToLoad);

	// Save maze as last maze
	//maze.save(Program::getInstance()->config.path + "/tmp_last_maze.txt");

	// Random house placement
	maze.randomizeHouses();

	running = true;
	lastGameTick = SDL_GetTicks();
	ticksSinceLastSecond = 0;

	// mouse (input device)
	Cursor* mouseControlledCursor = NULL;
	for (int p=0; p<4; p++) {
		if (MenuPlayers::playerControls[p] == 0) {
			mouseControlledCursor = cursors[p];
			mouseControlledCursor->setFromMouse();
			break;
		}
	}
	float mouseDownX = 0;
	float mouseDownY = 0;
	int mouseDownI = -1;
	int mouseDownJ = -1;

	for (int p=0; p<4; p++) {
		if (MenuPlayers::playerControls[p] == 2) {
			AIs.push_back(AI(p, &maze));
		}
	}

	for (unsigned int i=0; i<AIs.size(); i++) {
		AIs[i].init();
	}

	// keyboard
	Cursor* keyboardControlledCursor = NULL;
	for (int p=0; p<4; p++) {
		if (MenuPlayers::playerControls[p] == 1) {
			keyboardControlledCursor = cursors[p];
			break;
		}
	}
	if (keyboardControlledCursor != NULL) {
		keyboardControlledCursor->x = maze.x0 + maze.cellWidth/2;
		keyboardControlledCursor->y = maze.y0 + maze.cellHeight/2;
	}

	// Prepare rendering
	prepareRender();

	// Game main loop
	while (true) {
		if (initVideoCounter != Program::getInstance()->initVideoCounter) {
			// Video was reinitialized, so we need to resend display lists
			prepareRender();
		}

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT);


		// Maze rendering
		maze.render(this);


		// show scores
		for (int p=0; p<players; p++) {
			glLoadIdentity();
			glTranslatef(0.9, 0.6-0.15*p, 0);
			glScalef(0.2, 0.15, 1);
			Program::getInstance()->playerColors[p].gl();
			RenderFlatText::render(Functions::toString(scores[p]), 1);
		}


		// show remaining time
		glLoadIdentity();
		glTranslatef(0.58, 0.8, 0);
		glScalef(0.2, 0.15, 1);
		glColor3f(1, 1, 1);
		int remainingTime = timeLimit - time;
		int remainingTimeSec = Functions::roundCeil((remainingTime)/100.0f);
		char remainingTimeString[10];
		snprintf(remainingTimeString, sizeof(remainingTimeString), "%d:%02d", remainingTimeSec/60, remainingTimeSec%60);
		if (remainingTime >= 500 || remainingTime <= 0 || (remainingTime % 75 > 37)) {
			RenderFlatText::render(remainingTimeString);
		}



		// show message
		{
			float f = (500 - (time - messageTime)) / 500.0f;
			if (f > 0) {
				glLoadIdentity();
				glTranslatef(0.72, -0.2, 0);
				glScalef(0.08, 0.08, 1);
				glColor3f(0.2+f*0.8, 0.1+0.9*f, f);
				RenderFlatText::render(message, 0);
			}
		}


		// show eagle
		if (eagleOwner < 0) {
			glLoadIdentity();
			glTranslatef(0.62, -0.8, -0.0005);
			glScalef(0.15, 0.15, 1);
			glColor3f(1, 0.9, 0.5);
			Images::renderImage(0);
		}


		// show FPS counter
		Program::getInstance()->fps->renderInGame();


		// show pause
		if (pause) {
			glLoadIdentity();
			glTranslatef(0, -0.1, 0);
			glColor3f(1, 1, 1);
			glScalef(0.5, 0.5, 1);
			RenderFlatText::render("PAUSE", 0);
		}

		// things to show when game has ended
		if (ended) {
			// show winner
			glLoadIdentity();
			glTranslatef(0, -0.4, 0);
			glScalef(0.8, 0.5, 1);
			glColor3f(1, 1, 1);
			Functions::drawSquare();
			glScalef(0.95, 0.95, 1);
			glColor3f(0, 0, 0);
			Functions::drawSquare();

			glLoadIdentity();
			glTranslatef(0, -0.4, 0);
			glColor3f(1, 1, 1);
			glScalef(0.45, 0.45, 1);
			int leader = getLeader();
			if (leader == -1) {
				glColor3f(1, 1, 1);
				RenderFlatText::render("TIE", 0);
			} else {
				Program::getInstance()->playerColors[leader].gl();
				RenderFlatText::render("WINNER", 0);
			}
		}


		// render buttons
		for (int b=0; b < (int) buttons.size(); b++) {
			glLoadIdentity();
			if (pressedButton == b) {
				buttons[b].render(2);
			} else {
				buttons[b].render(buttons[b].over(cursor.x, cursor.y));
			}
		}

		// show cursors
		if (!ended) {
			for (int i = 0; i < players; i++) {
				if (MenuPlayers::playerControls[i] != 2 && MenuPlayers::playerControls[i] != 3) {
					// cursor is owned by real player (neither AI nor none)
					cursors[i]->render();
				}
			}
		}
		if (ended || mouseControlledCursor == NULL) {
			cursor.render();
		}


		// Display new image
		SDL_GL_SwapBuffers();


		// Event handling
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			Program::getInstance()->generalEventHandler(&event);
			switch (event.type) {
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					Program::getInstance()->scene = 0;
					return;
				}
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_PAUSE
						|| event.key.keysym.sym == SDLK_p) {
					togglePause();
				}
				if (keyboardControlledCursor != NULL) {
					float newX = keyboardControlledCursor->x;
					float newY = keyboardControlledCursor->y;
					bool changed = false;
					bool arrowAdded = false;
					int direction = 0;
					if (event.key.keysym.sym == SDLK_LEFT) {
						newX -= maze.cellWidth;
						changed = true;
					} else if (event.key.keysym.sym == SDLK_RIGHT) {
						newX += maze.cellWidth;
						changed = true;
					} else if (event.key.keysym.sym == SDLK_UP) {
						newY += maze.cellHeight;
						changed = true;
					} else if (event.key.keysym.sym == SDLK_DOWN) {
						newY -= maze.cellHeight;
						changed = true;
					} else if (event.key.keysym.sym == SDLK_i) {
						arrowAdded = true;
						direction = 1;
					} else if (event.key.keysym.sym == SDLK_j) {
						arrowAdded = true;
						direction = 2;
					} else if (event.key.keysym.sym == SDLK_k) {
						arrowAdded = true;
						direction = 3;
					} else if (event.key.keysym.sym == SDLK_l) {
						arrowAdded = true;
						direction = 0;
					}

					if (changed) {
						if (newX >= maze.x0 && newX <= maze.x0 + maze.width*maze.cellWidth) {
							keyboardControlledCursor->x = newX;
						}
						if (newY >= maze.y0 && newY <= maze.y0 + maze.height*maze.cellHeight) {
							keyboardControlledCursor->y = newY;
						}
					}

					if (arrowAdded && running) {
						maze.addArrow(
								keyboardControlledCursor->getCellI(&maze),
								keyboardControlledCursor->getCellJ(&maze),
								keyboardControlledCursor->getPlayer(), direction);
					}
				}
				break;
			case SDL_MOUSEMOTION:
				cursor.setFromWindowXY(event.motion.x, event.motion.y);
				if (mouseControlledCursor != NULL) {
					mouseControlledCursor->setFromWindowXY(event.motion.x, event.motion.y);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
					for (int b=0; b<(int) buttons.size(); b++) {
						if (buttons[b].over(cursor.x, cursor.y)) {
							pressedButton = b;
						}
					}
				}
				if (!ended && mouseControlledCursor != NULL) {
					mouseDownX = mouseControlledCursor->x;
					mouseDownY = mouseControlledCursor->y;
					mouseDownI = mouseControlledCursor->getCellI(&maze);
					mouseDownJ = mouseControlledCursor->getCellJ(&maze);
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
					if (pressedButton >= 0) {
						if (buttons[pressedButton].over(cursor.x, cursor.y)) {
							if (pressedButton == 0) {
								return;
							} else if (pressedButton == 1) {
								Program::getInstance()->scene = 0;
								return;
							} else if (pressedButton == 2) {
								togglePause();
							} else if (pressedButton == 3) {
								Program::getInstance()->scene = 0;
								return;
							}
						}
						pressedButton = -1;
					}
				}
				if (!ended && mouseControlledCursor != NULL && mouseDownI != -1 && mouseDownJ != -1) {
					int direction = -1;
					float delta1 = (mouseControlledCursor->x - mouseDownX) / maze.cellWidth;
					if (delta1 < 0) delta1 = -delta1;
					float delta2 = (mouseControlledCursor->y - mouseDownY) / maze.cellHeight;
					if (delta2 < 0) delta2 = -delta2;
					//cout << delta1 << delta2 << endl;
					if (delta1 > delta2 && delta1 > 0.2) {
						if (mouseControlledCursor->x > mouseDownX) {
							direction = 0;
						} else {
							direction = 2;
						}
					} else if (delta1 < delta2 && delta2 > 0.2) {
						if (mouseControlledCursor->y > mouseDownY) {
							direction = 1;
						} else {
							direction = 3;
						}
					} else {
						if (event.button.button == SDL_BUTTON_LEFT) {
							direction = 2;
						} else if (event.button.button == SDL_BUTTON_RIGHT) {
							direction = 0;
						} else if (event.button.button == SDL_BUTTON_WHEELUP) {
							direction = 1;
						} else if (event.button.button == SDL_BUTTON_WHEELDOWN) {
							direction = 3;
						}
					}
					if (direction != -1 && running) {
						maze.addArrow(mouseDownI, mouseDownJ, mouseControlledCursor->getPlayer(), direction);
					}
				}
				break;
			}
		}

		// Run physical engine 100 times per second
		if (running) {
			Uint32 t = SDL_GetTicks();
			int k = 0;
			// The && k < 5 is because we want
			// to force rendering every 5 ticks (this is at least 20 FPS)
			while (lastGameTick + 10 < t && k < 5) {
				// Run one physical iteration
				maze.runTick(this);
				for (int l=0; l<(int) AIs.size(); l++) {
					if (time % 100 == 25*l) {
						// Allow AI to place an arrow
						AIs[l].play();
					}
				}
				time++;
				if (lastGameTick < t - 500) {
					// We really are late (more than 50 ticks late), so forget about it
					lastGameTick = t;
				} else {
					lastGameTick += 10;
				}
				k++;
			}

			if (specialMode != 0 && time > specialModeStart + 1000) {
				specialMode = 0;
				showMessage("Back to normal");
			}
		}

		// Wait the correct delay to reach the correct number of FPS
		Program::getInstance()->fps->waitInGame();

		if (running && time > timeLimit) {
			running = false;
			ended = true;
			buttons[0].shown = true;
			buttons[1].shown = true;
			buttons[2].shown = false;
			buttons[3].shown = false;
		}

		//Functions::verify(glGetError() == GL_NO_ERROR);
	}
}

