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
/* $Id: test16.c 887 2009-03-24 01:03:11Z bcoconni $ */

/** \file
 * Test for the function glcGetMaxCharMetric()
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
GLint myFont = 0;


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
  GLint c = 0;
  GLfloat bboxMax[8] = {10000.f, 10000.f, -10000.f, 10000.f, -10000.f, -10000.f,
			10000.f, -10000.f};

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glcGetMaxCharMetric(GLC_BOUNDS, bbox);
  glLoadIdentity();
  glScalef(120.f, 120.f, 1.f);
  glTranslatef(0.5f, 1.f, 0.f);
  glColor3f(0.f, 1.f, 1.f);
  glBegin(GL_LINE_LOOP);
  for (i = 0; i < 4; i++)
    glVertex2fv(&bbox[2*i]);
  glEnd();

  glLoadIdentity();
  glScalef(120.f, 120.f, 1.f);
  glColor3f(1.f, 0.f, 0.f);
  glTranslatef(0.5f, 1.f, 0.f);
  for (c = 32; c < 0x110000; c++) {
    if (!glcGetFontMap(myFont, c))
      continue;
    glPushMatrix();
    glcRenderChar(c);
    glPopMatrix();
    glcGetCharMetric(c, GLC_BOUNDS, bbox);

    if (bbox[0] < bboxMax[0]) {
      bboxMax[0] = bbox[0];
      bboxMax[6] = bbox[6];
    }
    if (bbox[1] < bboxMax[1]) {
      bboxMax[1] = bbox[1];
      bboxMax[3] = bbox[3];
    }
    if (bbox[2] > bboxMax[2]) {
      bboxMax[2] = bbox[2];
      bboxMax[4] = bbox[4];
    }
    if (bbox[5] > bboxMax[5]) {
      bboxMax[5] = bbox[5];
      bboxMax[7] = bbox[7];
    }
  }

  glColor3f(0.f, 1.f, 0.f);
  glBegin(GL_LINE_LOOP);
  for (i = 0; i < 4; i++)
    glVertex2fv(&bboxMax[2*i]);
  glEnd();

  glFlush();
}

int main(int argc, char **argv)
{
  GLint ctx = 0;
  GLint masterCount = 0;
  GLint master = 0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(640, 300);
  glutCreateWindow("Test16");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  glEnable(GL_TEXTURE_2D);

  /* Set up and initialize GLC */
  ctx = glcGenContext();
  glcContext(ctx);

  masterCount = glcGeti(GLC_MASTER_COUNT);
  for (master = 0; master < masterCount; master++) {
    GLint letter = 0;

    for (letter = 'A'; letter < 'F'; letter++) {
      if (!glcGetMasterMap(master, letter))
	break;
    }

    if (letter == 'F')
      break;
  }

  myFont = glcGenFontID();
  glcNewFontFromMaster(myFont, master);
  glcFont(myFont);
  glcStringType(GLC_UCS4);
  glcRenderStyle(GLC_TEXTURE);
  glcDisable(GLC_GL_OBJECTS);
  glcDisable(GLC_AUTO_FONT);

  glutMainLoop();
  return 0;
}
