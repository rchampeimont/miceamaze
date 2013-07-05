// Copyright (c) 2012, RAPHAEL CHAMPEIMONT
// All rights reserved.

#include <iostream>
#include "RenderText.h"



using namespace std;

RenderText *RenderText::instance = NULL;


void RenderText::render(string s) {
#ifdef _WIN32
	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base);									// Sets The Base Character to 0
	glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
#endif
}

RenderText::RenderText() {
#ifdef _WIN32
	HDC hDC = GetDC(NULL);

	HFONT	font;										// Windows Font ID

	base = glGenLists(256);								// Storage For 256 Characters

	font = CreateFont(	-12,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_NORMAL,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Arial");				// Font Name

	SelectObject(hDC, font);							// Selects The Font We Created

	wglUseFontOutlines(	hDC,							// Select The Current DC
						0,								// Starting Character
						255,							// Number Of Display Lists To Build
						base,							// Starting Display Lists
						0.0f,							// Deviation From The True Outlines
						0.2f,							// Font Thickness In The Z Direction
						WGL_FONT_POLYGONS,				// Use Polygons, Not Lines
						gmf);							// Address Of Buffer To Recieve Data
#endif
}

RenderText::~RenderText() {
#ifdef _WIN32
  glDeleteLists(base, 256);								// Delete All 256 Characters
#endif
}

