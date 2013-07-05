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


#include "Images.h"
#include "Program.h"

vector<unsigned int> Images::textures;
vector<int> Images::widths;
vector<int> Images::heights;

void Images::loadTextures() {
	loadImage("eagle.png");
	loadImage("eagle2.png");
}

void Images::loadImage(string filename) {
	IntXY wh;
	unsigned int texture = Program::getInstance()->loadTexture((Program::getInstance()->dataPath + "/images/" + filename).c_str(), &wh);
	textures.push_back(texture);
	widths.push_back(wh.x);
	heights.push_back(wh.y);
}

void Images::clearTextures() {
	textures.clear();
	widths.clear();
	heights.clear();
}

void Images::renderImage(int imageIndex) {
	if ((unsigned) imageIndex >= textures.size() || imageIndex < 0)
		return;
	glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[imageIndex]);
	glBegin(GL_QUADS);
	FloatXY r = scaleRatio(imageIndex);
	glTexCoord2f(0, 0); glVertex3f(-r.x, -r.y*Program::getInstance()->screenRatio, 0);
	glTexCoord2f(0, 1); glVertex3f(-r.x, r.y*Program::getInstance()->screenRatio, 0);
	glTexCoord2f(1, 1); glVertex3f(r.x, r.y*Program::getInstance()->screenRatio, 0);
	glTexCoord2f(1, 0); glVertex3f(r.x, -r.y*Program::getInstance()->screenRatio, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

FloatXY Images::scaleRatio(int imageIndex) {
	FloatXY res;
	int w = widths[imageIndex];
	int h = heights[imageIndex];
	if (w > h) {
		res.x = 1;
		res.y = (float) h / (float) w;
	} else {
		res.y = 1;
		res.x = (float) w / (float) h;
	}
	return res;
}
