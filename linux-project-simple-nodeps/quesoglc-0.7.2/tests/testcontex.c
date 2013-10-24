/* QuesoGLC
 * A free implementation of the OpenGL Character Renderer (GLC)
 * Copyright (c) 2002, 2004-2007, Bertrand Coconnier
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
/* $Id: testcontex.c 679 2007-12-20 21:42:13Z bcoconni $ */

/** \file
 * Test the GLC functions of the context group and the default state of the
 * contexts.
 */

#include "GL/glc.h"
#include <stdio.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int main(int argc, char **argv)
{
  int ctx = 0;
  GLCenum error = GLC_NONE;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("testcontex");

  error = glcGetError();
  if (error) {
    printf("Unexpected error #0x%X\n", error);
    return -1;
  }
  glcDisable(GLC_AUTO_FONT);
  error = glcGetError();
  if (error != GLC_STATE_ERROR) {
    printf("GLC_STATE_ERROR expected. Error #0x%X instead\n", error);
    return -1;
  }
  error = glcGetError();
  if (error) {
    printf("Error state should have been reset.\nError #0x%X instead\n",
	   error);
    return -1;
  }

  ctx = glcGenContext();
  glcContext(ctx);
  error = glcGetError();
  if (error) {
    printf("Unexpected error #0x%X\n", error);
    return -1;
  }

  if (!glcIsEnabled(GLC_AUTO_FONT)) {
    printf("GLC_AUTO_FONT should be enabled by default\n");
    return -1;
  }
  if (!glcIsEnabled(GLC_GL_OBJECTS)) {
    printf("GLC_GL_OBJECTS should be enabled by default\n");
    return -1;
  }
  if (!glcIsEnabled(GLC_MIPMAP)) {
    printf("GLC_MIPMAP should be enabled by default\n");
    return -1;
  }

  glcDisable(GLC_AUTO_FONT);
  if (glcIsEnabled(GLC_AUTO_FONT)) {
    printf("GLC_AUTO_FONT should be disabled now\n");
    return -1;
  }
  if (!glcIsEnabled(GLC_GL_OBJECTS)) {
    printf("GLC_GL_OBJECTS should be enabled\n");
    return -1;
  }
  if (!glcIsEnabled(GLC_MIPMAP)) {
    printf("GLC_MIPMAP should be enabled\n");
    return -1;
  }

  glcDisable(GLC_GL_OBJECTS);
  if (glcIsEnabled(GLC_AUTO_FONT)) {
    printf("GLC_AUTO_FONT should be disabled now\n");
    return -1;
  }
  if (glcIsEnabled(GLC_GL_OBJECTS)) {
    printf("GLC_GL_OBJECTS should be disbled\n");
    return -1;
  }
  if (!glcIsEnabled(GLC_MIPMAP)) {
    printf("GLC_MIPMAP should be enabled\n");
    return -1;
  }

  error = glcGetError();
  if (error) {
    printf("Unexpected error #0x%X\n", error);
    return -1;
  }
  glcDisable(0);
  error = glcGetError();
  if (error != GLC_PARAMETER_ERROR) {
    printf("GLC_PARAMETER_ERROR expected. Error #0x%X instead\n", error);
    return -1;
  }

  printf("Tests successful!\n");

  return 0;
}
