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


#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include "SDL.h"

#include "Program.h"
#include "Maze.h"
#include "Cursor.h"

using namespace std;

class Game {
	int initVideoCounter;
	vector<Button> buttons;
public:
	bool running;
	bool ended;
	Uint32 lastSecond;
	Uint32 lastGameTick;
	int framesSinceLastSecond;
	int ticksSinceLastSecond;
	int frameDelay;
	Maze maze;
	vector<Cursor*> cursors;
	vector<int> scores;
	int players;
	int time;
	int timeLimit;
	int eagleOwner;
	bool pause;
	int specialMode;
	int specialModeStart;
	string message;
	int messageTime;
	vector<AI> AIs;
	Cursor cursor;

	Game();
	~Game();
	void run();
	void togglePause();
	void magicHappens(int);
	void showMessage(string);
	int getLeader();
	void prepareRender();
};

#endif /* GAME_H_ */
