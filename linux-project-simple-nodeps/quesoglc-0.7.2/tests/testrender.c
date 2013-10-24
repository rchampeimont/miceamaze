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
/* $Id: testrender.c 713 2008-01-26 20:10:13Z bcoconni $ */

/* Example of how to render text using the rendering styles defined in the
 * GLC specs.
 */

#include "GL/glc.h"
#include <stdio.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>

#ifdef __WIN32__
/* Font face "Arial Regular" for Windows in order to make a regression test for
 * bug #1856336.
 */
#define FAMILY "Arial"
#define FACE "Regular"
#else
#define FAMILY "Courier"
#define FACE "Italic"
#endif

void display(void)
{
  GLfloat BitmapBoundingBox[8];

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glcRenderStyle(GLC_LINE);
  glColor3f(0., 1., 0.);
  glTranslatef(100., 50., 0.);
  glRotatef(45., 0., 0., 1.);
  glScalef(90., 90., 0.);
  glcRenderChar('L');
  glcRenderString("ine");

  glLoadIdentity();
  glcRenderStyle(GLC_TEXTURE);
  glColor3f(0., 0., 1.);
  glTranslatef(30., 150., 0.);
  glScalef(90., 90., 0.);
  glcRenderChar('T');
  glcRenderString("exture");

  glLoadIdentity();
  glcRenderStyle(GLC_BITMAP);
  glColor3f(1., 0., 0.);
  glRasterPos2f(30., 200.);
  glcLoadIdentity();
  glcScale(90., 90.);
  glcRotate(-10.);
  glcRenderChar('B');
  glcRenderString("itmap");

  glcLoadIdentity();
  glcMeasureString(GL_FALSE, "Bitmap");
  glcGetStringMetric(GLC_BOUNDS, BitmapBoundingBox);
  glTranslatef(30., 200., 0.);
  glScalef(90., 90., 1.);
  glRotatef(-10., 0., 0., 1.);
  glBegin(GL_LINE_LOOP);
    glVertex2fv(&(BitmapBoundingBox[0]));
    glVertex2fv(&(BitmapBoundingBox[2]));
    glVertex2fv(&(BitmapBoundingBox[4]));
    glVertex2fv(&(BitmapBoundingBox[6]));
  glEnd();

  glLoadIdentity();
  glcRenderStyle(GLC_TRIANGLE);
  glColor3f(1., 1., 0.);
  glTranslatef(30., 50., 0.);
  glScalef(90., 90., 0.);
  glcRenderString("Tr");
  glcRenderStyle(GLC_LINE);
  glcRenderChar('i');
  glcRenderStyle(GLC_TRIANGLE);
  glcRenderString("angle");

  glFlush();
}

void reshape(int width, int height)
{
  glClearColor(0., 0., 0., 0.);
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-0.325, width - 0.325, -0.325, height - 0.325);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
  GLint i = 0;

  switch(key) {
  case 27: /* Escape Key */
    printf("Textures : %d\n", glcGeti(GLC_TEXTURE_OBJECT_COUNT));
    for (i = 0; i < glcGeti(GLC_TEXTURE_OBJECT_COUNT); i++)
      printf("Texture #%d : %d\n", i, glcGetListi(GLC_TEXTURE_OBJECT_LIST, i));
    printf("Display Lists : %d\n", glcGeti(GLC_LIST_OBJECT_COUNT));
    for (i = 0; i < glcGeti(GLC_LIST_OBJECT_COUNT); i++)
      printf("Display List #%d : %d\n", i,
	     glcGetListi(GLC_LIST_OBJECT_LIST, i));
    printf("Buffer Objects : %d\n", glcGeti(GLC_BUFFER_OBJECT_COUNT_QSO));
    for (i = 0; i < glcGeti(GLC_BUFFER_OBJECT_COUNT_QSO); i++)
      printf("Buffer Object #%d : %d\n", i,
	     glcGetListi(GLC_BUFFER_OBJECT_LIST_QSO, i));
    glcDeleteGLObjects();
    glcDeleteContext(glcGetCurrentContext());
    glcContext(0);
    exit(0);
  default:
    break;
  }
}

int main(int argc, char **argv)
{
  int ctx = 0;
  int font = 0;
  GLCenum error = 0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(640, 480);
  glutCreateWindow("QuesoGLC");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  glEnable(GL_TEXTURE_2D);

  ctx = glcGenContext();
  glcContext(ctx);
  font = glcNewFontFromFamily(glcGenFontID(), FAMILY);
  glcFont(font);
  error = glcGetError();
  glcFontFace(font, FACE);

#ifdef __WIN32__
  /* Regression test for bug #1856336 */
  error = glcGetError();
  if (error == GLC_RESOURCE_ERROR) {
    printf("Unable to select the font %s %s\n", FAMILY, FACE);
    return -1;
  }
#endif

  glutMainLoop();
  glcDeleteContext(ctx);
  return 0;
}
