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
/* $Id: omaster.c 824 2008-08-09 19:09:36Z bcoconni $ */

/** \file
 * defines the object __GLCmaster which manage the masters
 */

#include "internal.h"
#include <string.h>



/* Constructor of the object : it allocates memory and initializes the member
 * of the new object.
 */
__GLCmaster* __glcMasterCreate(GLint inMaster, __GLCcontext* inContext)
{
  __GLCmaster* This = NULL;
  GLCchar32 hashValue =
	((GLCchar32*)GLC_ARRAY_DATA(inContext->masterHashTable))[inMaster];
  FcObjectSet* objectSet = NULL;
  FcFontSet *fontSet = NULL;
  FcPattern *pattern = NULL;
  int i = 0;

  /* Use Fontconfig to get the default font files */
  pattern = FcPatternCreate();
  if (!pattern) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }
  objectSet = FcObjectSetBuild(FC_FAMILY, FC_FOUNDRY, FC_OUTLINE, FC_SPACING,
			       NULL);
  if (!objectSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcPatternDestroy(pattern);
    return NULL;
  }
  fontSet = FcFontList(inContext->config, pattern, objectSet);
  FcObjectSetDestroy(objectSet);
  FcPatternDestroy(pattern);
  if (!fontSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }

  /* Parse the font set looking for a font with an outline and which hash value
   * matches the hash value of the master we are looking for.
   */
  for (i = 0; i < fontSet->nfont; i++) {
    FcBool outline = FcFalse;
    FcResult result = FcResultMatch;
    FcChar8* family = NULL;
    int fixed = 0;
    FcChar8* foundry = NULL;

    result = FcPatternGetBool(fontSet->fonts[i], FC_OUTLINE, 0, &outline);
    assert(result != FcResultTypeMismatch);
    if (!outline)
      continue;

    result = FcPatternGetString(fontSet->fonts[i], FC_FAMILY, 0, &family);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetString(fontSet->fonts[i], FC_FOUNDRY, 0, &foundry);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetInteger(fontSet->fonts[i], FC_SPACING, 0, &fixed);
    assert(result != FcResultTypeMismatch);

    if (foundry)
      pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family,
			       FC_FOUNDRY, FcTypeString, foundry, FC_SPACING,
			       FcTypeInteger, fixed, NULL);
    else
      pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family,
			       FC_SPACING, FcTypeInteger, fixed, NULL);

    if (!pattern) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcFontSetDestroy(fontSet);
      return NULL;
    }

    if (hashValue == FcPatternHash(pattern))
      break;

    FcPatternDestroy(pattern);
  }

  assert(i < fontSet->nfont);
  FcFontSetDestroy(fontSet);

  This = (__GLCmaster*)__glcMalloc(sizeof(__GLCmaster));
  if (!This) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcPatternDestroy(pattern);
    return NULL;
  }

  /* Duplicate the pattern of the found font (otherwise it will be deleted with
   * the font set).
   */
  This->pattern = pattern;
  return This;
}



/* Destructor of the object */
void __glcMasterDestroy(__GLCmaster* This)
{
  FcPatternDestroy(This->pattern);

  __glcFree(This);
}



/* Get the style name of the face identified by inIndex  */
GLCchar8* __glcMasterGetFaceName(__GLCmaster* This, __GLCcontext* inContext,
				 GLint inIndex)
{
  FcObjectSet* objectSet = NULL;
  FcFontSet *fontSet = NULL;
  FcResult result = FcResultMatch;
  GLCchar8* string = NULL;
  GLCchar8* faceName;
  int i = 0;
  FcPattern* pattern = FcPatternCreate();

  if (!pattern) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }

  objectSet = FcObjectSetBuild(FC_FAMILY, FC_FOUNDRY, FC_SPACING, FC_OUTLINE,
			       FC_STYLE, NULL);
  if (!objectSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcPatternDestroy(pattern);
    return NULL;
  }
  fontSet = FcFontList(inContext->config, pattern, objectSet);
  FcObjectSetDestroy(objectSet);
  FcPatternDestroy(pattern);
  if (!fontSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }

  for (i = 0; i < fontSet->nfont; i++) {
    FcChar8* family = NULL;
    int fixed = 0;
    FcChar8* foundry = NULL;
    FcBool outline = FcFalse;
    FcBool equal = FcFalse;

    /* Check whether the glyphs are outlines */
    result = FcPatternGetBool(fontSet->fonts[i], FC_OUTLINE, 0, &outline);
    assert(result != FcResultTypeMismatch);

    if (!outline)
      continue;

    result = FcPatternGetString(fontSet->fonts[i], FC_FAMILY, 0, &family);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetString(fontSet->fonts[i], FC_FOUNDRY, 0, &foundry);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetInteger(fontSet->fonts[i], FC_SPACING, 0, &fixed);
    assert(result != FcResultTypeMismatch);

    if (foundry)
      pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family,
			       FC_FOUNDRY, FcTypeString, foundry, FC_SPACING,
			       FcTypeInteger, fixed, NULL);
    else
      pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family,
			       FC_SPACING, FcTypeInteger, fixed, NULL);

    if (!pattern) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcFontSetDestroy(fontSet);
      return NULL;
    }

    equal = FcPatternEqual(pattern, This->pattern);
    FcPatternDestroy(pattern);
    if (equal) {
      if (inIndex)
	inIndex--;
      else
	break;
    }
  }

  if (i == fontSet->nfont) {
    __glcRaiseError(GLC_PARAMETER_ERROR);
    FcFontSetDestroy(fontSet);
    return NULL;
  }

  result = FcPatternGetString(fontSet->fonts[i], FC_STYLE, 0, &string);
  assert(result != FcResultTypeMismatch);

#ifdef __WIN32__
  faceName = (GLCchar8*)_strdup((const char*)string);
#else
  faceName = (GLCchar8*)strdup((const char*)string);
#endif
  FcFontSetDestroy(fontSet);
  if (!faceName) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }

  return faceName;
}



/* Is this a fixed font ? */
GLboolean __glcMasterIsFixedPitch(__GLCmaster* This)
{
  FcResult result = FcResultMatch;
  int fixed = 0;

  result = FcPatternGetInteger(This->pattern, FC_SPACING, 0, &fixed);
  assert(result != FcResultTypeMismatch);
  return fixed ? GL_TRUE: GL_FALSE;
}



/* Get the face coutn of the master */
GLint __glcMasterFaceCount(__GLCmaster* This, __GLCcontext* inContext)
{
  FcObjectSet* objectSet = NULL;
  FcFontSet *fontSet = NULL;
  GLint count = 0;
  int i = 0;
  FcPattern* pattern = FcPatternCreate();

  if (!pattern) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return 0;
  }

  objectSet = FcObjectSetBuild(FC_FAMILY, FC_FOUNDRY, FC_SPACING, FC_OUTLINE,
			       FC_STYLE, NULL);
  if (!objectSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcPatternDestroy(pattern);
    return 0;
  }
  fontSet = FcFontList(inContext->config, pattern, objectSet);
  FcObjectSetDestroy(objectSet);
  FcPatternDestroy(pattern);
  if (!fontSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return 0;
  }

  for (i = 0; i < fontSet->nfont; i++) {
    FcChar8* family = NULL;
    int fixed = 0;
    FcChar8* foundry = NULL;
    FcBool outline = FcFalse;
    FcResult result = FcResultMatch;
    FcBool equal = FcFalse;

    /* Check whether the glyphs are outlines */
    result = FcPatternGetBool(fontSet->fonts[i], FC_OUTLINE, 0, &outline);
    assert(result != FcResultTypeMismatch);

    if (!outline)
      continue;

    result = FcPatternGetString(fontSet->fonts[i], FC_FAMILY, 0, &family);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetString(fontSet->fonts[i], FC_FOUNDRY, 0, &foundry);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetInteger(fontSet->fonts[i], FC_SPACING, 0, &fixed);
    assert(result != FcResultTypeMismatch);

    if (foundry)
      pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family,
			       FC_FOUNDRY, FcTypeString, foundry, FC_SPACING,
			       FcTypeInteger, fixed, NULL);
    else
      pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family,
			       FC_SPACING, FcTypeInteger, fixed, NULL);

    if (!pattern) {
      __glcRaiseError(GLC_RESOURCE_ERROR);
      FcFontSetDestroy(fontSet);
      return 0;
    }

    equal = FcPatternEqual(pattern, This->pattern);
    FcPatternDestroy(pattern);
    if (equal)
      count++;
  }

  FcFontSetDestroy(fontSet);
  return count;
}



/* This subroutine is called whenever the user wants to access to informations
 * that have not been loaded from the font files yet. In order to reduce disk
 * accesses, informations such as the master format, full name or version are
 * read "just in time" i.e. only when the user requests them.
 */
const GLCchar8* __glcMasterGetInfo(__GLCmaster* This, __GLCcontext* inContext,
				   GLCenum inAttrib)
{
  __GLCfaceDescriptor* faceDesc = NULL;
  FcResult result = FcResultMatch;
  GLCchar8 *string = NULL;
  const GLCchar8* info = NULL;
  const GLCchar8 *buffer = NULL;

  /* Get the Unicode string which corresponds to the requested attribute */
  switch(inAttrib) {
  case GLC_FAMILY:
    result = FcPatternGetString(This->pattern, FC_FAMILY, 0, &string);
    assert(result != FcResultTypeMismatch);
    return string;
  case GLC_VENDOR:
    result = FcPatternGetString(This->pattern, FC_FOUNDRY, 0, &string);
    assert(result != FcResultTypeMismatch);
    return string;
  case GLC_VERSION:
  case GLC_FULL_NAME_SGI:
  case GLC_MASTER_FORMAT:
    faceDesc = __glcFaceDescCreate(This, NULL, inContext, 0);

    if (!faceDesc)
      return NULL;

#ifndef GLC_FT_CACHE
    if (!__glcFaceDescOpen(faceDesc, inContext)) {
      __glcFaceDescDestroy(faceDesc, inContext);
      return NULL;
    }
#endif

    info = __glcFaceDescGetFontFormat(faceDesc, inContext, inAttrib);
    if (info) {
      /* Convert the string and store it in the context buffer */
      buffer = (const GLCchar8*)__glcConvertFromUtf8ToBuffer(inContext, info,
					    inContext->stringState.stringType);
    }
    else
      __glcRaiseError(GLC_RESOURCE_ERROR);

    if (faceDesc) {
#ifndef GLC_FT_CACHE
      __glcFaceDescClose(faceDesc);
#endif
      __glcFaceDescDestroy(faceDesc, inContext);
    }

    return buffer;
  }


  /* The program is not supposed to reach the end of the function.
   * The following return is there to prevent the compiler to issue
   * a warning about 'control reaching the end of a non-void function'.
   */
    return (const GLCchar8*)"";
}



/* Create a master on the basis of the family name */
__GLCmaster* __glcMasterFromFamily(__GLCcontext* inContext, GLCchar8* inFamily)
{
  FcObjectSet* objectSet = NULL;
  FcFontSet *fontSet = NULL;
  int i = 0;
  FcPattern* pattern = FcPatternCreate();

  if (!pattern) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }

  objectSet = FcObjectSetBuild(FC_FAMILY, FC_FOUNDRY, FC_OUTLINE, FC_SPACING,
			       NULL);
  if (!objectSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcPatternDestroy(pattern);
    return NULL;
  }
  fontSet = FcFontList(inContext->config, pattern, objectSet);
  FcObjectSetDestroy(objectSet);
  FcPatternDestroy(pattern);
  if (!fontSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }

  for (i = 0; i < fontSet->nfont; i++) {
    FcBool outline = FcFalse;
    FcResult result = FcResultMatch;
    FcChar8* family = NULL;
    int fixed = 0;
    FcChar8* foundry = NULL;

    result = FcPatternGetBool(fontSet->fonts[i], FC_OUTLINE, 0, &outline);
    assert(result != FcResultTypeMismatch);
    if (!outline)
      continue;

    result = FcPatternGetString(fontSet->fonts[i], FC_FAMILY, 0, &family);
    assert(result != FcResultTypeMismatch);
    if (strcmp((const char*)family, (const char*)inFamily))
      continue;

    result = FcPatternGetString(fontSet->fonts[i], FC_FOUNDRY, 0, &foundry);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetInteger(fontSet->fonts[i], FC_SPACING, 0, &fixed);
    assert(result != FcResultTypeMismatch);

    if (foundry)
      pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family,
			       FC_FOUNDRY, FcTypeString, foundry, FC_SPACING,
			       FcTypeInteger, fixed, NULL);
    else
      pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family,
			       FC_SPACING, FcTypeInteger, fixed, NULL);

    if (pattern) {
      __GLCmaster* This = (__GLCmaster*)__glcMalloc(sizeof(__GLCmaster));
      if (!This) {
	__glcRaiseError(GLC_RESOURCE_ERROR);
	FcFontSetDestroy(fontSet);
	return NULL;
      }

      This->pattern = pattern;
      FcFontSetDestroy(fontSet);
      return This;
    }
  }

  __glcRaiseError(GLC_RESOURCE_ERROR);
  FcFontSetDestroy(fontSet);
  return NULL;
}



/* Create a master which contains at least a font which math the character
 * identified by inCode.
 */
__GLCmaster* __glcMasterMatchCode(__GLCcontext* inContext, GLint inCode)
{
  __GLCmaster* This = NULL;
  FcPattern* pattern = NULL;
  FcFontSet* fontSet = NULL;
  FcFontSet* fontSet2 = NULL;
  FcObjectSet* objectSet = NULL;
  FcResult result = FcResultMatch;
  int f = 0;
  FcChar8* family = NULL;
  int fixed = 0;
  FcChar8* foundry = NULL;
  FcCharSet* charSet = FcCharSetCreate();

  if (!charSet)
    return NULL;

  if (!FcCharSetAddChar(charSet, inCode)) {
    FcCharSetDestroy(charSet);
    return NULL;
  }
  pattern = FcPatternBuild(NULL, FC_CHARSET, FcTypeCharSet, charSet,
			   FC_OUTLINE, FcTypeBool, FcTrue, NULL);
  FcCharSetDestroy(charSet);
  if (!pattern) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }

  if (!FcConfigSubstitute(inContext->config, pattern, FcMatchPattern)) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcPatternDestroy(pattern);
    return NULL;
  }
  FcDefaultSubstitute(pattern);
  fontSet = FcFontSort(inContext->config, pattern, FcFalse, NULL, &result);
  FcPatternDestroy(pattern);
  if ((!fontSet) || (result == FcResultTypeMismatch)) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    return NULL;
  }

  for (f = 0; f < fontSet->nfont; f++) {
    FcBool outline = FcFalse;

    result = FcPatternGetBool(fontSet->fonts[f], FC_OUTLINE, 0, &outline);
    assert(result != FcResultTypeMismatch);
    result = FcPatternGetCharSet(fontSet->fonts[f], FC_CHARSET, 0, &charSet);
    assert(result != FcResultTypeMismatch);

    if (outline && FcCharSetHasChar(charSet, inCode))
      break;
  }

  if (f == fontSet->nfont) {
    FcFontSetDestroy(fontSet);
    return NULL;
  }

  /* Ugly hack to extract a subset of the pattern fontSet->fonts[f]
   * (otherwise the hash value will not match any value of the hash table).
   */
  objectSet = FcObjectSetBuild(FC_FAMILY, FC_FOUNDRY, FC_OUTLINE, FC_SPACING,
			       NULL);
  if (!objectSet) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcFontSetDestroy(fontSet);
    return NULL;
  }
  fontSet2 = FcFontList(inContext->config, fontSet->fonts[f], objectSet);
  FcObjectSetDestroy(objectSet);
  if (!fontSet2) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcFontSetDestroy(fontSet);
    return NULL;
  }

  assert(fontSet2->nfont);

  This = (__GLCmaster*)__glcMalloc(sizeof(__GLCmaster));
  if (!This) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    FcFontSetDestroy(fontSet);
    FcFontSetDestroy(fontSet2);
    return NULL;
  }

  /* Duplicate the pattern of the found font (otherwise it will be deleted with
   * the font set).
   */
  result = FcPatternGetString(fontSet2->fonts[0], FC_FAMILY, 0, &family);
  assert(result != FcResultTypeMismatch);
  result = FcPatternGetString(fontSet2->fonts[0], FC_FOUNDRY, 0, &foundry);
  assert(result != FcResultTypeMismatch);
  result = FcPatternGetInteger(fontSet2->fonts[0], FC_SPACING, 0, &fixed);
  assert(result != FcResultTypeMismatch);

  if (foundry)
    pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family, FC_FOUNDRY,
			     FcTypeString, foundry, FC_SPACING, FcTypeInteger,
			     fixed, NULL);
  else
    pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, family, FC_SPACING,
			     FcTypeInteger, fixed, NULL);

  FcFontSetDestroy(fontSet2);
  FcFontSetDestroy(fontSet);
  if (!pattern) {
    __glcRaiseError(GLC_RESOURCE_ERROR);
    __glcFree(This);
    return NULL;
  }

  This->pattern = pattern;
  return This;
}



GLint __glcMasterGetID(__GLCmaster* This, __GLCcontext* inContext)
{
  GLCchar32 hashValue = GLC_MASTER_HASH_VALUE(This);
  GLint i = 0;
  GLCchar32* hashTable = (GLCchar32*)GLC_ARRAY_DATA(inContext->masterHashTable);

  for (i = 0; i < GLC_ARRAY_LENGTH(inContext->masterHashTable); i++) {
    if (hashValue == hashTable[i])
      break;
  }
  assert(i < GLC_ARRAY_LENGTH(inContext->masterHashTable));

  return i;
}
