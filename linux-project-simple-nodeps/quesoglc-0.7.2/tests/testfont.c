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
/* $Id: testfont.c 822 2008-08-09 12:53:58Z bcoconni $ */

/** \file
 * Test the GLC functions of the font group. It can be used either :
 * - by checking the output
 * - by checking that it does not crash
 * - by checking memory leaks with a tool like valgrind
 */

#include "GL/glc.h"
#include <stdio.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int main(int argc, char **argv)
{
  GLint ctx = 0;
  GLint font = 0;
  GLint dummyFont = 0;
  int i = 0;
  int last = 0;
  int count = 0;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("testfont");

  ctx = glcGenContext();
  glcContext(ctx);

#ifdef __WIN32__
  font = glcNewFontFromFamily(1, "Times New Roman");
  glcFontFace(font, "Bold");
  glcFont(font);
  font = glcNewFontFromFamily(3, "Courier New");
  glcAppendFont(font);
  glcFontFace(font, "Italic");
#else
  /* For platforms other than Windows, available fonts may vary from a platform
   * to another. It even vary from one Linux distribution to another !!! So we
   * can not rely on a font name; instead, we will look for masters with
   * specific properties : here we search for masters that contain the letter
   * 'A' (which usually means that the whole latin alphabet is available) and
   * that have at least 2 faces so that we can choose another face than the
   * default one.
   */
  /* Find a master that contains the letter 'A' (code 65) */
  for (i = 0; i < glcGeti(GLC_MASTER_COUNT); i++) {
    if (glcGetMasterMap(i, 65) && (glcGetMasteri(i, GLC_FACE_COUNT) > 1))
      break;
  }
  /* Create a font with the second face of the master */
  font = glcNewFontFromMaster(1, i);
  glcFontFace(font, glcGetMasterListc(i, GLC_FACE_LIST, 1));
  glcFont(font);

  /* Find another master */
  for (; i < glcGeti(GLC_MASTER_COUNT); i++) {
    if (glcGetMasterMap(i, 65) && (glcGetMasteri(i, GLC_FACE_COUNT) > 1))
      break;
  }
  font = glcNewFontFromMaster(3, i);
  glcAppendFont(font);
#endif

  dummyFont = glcGenFontID();
  if (dummyFont != 2) {
    printf("Unexpected font ID #%d (should have been #2)\n", dummyFont);
    return -1;
  }

  dummyFont = glcGenFontID();
  if (dummyFont != 4) {
    printf("Unexpected font ID #%d (should have been #4)\n", dummyFont);
    return -1;
  }

  if (!glcIsFont(font)) {
    printf("Font %d not recognized as a legal font ID while it should\n", font);
    return -1;
  }

  printf("Face : %s\n", (char *)glcGetFontFace(font));
  printf("GenFontID : %d\n", glcGenFontID());
  printf("Font #%d family : %s\n", font,
	 (char *)glcGetFontc(font, GLC_FAMILY));
  printf("Font #%d master format : %s\n", font,
	 (char *)glcGetFontc(font, GLC_MASTER_FORMAT));
  printf("Font #%d vendor : %s\n", font,
	 (char *)glcGetFontc(font, GLC_VENDOR));
  printf("Font #%d version : %s\n", font,
	 (char *)glcGetFontc(font, GLC_VERSION));
  count = glcGetFonti(font, GLC_FACE_COUNT);
  printf("Font #%d faces : %d\n", font, count);
  for (i = 0; i < count; i++)
    printf("Font #%d face #%d: %s\n", font, i,
	   (char *)glcGetFontListc(font, GLC_FACE_LIST, i));

  count = glcGetFonti(font, GLC_CHAR_COUNT);
  printf("Font #%d character count : %d\n", font, count);
  for (i = 0; i < count; i++)
    printf("Font #%d char #%d: %s\n", font, i,
	   (char*)glcGetFontListc(font, GLC_CHAR_LIST, i));

  printf("Font #%d is fixed pitch : %s\n", font,
	 glcGetFonti(font, GLC_IS_FIXED_PITCH) ? "YES" : "NO");
  printf("Font #%d max mapped code : %d\n", font,
	 glcGetFonti(font, GLC_MAX_MAPPED_CODE));
  printf("Font #%d min mapped code : %d\n", font,
	 glcGetFonti(font, GLC_MIN_MAPPED_CODE));

  printf("Font count : %d\n", glcGeti(GLC_FONT_COUNT));
  printf("Current font count : %d\n", glcGeti(GLC_CURRENT_FONT_COUNT));
  printf("Font #%d : %d\n", 1, glcGetListi(GLC_FONT_LIST, 1));
  printf("Current Font #%d : %d\n", 1, glcGetListi(GLC_CURRENT_FONT_LIST, 1));

  printf("Font Map #%d : %s\n", 65, (char *)glcGetFontMap(font, 65));
  printf("Font Map #%d : %s\n", 92, (char *)glcGetFontMap(font, 92));

  /* The characters maps are given in disorder in order to test the insertion
   * algorithm in glcFontMap()
   */
  glcFontMap(font, 87, "LATIN CAPITAL LETTER A");
  glcFontMap(font, 90, "LATIN CAPITAL LETTER D");
  glcFontMap(font, 89, "LATIN CAPITAL LETTER C");
  glcFontMap(font, 88, "LATIN CAPITAL LETTER B");

  /* Check that the characters are correctly mapped */
  for (i = 87; i < 91; i++)
    printf("Font Map #%d : %s\n", i, (char *)glcGetFontMap(font, i));

  /* The character code 88 is removed from the font map */
  glcFontMap(font, 88, NULL);
  printf("Character 88 removed from the font map\n");

  /* Check that the remaining characters in the font map are still correctly
   * mapped.
   */
  for (i = 87; i < 91; i++)
    printf("Font Map #%d : %s\n", i, (char *)glcGetFontMap(font, i));

  i = 1;
  last = glcGetMasteri(i, GLC_CHAR_COUNT);
  printf("Master #%d\n", i);
  printf("- Vendor : %s\n", (char *)glcGetMasterc(i, GLC_VENDOR));
  printf("- Format : %s\n", (char *)glcGetMasterc(i, GLC_MASTER_FORMAT));
  printf("- Face count : %d\n", glcGetMasteri(i, GLC_FACE_COUNT));
  printf("- Family : %s\n", (char *)glcGetMasterc(i, GLC_FAMILY));
  printf("- Min Mapped Code : %d\n", glcGetMasteri(i,GLC_MIN_MAPPED_CODE));
  printf("- Max Mapped Code : %d\n", glcGetMasteri(i,GLC_MAX_MAPPED_CODE));
  printf("- Char Count : %d\n", last);
  printf("- Last Character : %s\n", 
	 (char *)glcGetMasterListc(i, GLC_CHAR_LIST, last-1));

  glcDeleteFont(font);
  glcRemoveCatalog(1);

  return 0;
}
