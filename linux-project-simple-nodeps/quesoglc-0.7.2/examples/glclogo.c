/* 
   glclogo.c - Rendering Adobe Type1 fonts via GLC. 

   cc -o glclogo glclogo.c -lglut -lGLC -lGLU -lGL -lXmu -lX11 -lm
*/

/* 
   Copyright (c) Gerard Lanois, 1998.

   This program is freely distributable without licensing fees and is
   provided without guarantee or warrantee expressed or implied. This
   program is -not- in the public domain.

   Fixes by Bertrand Coconnier for compilation with gcc :
   - Added #include <stdlib.h> in order to declare exit()
   - Changed the type of main() to int (otherwise gcc complains)
   - Added 'return 0;' at the end of main() in order to be compliant with
     its int type.
   - Removed the unused function my_init()
   Fixes by Brandon Casey :
   - GLC_GL_OBJECTS is enabled by default, it should be disabled here since
     display lists are created by the program.
*/

#include <GL/glc.h>
#if defined __APPLE__ && defined __MACH__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <stdio.h>            /* fprintf(), stderr            */
#include <string.h>           /* strcmp(), strcpy(), strlen() */
#include <stdlib.h>           /* exit()                       */

static float scale;
static int width;
static int height;

/* Forward declarations. */
static void check_glc_error(char* routine_name);
static void draw_letters(float scale);
static void access_font();

void
draw_logo(float scale)
{
   /* Draws the unofficial OpenGL logo using GLC, the OpenGL */
   /* character rendering library.                           */

   static GLboolean have_dlist = GL_FALSE;
   static GLuint dlist;

   access_font();

   if (have_dlist == GL_FALSE) {

      /* Compile the graphics into a display list. */
      dlist = glGenLists(1);
      if (!glIsList(dlist)) {
         fprintf(stderr, "glGenLists() failed.\n");
      }
      else {

         glNewList(dlist, GL_COMPILE); {

            draw_letters(scale);

         } glEndList();

         have_dlist = GL_TRUE;
      }
   }

   /* Draw everything by calling the display list. */
   if (glIsList(dlist)) {
      glCallList(dlist);
   }

}

static void 
draw_letters(float scale)
{

   /* Draws each of the letters at a particular location */
   /* with a particular rotation in a particular color.  */

   unsigned int i;

   char* letters = "OpenGL";

   GLfloat colors[6][3] = {
      /* R, G, B */
      { 1.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 1.0 },
      { 1.0, 0.0, 0.0 },
      { 1.0, 1.0, 0.0 },
      { 1.0, 0.0, 1.0 }
   };

   GLfloat origin[3] = { -50.0, -15.0, 0.0 };

   GLfloat pos[6][6] = {
      /*   x,    y,   z,  xr,  yr, zr(deg) */
      {  0.0,  0.0, 0.0, 0.0, 0.0, 22.0 },  /* O */
      { 18.0, 12.0, 0.0, 0.0, 0.0, 22.0 },  /* p */
      { 30.0,  4.0, 0.0, 0.0, 0.0,  5.0 },  /* e */
      { 42.0, 15.0, 0.0, 0.0, 0.0, 15.0 },  /* n */
      { 65.0,  0.0, 0.0, 0.0, 0.0, 27.0 },  /* G */
      { 83.0,  5.0, 0.0, 0.0, 0.0, -4.0 }   /* L */
   };


   /* Draw letters as filled polygons. */
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   for (i=0; i<strlen(letters); i++) {

      glPushMatrix(); {

         glScalef(scale, scale, scale);

         glTranslatef(origin[0], origin[1], origin[2]);
         glTranslatef(pos[i][0], pos[i][1], pos[i][2]);
         glRotatef(pos[i][3], 1.0, 0.0, 0.0);
         glRotatef(pos[i][4], 0.0, 1.0, 0.0);
         glRotatef(pos[i][5], 0.0, 0.0, 1.0);

         glColor3fv(colors[i]);

         /* This is a "reasonable" font scaling - big enough to */
         /* see the letters when the overall scale is 1.0.      */
         glScalef(30.0, 30.0, 30.0);

         glcRenderChar(letters[i]);
         check_glc_error("glcRenderChar()");

      } glPopMatrix();
   }

}

static void
access_font()
{

   /* This routine contains all the necessary steps to setup a */
   /* font master and a face.  Subsequent glcRenderChar() and  */
   /* glcRenderString() calls will draw the characters in the  */
   /* specified font/face.                                     */

   static GLboolean have_context = GL_FALSE;
   GLint glc_context;

   GLint master_count;
#ifdef _WIN32_
   const char* master_name = "Times New Roman";
#else
   const char* master_name = "Times";
#endif
   GLint master;

   GLint face_count;
   const char* face_name   = "Bold";
   static GLint glc_font_id;

   GLint result;
   unsigned int i, j;

   if (have_context == GL_FALSE) {

      /* Only get a context once.                                           */
      /* When using SGI's implementation of GLC, don't call glcGetError()   */
      /* after  this  glcGenContext()  call  because  it  always  returns   */
      /* GLC_STATE_ERROR. That's probably a bug in SGI's GLC. This behavior */
      /* doesn't occur in QuesoGLC.                                         */ 
      /* So, in order to let glclogo run with both implementations, just    */
      /* validate the context. If it's OK, then go for it.                  */
      glc_context = glcGenContext();
      if (!glcIsContext(glc_context)) {
         fprintf(stderr, "Error - glcGenContext() failed.  Exiting.\n");
         exit(-1);
      }
      else {

         /* Context is valid.  Make it the current context. */
         glcContext(glc_context);
         check_glc_error("glcContext()");
         have_context = GL_TRUE;

         /* Get a unique font ID. */
         glc_font_id = glcGenFontID();

         /* Choose a master and a face. */
         master_count = glcGeti(GLC_MASTER_COUNT);
         master = 0; 
         for (i = 0; i < master_count; i++) {
            if (!strcmp((const char*)glcGetMasterc(i, GLC_FAMILY), 
                        master_name)) {
               face_count = glcGetMasteri(i, GLC_FACE_COUNT);
               for (j = 0; j < face_count; j++) {
                  if (!strcmp((const char*)glcGetMasterListc(i, GLC_FACE_LIST, j), face_name)) {
                     master = i;
                  }
               }
            }
         }
         
         /* Access the font family. */
         result = glcNewFontFromFamily(glc_font_id, 
                                       glcGetMasterc(master, GLC_FAMILY));
         check_glc_error("glcNewFontFromFamily()");
         if (result != glc_font_id) {
            fprintf(stderr, "Error - glcNewFontFromFamily() failed.  Exiting.\n");
            exit(-1);
         }
         else {

            /* Use the font. */
            glcFont(glc_font_id);
            check_glc_error("glcFont()");

            /* Use the face. */
            glcFontFace(glc_font_id, face_name);
            check_glc_error("glcFontFace()");

#if 0
            /* This only speeds up immediate mode rendering.              */
            /* Don't do this when compiling your own display list because */
            /* the polygons will go into the GLC internal display lists   */
            /* rather than the display list you are trying to construct.  */
            glcEnable(GLC_GL_OBJECTS);
            check_glc_error("glcEnable(GLC_GL_OBJECTS)");
#else
            glcDisable(GLC_GL_OBJECTS);
            check_glc_error("glcDisable(GLC_GL_OBJECTS)");
#endif

            /* Draw as polygons for smooth rotation & zoom. */
            glcRenderStyle(GLC_TRIANGLE);
            check_glc_error("glcRenderStyle(GLC_TRIANGLE)");
         }
      }
   }
}


static void
check_glc_error(char* routine_name)
{
   /* Utility routine to handle GLC errors. */

   GLboolean terminate = GL_TRUE;

   char* errname;
   switch (glcGetError()) {
   case GLC_NONE:
      errname = "GLC_NONE";
      terminate = GL_FALSE;
      break;
   case GLC_PARAMETER_ERROR:
      errname = "GLC_PARAMETER_ERROR";
      break;
   case GLC_RESOURCE_ERROR:
      errname = "GLC_RESOURCE_ERROR";
      break;
   case GLC_STATE_ERROR:
      errname = "GLC_STATE_ERROR";
      break;
   default:
      errname = "Unsupported Error Value";
      break;
   }

   if (terminate == GL_TRUE) {
      fprintf(stderr, "GLC ERROR - %s returned %s\n", routine_name, errname);
      exit(-1);
   }
}


static void
do_ortho(GLboolean use_jitter, GLdouble jx, GLdouble jy)
{
   GLdouble dx, dy;
   int w;
   int h;
   GLdouble size;
   GLdouble aspect;

   w = width;
   h = height;
   aspect = (GLdouble)w / (GLdouble)h;

   if (use_jitter == GL_FALSE) {
      dx = 0.0;
      dy = 0.0;
   }
   else {
      dx = -jx*3.0;
      dy = -jy*3.0;
   }

   /* Use the whole window. */
   glViewport(0, 0, w, h);

   /* We are going to do some 2-D orthographic drawing. */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   size = (GLdouble)((w >= h) ? w : h) / 2.0;
   if (w <= h) {
      aspect = (GLdouble)h/(GLdouble)w;
      glOrtho(-size+dx, size+dx, -size*aspect+dy, size*aspect+dy,
              -100000.0, 100000.0);
   }
   else {
      aspect = (GLdouble)w/(GLdouble)h;
      glOrtho(-size*aspect+dx, size*aspect+dx, -size+dy, size+dy,
              -100000.0, 100000.0);
   }

   /* Make the world and window coordinates coincide so that 1.0 in */
   /* model space equals one pixel in window space.                 */
   glScaled(aspect, aspect, 1.0);

   /* Now determine where to draw things. */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


void 
my_reshape(int w, int h)
{
   width  = w;
   height = h;
}

void 
my_handle_key(GLubyte key, GLint x, GLint y)
{
   switch (key) {

   case 27:    /* Esc - Quits the program. */
      exit(1);
      break;

   default:
      break;
   } 
}

#ifdef ANTIALIASING
typedef struct {
        GLdouble x, y;
} jitter_point_type;

static jitter_point_type j8[] =
{
        {-0.334818,  0.435331},
        { 0.286438, -0.393495},
        { 0.459462,  0.141540},
        {-0.414498, -0.192829},
        {-0.183790,  0.082102},
        {-0.079263, -0.317383},
        { 0.102254,  0.299133},
        { 0.164216, -0.054399}
};
#endif

void 
my_display(void)
{
#ifdef ANTIALIASING
   int passes = 8;
   int i;
#endif

#ifdef BLACK_LETTERS_WHITE_BACKGROUND
   glClearColor(1.0, 1.0, 1.0, 0.0);
#else
   glClearColor(0.0, 0.0, 0.0, 0.0);
#endif

#ifdef ANTIALIASING
   glClear(GL_ACCUM_BUFFER_BIT);

   for (i=0; i<passes; i++) {
      glClear(GL_COLOR_BUFFER_BIT);
      do_ortho(GL_TRUE, j8[i].x, j8[i].y);
      draw_logo(scale);
      glAccum(GL_ACCUM, 1.0/(GLfloat)passes);
   }
   glAccum(GL_RETURN, 1.0);
   glFlush();
#else
   glClear(GL_COLOR_BUFFER_BIT);
   do_ortho(GL_FALSE, 0.0, 0.0);
   draw_logo(scale);
#endif

   glutSwapBuffers();
}

int 
main(int argc, char **argv)
{

   glutInitWindowSize(800, 300);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGBA|GLUT_ACCUM|GLUT_DOUBLE);

   glutCreateWindow("OpenGL Logo");

   if (argc > 1) {
      sscanf(argv[1], "%f", &scale);
   }
   else {
      scale = 1.0;
   }

   glutDisplayFunc(my_display);
   glutReshapeFunc(my_reshape);
   glutKeyboardFunc(my_handle_key);

   glutMainLoop();

   return 0;
}
