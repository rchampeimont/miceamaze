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


#ifndef ANIMAL_H_
#define ANIMAL_H_

#include "Color.h"

class Maze;
class Game;

class Animal {
protected:
bool drill;
public:
	static int loadTexture(const char*);
	float x;
	float y;
	int curI, curJ;
	float angle; // angle in degrees (0 is the right, positive is anti-clockwise)
	int direction; // 0 = right, 1 = up, 2 = left, 3 = down
	float speed;
	int rotating; // 0 = not rotating, 1 = rotating left, -1 = rotating right
	Maze *maze;
	Color color;
	bool obeyed;
	bool dying;
	float dieAnim;
	float inHouse;
	float life;

	virtual void render() {}
	Animal(Maze*, int, int, int);
	int walk(Game*);
	void getFutureCell(int*, int*);

	virtual bool reachedHouse(Game *, int);
	virtual int isKilled() {
		return 0;
	}
	virtual ~Animal() {}
};

#endif /* ANIMAL_H_ */
