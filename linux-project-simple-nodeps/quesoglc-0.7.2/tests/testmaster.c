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
/* $Id: testmaster.c 679 2007-12-20 21:42:13Z bcoconni $ */

/** \file
 * Test the GLC functions of the master group. It can be used either :
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
  int ctx = 0;
  int numMasters = 0;
  int numFaces = 0;
  int numCatalogs = 0;
  int i = 0;
  int j = 0;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("testfont");

  ctx = glcGenContext();
  glcContext(ctx);

  numCatalogs = glcGeti(GLC_CATALOG_COUNT);
  printf("Catalogs : %d\n", numCatalogs);
  
  for (i = 0; i < numCatalogs; i++)
    printf("Catalog #%d : %s\n", i,
	   (const char *)glcGetListc(GLC_CATALOG_LIST, i));

  numMasters = glcGeti(GLC_MASTER_COUNT);
  printf("Masters : %d\n", numMasters);

  for (i = 0; i < numMasters; i++) {
    numFaces = glcGetMasteri(i, GLC_FACE_COUNT);
    printf("Master #%d\n", i);
    printf("- Vendor : %s\n", (char*)glcGetMasterc(i, GLC_VENDOR));
    printf("- Format : %s\n", (char*)glcGetMasterc(i, GLC_MASTER_FORMAT));
    printf("- Face count : %d\n", numFaces);
    printf("- Family : %s\n", (char*)glcGetMasterc(i, GLC_FAMILY));
    printf("- Version : %s\n", (char*)glcGetMasterc(i, GLC_VERSION));
    printf("- Is Fixed Pitch : %s\n", glcGetMasteri(i, GLC_IS_FIXED_PITCH) ?
	   "YES" : "NO");
    for (j = 0; j < numFaces; j++)
      printf("- Face #%d : %s\n", j, 
	     (char *)glcGetMasterListc(i, GLC_FACE_LIST, j));
    printf("- Master #%d maps 0x%X to %s\n", i, 65 + i,
	   (char *)glcGetMasterMap(i, 65 + i));
  }

  glcDeleteContext(ctx);
  return 0;
}
