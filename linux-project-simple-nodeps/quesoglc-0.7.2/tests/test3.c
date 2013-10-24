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
/* $Id: test3.c 679 2007-12-20 21:42:13Z bcoconni $ */

/** \file
 * The purpose of this test is to check that a context can be bound to only
 * one thread at a time. This test checks that if a context is current to
 * a thread other than the issuing thread then a GLC_STATE_ERROR is raised.
 * Pending deletion of contexts is also tested.
 */

#include "GL/glc.h"
#include <pthread.h>
#include <stdio.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLint ctx;
int magic = 0xdeadbeef;

void* da_thread(void *arg)
{
  GLCenum err;

  glutCreateWindow("test3.1");

  glcContext(ctx);
  err = glcGetError();
  if (err != GLC_STATE_ERROR) {
    printf("Thread 2 : Unexpected error : 0x%X\n", (int)err);
    return &magic;
  }

  /* Ask GLC to delete this context as soon as it is not current any thread */
  glcDeleteContext(ctx);
  err = glcGetError();
  if (err) {
    printf("Thread 2 : Unexpected error : 0x%X\n", (int)err);
    return &magic;
  }
  /* Check that the context has not been deleted yet. */
  if (!glcIsContext(ctx)) {
    printf("Thread 2 : Unexpected deletion of context %d\n", (int)ctx);
    return &magic;
  }

  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t thread;
  GLCenum err;
  void *return_value = NULL;
  GLint ctx2;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("test3");

  ctx2 = glcGenContext();
  ctx = glcGenContext();
  glcContext(ctx);
  err = glcGetError();
  if (err) {
    printf("Main Thread : Unexpected error : 0x%X\n", (int)err);
    return -1;
  }

  if (pthread_create(&thread, NULL, da_thread, NULL)) {
    printf("Main Thread : Failed to create pthread\n");
    return -1;
  }

  if (pthread_join(thread, &return_value)) {
    printf("Main Thread : Failed to join Thread 2\n");
    return -1;
  }

  if (return_value) {
    printf("Main Thread : An error occured in Thread 2\n");
    return -1;
  }

  /* Check that although the thread has requested a deletion of the context,
   * the context 'ctx' still exists.
   */
  if (!glcIsContext(ctx)) {
    printf("Main Thread : Unexpected deletion of context 'ctx'\n");
    return -1;
  }

  /* Change the current context. The pending deletion should be executed
   * at this step.
   */
  glcContext(ctx2);
  /* Verify that the context has been deleted */
  if (glcIsContext(ctx)) {
    printf("Main Thread : Pending deletion of context 'ctx' has not been executed\n");
    return -1;
  }

  /* Same as above (pending deletion of a context) but a little different :
   * - The deletion is now requested in the thread that owns the context
   * - glcContext(0) is called which means that the current context will be
   *   released and no other context will be made current (hence a different
   *   branch of code will be executed in glcContext().
   */
  glcDeleteContext(ctx2);
  err = glcGetError();
  if (err) {
    printf("Main Thread : Unexpected GLC error 0x%x\n", (int)err);
    return -1;
  }

  /* Release the current context */
  glcContext(0);
  /* Check that 'ctx2' has been deleted */
  if (glcIsContext(ctx2)) {
    printf("Main Thread : Pending deletion of context 'ctx2' has not been executed\n");
    return -1;
  }

  printf("Test successful!\n");
  return 0;
}
