// Copyright (c) 2012, RAPHAEL CHAMPEIMONT
// All rights reserved.

#ifndef RENDERTEXT_H_
#define RENDERTEXT_H_

#include "Program.h"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class RenderText {
	static RenderText *instance;
	RenderText();
	GLuint	base;				// Base Display List For The Font Set
	#ifdef _WIN32
	GLYPHMETRICSFLOAT gmf[256];	// Storage For Information About Our Outline Font Characters
	#endif
public:
	static RenderText *getInstance() {
		if (instance == NULL) {
			instance = new RenderText;
		}
		return instance;
	}
	void render(string s);
	~RenderText();
};


#endif /* RENDERTEXT_H_ */
