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
/* $Id: test4.c 587 2007-02-27 22:20:14Z bcoconni $ */

/** \file
 * This test checks that for routines that are not "global commands", GLC
 * issues GLC_STATE_ERROR if no context has been made current to the issuing
 * thread.
 */

#include "GL/glc.h"
#include <stdio.h>

#define TestErrorCode(func) \
    err = glcGetError(); \
    if (err != GLC_STATE_ERROR) { \
        printf(#func"() Unexpected error : 0x%x\n", (int)err); \
        return -1; \
    } \
    err = glcGetError(); \
    if (err) { \
        printf(#func"Error is not GLC_NONE but : 0x%x\n", (int)err); \
        return -1; \
    }

int main(void)
{
    GLCenum err;
    GLfloat tab[4];

    err = glcGetError();
    if (err) {
        printf("Unexpected error : 0x%x\n", (int)err);
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

    printf("Tests successful !\n");
    return 0;
}
