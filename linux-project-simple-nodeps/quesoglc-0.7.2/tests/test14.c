/* QuesoGLC
 * A free implementation of the OpenGL Character Renderer (GLC)
 * Copyright (c) 2002, 2004-2008, Bertrand Coconnier
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/* $Id: test14.c 887 2009-03-24 01:03:11Z bcoconni $ */

/** \file
 * Regression test for a bug which prevents letters l (LATIN SMALL LETTER L) and
 * i (LATIN SMALL LETTER I) to be displayed at small scales when GLC_HINTING_QSO
 * is enabled, GLC_GL_OBJECTS is disabled and the rendering style is
 * GLC_TEXTURE. Only "Sans Serif" fonts exhibit this behaviour.
 */

#include "GL/glc.h"
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>



void reshape(int width, int height)
{
  glClearColor(0., 0., 0., 0.);
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0., width, 0., height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
  switch(key) {
  case 27: /* Escape Key */
    exit(0);
  default:
    break;
  }
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.f, 0.f, 0.f);
  glLoadIdentity();
  glScalef(20.f, 20.f, 1.f);
  glTranslatef(2.f, 2.f, 0.f);
  glcDisable(GLC_HINTING_QSO);
  glcRenderString("lili without hinting (rendered at integer coordinates)");

  glLoadIdentity();
  glScalef(20.f, 20.f, 1.f);
  glTranslatef(2.025f, 3.f, 0.f);
  glcDisable(GLC_HINTING_QSO);
  glcRenderString("lili without hinting (rendered at non-integer coordinates)");

  glLoadIdentity();
  glScalef(20.f, 20.f, 1.f);
  glTranslatef(2.f, 4.f, 0.f);
  glcEnable(GLC_HINTING_QSO);
  glcRenderString("lili with hinting (rendered at integer coordinates)");

  glLoadIdentity();
  glScalef(20.f, 20.f, 1.f);
  glTranslatef(2.025f, 5.f, 0.f);
  glcEnable(GLC_HINTING_QSO);
  glcRenderString("lili with hinting (rendered at non-integer coordinates)");

  glFlush();
}

int main(int argc, char **argv)
{
  GLint ctx = 0;
  GLint myFont = 0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(640, 180);
  glutCreateWindow("Test14");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  glEnable(GL_TEXTURE_2D);

  /* Set up and initialize GLC */
  ctx = glcGenContext();
  glcContext(ctx);

  /* Create a font "Palatino Bold" */
  myFont = glcGenFontID();
#ifdef __WIN32__
  glcNewFontFromFamily(myFont, "Arial");
#else
  glcNewFontFromFamily(myFont, "DejaVu Sans");
#endif
  glcFont(myFont);
  glcRenderStyle(GLC_TEXTURE);
  glcDisable(GLC_GL_OBJECTS);

  glutMainLoop();
  return 0;
}
