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
/* $Id: test5.c 679 2007-12-20 21:42:13Z bcoconni $ */

/** \file
 * Checks the transformation routines
 */

#include "GL/glc.h"
#include <stdio.h>
#include <math.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* Check the transform routines */

int vectorEquality(GLfloat* vec, GLfloat epsilon)
{
  GLfloat outVec[4];
  int i;

  glcGetfv(GLC_BITMAP_MATRIX, outVec);
  for (i = 0; i < 4; i++)
    if (fabs(outVec[i] - vec[i]) > epsilon) return GL_FALSE;

  return GL_TRUE;
}

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
  GLfloat ref[4] = {1., 0., 0., 1.};
  GLint stackDepth = 0;
  GLint maxStackDepth = 0;
  GLint i = 0;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("test5");

  glcContext(ctx);
  if (!checkError(GLC_NONE))
    return -1;

  if (!vectorEquality(ref, .00001)) {
    printf("The initial bitmap matrix is not equal to the identity matrix\n");
    return -1;
  }

  maxStackDepth = glcGeti(GLC_MAX_MATRIX_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (!maxStackDepth) {
    printf("Max stack depth is expected not to be zero\n");
    return -1;
  }

  stackDepth = glcGeti(GLC_MATRIX_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth != 1) {
    printf("Current stack depth is not one\n");
    return -1;
  }

  glcPopMatrixQSO();
  if (!checkError(GLC_STACK_UNDERFLOW_QSO))
    return -1;

  /* Check that the faulty glcPopMatrixQSO left the stack depth unchanged */
  stackDepth = glcGeti(GLC_MATRIX_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth != 1) {
    printf("Current stack depth is not one\n");
    return -1;
  }

  ref[0] = 1.5;
  ref[1] = -2.5;
  ref[2] = 3.14159;
  ref[3] = 0.;
  glcLoadMatrix(ref);
  if (!checkError(GLC_NONE))
    return -1;

  if (!vectorEquality(ref, .00001)) {
    printf("glcLoadMatrix() failed\n");
    return -1;
  }

  for (i = 1; i < maxStackDepth; i++) {
    glcPushMatrixQSO();
    if (!checkError(GLC_NONE))
      return -1;

    stackDepth = glcGeti(GLC_MATRIX_STACK_DEPTH_QSO);
    if (!checkError(GLC_NONE))
      return -1;
    if (stackDepth != (i+1)) {
      printf("Stack depth has not been updated\n");
      return -1;
    }
  }

  glcPushMatrixQSO();
  if (!checkError(GLC_STACK_OVERFLOW_QSO))
    return -1;

  /* Check that the faulty glcPushMatrixQSO left the stack depth unchanged */
  stackDepth = glcGeti(GLC_MATRIX_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth != maxStackDepth) {
    printf("Current stack depth is not maximum\n");
    return -1;
  }

  for (i = 1; i < maxStackDepth; i++) {
    glcPopMatrixQSO();
    if (!checkError(GLC_NONE))
      return -1;

    stackDepth = glcGeti(GLC_MATRIX_STACK_DEPTH_QSO);
    if (!checkError(GLC_NONE))
      return -1;
    if (stackDepth != maxStackDepth-i) {
      printf("Stack depth has not been updated\n");
      return -1;
    }
  }

  glcPopMatrixQSO();
  if (!checkError(GLC_STACK_UNDERFLOW_QSO))
    return -1;

  /* Check that the faulty glcPopMatrixQSO left the stack depth unchanged */
  stackDepth = glcGeti(GLC_MATRIX_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth != 1) {
    printf("Current stack depth is not one\n");
    return -1;
  }

  glcPushMatrixQSO();
  if (!checkError(GLC_NONE))
    return -1;

  stackDepth = glcGeti(GLC_MATRIX_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth != 2) {
    printf("Current stack depth is not 2\n");
    return -1;
  }

  ref[0] = 0.;
  ref[1] = 1.;
  ref[2] = 1.;
  ref[3] = 0.;
  glcMultMatrix(ref);
  if (!checkError(GLC_NONE))
    return -1;

  ref[0] = 3.14159;
  ref[1] = 0.;
  ref[2] = 1.5;
  ref[3] = -2.5;
  if (!vectorEquality(ref, .00001)) {
    printf("glcMultMatrix() failed\n");
    return -1;
  }

  glcScale(-1., 2.);
  if (!checkError(GLC_NONE))
    return -1;
  ref[0] = -3.14159;
  ref[1] = 0.;
  ref[2] = 3.;
  ref[3] = -5.;
  if (!vectorEquality(ref, .00001)) {
    printf("glcScale() failed\n");
    return -1;
  }

  glcRotate(45.);
  if (!checkError(GLC_NONE))
    return -1;
  ref[0] = -.14159/sqrt(2.);
  ref[2] = 6.14159/sqrt(2.);
  ref[1] = -5./sqrt(2.);
  ref[3] = -5./sqrt(2.);
  if (!vectorEquality(ref, .00001)) {
    printf("glcRotate() failed\n");
    return -1;
  }

  glcLoadIdentity();
  if (!checkError(GLC_NONE))
    return -1;
  ref[0] = 1.;
  ref[1] = 0.;
  ref[2] = 0.;
  ref[3] = 1.;
  if (!vectorEquality(ref, .00001)) {
    printf("glcLoadMatrix() failed\n");
    return -1;
  }

  glcPopMatrixQSO();
  if (!checkError(GLC_NONE))
    return -1;
  stackDepth = glcGeti(GLC_MATRIX_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth != 1) {
    printf("Current stack depth is not one\n");
    return -1;
  }
  ref[0] = 1.5;
  ref[1] = -2.5;
  ref[2] = 3.14159;
  ref[3] = 0.;
  if (!vectorEquality(ref, .00001)) {
    printf("glcPush/PopMatrix() failed\n");
    return -1;
  }

  printf("Tests successful\n");
  return 0;
}
