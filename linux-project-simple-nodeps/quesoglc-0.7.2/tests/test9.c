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
/* $Id: test9.c 712 2008-01-26 19:46:49Z bcoconni $ */

/** \file
 * Regression test for bug #1754660 (reported by Alok Kulkarni) :
 * Since QuesoGLC version 0.6.0, a call to glcRenderString draws only the
 * first character of the given string when the call is executed as part of a
 * user-defined OpenGL display list.
 *
 * For example, the pseudo-code:
 *
 * {
 *  ...
 *  glcRenderStyle (GLC_TRIANGLE);
 *  ...
 *  glNewList (id, GL_COMPILE);
 *  ...
 *  glcRenderString ("ABCDE");
 *  ...
 *  glEndList ();
 *  ...
 *  glCallList (id);
 *  ...
 * }
 *
 * will render only the first character "A".
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render "Hello world!" */
  glColor3f(1.f, 0.f, 0.f);
#ifdef WITH_USER_LIST
  glCallList(id);
#else
  glLoadIdentity();
  glScalef(100.f, 100.f, 1.f);
  glTranslatef(.5f, .5f, 0.f);
  glcRenderString("Hello world!");
#endif

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

  glEnable(GL_TEXTURE_2D);

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
  glcRenderStyle(RENDER_STYLE);
#if (RENDER_STYLE == GLC_TEXTURE)
  printf("Render style : GLC_TEXTURE");
#elif (RENDER_STYLE == GLC_TRIANGLE)
  printf("Render style : GLC_TRIANGLE");
#endif

#ifdef WITH_GL_OBJECTS
  printf("\t with GL objects");
#else
  glcDisable(GLC_GL_OBJECTS);
  printf("\t without GL objects");
#endif

#ifdef WITH_USER_LIST
  printf("\twith user list\n");
#else
  printf("\twithout user list\n");
#endif

  /* Render the text at a size of 100 points */
  id = glGenLists(1);
  glNewList(id, GL_COMPILE);
  glLoadIdentity();
  glScalef(100.f, 100.f, 1.f);
  glTranslatef(.5f, .5f,0.f);
  glcRenderString("Hello world!");
  glEndList();

  glutMainLoop();
  return 0;
}
