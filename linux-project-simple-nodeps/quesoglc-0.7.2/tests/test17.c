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
/* $Id: test17.c 887 2009-03-24 01:03:11Z bcoconni $ */

/** \file
 * This test uses the instrumented memory management routines of QuesoGLC to
 * check that routines are robust with respect to memory failures when no
 * context has been made current to the issuing thread.
 */

#include "GL/glc.h"
#include <stdio.h>
#include <stdlib.h>

GLCAPI GLuint __glcMemAllocCount;
GLCAPI GLuint __glcMemAllocTrigger;
GLCAPI GLboolean __glcMemAllocFailOnce;

#define TestErrorCode(func) \
  err = glcGetError();		  \
  if (err != GLC_STATE_ERROR) {				       \
    printf(#func"() Unexpected error : 0x%x\n", (int)err);     \
    return -1;						       \
  }							       \
  err = glcGetError();					       \
  if (err) {							     \
    printf(#func"Error is not GLC_NONE but : 0x%x\n", (int)err);     \
    return -1;							     \
  }



static int testedSequence(const char* description)
{
  GLCenum err;
  GLint ctx = 0;
  GLint *list;
  GLfloat tab[4];

  ctx = glcGenContext();
  err = glcGetError();
  if ((err == GLC_RESOURCE_ERROR) && ctx) {
    printf("iteration #%d\t%s GLC_RESOURCE_ERROR in glcGenContext() ctx:%d\n",
	   __glcMemAllocTrigger, description, ctx);
    return -1;
  }
  if (glcIsContext(ctx) && (err == GLC_RESOURCE_ERROR)) {
    printf("iteration #%d\t%s bad result in glcIsContext() ctx:%d\n",
	   __glcMemAllocTrigger, description, ctx);
    return -1;
  }
  err = glcGetError();
  if (err) {
    printf("iteration #%d\t%s unexpected error : 0x%x\t glcIsContext()"
	   " ctx:%d\n", __glcMemAllocTrigger, description, err, ctx);
    return -1;
  }
  if (glcGetCurrentContext()) {
    printf("iteration #%d\t%s bad result in glcCurrentContext() ctx:%d\n",
	   __glcMemAllocTrigger, description, ctx);
    return -1;
  }
  err = glcGetError();
  if (err) {
    printf("iteration #%d\t%s unexpected error : 0x%x\t glcCurrentContext()"
	   " ctx:%d\n", __glcMemAllocTrigger, description, err, ctx);
    return -1;
  }
  list = glcGetAllContexts();
  err = glcGetError();
  if ((err == GLC_RESOURCE_ERROR) && list) {
    printf("iteration #%d\t%s GLC_RESOURCE_ERROR in glcGetAllContexts()"
	   " ctx:%d\n", __glcMemAllocTrigger, description, ctx);
    return -1;
  }
  free(list);

  glcDeleteContext(ctx);
  err = glcGetError();
  if (ctx && err) {
    printf("iteration #%d\t%s unexpected error : 0x%x\t glcDeleteContext()"
	   " ctx:%d\n", __glcMemAllocTrigger, description, err, ctx);
    return -1;
  }

  /* Context commands */

  glcCallbackFunc(GLC_OP_glcUnmappedCode, NULL);
  TestErrorCode(glcCallbackFunc);

  glcDataPointer(NULL);
  TestErrorCode(glcDataPointer);

  glcDeleteGLObjects();
  TestErrorCode(glcDeleteGLObjects);

  glcDisable(GLC_MIPMAP);
  TestErrorCode(glcDisable);

  glcEnable(GLC_GL_OBJECTS);
  TestErrorCode(glcEnable);

  glcGetCallbackFunc(GLC_OP_glcUnmappedCode);
  TestErrorCode(glcGetCallbackFunc);

  glcGetListc(GLC_CATALOG_LIST, 0);
  TestErrorCode(glcGetListc);

  glcGetListi(GLC_FONT_LIST, 0);
  TestErrorCode(glcGetListi);

  glcGetPointer(GLC_DATA_POINTER);
  TestErrorCode(glcGetPointer);

  glcGetc(GLC_RELEASE);
  TestErrorCode(glcGetc);

  glcGetf(GLC_RESOLUTION);
  TestErrorCode(glcGetf);

  glcGetfv(GLC_BITMAP_MATRIX, tab);
  TestErrorCode(glcGetfv);

  glcGeti(GLC_VERSION_MAJOR);
  TestErrorCode(glcGeti);

  glcIsEnabled(GLC_AUTO_FONT);
  TestErrorCode(glcIsEnabled);

  glcStringType(GLC_UCS1);
  TestErrorCode(glcStringType);

  /* Master commands */

  glcAppendCatalog("/");
  TestErrorCode(glcAppendCatalog);

  glcGetMasterListc(0, GLC_CHAR_LIST, 0);
  TestErrorCode(glcGetMasterListc);

  glcGetMasterMap(0,0);
  TestErrorCode(glcGetMasterMap);

  glcGetMasterc(0, GLC_FAMILY);
  TestErrorCode(glcGetMasterc);

  glcGetMasteri(0, GLC_CHAR_COUNT);
  TestErrorCode(glcGetMasteri);

  glcPrependCatalog("/");
  TestErrorCode(glcPrependCatalog);

  glcRemoveCatalog(0);
  TestErrorCode(glcRemoveCatalog);

  /* Font commands */

  glcAppendFont(0);
  TestErrorCode(glcAppendFont);

  glcDeleteFont(0);
  TestErrorCode(glcDeleteFont);

  glcFont(0);
  TestErrorCode(glcFont);

  glcFontFace(0, "");
  TestErrorCode(glcFontFace);

  glcFontMap(0, 0, "");
  TestErrorCode(glcFontMap);

  glcGenFontID();
  TestErrorCode(glcGenFontID);

  glcGetFontFace(0);
  TestErrorCode(glcGetFontFace);

  glcGetFontListc(0, GLC_CHAR_LIST, 0);
  TestErrorCode(glcGetFontListc);

  glcGetFontMap(0, 0);
  TestErrorCode(glcGetFontMap);

  glcGetFontc(0, GLC_FAMILY);
  TestErrorCode(glcGetFontc);

  glcGetFonti(0, GLC_IS_FIXED_PITCH);
  TestErrorCode(glcGetFonti);

  glcIsFont(0);
  TestErrorCode(glcIsFont);

  glcNewFontFromFamily(1, "");
  TestErrorCode(glcNewFontFromFamily);

  glcNewFontFromMaster(1, 0);
  TestErrorCode(glcNewFontFromMaster);

  /* Transformation commands */

  glcLoadIdentity();
  TestErrorCode(glcLoadIdentity);

  glcLoadMatrix(tab);
  TestErrorCode(glcLoadMatrix);

  glcMultMatrix(tab);
  TestErrorCode(glcMultMatrix);

  glcRotate(0.);
  TestErrorCode(glcRotate);

  glcScale(1., 1.);
  TestErrorCode(glcScale);

  /* Rendering commands */

  glcRenderChar(0);
  TestErrorCode(glcRenderChar);

  glcRenderCountedString(0, "");
  TestErrorCode(glcRenderCountedString);

  glcRenderString("");
  TestErrorCode(glcRenderString);

  glcRenderStyle(GLC_LINE);
  TestErrorCode(glcRenderStyle);

  glcReplacementCode(0);
  TestErrorCode(glcReplacementCode);

  glcResolution(0.);
  TestErrorCode(glcResolution);

  /* Measurement commands*/

  glcGetCharMetric(0, GLC_BOUNDS, tab);
  TestErrorCode(glcGetCharMetric);

  glcGetMaxCharMetric(GLC_BASELINE, tab);
  TestErrorCode(glcGetMaxCharMetric);

  glcGetStringCharMetric(0, GLC_BOUNDS, tab);
  TestErrorCode(glcGetStringCharMetric);

  glcGetStringMetric(GLC_BOUNDS, tab);
  TestErrorCode(glcGetStringMetric);

  glcMeasureCountedString(GL_TRUE, 1, "");
  TestErrorCode(glcMeasureCountedString);

  glcMeasureString(GL_FALSE, "");
  TestErrorCode(glcMeasureString);

  return 0;
}



int main(void)
{
  GLCenum err;
  int result = 0;

  err = glcGetError();
  if (err) {
    printf("Unexpected error : 0x%x\n", (int)err);
    return -1;
  }

  __glcMemAllocCount = 0;
  __glcMemAllocTrigger = 0;
  __glcMemAllocFailOnce = GL_TRUE;

  result = testedSequence("Initial run");
  if (result)
    return result;
  printf("Number of memory allocation: %d\n", __glcMemAllocCount);

  if (__glcMemAllocCount) {
    GLuint mallocCount = __glcMemAllocCount;
    GLuint i = 0;

    for (i = 0; i < mallocCount; i++) {
      __glcMemAllocCount = 0;
      __glcMemAllocTrigger = i + 1;
      __glcMemAllocFailOnce = GL_TRUE;

      result = testedSequence("Fail once");
      if (result)
	return result;

      __glcMemAllocCount = 0;
      __glcMemAllocFailOnce = GL_FALSE;

      result = testedSequence("Repeated fails");
      if (result)
	return result;
    }
  }

  printf("Tests successful !\n");
  return 0;
}
