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
/* $Id: test8.c 575 2007-02-25 13:17:12Z bcoconni $ */

/** \file
 * Checks that GLC routines supports empty strings
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

int main(int argc, char **argv)
{
  GLint ctx = glcGenContext();
  GLint font = 0;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("test8");

  glcContext(ctx);
  if (!checkError(GLC_NONE))
    return -1;

  glcAppendCatalog("");
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  glcPrependCatalog("");
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  font = glcNewFontFromMaster(glcGenFontID(), 1);
  if (!checkError(GLC_NONE))
    return -1;

  glcFontFace(font, "");
  if (!checkError(GLC_RESOURCE_ERROR)) /* The face does not exist */
    return -1;

  glcFontMap(font, 32, "");
  if (!checkError(GLC_PARAMETER_ERROR)) /* There is no empty character name */
    return -1;

  glcNewFontFromFamily(glcGenFontID(), "");
  if (!checkError(GLC_RESOURCE_ERROR)) /* The family does not exist */
    return -1;

  glcRenderCountedString(1, "");
  if (!checkError(GLC_NONE))
    return -1;

  glcRenderString("");
  if (!checkError(GLC_NONE))
    return -1;

  glcMeasureCountedString(GL_FALSE, 1, "");
  if (!checkError(GLC_NONE))
    return -1;

  glcMeasureString(GL_FALSE, "");
  if (!checkError(GLC_NONE))
    return -1;

  glcDeleteContext(ctx);
  if (!checkError(GLC_NONE))
    return -1;

  printf("Tests successful\n");
  return 0;
}
