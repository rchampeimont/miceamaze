/* QuesoGLC
 * A free implementation of the OpenGL Character Renderer (GLC)
 * Copyright (c) 2002, 2004-2009, Bertrand Coconnier
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
/* $Id: test15.c 887 2009-03-24 01:03:11Z bcoconni $ */

/** \file
 * Regression test for a bug which did not include trailing spaces in
 * calculation of the bounding boxes.
 */

#include "GL/glc.h"
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>

GLuint id = 0;


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
  GLfloat bbox[8];
  int i = 0;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glScalef(100.f, 100.f, 1.f);
  glTranslatef(0.5f, 0.5f, 0.f);
  glColor4f(1.f, 0.f, 0.f, 1.f);
  glcRenderCountedString(4, "ABCDE");
  glLoadIdentity();
  glScalef(100.f, 100.f, 1.f);
  glTranslatef(0.5f, 1.5f, 0.f);

  glcMeasureString(GL_FALSE, " ABCDE ");
  glcGetStringMetric(GLC_BOUNDS, bbox);
  glColor3f(0.f, 1.f, 1.f);
  glBegin(GL_LINE_LOOP);
  for (i = 0; i < 4; i++)
    glVertex2fv(&bbox[2*i]);
  glEnd();

  glcGetStringMetric(GLC_BASELINE, bbox);
  glColor3f(1.f, 1.f, 0.f);
  glBegin(GL_LINE);
  for (i = 0; i < 2; i++)
    glVertex2fv(&bbox[2*i]);
  glEnd();

  glColor3f(1.f, 0.f, 0.f);
  glcRenderString(" ABCDE ");

  glFlush();
}

int main(int argc, char **argv)
{
  GLint ctx = 0;
  GLint myFont = 0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(640, 300);
  glutCreateWindow("Test15");
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
  glcNewFontFromFamily(myFont, "Palatino Linotype");
#else
  glcNewFontFromFamily(myFont, "Palatino");
#endif
  glcFontFace(myFont, "Bold");
  glcFont(myFont);
  glcRenderStyle(GLC_TEXTURE);

  glutMainLoop();
  return 0;
}
