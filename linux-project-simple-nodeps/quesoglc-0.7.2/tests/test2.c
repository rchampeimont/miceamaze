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
/* $Id: test2.c 679 2007-12-20 21:42:13Z bcoconni $ */

/** \file
 * Check GLC routines in a multithreaded app when no GL context has been bound
 */

#include "GL/glc.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#if defined __APPLE__ && defined __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

pthread_mutex_t mutex;
pthread_cond_t cond;

void* thread2(void *arg)
{
  int i;
  int ctx;

  /* Since the mutex is locked when this thread is created execution suspend
   * here waiting for the Main Thread to call pthread_cond_wait()
   */
  printf("Thread2 : Try to lock mutex\n");
  if (pthread_mutex_lock(&mutex)) {
    printf("Thread2 : Can't lock mutex\n");
    return NULL;
  }
  printf("Thread2 : Mutex locked\n");

  /* Immediately unlock the mutex so that the Main Thread can execute after
   * pthread_cond_wait() returns.
   */
  if (pthread_mutex_unlock(&mutex)) {
    printf("Thread2 : Can't unlock mutex\n");
    return NULL;
  }
  printf("Thread2 : Mutex unlocked\n");

  /* Send the signal to the Main Thread so that both begins synchronously to
   * generate contexts.
   */
  printf("Thread2 : sending signal\n");
  if (pthread_cond_signal(&cond)) {
    printf("Thread2 : Can't send signal\n");
    return NULL;
  }
  printf("Thread2 : signal sent\n");

  /* Generate 8 contexts */
  for (i=0;i<8;i++) {
    if (i==0)
      printf("Thread2 : request context creation\n");
    ctx = glcGenContext();
    printf("Thread2 : context %d created\n", ctx);
  }

  /* Wait for context 12 to be created.
   * Note that context 12 may have been created by the current thread
   */
  i = 0;
  while (!glcIsContext(12)) {
    i++;
    if (i>5000) {
      i = 0;
      printf("Thread2 : context 12 not yet created\n");
    }
  }
  printf("Thread2 : context 12 is created\n");

  printf("Thread2 : terminated\n");

  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t t2;
  int i;
  int ctx;

  /* Needed to initialize an OpenGL context */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("test2");

  /* Initialize the mutex and the condition variable */
  if (pthread_mutex_init(&mutex, NULL)) {
    printf("Main Thread : can't create mutex\n");
    return -1;
  }
  printf("Main Thread : Mutex created\n");

  if (pthread_cond_init(&cond, NULL)) {
    printf("Main Thread : can't initialize the condition variable\n");
    return -1;
  }
  printf("Main Thread : Condition variable created\n");

  /* Lock the mutex so that the 2nd Thread wait for the main thread to be ready
   */
  if (pthread_mutex_lock(&mutex)) {
    printf("Main Thread : can't lock mutex\n");
    return -1;
  }
  printf("Main Thread : Mutex locked\n");

  /* Create the 2nd thread */
  if (pthread_create(&t2, NULL, thread2, NULL)) {
    printf("Main Thread : can't create pthread\n");
    return -1;
  }
  printf("Main Thread : created Thread2\n");

  printf("Main Thread : wait for condition variable\n");
  /* Wait for the signal from Thread 2 */
  if (pthread_cond_wait(&cond, &mutex)) {
    printf("Main Thread : failed to wait for condition variable\n");
    return -1;
  }
  printf("Main Thread : Condition variable released\n");

  /* Generate 8 contexts */
  for (i=0; i<8; i++) {
    if (i==0)
      printf("Main thread : request context creation\n");
    ctx = glcGenContext();
    printf("Main Thread : context %d created\n", ctx);
  }

  glcContext(ctx);
  printf("Main Thread : current context %d\n", glcGetCurrentContext());

  /* Wait for context 12 to be created.
   * Note that context 12 may have been created by the current thread
   */
  i = 0;
  while (!glcIsContext(12)) {
    i++;
    if (i>5000) {
      i = 0;
      printf("Main Thread : context 12 not yet created\n");
    }
  }
  printf("Main Thread : context 12 is created\n");

  /* Destroy the mutex and the condition variable */
  if (pthread_cond_destroy(&cond)) {
    printf("Main Thread : Can't destroy condition variable\n");
    return -1;
  }
  printf("Main Thread : Condition variable destroyed\n");

  /* The mutex has been locked when pthread_cond_wait() was successful so we
   * have to unlock it
   */
  if (pthread_mutex_unlock(&mutex)) {
    printf("Main Thread : failed to unlock mutex\n");
  }

  if (pthread_mutex_destroy(&mutex)) {
    printf("Main Thread : Can't destroy mutex\n");
    return -1;
  }
  pthread_join(t2, NULL);
  printf("Main Thread : Mutex destroyed\n");
  printf("\nTests successful !\n");

  return 0;
}
