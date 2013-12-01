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


#include "Program.h"
#include "Game.h"
#include "RenderFlatText.h"

using namespace std;

Program *Program::instance = NULL;

Program::Program(int _argc, char *_argv[]) {
	Functions::verify(instance == NULL, "instance is not NULL");
	instance = this;

	argc = _argc;
	argv = _argv;
	initVideoCounter = 0;
	screenBPP = 32;
	emptyCursor = NULL;
	playerColors.push_back(Color(0, 0, 1));
	playerColors.push_back(Color(1, 0, 0));
	playerColors.push_back(Color(0, 1, 0));
	playerColors.push_back(Color(0.7, 0, 0.7));

	// SETTINGS THAT CAN BE CHANGED
	// full screen by default?
	fullscreen = true;
	mazeToLoad = "";
	resizedWindow.x = 0;
	resizedWindow.y = 0;

	// The game will always display in a 4/3 rectangle.
	screenRatio = 4.0f/3.0f;

	fps = NULL;
	scene = 0;
	screenHeight = 0;
	screenWidth = 0;
	screenOffsetX = 0;
	screenOffsetY = 0;
}

Program::~Program() {
	if (fps != NULL)
		delete fps;
	Mix_CloseAudio();
	Mix_Quit();
	SDL_FreeCursor(emptyCursor);
	SDL_Quit();
	instance = NULL;
}


// Called when changing video settings
void Program::reinitVideo() {
	initVideo();
#ifdef _WIN32
	// On Windows, SDL_SetVideoMode unloads the textures
	// so we need to reload them. On Linux, it doesn't
	// so we mustn't load them a second time (otherwise memory leak!).
	initVideoCounter++;
	loadAllTextures();
	RenderFlatText::reinit();
#endif
}

void Program::loadAllTextures() {
	Spawner::loadTexture();
	Snake::loadTexture();
	Mouse::loadTexture();
	House::loadTextures();
	Images::loadTextures();
}

// Taken from SDL man page
SDL_Cursor *Program::init_system_cursor(const char *image[]) {
	int i, row, col;
	Uint8 data[4*32];
	Uint8 mask[4*32];
	int hot_x, hot_y;

	i = -1;
	for ( row=0; row<32; ++row ) {
		for ( col=0; col<32; ++col ) {
			if ( col % 8 ) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				++i;
				data[i] = mask[i] = 0;
			}
			switch (image[4+row][col]) {
			case 'X':
			data[i] |= 0x01;
			mask[i] |= 0x01;
			break;
			case '.':
				mask[i] |= 0x01;
				break;
			case ' ':
				break;
			}
		}
	}
	sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

// Called on video init or re-init
// resize = initVideo() is called as the result of a
// window resize, therefore it should not "decide" the window size
void Program::initVideo() {
	Uint32 videoFlags = SDL_OPENGL;
	if (fullscreen) {
		// Use native resolution
		videoFlags |= SDL_FULLSCREEN;
		screenWidth = nativeResolution.x;
		screenHeight = nativeResolution.y;
	} else {
		//videoFlags |= SDL_RESIZABLE;
		if (resizedWindow.x == 0 && resizedWindow.y == 0) {
			// Create a window of 1024x768
			screenWidth = 1024;
			screenHeight = 768;
		} else {
			screenWidth = resizedWindow.x;
			screenHeight = resizedWindow.y;
		}
	}

	//cout << "Initializing in " << screenWidth << "x" << screenHeight << endl;
	if (SDL_SetVideoMode(screenWidth, screenHeight, screenBPP, videoFlags) == NULL) {
		Functions::fatalError(string("Unable to initialize video") + SDL_GetError());
		exit(1);
	}

	if (fullscreen) {
		SDL_WM_GrabInput(SDL_GRAB_ON);
	} else {
		SDL_WM_GrabInput(SDL_GRAB_OFF);
	}

	// OpenGL settings
	glDisable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glDisable(GL_DITHER);

	glViewport(0, 0, screenWidth, screenHeight);

	// We put black around the 4/3 rectangle if the screen ratio is different from 4/3.
	int correctWidth = screenHeight * screenRatio;
	if (correctWidth < screenWidth) {
		// Width is too high (typically we have a 16/9 screen)
		screenOffsetX = (screenWidth-correctWidth)/2;
		screenOffsetY = 0;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glScalef((float) screenHeight * screenRatio / screenWidth, 1, 1);
		glMatrixMode(GL_MODELVIEW);
		screenWidth = correctWidth;
	} else if (correctWidth > screenWidth) {
		// Height is too high (less usual)
		int correctHeight = screenWidth / screenRatio;
		screenOffsetX = 0;
		screenOffsetY = (screenHeight-correctHeight)/2;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glScalef(1, (float) screenWidth / screenRatio / screenHeight, 1);
		glMatrixMode(GL_MODELVIEW);
		screenHeight = correctHeight;
	} else {
		screenOffsetX = 0;
		screenOffsetY = 0;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}
}

// Change between window mode / fullscreen
void Program::toggleFullscreen() {
	fullscreen = ! fullscreen;
	reinitVideo();
	Program::getInstance()->config.save();
}


int Program::loadTexture(string path) {
	return loadTexture(path, NULL);
}

int Program::loadTexture(string path, IntXY *wh) {
	int texture = SOIL_load_OGL_texture
			(
					path.c_str(),
					SOIL_LOAD_AUTO,
					SOIL_CREATE_NEW_ID,
					SOIL_FLAG_INVERT_Y
			);

	Functions::verify(texture != 0, "could not load texture from", path.c_str());

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (wh != NULL) {
		int w, h, c;
		SOIL_free_image_data(SOIL_load_image(path.c_str(), &w, &h, &c, SOIL_LOAD_AUTO));
		wh->x = w;
		wh->y = h;
	}

	return texture;
}

void Program::run() {
	// Load user configuration
	config.load();

	// Check for data directory
	bool foundData = false;
	FILE *file = NULL;
#ifdef DATAPATH
	dataPath = DATAPATH;
	file = fopen((dataPath + "/MiceAmaze.txt").c_str(), "r");
	if (file == NULL) {
		cout << "could not open " << DATAPATH << "/MiceAmaze.txt" << " -> using ./data as data dir" << endl;
	} else {
		foundData = true;
		fclose(file);
	}
#endif

	if (!foundData) {
		dataPath = "data";
		file = fopen((dataPath + "/MiceAmaze.txt").c_str(), "r");
		Functions::verify(file != NULL, "Could not find MiceAmaze data. (cannot read data/MiceAmaze.txt)");
		fclose(file);
	}

	// init SDL
	if (SDL_Init(
			SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)
			< 0) {
		cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
		exit(1);
	}

	// request video hardware information
	const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();
	nativeResolution.x = videoInfo->current_w;
	nativeResolution.y = videoInfo->current_h;

	// initialize display
	SDL_WM_SetCaption(Functions::getAppName().c_str(), Functions::getAppName().c_str());

	SDL_Surface *icon = SDL_LoadBMP((Program::getInstance()->dataPath + "/images/icon32.bmp").c_str());
	SDL_WM_SetIcon(icon, NULL);

	char s[] = "SDL_VIDEO_CENTERED=center";
	SDL_putenv(s);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Init video
	initVideo();

	// Load textures
	loadAllTextures();
	RenderFlatText::init();

	const char *arrow2[] = {
			/* width height num_colors chars_per_pixel */
			"    32    32        3            1",
			/* colors */
			"X c #000000",
			". c #ffffff",
			"  c None",
			/* pixels */
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"                                ",
			"0,0"
	};
	emptyCursor = init_system_cursor(arrow2);

	SDL_SetCursor(Program::getInstance()->emptyCursor);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// Sound
	int flags = MIX_INIT_MP3;
	int initted = Mix_Init(flags);
	if ((initted&flags) != flags) {
		string err = "Sound init error ";
		err = err + Mix_GetError();
	    Functions::error(err);
	}
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
		Functions::error(Mix_GetError());
	}
	sound = new Sound();
	sound->init();

	// Set FPS limit depending if battery/AC
	fps = new FPS();
	fps->decideLimitFPS();

	// Initialize random number generator
	srand(SDL_GetTicks());

	scene = 0;

	while (true) {
		switch (scene) {
		case 0:
		{
			MenuMain *menu = new MenuMain();
			menu->run();
			delete menu;
			break;
		}
		case 1:
		{
			MenuSelectMaze *menu = new MenuSelectMaze();
			menu->run();
			delete menu;
			break;
		}
		case 2:
		{
			Game *game = new Game();
			game->run();
			delete game;
			break;
		}
		case 3:
		{
			MenuAbout *menu = new MenuAbout();
			menu->run();
			delete menu;
			break;
		}
		case 4:
		{
			MenuPlayers *menu = new MenuPlayers();
			menu->run();
			delete menu;
			break;
		}
		case 5:
		{
			MenuOptions *menu = new MenuOptions();
			menu->run();
			delete menu;
			break;
		}
		case 6:
		{
			HowToPlay *menu = new HowToPlay();
			menu->run();
			delete menu;
			break;
		}
		}
	}

}

// General event handling valid in any situation:
// Alt+F4 pressed, windows close clicked, fullscreen, ...
void Program::generalEventHandler(SDL_Event *event) {
	switch (event->type) {
	case SDL_QUIT:
		// User clicked on the close window button.
		exit(0);
		break;
	case SDL_KEYDOWN:
		if ((event->key.keysym.sym == SDLK_F4
				&& (event->key.keysym.mod & KMOD_LALT))
				|| (event->key.keysym.sym == SDLK_F4
						&& (event->key.keysym.mod & KMOD_RALT))
						|| (event->key.keysym.sym == SDLK_F12)
		) {
			// Alt+F4 was pressed, so exit.
			exit(0);
		} else if ((event->key.keysym.sym == SDLK_RETURN
				&& (event->key.keysym.mod & KMOD_LALT))
				|| (event->key.keysym.sym == SDLK_RETURN
						&& (event->key.keysym.mod & KMOD_RALT))
						|| (event->key.keysym.sym == SDLK_F11)
		) {
			// Switch between window / full screen
			Program::getInstance()->toggleFullscreen();
		}
		break;
		/*
		case SDL_VIDEORESIZE:
			resizedWindow.x = event->resize.w;
			resizedWindow.y = event->resize.h;
			reinitVideo();
			break;
		 */
	}
}

