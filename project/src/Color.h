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


#ifndef COLOR_H_
#define COLOR_H_

#include "Functions.h"

class Color {
public:
	float r, g, b, a;
	Color(float rc, float gc, float bc) {
		r = rc;
		g = gc;
		b = bc;
		a = 1;
	}
	Color(float rc, float gc, float bc, float ac) {
		r = rc;
		g = gc;
		b = bc;
		a = ac;
	}
	Color() {
		r = g = b = 0;
		a = 1;
	}
	void black() {
		r = g = b = 0;
		a = 1;
	}
	void white() {
		r = g = b = 1;
		a = 1;
	}
	void gl() {
		glColor4f(r, g, b, a);
	}
	void gl4() {
		glColor4f(r, g, b, a);
	}
	void gl3() {
		glColor3f(r, g, b);
	}
};

#endif /* COLOR_H_ */
