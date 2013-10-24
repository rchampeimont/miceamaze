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
/* $Id: test6.c 803 2008-06-04 22:14:49Z bcoconni $ */

/** \file
 * Checks the Unicode conversion code and that error conditions that generates
 * errors.
 */

#ifdef HAVE_CONFIG_H
#include "qglc_config.h"
#endif
#ifdef HAVE_LIBGLEW
#include <GL/glew.h>
#else
#include "GL/glew.h"
#endif
#include <GL/glc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define QUESOGLC_MAJOR 0
#define QUESOGLC_MINOR 2

GLEWAPI GLEWContext* glewGetContext(void);

static GLCchar* __glcExtensions1 = (GLCchar*) "GLC_QSO_attrib_stack"
  " GLC_QSO_extrude GLC_QSO_hinting GLC_QSO_kerning GLC_QSO_matrix_stack"
  " GLC_QSO_utf8 GLC_SGI_full_name";
static GLCchar* __glcExtensions2 = (GLCchar*) "GLC_QSO_attrib_stack"
  " GLC_QSO_buffer_object GLC_QSO_extrude GLC_QSO_hinting GLC_QSO_kerning"
  " GLC_QSO_matrix_stack GLC_QSO_utf8 GLC_SGI_full_name";
static GLCchar* __glcRelease = (GLCchar*) QUESOGLC_VERSION;
static GLCchar* __glcVendor = (GLCchar*) "The QuesoGLC Project";

static GLCchar* __glcExtensionsUCS2 = NULL;
static GLCchar* __glcReleaseUCS2 = NULL;
static GLCchar* __glcVendorUCS2 = NULL;
static GLCchar* __glcExtensionsUCS4 = NULL;
static GLCchar* __glcReleaseUCS4 = NULL;
static GLCchar* __glcVendorUCS4 = NULL;

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

GLboolean CALLBACK callback(GLint inCode)
{
  printf("Code 0x%X\n", inCode);
  return GL_FALSE;
}

GLboolean checkIsEnabled(GLboolean autofont, GLboolean glObjects,
			 GLboolean mipmap, GLboolean hinting, GLboolean extrude,
			 GLboolean kerning)
{
  if (!checkError(GLC_NONE))
    return GL_FALSE;

  if ((glcIsEnabled(GLC_AUTO_FONT) != autofont)
      || (glcIsEnabled(GLC_GL_OBJECTS) != glObjects)
      || (glcIsEnabled(GLC_MIPMAP) != mipmap)
      || (glcIsEnabled(GLC_HINTING_QSO) != hinting)
      || (glcIsEnabled(GLC_EXTRUDE_QSO) != extrude)
      || (glcIsEnabled(GLC_KERNING_QSO) != kerning)) {
    printf("GLC_AUTO_FONT %s (expected %s)\n" ,
	   glcIsEnabled(GLC_AUTO_FONT) ? "GL_TRUE" : "GL_FALSE",
	   autofont ? "GL_TRUE" : "GL_FALSE");
    printf("GLC_GL_OBJECTS %s (expected %s)\n" ,
	   glcIsEnabled(GLC_GL_OBJECTS) ? "GL_TRUE" : "GL_FALSE",
	   glObjects ? "GL_TRUE" : "GL_FALSE");
    printf("GLC_MIPMAP %s (expected %s)\n" ,
	   glcIsEnabled(GLC_MIPMAP) ? "GL_TRUE" : "GL_FALSE",
	   mipmap ? "GL_TRUE" : "GL_FALSE");
    printf("GLC_HINTING_QSO %s (expected %s)\n" ,
	   glcIsEnabled(GLC_HINTING_QSO) ? "GL_TRUE" : "GL_FALSE",
	   hinting ? "GL_TRUE" : "GL_FALSE");
    printf("GLC_EXTRUDE_QSO %s (expected %s)\n" ,
	   glcIsEnabled(GLC_EXTRUDE_QSO) ? "GL_TRUE" : "GL_FALSE",
	   extrude ? "GL_TRUE" : "GL_FALSE");
    printf("GLC_KERNING_QSO %s (expected %s)\n" ,
	   glcIsEnabled(GLC_KERNING_QSO) ? "GL_TRUE" : "GL_FALSE",
	   kerning ? "GL_TRUE" : "GL_FALSE");
    return GL_FALSE;
  }

  if (!checkError(GLC_NONE))
    return GL_FALSE;

  return GL_TRUE;
}

GLboolean convertStringUCS2(GLCchar** inString, GLCchar* inStringUCS1)
{
  unsigned char* ucs1 = (unsigned char*)inStringUCS1;
  unsigned short* ucs2 = NULL;

  *inString = (GLCchar*)malloc((strlen(inStringUCS1)+1)*2);
  ucs2 = (unsigned short*)(*inString);

  if (!ucs2) {
    printf("Couldn't allocate memory\n");
    return GL_FALSE;
  }

  do {
    *(ucs2++) = (unsigned short)(*ucs1);
  } while (*(ucs1++));

  return GL_TRUE;
}

GLboolean convertStringUCS4(GLCchar** inString, GLCchar* inStringUCS1)
{
  unsigned char* ucs1 = (unsigned char*)inStringUCS1;
  unsigned int* ucs4 = NULL;

  *inString = (GLCchar*)malloc((strlen(inStringUCS1)+1)*4);
  ucs4 = (unsigned int*)(*inString);

  if (!ucs4) {
    printf("Couldn't allocate memory\n");
    return GL_FALSE;
  }

  do {
    *(ucs4++) = (unsigned short)(*ucs1);
  } while (*(ucs1++));

  return GL_TRUE;
}

int main(int argc, char **argv)
{
  GLint ctx = glcGenContext();
  GLint count = 0;
  GLint maxStackDepth = 0;
  GLint stackDepth = 0;
  GLint i = 0;
  GLCchar* __glcExtensions = NULL;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("test6");

  glcContext(ctx);

  if (glewIsSupported("GL_ARB_pixel_buffer_object")
      || glewIsSupported("GL_ARB_vertex_buffer_object"))
    __glcExtensions = __glcExtensions2;
  else
    __glcExtensions = __glcExtensions1;

  if (!checkError(GLC_NONE))
    return -1;

  if (!convertStringUCS2(&__glcExtensionsUCS2, __glcExtensions))
    return -1;

  if (!convertStringUCS2(&__glcReleaseUCS2, __glcRelease))
    return -1;

  if (!convertStringUCS2(&__glcVendorUCS2, __glcVendor))
    return -1;

  if (!convertStringUCS4(&__glcExtensionsUCS4, __glcExtensions))
    return -1;

  if (!convertStringUCS4(&__glcReleaseUCS4, __glcRelease))
    return -1;

  if (!convertStringUCS4(&__glcVendorUCS4, __glcVendor))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGetCallbackFunc(GLC_OP_glcUnmappedCode)) {
    printf("Unexpected callback function\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcCallbackFunc(GLC_OP_glcUnmappedCode, callback);

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGetCallbackFunc(GLC_OP_glcUnmappedCode) != callback) {
    printf("Got unexpected callback function\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcGetCallbackFunc((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  glcCallbackFunc((GLCenum)0, callback);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGetPointer(GLC_DATA_POINTER)) {
    printf("Unexpected data pointer\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcDataPointer(&ctx);

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGetPointer(GLC_DATA_POINTER) != &ctx) {
    printf("Got unexpected data pointer\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcGetPointer((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  if (!glcIsEnabled(GLC_AUTO_FONT)) {
    printf("GLC_AUTOFONT is disabled\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (!glcIsEnabled(GLC_GL_OBJECTS)) {
    printf("GLC_GL_OBJECTS is disabled\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (!glcIsEnabled(GLC_MIPMAP)) {
    printf("GLC_MIPMAP is disabled\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcIsEnabled(GLC_HINTING_QSO)) {
    printf("GLC_HINTING_QSO is enabled\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcIsEnabled(GLC_EXTRUDE_QSO)) {
    printf("GLC_EXTRUDE_QSO is enabled\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcIsEnabled(GLC_KERNING_QSO)) {
    printf("GLC_KERNING_QSO is enabled\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  maxStackDepth = glcGeti(GLC_MAX_ATTRIB_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;

  if (!maxStackDepth) {
    printf("Max stack depth is expected not to be zero\n");
    return -1;
  }

  stackDepth = glcGeti(GLC_ATTRIB_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth) {
    printf("Current stack depth is not zero\n");
    return -1;
  }

  glcPopAttribQSO();
  if (!checkError(GLC_STACK_UNDERFLOW_QSO))
    return -1;

  /* Check that the faulty glcPopAttribQSO left the stack depth unchanged */
  stackDepth = glcGeti(GLC_ATTRIB_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth) {
    printf("Current stack depth is not zero\n");
    return -1;
  }

  for (i = 0; i < maxStackDepth; i++) {
    glcPushAttribQSO(GLC_ENABLE_BIT_QSO);
    if (!checkError(GLC_NONE))
      return -1;

    stackDepth = glcGeti(GLC_ATTRIB_STACK_DEPTH_QSO);
    if (!checkError(GLC_NONE))
      return -1;
    if (stackDepth != (i+1)) {
      printf("Stack depth has not been updated\n");
      return -1;
    }
  }

  glcPushAttribQSO(GLC_ENABLE_BIT_QSO);
  if (!checkError(GLC_STACK_OVERFLOW_QSO))
    return -1;

  /* Check that the faulty glcPushAttribQSO left the stack depth unchanged */
  stackDepth = glcGeti(GLC_ATTRIB_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth != maxStackDepth) {
    printf("Current stack depth is not maximum\n");
    return -1;
  }

  for (i = 0; i < maxStackDepth; i++) {
    glcPopAttribQSO();
    if (!checkError(GLC_NONE))
      return -1;

    stackDepth = glcGeti(GLC_ATTRIB_STACK_DEPTH_QSO);
    if (!checkError(GLC_NONE))
      return -1;
    if (stackDepth != maxStackDepth-i-1) {
      printf("Stack depth has not been updated\n");
      return -1;
    }
  }

  glcPopAttribQSO();
  if (!checkError(GLC_STACK_UNDERFLOW_QSO))
    return -1;

  /* Check that the faulty glcPopAttribQSO left the stack depth unchanged */
  stackDepth = glcGeti(GLC_ATTRIB_STACK_DEPTH_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (stackDepth) {
    printf("Current stack depth is not zero\n");
    return -1;
  }

  glcIsEnabled((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  glcDisable(GLC_AUTO_FONT);

  if (!checkIsEnabled(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE))
    return -1;

  glcDisable(GLC_GL_OBJECTS);

  if (!checkIsEnabled(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE,
		      GL_FALSE))
    return -1;

  glcDisable(GLC_MIPMAP);

  if (!checkIsEnabled(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE,
		      GL_FALSE))
    return -1;

  glcPushAttribQSO(GLC_ENABLE_BIT_QSO);
  if (!checkError(GLC_NONE))
    return -1;

  glcEnable(GLC_AUTO_FONT);

  if (!checkIsEnabled(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE,
		      GL_FALSE))
    return -1;

  glcEnable(GLC_GL_OBJECTS);

  if (!checkIsEnabled(GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE))
    return -1;

  glcEnable(GLC_MIPMAP);

  if (!checkIsEnabled(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE))
    return -1;

  glcPopAttribQSO();
  if (!checkError(GLC_NONE))
    return -1;
  if (!checkIsEnabled(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE,
		      GL_FALSE))
    return -1;

  glcEnable((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  glcDisable((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_CURRENT_FONT_COUNT)) {
    printf("GLC_CURRENT_FONT_LIST is expected to be empty\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_FONT_COUNT)) {
    printf("GLC_FONT_LIST is expected to be empty\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_LIST_OBJECT_COUNT)) {
    printf("GLC_LIST_OBJECT_LIST is expected to be empty\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_MEASURED_CHAR_COUNT)) {
    printf("Some character have been measured\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_RENDER_STYLE) != GLC_BITMAP) {
    printf("The render style is not GLC_BITMAP\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_REPLACEMENT_CODE)) {
    printf("A GLC_REPLACEMENT_CODE has been defined\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_STRING_TYPE) != GLC_UCS1) {
    printf("The string type is not GLC_UCS1\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_TEXTURE_OBJECT_COUNT)) {
    printf("GLC_TEXTURE_OBJECT_LIST is expected to be empty\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if ((glcGeti(GLC_VERSION_MAJOR) != QUESOGLC_MAJOR)
      || (glcGeti(GLC_VERSION_MINOR) != QUESOGLC_MINOR)) {
    printf("GLC version %d.%d (expected to be %d.%d)\n",
	   glcGeti(GLC_VERSION_MAJOR), glcGeti(GLC_VERSION_MINOR),
	   QUESOGLC_MAJOR, QUESOGLC_MINOR);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcGeti((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  glcEnable(GLC_KERNING_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (!checkIsEnabled(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE,
		      GL_TRUE))
    return -1;
  glcPushAttribQSO(GLC_ENABLE_BIT_QSO|GLC_STRING_BIT_QSO);
  if (!checkError(GLC_NONE))
    return -1;

  glcStringType(GLC_UCS2);
  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_STRING_TYPE) != GLC_UCS2) {
    printf("String type was expected to be GLC_UCS2\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcEnable(GLC_HINTING_QSO);
  if (!checkError(GLC_NONE))
    return -1;
  if (!checkIsEnabled(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE))
    return -1;
  glcPushAttribQSO(GLC_ENABLE_BIT_QSO|GLC_STRING_BIT_QSO);
  if (!checkError(GLC_NONE))
    return -1;

  glcEnable(GLC_AUTO_FONT);
  glcRenderStyle(GLC_TRIANGLE);
  if (!checkError(GLC_NONE))
    return -1;
  if (!checkIsEnabled(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE))
    return -1;
  if (glcGeti(GLC_ATTRIB_STACK_DEPTH_QSO) != 2) {
    printf("Attrib stack depth is expected to be 2\n");
    return -1;
  }

  glcStringType(GLC_UCS4);
  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_STRING_TYPE) != GLC_UCS4) {
    printf("String type was expected to be GLC_UCS4\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcPopAttribQSO();
  if (!checkError(GLC_NONE))
    return -1;
  if (!checkIsEnabled(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE))
    return -1;
  if (glcGeti(GLC_STRING_TYPE) != GLC_UCS2) {
    printf("String type was expected to be GLC_UCS2\n");
    return -1;
  }
  if (glcGeti(GLC_RENDER_STYLE) != GLC_TRIANGLE) {
    printf("The render style is not GLC_TRIANGLE\n");
    return -1;
  }

  glcPopAttribQSO();
  if (!checkError(GLC_NONE))
    return -1;
  if (!checkIsEnabled(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE,
		      GL_TRUE))
    return -1;
  if (glcGeti(GLC_STRING_TYPE) != GLC_UCS1) {
    printf("String type was expected to be GLC_UCS1\n");
    return -1;
  }
  if (glcGeti(GLC_RENDER_STYLE) != GLC_TRIANGLE) {
    printf("The render style is not GLC_TRIANGLE\n");
    return -1;
  }

  glcStringType(GLC_UTF8_QSO);
  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_STRING_TYPE) != GLC_UTF8_QSO) {
    printf("String type was expected to be GLC_UTF8_QSO\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcStringType((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  if (strcmp(glcGetc(GLC_EXTENSIONS), __glcExtensions)) {
    printf("GLC_EXTENSIONS : %s\n(expected to be %s)\n",
    	  (char*)glcGetc(GLC_EXTENSIONS), (char*)__glcExtensions);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (strcmp(glcGetc(GLC_RELEASE), __glcRelease)) {
    printf("GLC_RELEASE : %s (expected to be %s)\n",
    	  (char*)glcGetc(GLC_RELEASE), (char*)__glcRelease);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (strcmp(glcGetc(GLC_VENDOR), __glcVendor)) {
    printf("GLC_VENDOR : %s (expected to be %s)",
    	  (char*)glcGetc(GLC_VENDOR), (char*)__glcVendor);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcStringType(GLC_UCS2);

  if (!checkError(GLC_NONE))
    return -1;

  if (memcmp(glcGetc(GLC_EXTENSIONS), __glcExtensionsUCS2,
      strlen(__glcExtensions)+1)*2) {
    printf("GLC_EXTENSIONS in UCS2 is wrong\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (memcmp(glcGetc(GLC_RELEASE), __glcReleaseUCS2,
      strlen(__glcRelease)+1)*2) {
    printf("GLC_RELEASE in UCS2 is wrong\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (memcmp(glcGetc(GLC_VENDOR), __glcVendorUCS2,
      strlen(__glcVendor)+1)*2) {
    printf("GLC_VENDOR in UCS2 is wrong\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcStringType(GLC_UCS4);

  if (!checkError(GLC_NONE))
    return -1;

  if (memcmp(glcGetc(GLC_EXTENSIONS), __glcExtensionsUCS4,
      strlen(__glcExtensions)+1)*4) {
    printf("GLC_EXTENSIONS in UCS4 is wrong\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (memcmp(glcGetc(GLC_RELEASE), __glcReleaseUCS4,
      strlen(__glcRelease)+1)*4) {
    printf("GLC_RELEASE in UCS4 is wrong\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (memcmp(glcGetc(GLC_VENDOR), __glcVendorUCS4,
      strlen(__glcVendor)+1)*4) {
    printf("GLC_VENDOR in UCS4 is wrong\n");
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcStringType(GLC_UTF8_QSO);

  if (!checkError(GLC_NONE))
    return -1;

  if (strcmp(glcGetc(GLC_EXTENSIONS), __glcExtensions)) {
    printf("GLC_EXTENSIONS : %s (expected to be %s)",
    	  (char*)glcGetc(GLC_EXTENSIONS), (char*)__glcExtensions);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (strcmp(glcGetc(GLC_RELEASE), __glcRelease)) {
    printf("GLC_RELEASE : %s (expected to be %s)",
    	  (char*)glcGetc(GLC_RELEASE), (char*)__glcRelease);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  if (strcmp(glcGetc(GLC_VENDOR), __glcVendor)) {
    printf("GLC_VENDOR : %s (expected to be %s)",
    	  (char*)glcGetc(GLC_VENDOR), (char*)__glcVendor);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcGetc((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  glcStringType(GLC_UCS1);

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGetf(GLC_RESOLUTION) > 1E-5) {
    printf("Initial value of GLC_RESOLUTION is %f (expected 0.)\n",
	   glcGetf(GLC_RESOLUTION));
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcResolution(-10.);
  if (!checkError(GLC_PARAMETER_ERROR)) {
    printf("Negative resolutions are illegal and should raise a GLC_PARAMETER_ERROR\n");
    return -1;
  }
  if ((glcGetf(GLC_RESOLUTION)-72.) > 1E-5) {
    printf("Initial value of GLC_RESOLUTION has been altered %f (expected 72.)\n",
	   glcGetf(GLC_RESOLUTION));
    return -1;
  }

  glcResolution(90.);

  if (!checkError(GLC_NONE))
    return -1;

  if ((glcGetf(GLC_RESOLUTION)-90.) > 1E-5) {
    printf("Current value of GLC_RESOLUTION is %f (expected 90.)\n",
	   glcGetf(GLC_RESOLUTION));
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcGetf((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  glcReplacementCode((GLint)'?');

  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_REPLACEMENT_CODE)  != (GLint)'?') {
    printf("Unexpected GLC_REPLACEMENT_CODE 0x%X (expected 0x%X)\n",
	   glcGeti(GLC_REPLACEMENT_CODE), (GLint)'?');
    return -1;
  }

  glcRenderStyle((GLCenum)0);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  glcRenderStyle(GLC_LINE);
  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_RENDER_STYLE) != GLC_LINE) {
    printf("Unexpected GLC_RENDER_STYLE 0x%X (expected 0x%X)\n",
	   glcGeti(GLC_RENDER_STYLE), GLC_LINE);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcRenderStyle(GLC_TEXTURE);
  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_RENDER_STYLE) != GLC_TEXTURE) {
    printf("Unexpected GLC_RENDER_STYLE 0x%X (expected 0x%X)\n",
	   glcGeti(GLC_RENDER_STYLE), GLC_TEXTURE);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  glcRenderStyle(GLC_TRIANGLE);
  if (!checkError(GLC_NONE))
    return -1;

  if (glcGeti(GLC_RENDER_STYLE) != GLC_TRIANGLE) {
    printf("Unexpected GLC_RENDER_STYLE 0x%X (expected 0x%X)\n",
	   glcGeti(GLC_RENDER_STYLE), GLC_TRIANGLE);
    return -1;
  }

  if (!checkError(GLC_NONE))
    return -1;

  count = glcGeti(GLC_CATALOG_COUNT);
  if (!checkError(GLC_NONE))
    return -1;

  glcGetListc(GLC_CATALOG_LIST, -1);
  if (!checkError(GLC_PARAMETER_ERROR))
    return -1;

  if (!checkError(GLC_NONE))
    return -1;

  if (count) {
    GLint i = 0;

    glcGetListc(GLC_CATALOG_LIST, count);
    if (!checkError(GLC_PARAMETER_ERROR))
      return -1;

    if (!checkError(GLC_NONE))
      return -1;

    for (i = 0; i < count; i++) {
      glcGetListc(GLC_CATALOG_LIST, i);
      if (!checkError(GLC_NONE))
	return -1;
    }
  }

  glcContext(0);

  if (!checkError(GLC_NONE))
    return -1;

  glcDeleteContext(ctx);

  if (!checkError(GLC_NONE))
    return -1;

  free(__glcExtensionsUCS2);
  free(__glcReleaseUCS2);
  free(__glcVendorUCS2);
  free(__glcExtensionsUCS4);
  free(__glcReleaseUCS4);
  free(__glcVendorUCS4);

  printf("Tests successful\n");
  return 0;
}
