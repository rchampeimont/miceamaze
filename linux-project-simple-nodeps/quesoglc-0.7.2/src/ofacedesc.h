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
/* $Id: ofacedesc.h 838 2008-10-18 21:35:03Z bcoconni $ */

/** \file
 * header of the object __GLCfaceDescriptor that contains the description of a
 * face.
 */

#ifndef __glc_ofacedesc_h
#define __glc_ofacedesc_h

#include "omaster.h"

typedef struct __GLCrendererDataRec __GLCrendererData;
typedef struct __GLCfaceDescriptorRec __GLCfaceDescriptor;

struct __GLCfaceDescriptorRec {
  FT_ListNodeRec node;
  FcPattern* pattern;
  FT_Face face;
#ifndef GLC_FT_CACHE
  int faceRefCount;
#endif
  FT_ListRec glyphList;
};


__GLCfaceDescriptor* __glcFaceDescCreate(__GLCmaster* inMaster,
					 const GLCchar8* inFace,
					 __GLCcontext* inContext, GLint inCode);
void __glcFaceDescDestroy(__GLCfaceDescriptor* This,
			  __GLCcontext* inContext);
#ifndef GLC_FT_CACHE
FT_Face __glcFaceDescOpen(__GLCfaceDescriptor* This,
			  __GLCcontext* inContext);
void __glcFaceDescClose(__GLCfaceDescriptor* This);
#endif
__GLCglyph* __glcFaceDescGetGlyph(__GLCfaceDescriptor* This, GLint inCode,
				  __GLCcontext* inContext);
void __glcFaceDescDestroyGLObjects(__GLCfaceDescriptor* This,
				   __GLCcontext* inContext);
GLboolean __glcFaceDescPrepareGlyph(__GLCfaceDescriptor* This,
				    __GLCcontext* inContext, GLfloat inScaleX,
				    GLfloat inScaleY, GLCulong inGlyphIndex);
GLfloat* __glcFaceDescGetBoundingBox(__GLCfaceDescriptor* This,
				     GLCulong inGlyphIndex, GLfloat* outVec,
				     GLfloat inScaleX, GLfloat inScaleY,
				     __GLCcontext* inContext);
GLfloat* __glcFaceDescGetAdvance(__GLCfaceDescriptor* This,
				 GLCulong inGlyphIndex, GLfloat* outVec,
				 GLfloat inScaleX, GLfloat inScaleY,
				 __GLCcontext* inContext);
const GLCchar8* __glcFaceDescGetFontFormat(__GLCfaceDescriptor* This,
					   __GLCcontext* inContext,
					   GLCenum inAttrib);
GLfloat* __glcFaceDescGetMaxMetric(__GLCfaceDescriptor* This, GLfloat* outVec,
				   __GLCcontext* inContext);
GLfloat* __glcFaceDescGetKerning(__GLCfaceDescriptor* This,
				 GLCuint inGlyphIndex, GLCuint inPrevGlyphIndex,
				 GLfloat inScaleX, GLfloat inScaleY,
				 GLfloat* outVec, __GLCcontext* inContext);
GLCchar8* __glcFaceDescGetStyleName(__GLCfaceDescriptor* This);
GLboolean __glcFaceDescIsFixedPitch(__GLCfaceDescriptor* This);
GLboolean __glcFaceDescOutlineDecompose(__GLCfaceDescriptor* This,
                                        __GLCrendererData* inData,
                                        __GLCcontext* inContext);
GLboolean __glcFaceDescGetBitmapSize(__GLCfaceDescriptor* This, GLint* outWidth,
                                     GLint *outHeight, GLfloat inScaleX,
				     GLfloat inScaleY, GLint* outPixBoundingBox,
				     int inFactor, __GLCcontext* inContext);
GLboolean __glcFaceDescGetBitmap(__GLCfaceDescriptor* This, GLint inWidth,
                                 GLint inHeight, void* inBuffer,
                                 __GLCcontext* inContext);
GLboolean __glcFaceDescOutlineEmpty(__GLCfaceDescriptor* This);
__GLCcharMap* __glcFaceDescGetCharMap(__GLCfaceDescriptor* This,
				      __GLCcontext* inContext);
#endif /* __glc_ofacedesc_h */
