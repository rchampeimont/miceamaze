/* QuesoGLC
 * A free implementation of the OpenGL Character Renderer (GLC)
 * Copyright (c) 2002, 2004-2006, Bertrand Coconnier
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
/* $Id: test13.c 828 2008-08-18 19:51:08Z bcoconni $ */

/** \file
 * Checks the functions which manage the font lists GLC_FONT_LIST and
 * GLC_CURRENT_FONT_LIST.
 */

#include "GL/glc.h"
#include <stdio.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLboolean checkError(GLCenum expectedError)
{
  GLCenum err = glcGetError();

  if (err == expectedError)
    return GL_TRUE;

  switch(err) {
  case GLC_NONE:
    printf("Unexpected GLC_NONE error\n");
    return GL_FALSE;
  case GLC_STATE_ERROR:
    printf("Unexpected GLC_STATE_ERROR\n");
    return GL_FALSE;
  case GLC_PARAMETER_ERROR:
    printf("Unexpected GLC_PARAMETER_ERROR\n");
    return GL_FALSE;
  case GLC_RESOURCE_ERROR:
    printf("Unexpected GLC_RESOURCE_ERROR\n");
    return GL_FALSE;
  case GLC_STACK_OVERFLOW_QSO:
    printf("Unexpected GLC_STACK_OVERFLOW_QSO\n");
    return GL_FALSE;
  case GLC_STACK_UNDERFLOW_QSO:
    printf("Unexpected GLC_STACK_UNDERFLOW_QSO\n");
    return GL_FALSE;
  default:
    printf("Unknown error 0x%X\n", err);
    return GL_FALSE;
  }
}

GLboolean checkFontLists(GLint fonts, GLint currentFonts)
{
  if (glcGeti(GLC_FONT_COUNT) != fonts) {
    printf("GLC_FONT_LIST should contain %d fonts\n", fonts);
    return GL_FALSE;
  }
  if (!checkError(GLC_NONE))
    return GL_FALSE;

  if (glcGeti(GLC_CURRENT_FONT_COUNT) != currentFonts) {
    printf("GLC_CURRENT_FONT_LIST should contain %d fonts\n", currentFonts);
    return GL_FALSE;
  }
  if (!checkError(GLC_NONE))
    return GL_FALSE;

  return GL_TRUE;
}

int main(int argc, char **argv)
{
  GLint ctx = glcGenContext();
  GLint font[3] = {0, 0, 0};

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("test13");

  glcContext(ctx);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(0, 0))
    return -1;

  font[0] = glcNewFontFromMaster(glcGenFontID(), 1);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(1, 0))
    return -1;

  font[1] = glcNewFontFromMaster(glcGenFontID(), 2);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(2, 0))
    return -1;

  glcFont(font[0]);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(2, 1))
    return -1;

  glcAppendFont(font[1]);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(2, 2))
    return -1;

  glcFont(font[1] + 1000);
  if (!checkError(GLC_PARAMETER_ERROR)) /* The font does not exist */
    return -1;

  glcAppendFont(font[1] + 1000);
  if (!checkError(GLC_PARAMETER_ERROR)) /* The font does not exist */
    return -1;

  glcAppendFont(font[1]);
  if (!checkError(GLC_PARAMETER_ERROR)) /* The font is already current */
    return -1;

  if (!checkFontLists(2, 2))
    return -1;

  glcFont(0);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(2, 0))
    return -1;

  font[2] = glcNewFontFromMaster(glcGenFontID(), 3);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(3, 0))
    return -1;

  glcAppendFont(font[0]);
  if (!checkError(GLC_NONE))
    return -1;

  glcAppendFont(font[1]);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(3, 2))
    return -1;

  glcFont(font[2]);
  if (!checkError(GLC_NONE))
    return -1;

  if (!checkFontLists(3, 1))
    return -1;

  glcDeleteContext(ctx);
  if (!checkError(GLC_NONE))
    return -1;

  printf("Tests successful\n");
  return 0;
}
