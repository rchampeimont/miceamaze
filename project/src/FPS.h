// Copyright (c) 2012, River CHAMPEIMONT
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


#ifndef FPS_H_
#define FPS_H_

#include "Program.h"

class FPS {
public:
	int fps;
	int framesSinceLastSecond;
	Uint32 lastSecond;
	int limitFPS;
	int frameDelay;

	FPS();
	~FPS();
	void decideLimitFPS();
	void renderInMenu();
	void renderInGame();
	void waitInMenu();
	void waitInGame();

private:
	// Function to count FPS
	void update() {
		Functions::clearGlErrors("FPS::update()");
		framesSinceLastSecond++;
		Uint32 t = SDL_GetTicks();
		if (t - lastSecond > 1000) {
			fps = framesSinceLastSecond;
			lastSecond = t;
			framesSinceLastSecond = 0;
		}
	}
};


#endif /* FPS_H_ */
