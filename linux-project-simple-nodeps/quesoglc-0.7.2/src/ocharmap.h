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
/* $Id: ocharmap.h 800 2008-06-04 21:47:50Z bcoconni $ */

/** \file
 * header of the object __GLCcharMap which manage the charmaps of both the fonts
 * and the masters.
 */

#ifndef __glc_ocharmap_h
#define __glc_ocharmap_h

#include "ocontext.h"
#include "oglyph.h"

typedef struct __GLCcharMapElementRec __GLCcharMapElement;
typedef struct __GLCcharMapRec __GLCcharMap;
typedef struct __GLCmasterRec __GLCmaster;

struct __GLCcharMapElementRec {
  GLCulong mappedCode;
  __GLCglyph* glyph;
};

struct __GLCcharMapRec {
  FcCharSet* charSet;
  __GLCarray* map;
};

__GLCcharMap* __glcCharMapCreate(__GLCmaster* inMaster, __GLCcontext* inContext);
void __glcCharMapDestroy(__GLCcharMap* This);
void __glcCharMapAddChar(__GLCcharMap* This, GLint inCode,
			 __GLCglyph* inGlyph);
void __glcCharMapRemoveChar(__GLCcharMap* This, GLint inCode);
const GLCchar8* __glcCharMapGetCharName(__GLCcharMap* This, GLint inCode);
__GLCglyph* __glcCharMapGetGlyph(__GLCcharMap* This, GLint inCode);
GLboolean __glcCharMapHasChar(__GLCcharMap* This, GLint inCode);
const GLCchar8* __glcCharMapGetCharNameByIndex(__GLCcharMap* This,
					       GLint inIndex);
GLint __glcCharMapGetCount(__GLCcharMap* This);
GLint __glcCharMapGetMaxMappedCode(__GLCcharMap* This);
GLint __glcCharMapGetMinMappedCode(__GLCcharMap* This);
#endif
