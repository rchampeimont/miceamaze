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


#ifndef MAINCLASS_H_
#define MAINCLASS_H_

#include "Functions.h"

// Our headers
#include "IntXY.h"
#include "FloatXY.h"
#include "Button.h"
#include "Maze.h"
#include "AI.h"
#include "Game.h"
#include "MenuSelectMaze.h"
#include "MenuMain.h"
#include "MenuAbout.h"
#include "MenuPlayers.h"
#include "MenuOptions.h"
#include "HowToPlay.h"
#include "Color.h"
#include "Cursor.h"
#include "Config.h"
#include "Images.h"
#include "FPS.h"
#include "RenderFlatText.h"
#include "Button.h"
#include "Sound.h"

class FPS;
class Sound;

using namespace std;

// This class is a singleton.
class Program {
	static Program *instance;
public:
	int argc;
	char **argv;
	bool fullscreen;
	int screenWidth, screenHeight, screenBPP, screenOffsetX, screenOffsetY;
	float screenRatio;
	SDL_Cursor *emptyCursor;
	vector<Color> playerColors;
	Config config;
	IntXY nativeResolution;
	int initVideoCounter;
	int scene;
	string mazeToLoad;
	IntXY resizedWindow;
	FPS *fps;
	string dataPath;
	Sound *sound;

	Program(int argc, char *argv[]);
	void run();
	void titleScreen();
	~Program();
	SDL_Cursor *init_system_cursor(const char *[]);
	void initVideo();
	void autoDetectFps();
	void toggleFullscreen();
	void reinitVideo();
	void loadAllTextures();
	int loadTexture(string);
	int loadTexture(string, IntXY *);

	void generalEventHandler(SDL_Event *event);

	// Get instance of MainClass
	static Program *getInstance() {
		return instance;
	}

	void glCoordsFromWindowCoords(int x, int y, float *xgl, float *ygl) {
		if (xgl != NULL) *xgl = (float) (x-screenOffsetX) / screenWidth * 2 - 1;
		if (ygl != NULL) *ygl = - ((float) (y-screenOffsetY) / screenHeight * 2 - 1);
	}

};



#endif
