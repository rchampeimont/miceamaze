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
/* $Id: omaster.h 807 2008-06-07 18:33:28Z bcoconni $ */

/** \file
 * header of the object __GLCmaster which manage the masters
 */

#ifndef __glc_omaster_h
#define __glc_omaster_h

#include <fontconfig/fontconfig.h>

#include "ocharmap.h"

#define GLC_MASTER_HASH_VALUE(master) FcPatternHash((master)->pattern)

struct __GLCmasterRec {
  FcPattern* pattern;
};

__GLCmaster* __glcMasterCreate(GLint inMaster, __GLCcontext* inContext);
void __glcMasterDestroy(__GLCmaster* This);
GLCchar8* __glcMasterGetFaceName(__GLCmaster* This, __GLCcontext* inContext,
				 GLint inIndex);
GLboolean __glcMasterIsFixedPitch(__GLCmaster* This);
GLint __glcMasterFaceCount(__GLCmaster* This, __GLCcontext* inContext);
const GLCchar8* __glcMasterGetInfo(__GLCmaster* This, __GLCcontext* inContext,
			     GLCenum inAttrib);
__GLCmaster* __glcMasterFromFamily(__GLCcontext* inContext, GLCchar8* inFamily);
__GLCmaster* __glcMasterMatchCode(__GLCcontext* inContext, GLint inCode);
GLint __glcMasterGetID(__GLCmaster* This, __GLCcontext* inContext);
#endif
