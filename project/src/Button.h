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


#ifndef BUTTON_H_
#define BUTTON_H_

class Button {
public:
	float x, y, w, h, textSizeFactor;
	string text;
	Color color;
	bool shown;

	Button(float x0, float y0, float w0, float h0, string text0, float textSizeFactor0) : color(1,1,1) {
		x = x0;
		y = y0;
		w = w0;
		h = h0;
		text = text0;
		textSizeFactor = textSizeFactor0;
		shown = true;
	}

	Button(float x0, float y0, float w0, float h0, string text0) : color(1,1,1) {
		x = x0;
		y = y0;
		w = w0;
		h = h0;
		text = text0;
		textSizeFactor = 1.0f;
		shown = true;
	}

	bool over(float curx, float cury) {
		if (!shown) return false;
		return curx >= x && curx <= x+w && cury >= y && cury <= y+h;
	}

	void render(int state);

	void select(bool state) {
		if (state) {
			color.r = 1;
			color.g = 0.5;
			color.b = 0;
		} else {
			color.white();
		}
	}
};


#endif /* BUTTON_H_ */
