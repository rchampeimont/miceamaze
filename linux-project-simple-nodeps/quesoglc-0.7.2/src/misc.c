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
/* $Id: misc.c 887 2009-03-24 01:03:11Z bcoconni $ */

/** \file
 * defines miscellaneous utility routines used throughout QuesoGLC.
 */

#include <math.h>

#include "internal.h"


#ifdef DEBUGMODE
GLCAPI GLuint __glcMemAllocCount = 0;
GLCAPI GLuint __glcMemAllocTrigger = 0;
GLCAPI GLboolean __glcMemAllocFailOnce = GL_TRUE;
#endif

/* QuesoGLC own allocation and memory management routines */
void* __glcMalloc(size_t size)
{
#ifdef DEBUGMODE
  __glcMemAllocCount++;

  if (__glcMemAllocFailOnce) {
    if (__glcMemAllocCount == __glcMemAllocTrigger)
      return NULL;
  }
  else if (__glcMemAllocCount >= __glcMemAllocTrigger)
    return NULL;
#endif
  return malloc(size);
}

void __glcFree(void *ptr)
{
  /* Not all implementations of free() accept NULL. Moreover this allows to
   * detect useless calls.
   */
  assert(ptr);

  free(ptr);
}

void* __glcRealloc(void *ptr, size_t size)
{
#ifdef DEBUGMODE
  __glcMemAllocCount++;

  if (__glcMemAllocFailOnce) {
    if (__glcMemAllocCount == __glcMemAllocTrigger)
      return NULL;
  }
  else if (__glcMemAllocCount >= __glcMemAllocTrigger)
    return NULL;
#endif
  return realloc(ptr, size);
}



/* Find a token in a list of tokens separated by 'separator' */
GLCchar8* __glcFindIndexList(GLCchar8* inString, GLuint inIndex,
			     const GLCchar8* inSeparator)
{
  GLuint occurence = 0;
  GLCchar8* s = inString;
  const GLCchar8* sep = inSeparator;

  if (!inIndex)
    return s;

  for (; *s != '\0'; s++) {
    if (*s == *sep) {
      occurence++;
      if (occurence == inIndex)
	break;
    }
  }

  return (GLCchar8 *) s;
}



#ifndef HAVE_TLS
/* Each thread has to store specific informations so they can be retrieved
 * later. __glcGetThreadArea() returns a struct which contains thread specific
 * info for GLC.
 * If the '__GLCthreadArea' of the current thread does not exist, it is created
 * and initialized.
 * IMPORTANT NOTE : __glcGetThreadArea() must never use __glcMalloc() and
 *    __glcFree() since those functions could use the exceptContextStack
 *    before it is initialized.
 */
__GLCthreadArea* __glcGetThreadArea(void)
{
  __GLCthreadArea *area = NULL;

#ifdef __WIN32__
  area = (__GLCthreadArea*)TlsGetValue(__glcCommonArea.threadKey);
#else
  area = (__GLCthreadArea*)pthread_getspecific(__glcCommonArea.threadKey);
#endif
  if (area)
    return area;

  area = (__GLCthreadArea*)malloc(sizeof(__GLCthreadArea));
  if (!area)
    return NULL;

  area->currentContext = NULL;
  area->errorState = GLC_NONE;
  area->lockState = 0;
  area->exceptionStack.head = NULL;
  area->exceptionStack.tail = NULL;
  area->failedTry = GLC_NO_EXC;
#ifdef __WIN32__
  if (!TlsSetValue(__glcCommonArea.threadKey, (LPVOID)area)) {
    free(area);
    return NULL;
  }
#else
  pthread_setspecific(__glcCommonArea.threadKey, (void*)area);
#endif

#ifdef __WIN32__
  if (__glcCommonArea.threadID == GetCurrentThreadId())
#else
  if (pthread_equal(__glcCommonArea.threadID, pthread_self()))
#endif
    __glcThreadArea = area;
  return area;
}
#endif /* HAVE_TLS */



#ifndef HAVE_TLS
/* Raise an error. This function must be called each time the current error
 * of the issuing thread must be set
 */
void __glcRaiseError(GLCenum inError)
{
  GLCenum error = GLC_NONE;
  __GLCthreadArea *area = NULL;

  area = GLC_GET_THREAD_AREA();
  assert(area);

  /* An error can only be raised if the current error value is GLC_NONE.
   * However, when inError == GLC_NONE then we must force the current error
   * value to GLC_NONE whatever its previous value was.
   */
  error = area->errorState;
  if (!error || !inError)
    area->errorState = inError;
}
#endif /* HAVE_TLS */



#ifndef HAVE_TLS
/* Get the current context of the issuing thread */
__GLCcontext* __glcGetCurrent(void)
{
  __GLCthreadArea *area = NULL;

  area = __glcGetThreadArea(); /* Don't use GLC_GET_THREAD_AREA */
  assert(area);

  return area->currentContext;
}
#endif



/* Process the character in order to find a font that maps the code and to
 * render the corresponding glyph. Replacement code and '<hexcode>' format
 * are issued if necessary. The previous code is updated accordingly.
 * 'inCode' must be given in UCS-4 format
 */
void* __glcProcessChar(__GLCcontext *inContext, GLint inCode,
		       __GLCcharacter* inPrevCode, GLboolean inIsRTL,
		       __glcProcessCharFunc inProcessCharFunc,
		       void* inProcessCharData)
{
  GLint repCode = 0;
  __GLCfont* font = NULL;
  void* ret = NULL;

  if (!inCode)
    return NULL;

  /* Get a font that maps inCode */
  font = __glcContextGetFont(inContext, inCode);
  if (font) {
    /* A font has been found */
    if (font != inPrevCode->font)
      inPrevCode->code = 0; /* The font has changed, kerning must be disabled */
    ret = inProcessCharFunc(inCode, inPrevCode->code, inIsRTL, font, inContext,
			    inProcessCharData, GL_FALSE);
    inPrevCode->code = inCode;
    inPrevCode->font = font;
    return ret;
  }

  /* __glcCtxGetFont() can not find a font that maps inCode, we then attempt to
   * produce an alternate rendering.
   */

  /* If the variable GLC_REPLACEMENT_CODE is nonzero, and __glcCtxGetFont()
   * finds a font that maps the replacement code, we now render the character
   * that the replacement code is mapped to
   */
  repCode = inContext->stringState.replacementCode;
  font = __glcContextGetFont(inContext, repCode);
  if (repCode && font) {
    if (font != inPrevCode->font)
      inPrevCode->code = 0; /* The font has changed, kerning must be disabled */
    ret = inProcessCharFunc(repCode, inPrevCode->code, inIsRTL, font, inContext,
			    inProcessCharData, GL_FALSE);
    inPrevCode->code = inCode;
    inPrevCode->font = font;
    return ret; 
  }
  else {
    /* If we get there, we failed to render both the character that inCode maps
     * to and the replacement code. Now, we will try to render the character
     * sequence "\<hexcode>", where '\' is the character REVERSE SOLIDUS 
     * (U+5C), '<' is the character LESS-THAN SIGN (U+3C), '>' is the character
     * GREATER-THAN SIGN (U+3E), and 'hexcode' is inCode represented as a
     * sequence of hexadecimal digits. The sequence has no leading zeros, and
     * alphabetic digits are in upper case. The GLC measurement commands treat
     * the sequence as a single character.
     */
    char buf[11];
    GLint i = 0;
    GLint n = 0;

    /* Check if a font maps hexadecimal digits */
#ifdef _MSC_VER
    n = sprintf_s(buf, 11, "\\<%X>", (int)inCode);
    if (n < 0) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      return NULL;
    }
#else
    n = snprintf(buf, 11, "\\<%X>", (int)inCode);
#endif
    for (i = 0; i < n; i++) {
      if (!__glcContextGetFont(inContext, buf[i]))
	/* The code is not rendered, the previous code is thus left unchanged */
	return NULL;
    }

    /* Render the '\<hexcode>' sequence */
    for (i = 0; i < n; i++) {
      GLint pos = inIsRTL ? n-i-1 : i;

      font = __glcContextGetFont(inContext, buf[pos]);
      if (font != inPrevCode->font)
	inPrevCode->code = 0; /*The font has changed, kerning must be disabled*/
      ret = inProcessCharFunc(buf[pos], inPrevCode->code, inIsRTL, font,
			      inContext, inProcessCharData, GL_TRUE);
      inPrevCode->code = buf[pos];
      inPrevCode->font = font;
    }
    return ret;
  }
}



/* Store an 4x4 identity matrix in 'm' */
static void __glcMakeIdentity(GLfloat* m)
{
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}



/* Invert a 4x4 matrix stored in inMatrix. The result is stored in outMatrix
 * It uses the Gauss-Jordan elimination method
 */
static GLboolean __glcInvertMatrix(GLfloat* inMatrix, GLfloat* outMatrix)
{
  int i, j, k, swap;
  GLfloat t;
  GLfloat temp[4][4];

  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      temp[i][j] = inMatrix[i*4+j];
    }
  }
  __glcMakeIdentity(outMatrix);

  for (i = 0; i < 4; i++) {
    /* Look for largest element in column */
    swap = i;
    for (j = i + 1; j < 4; j++) {
      if (fabs(temp[j][i]) > fabs(temp[i][i])) {
	swap = j;
      }
    }

    if (swap != i) {
      /* Swap rows */
      for (k = 0; k < 4; k++) {
	t = temp[i][k];
	temp[i][k] = temp[swap][k];
	temp[swap][k] = t;

	t = outMatrix[i*4+k];
	outMatrix[i*4+k] = outMatrix[swap*4+k];
	outMatrix[swap*4+k] = t;
      }
    }

    if (fabs(temp[i][i]) < GLC_EPSILON) {
      /* No non-zero pivot. The matrix is singular, which shouldn't
       * happen. This means the user gave us a bad matrix.
       */
      return GL_FALSE;
    }

    t = temp[i][i];
    for (k = 0; k < 4; k++) {
      temp[i][k] /= t;
      outMatrix[i*4+k] /= t;
    }
    for (j = 0; j < 4; j++) {
      if (j != i) {
	t = temp[j][i];
	for (k = 0; k < 4; k++) {
	  temp[j][k] -= temp[i][k]*t;
	  outMatrix[j*4+k] -= outMatrix[i*4+k]*t;
	}
      }
    }
  }
  return GL_TRUE;
}



/* Mutiply two 4x4 matrices, the operands are stored in inMatrix1 and inMatrix2
 * The result is stored in outMatrix which can be neither inMatrix1 nor
 * inMatrix2.
 */
static void __glcMultMatrices(GLfloat* inMatrix1, GLfloat* inMatrix2,
			      GLfloat* outMatrix)
{
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      outMatrix[i*4+j] = 
	inMatrix1[i*4+0]*inMatrix2[0*4+j] +
	inMatrix1[i*4+1]*inMatrix2[1*4+j] +
	inMatrix1[i*4+2]*inMatrix2[2*4+j] +
	inMatrix1[i*4+3]*inMatrix2[3*4+j];
    }
  }
}



/* Compute an optimal size for the glyph to be rendered on the screen if no
 * display list is planned to be built.
 */
void __glcGetScale(__GLCcontext* inContext, GLfloat* outTransformMatrix,
		   GLfloat* outScaleX, GLfloat* outScaleY)
{
  int i = 0;

  if (inContext->renderState.renderStyle != GLC_BITMAP) {
    /* Compute the matrix that transforms object space coordinates to viewport
     * coordinates. If we plan to use object space coordinates, this matrix is
     * set to identity.
     */
    GLfloat projectionMatrix[16];
    GLfloat modelviewMatrix[16];
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

    __glcMultMatrices(modelviewMatrix, projectionMatrix, outTransformMatrix);

    if (!inContext->enableState.glObjects && inContext->enableState.hinting) {
      GLfloat rs[16], m[16];
      /* Get the scale factors in each X, Y and Z direction */
      GLfloat sx = sqrt(outTransformMatrix[0] * outTransformMatrix[0]
			+outTransformMatrix[1] * outTransformMatrix[1]
			+outTransformMatrix[2] * outTransformMatrix[2]);
      GLfloat sy = sqrt(outTransformMatrix[4] * outTransformMatrix[4]
			+outTransformMatrix[5] * outTransformMatrix[5]
			+outTransformMatrix[6] * outTransformMatrix[6]);
      GLfloat sz = sqrt(outTransformMatrix[8] * outTransformMatrix[8]
			+outTransformMatrix[9] * outTransformMatrix[9]
			+outTransformMatrix[10] * outTransformMatrix[10]);
      GLfloat x = 0., y = 0.;

      memset(rs, 0, 16 * sizeof(GLfloat));
      rs[15] = 1.;
      for (i = 0; i < 3; i++) {
	rs[0+4*i] = outTransformMatrix[0+4*i] / sx;
	rs[1+4*i] = outTransformMatrix[1+4*i] / sy;
	rs[2+4*i] = outTransformMatrix[2+4*i] / sz;
      }
      if (!__glcInvertMatrix(rs, rs)) {
	*outScaleX = 0.f;
	*outScaleY = 0.f;
	return;
      }

      __glcMultMatrices(rs, outTransformMatrix, m);
      x = ((m[0] + m[12])/(m[3] + m[15]) - m[12]/m[15]) * viewport[2] * 0.5;
      y = ((m[1] + m[13])/(m[3] + m[15]) - m[13]/m[15]) * viewport[3] * 0.5;
      *outScaleX = sqrt(x*x+y*y);
      x = ((m[4] + m[12])/(m[7] + m[15]) - m[12]/m[15]) * viewport[2] * 0.5;
      y = ((m[5] + m[13])/(m[7] + m[15]) - m[13]/m[15]) * viewport[3] * 0.5;
      *outScaleY = sqrt(x*x+y*y);
    }
    else {
      *outScaleX = GLC_POINT_SIZE;
      *outScaleY = GLC_POINT_SIZE;
    }
  }
  else {
    GLfloat determinant = 0., norm = 0.;
    GLfloat *transform = inContext->bitmapMatrix;

    /* Compute the norm of the transformation matrix */
    for (i = 0; i < 4; i++) {
      if (fabsf(transform[i]) > norm)
	norm = fabsf(transform[i]);
    }

    determinant = transform[0] * transform[3] - transform[1] * transform[2];

    /* If the transformation is degenerated, nothing needs to be rendered */
    if (fabsf(determinant) < norm * GLC_EPSILON) {
      *outScaleX = 0.f;
      *outScaleY = 0.f;
      return;
    }

    if (inContext->enableState.hinting) {
      *outScaleX = sqrt(transform[0]*transform[0]+transform[1]*transform[1]);
      *outScaleY = sqrt(transform[2]*transform[2]+transform[3]*transform[3]);
    }
    else {
      *outScaleX = GLC_POINT_SIZE;
      *outScaleY = GLC_POINT_SIZE;
    }
  }
}



/* Save the GL State in a structure */
void __glcSaveGLState(__GLCglState* inGLState, __GLCcontext* inContext,
		      GLboolean inAll)
{
  if (inAll || inContext->renderState.renderStyle == GLC_TEXTURE) {
    inGLState->blend = glIsEnabled(GL_BLEND);
    glGetIntegerv(GL_BLEND_SRC, &inGLState->blendSrc);
    glGetIntegerv(GL_BLEND_DST, &inGLState->blendDst);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &inGLState->textureID);
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
		  &inGLState->textureEnvMode);
    if ((!inContext->enableState.glObjects) && GLEW_ARB_pixel_buffer_object)
      glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING_ARB,
		    &inGLState->bufferObjectID);
  }

  if (inContext->enableState.glObjects && GLEW_ARB_vertex_buffer_object)
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING_ARB, &inGLState->bufferObjectID);
}



/* Restore the GL State from a structure */
void __glcRestoreGLState(__GLCglState* inGLState, __GLCcontext* inContext,
			 GLboolean inAll)
{
  if (inAll || inContext->renderState.renderStyle == GLC_TEXTURE) {
    if (!inGLState->blend)
      glDisable(GL_BLEND);
    glBlendFunc(inGLState->blendSrc, inGLState->blendDst);
    glBindTexture(GL_TEXTURE_2D, inGLState->textureID);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, inGLState->textureEnvMode);
    if ((!inContext->enableState.glObjects) && GLEW_ARB_pixel_buffer_object)
      glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, inGLState->bufferObjectID);
  }

  if (inContext->enableState.glObjects && GLEW_ARB_vertex_buffer_object)
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, inGLState->bufferObjectID);
}



/* Function for GLEW so that it can get a context */
GLEWAPI GLEWContext* glewGetContext(void)
{
  __GLCcontext* ctx = GLC_GET_CURRENT_CONTEXT();

  if (!ctx) {
    __glcRaiseError(GLC_STATE_ERROR);
    return NULL;
  }

  return &ctx->glewContext;
}



/* This function initializes the thread management of QuesoGLC when TLS is not
 * available. It must be called once (see the macro GLC_INIT_THREAD)
 */
#ifndef HAVE_TLS
void __glcInitThread(void) {
#ifdef __WIN32__
  __glcCommonArea.threadID = GetCurrentThreadId();
#else
  __glcCommonArea.threadID = pthread_self();
#endif /* __WIN32__ */
}
#endif /* HAVE_TLS */
