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
/* $Id: test7.c 816 2008-08-01 23:54:22Z bcoconni $ */

/** \file
 * Check that glcGetCharMetric() and glcGetStringCharMetric() return consistant
 * values.
 */

#include "GL/glc.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define EPSILON 1E-5
static char* string = "Hello";
#define CheckError() \
  err = glcGetError(); \
  if (err) { \
    printf("Unexpected error : 0x%X\n", err); \
    return -1; \
  }

int main(int argc, char **argv) {
  GLint ctx;
  GLCenum err;
  GLint length = 0;
  GLint i, j, n;
  GLint font = 0;
  GLfloat baseline1[4], baseline2[4];
  GLfloat boundingBox1[8], boundingBox2[8];
  GLfloat v1, v2, norm, area;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("test7");

  ctx = glcGenContext();
  CheckError();

  glcContext(ctx);
  CheckError();

  length = glcMeasureCountedString(GL_TRUE, strlen(string)-1, string);
  CheckError();

  if ((!length) || (length != (strlen(string)-1))) {
    printf("glcMeasureString() failed to measure %d characters"
	   " (%d measured instead)\n", (int)strlen(string), length);
    return -1;
  }

  n = glcGeti(GLC_MEASURED_CHAR_COUNT);
  CheckError();

  if (length != n) {
    printf("glcGeti(GLC_MEASURED_CHAR_COUNT) == %d is not consistent with the"
	   " value returned by glcMeasureString() == %d\n", n, length);
    return -1;
  }

  length = glcMeasureString(GL_TRUE, string);
  CheckError();

  if ((!length) || (length != strlen(string))) {
    printf("glcMeasureString() failed to measure %d characters"
	   " (%d measured instead)\n", (int)strlen(string), length);
    return -1;
  }

  n = glcGeti(GLC_MEASURED_CHAR_COUNT);
  CheckError();

  if (length != n) {
    printf("glcGeti(GLC_MEASURED_CHAR_COUNT) == %d is not consistent with the"
	   " value returned by glcMeasureString() == %d\n", n, length);
    return -1;
  }

  for (i = 0; i < strlen(string); i++) {
    if (!glcGetCharMetric(string[i], GLC_BASELINE, baseline1)) {
      printf("Failed to measure the baseline of the character %c\n",
	     string[i]);
      return -1;
    }

    CheckError();

    if (!glcGetStringCharMetric(i, GLC_BASELINE, baseline2)) {
      printf("Failed to get the baseline of the %dth character of the string"
	     " %s\n", i, string);
      return -1;
    }

    CheckError();

    for (j = 0; j < 2; j++) {
      v1 = fabs(baseline1[j + 2] - baseline1[j]);
      v2 = fabs(baseline2[j + 2] - baseline2[j]);
      norm = v1 > v2 ? v1 : v2;

      if (fabs(v1 - v2) > EPSILON * norm) {
	printf("Baseline values differ [rank %d char %d] %f (char),"
	       " %f (string)\n", j, i, v1, v2);
	return -1;
      }
    }

    if (!glcGetCharMetric(string[i], GLC_BOUNDS, boundingBox1)) {
      printf("Failed to measure the bounding box of the character %c\n",
	     string[i]);
      return -1;
    }

    CheckError();

    if (!glcGetStringCharMetric(i, GLC_BOUNDS, boundingBox2)) {
      printf("Failed to get the bounding box of the %dth character of the"
	     " string %s\n", i, string);
      return -1;
    }

    CheckError();

    for (j = 0; j < 6; j++) {
      v1 = fabs(boundingBox1[j + 2] - boundingBox1[j]);
      v2 = fabs(boundingBox2[j + 2] - boundingBox2[j]);
      norm = v1 > v2 ? v1 : v2;

      if (fabs(v1 - v2) > EPSILON * norm) {
	printf("Bounding Box values differ [rank %d char %d] %f (char),"
	       " %f (string)", j, i, v1, v2);
	return -1;
      }
    }
  }

  if (!glcGeti(GLC_FONT_COUNT)) {
    printf("GLC_FONT_LIST is empty\n");
    return -1;
  }

  CheckError();

  if (!glcGeti(GLC_CURRENT_FONT_COUNT)) {
    printf("GLC_CURRENT_FONT_LIST is empty\n");
    return -1;
  }

  CheckError();

  if (!glcGetMaxCharMetric(GLC_BASELINE, baseline1)) {
    printf("Failed to get the max baseline of the current fonts\n");
    return -1;
  }

  CheckError();

  v1 = fabs(baseline1[1]);
  v2 = fabs(baseline1[3]);
  norm = v1 > v2 ? v1 : v2;

  if (fabs(v1 - v2) < EPSILON * norm) {
    v1 = fabs(baseline1[0]);
    v2 = fabs(baseline1[2]);
    norm = v1 > v2 ? v1 : v2;
    if ((fabs(v1 - v2) < EPSILON * norm) || (baseline1[2] < baseline1[0])) {
      printf("Right and left side of the max baseline are swapped\n");
      printf("%f %f %f %f\n", baseline1[0], baseline1[1], baseline1[2],
	     baseline1[3]);
      font = glcGetListi(GLC_FONT_LIST, 0);
      printf("Family : %s\n", (char*)glcGetFontc(font, GLC_FAMILY));
      printf("Face : %s\n", (char*)glcGetFontFace(font));
      return -1;
    }
  }

  if (!glcGetMaxCharMetric(GLC_BOUNDS, boundingBox1)) {
    printf("Failed to get the max bounding box of the current fonts\n");
    return -1;
  }

  CheckError();

  area = 0.;
  for (i = 0; i < 3; i++) {
    area += boundingBox1[2*i] * boundingBox1[2*(i+1)+1] 
      - boundingBox1[2*(i+1)] * boundingBox1[2*i+1];
  }

  if (fabs(area * .5) < EPSILON) {
    printf("Max area of the characters is null\n");
    return -1;
  }

  /* Regression test for bug #1821219 (glcGetCharMetric randomly crashes when
   * requesting measurement for the space character).
   */

  for (i = 0; i < glcGeti(GLC_MASTER_COUNT); i++) {
    GLint font = glcGenFontID();

    if (!glcNewFontFromMaster(font, i)) {
      printf("Can not get a font from master %s\n",
             (char*)glcGetMasterc(i, GLC_FAMILY));
      return -1;
    }

    CheckError();

    glcFont(font);

    CheckError();

    if (!glcGetFontMap(font, ' ')) {
      printf("INFO : Family %s %s has no space character\n",
	     (char*)glcGetFontc(font, GLC_FAMILY), (char*)glcGetFontFace(font));
      continue;
    }

    if (!glcGetCharMetric(' ', GLC_BOUNDS, boundingBox1)) {
      printf("Failed to get the bounding box of the space character\n");
      printf("Family : %s\n", (char*)glcGetFontc(font, GLC_FAMILY));
      printf("Face : %s\n", (char*)glcGetFontFace(font));
      return -1;
    }

    CheckError();

    if (!glcGetCharMetric(' ', GLC_BASELINE, baseline1)) {
      printf("Failed to get the baseline of the space character\n");
      printf("Family : %s\n", (char*)glcGetFontc(font, GLC_FAMILY));
      printf("Face : %s\n", (char*)glcGetFontFace(font));
      return -1;
    }

    CheckError();

    v1 = fabs(baseline1[1]);
    v2 = fabs(baseline1[3]);
    norm = v1 > v2 ? v1 : v2;

    if (fabs(v1 - v2) < EPSILON * norm) {
      v1 = fabs(baseline1[0]);
      v2 = fabs(baseline1[2]);
      norm = v1 > v2 ? v1 : v2;
      if ((fabs(v1 - v2) < EPSILON * norm) || (baseline1[2] < baseline1[0])) {
        printf("Right and left side of the baseline are swapped\n");
        printf("%f %f %f %f\n", baseline1[0], baseline1[1], baseline1[2],
               baseline1[3]);
	printf("Family : %s\n", (char*)glcGetFontc(font, GLC_FAMILY));
	printf("Face : %s\n", (char*)glcGetFontFace(font));
        return -1;
      }
    }
  }

  glcDeleteContext(ctx);
  glcContext(0);

  printf("Tests successful\n");
  return 0;
}
