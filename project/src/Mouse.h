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


#ifndef MOUSE_H_
#define MOUSE_H_

#include "Color.h"
#include "Animal.h"

class Maze;

// Class for mouse (the animal, not the input device)
class Mouse: public Animal {
	 bool magic;
	 bool sick;

public:
	static unsigned int mouseTexture, sickMouseTexture, drillMouseTexture;
	static void loadTexture();
	Mouse(Maze *, int, int, int);
	bool reachedHouse(Game*, int);
	void render();
	int isKilled();
	void makeMagic();
	void makeSick();
	void makeDrill();
	Color adjustColor(Color);
	bool isMagic() {
		return magic;
	}
	bool isSick() {
		return sick;
	}
	bool isDrill() {
	return drill;
	}
};

#endif /* MOUSE_H_ */
