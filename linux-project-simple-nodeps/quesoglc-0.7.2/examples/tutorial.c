/* QuesoGLC
 * A free implementation of the OpenGL Character Renderer (GLC)
 * Copyright (c) 2002-2005, Bertrand Coconnier
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
/* $Id: tutorial.c 458 2006-09-01 22:24:14Z bcoconni $ */

#include "GL/glc.h"
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>

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

  /* Render "Hello world!" */
  glColor3f(1.f, 0.f, 0.f);
  glRasterPos2f(50.f, 50.f);
  glcRenderString("Hello world!");

  glFlush();
}

int main(int argc, char **argv)
{
  GLint ctx = 0;
  GLint myFont = 0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(640, 180);
  glutCreateWindow("Tutorial");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  /* Set up and initialize GLC */
  ctx = glcGenContext();
  glcContext(ctx);
  glcAppendCatalog("/usr/lib/X11/fonts/Type1");

  /* Create a font "Palatino Bold" */
  myFont = glcGenFontID();
#ifdef __WIN32__
  glcNewFontFromFamily(myFont, "Palatino Linotype");
#else
  glcNewFontFromFamily(myFont, "Palatino");
#endif
  glcFontFace(myFont, "Bold");
  glcFont(myFont);

  /* Render the text at a size of 100 points */
  glcScale(100.f, 100.f);

  glutMainLoop();
  return 0;
}
