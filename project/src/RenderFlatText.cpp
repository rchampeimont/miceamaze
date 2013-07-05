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

#include "RenderFlatText.h"
#include "Program.h"

//#undef FONT_GLC

#ifdef FONT_WIN
GLuint RenderFlatText::base = 0;
GLYPHMETRICSFLOAT RenderFlatText::gmf[256];
HDC RenderFlatText::hDC;
HFONT RenderFlatText::font;
#else
#ifdef FONT_GLC
GLint RenderFlatText::ctx;
GLint RenderFlatText::defaultFont;
#endif
#endif

void RenderFlatText::init() {
#ifdef FONT_WIN
	hDC = GetDC(NULL);
	if (AddFontResourceExA(
			(Program::getInstance()->dataPath + "/fonts/DejaVuSans.ttf").c_str(),
			FR_PRIVATE,
			NULL) == 0) {
		Functions::error("error loading font (AddFontResourceExA)");
	}
	font = CreateFont(  0,								// Height Of Font
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
						"DejaVu Sans");						// Font Name

	if (font == NULL) {
		Functions::error("error loading font (CreateFont)");
	}

	Functions::verify(SelectObject(hDC, font) != NULL, "error loading font (SelectObject)"); // Selects The Font We Created

	reinit();
#else
#ifdef FONT_GLC
	ctx = glcGenContext();
	glcContext(ctx);

	glcEnable(GLC_GL_OBJECTS);
	//glcDisable(GLC_GL_OBJECTS);
	glcEnable(GLC_MIPMAP);
	glcEnable(GLC_HINTING_QSO);
	glcEnable(GLC_KERNING_QSO);
	glcRenderStyle(GLC_TRIANGLE);

	defaultFont = glcGenFontID();
	glcAppendCatalog((Program::getInstance()->dataPath + "/fonts").c_str());
	Functions::verify(glcNewFontFromFamily(defaultFont, "DejaVu Sans"), "could not load font (glcNewFontFromFamily error)");
	Functions::verify(glcFontFace(defaultFont, "Book") == GL_TRUE, "could not load font (glcFontFace error)");
	glcFont(defaultFont);
#endif
#endif
}

void RenderFlatText::reinit() {
#ifdef FONT_WIN
	base = glGenLists(256);								// Storage For 256 Characters
	{
		int c = glGetError();
		if (c != GL_NO_ERROR) {
			Functions::error(string("GL error = ") + Functions::toString(c));
		}
	}
	if (!wglUseFontOutlines(hDC,						// Select The Current DC
						0,								// Starting Character
						256,							// Number Of Display Lists To Build
						base,							// Starting Display Lists
						0.0f,							// Deviation From The True Outlines
						0.0f,							// Font Thickness In The Z Direction
						WGL_FONT_POLYGONS,				// Use Polygons, Not Lines
						gmf)) {
		Functions::error("error loading font (wglUseFontOutlines)");
	}
#endif
}

// align: -1 = left (original position = left of the text)
// align:  0 = center (original position = middle of the text)
// align:  1 = right (original position = right of the text)
void RenderFlatText::render(string s, int align) {
#ifdef FONT_WIN
	glScalef(1.2*0.75, 1.2, 1);

	if (align == 0 || align == 1) {
		float delta = 0;
		for (unsigned i=0; i<s.length(); i++) {
			int k = s[i];
			if (k < 0) k += 256;
			if (k >= 256) continue;
			delta += gmf[k].gmfCellIncX;
		}
		if (align == 1) {
			glTranslatef(-delta, 0, 0);
		} else if (align == 0) {
			glTranslatef(-delta/2, 0, 0);
		}
	}

	//glPushAttrib(GL_LIST_BIT);
	glListBase(base);
	glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());
	//glPopAttrib();
#else
#ifdef FONT_GLC
	glScalef(0.75, 1, 1);

	if (align == 0 || align == 1) {
		glcMeasureString(GL_FALSE, s.c_str());
		GLfloat box[8];
		glcGetStringMetric(GLC_BOUNDS, box);
		float delta = box[2] - box[0];
		if (align == 1) {
			glTranslatef(-delta, 0, 0);
		} else if (align == 0) {
			glTranslatef(-delta/2, 0, 0);
		}
	}

	glcRenderString(s.c_str());
#endif
#endif
}
