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


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x500
#endif
#endif

// All external dependencies of headers are here
// The important thing here is to not include
// headers that could include this file.

// Include everything we may need
// C++ standard libs
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef _WIN32
// Windows
#include <windows.h>
#include <shlobj.h>
#include <wingdi.h>
#else
// Linux
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#endif

// SDL
#include "SDL.h"

// OpenGL
#ifdef __APPLE__
// OS X
#include "OpenGL/gl.h"
#else
// Linux, Windows, ...
#include "GL/gl.h"
#endif

// SOIL
#ifndef _WIN32
#include "SOIL/SOIL.h"
#else
#include "SOIL.h"
#endif

#ifndef EMSCRIPTEN
#ifdef _WIN32
#define FONT_WIN
#else
#define FONT_GLC
#endif
#endif

#ifdef FONT_GLC
// GLC
#include "GL/glc.h"
#endif


// Very basic dependency-free internal headers
#include "Version.h"
#include "IntXY.h"
#include "Color.h"

using namespace std;


// All static functions we might need anywhere
class Functions {
public:
	static string getAppName() {
		return "MiceAmaze";
	}
	static string getVersion() {
		return VERSION;
	}
	static string getCopyrightASCII() {
		return "(c) 2012-2013 Raphael Champeimont";
	}
	static string getCopyright() {
		return "\xa9 2012-2013 Rapha\xebl Champeimont";
	}
	static string getLicense();

	static vector<string> listFiles(string);

	static float fabs(float x) {
		if (x < 0) {
			return -x;
		} else {
			return x;
		}
	}

	static int roundCeil(float x) {
		int i = x;
		if (i < x) {
			return i+1;
		} else {
			return i;
		}
	}




	// Check condition is true, otherwise exit.
	static void verify(int condition) {
		if (!condition) {
			fatalError("A fatal error occured.");
		}
	}
	static void verify(int condition, const char *message) {
		if (!condition) {
			fatalError(message);
		}
	}
	static void verify(int condition, const char *message, int k) {
		if (!condition) {
			fatalError(string(message) + " " + toString(k));
		}
	}
	static void verify(int condition, const char *message, const char *message2) {
		if (!condition) {
			fatalError(string(message) + " " + string(message2));
		}
	}

	static void message(string message);
	static void message(const char *message);

	static void error(string message);
	static void error(const char *message);

	static void fatalError(string message);
	static void fatalError(const char *message);

	static void clearGlErrors() {
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR) {
			error("OpenGL error: " + toString(errorCode));
		}
	}

	static string toString(int x) {
		char s[20];
		snprintf(s, sizeof(s), "%d", x);
		s[sizeof(s)-1] = '\0';
		return string(s);
	}

	static void drawSquare() {
		glBegin(GL_QUADS);
		glVertex2f(-1, -1);
		glVertex2f(-1,  1);
		glVertex2f( 1,  1);
		glVertex2f( 1, -1);
		glEnd();
	}


	static void drawSquare(float x, float y, float w, float h) {
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x, y+h);
		glVertex2f(x+w, y+h);
		glVertex2f(x+w, y);
		glEnd();
	}


	static void drawStupidTriangle() {
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex3f(0.0f, 1.0f, 0.0f); // Top
		glColor3f(0, 1, 0);
		glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom Left
		glColor3f(0, 0, 1);
		glVertex3f(1.0f, -1.0f, 0.0f); // Bottom Right
		glEnd();
	}

	static void drawMulticolorCube() {
		glBegin(GL_QUADS); // Draw A Quad
		glColor3f(1, 1, 0);
		glVertex3f(1.0f, 1.0f, -1.0f); // Top Right Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f, -1.0f); // Top Left Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f, 1.0f); // Bottom Left Of The Quad (Top)
		glVertex3f(1.0f, 1.0f, 1.0f); // Bottom Right Of The Quad (Top)
		glColor3f(0, 1, 1);
		glVertex3f(1.0f, -1.0f, 1.0f); // Top Right Of The Quad (Bottom)
		glVertex3f(-1.0f, -1.0f, 1.0f); // Top Left Of The Quad (Bottom)
		glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom Left Of The Quad (Bottom)
		glVertex3f(1.0f, -1.0f, -1.0f); // Bottom Right Of The Quad (Bottom)
		glColor3f(0, 1, 0);
		glVertex3f(1.0f, 1.0f, 1.0f); // Top Right Of The Quad (Front)
		glVertex3f(-1.0f, 1.0f, 1.0f); // Top Left Of The Quad (Front)
		glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom Left Of The Quad (Front)
		glVertex3f(1.0f, -1.0f, 1.0f); // Bottom Right Of The Quad (Front)
		glColor3f(1, 0, 0);
		glVertex3f(1.0f, -1.0f, -1.0f); // Top Right Of The Quad (Back)
		glVertex3f(-1.0f, -1.0f, -1.0f); // Top Left Of The Quad (Back)
		glVertex3f(-1.0f, 1.0f, -1.0f); // Bottom Left Of The Quad (Back)
		glVertex3f(1.0f, 1.0f, -1.0f); // Bottom Right Of The Quad (Back)
		glColor3f(0, 0, 1);
		glVertex3f(-1.0f, 1.0f, 1.0f); // Top Right Of The Quad (Left)
		glVertex3f(-1.0f, 1.0f, -1.0f); // Top Left Of The Quad (Left)
		glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom Left Of The Quad (Left)
		glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom Right Of The Quad (Left)
		glColor3f(1, 0, 1);
		glVertex3f(1.0f, 1.0f, -1.0f); // Top Right Of The Quad (Right)
		glVertex3f(1.0f, 1.0f, 1.0f); // Top Left Of The Quad (Right)
		glVertex3f(1.0f, -1.0f, 1.0f); // Bottom Left Of The Quad (Right)
		glVertex3f(1.0f, -1.0f, -1.0f); // Bottom Right Of The Quad (Right)
		glEnd();
	}

	static void drawCube() {
		glBegin(GL_QUADS); // Draw A Quad
		glVertex3f(1.0f, 1.0f, -1.0f); // Top Right Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f, -1.0f); // Top Left Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f, 1.0f); // Bottom Left Of The Quad (Top)
		glVertex3f(1.0f, 1.0f, 1.0f); // Bottom Right Of The Quad (Top)
		glVertex3f(1.0f, -1.0f, 1.0f); // Top Right Of The Quad (Bottom)
		glVertex3f(-1.0f, -1.0f, 1.0f); // Top Left Of The Quad (Bottom)
		glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom Left Of The Quad (Bottom)
		glVertex3f(1.0f, -1.0f, -1.0f); // Bottom Right Of The Quad (Bottom)
		glVertex3f(1.0f, 1.0f, 1.0f); // Top Right Of The Quad (Front)
		glVertex3f(-1.0f, 1.0f, 1.0f); // Top Left Of The Quad (Front)
		glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom Left Of The Quad (Front)
		glVertex3f(1.0f, -1.0f, 1.0f); // Bottom Right Of The Quad (Front)
		glVertex3f(1.0f, -1.0f, -1.0f); // Top Right Of The Quad (Back)
		glVertex3f(-1.0f, -1.0f, -1.0f); // Top Left Of The Quad (Back)
		glVertex3f(-1.0f, 1.0f, -1.0f); // Bottom Left Of The Quad (Back)
		glVertex3f(1.0f, 1.0f, -1.0f); // Bottom Right Of The Quad (Back)
		glVertex3f(-1.0f, 1.0f, 1.0f); // Top Right Of The Quad (Left)
		glVertex3f(-1.0f, 1.0f, -1.0f); // Top Left Of The Quad (Left)
		glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom Left Of The Quad (Left)
		glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom Right Of The Quad (Left)
		glVertex3f(1.0f, 1.0f, -1.0f); // Top Right Of The Quad (Right)
		glVertex3f(1.0f, 1.0f, 1.0f); // Top Left Of The Quad (Right)
		glVertex3f(1.0f, -1.0f, 1.0f); // Bottom Left Of The Quad (Right)
		glVertex3f(1.0f, -1.0f, -1.0f); // Bottom Right Of The Quad (Right)
		glEnd();
	}


};

#endif /* FUNCTIONS_H_ */
