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

#include "trackball.h"

static float scale;
static int width;
static int height;
static float curquat[4], lastquat[4];
static int moving = 0;
static int beginx, beginy;
static int scaling;
static GLboolean with_dlist;
static int menu, menuPolygon, menuRender;
static GLenum polygonMode = GL_FILL;
static GLint renderStyle = GLC_TRIANGLE;
static int frame = 0, msec = 0;
static GLboolean with_light;
static GLbitfield clearMode = GL_COLOR_BUFFER_BIT;

/* Forward declarations. */
static void check_glc_error(char* routine_name);
static void draw_letters(void);
static void access_font();

static void 
draw_letters(void)
{

   /* Draws each of the letters at a particular location */
   /* with a particular rotation in a particular color.  */

   unsigned int i;

   char* letters = "OpenGL";

   GLfloat colors[6][4] = {
      /* R, G, B */
      { 1.0, 0.0, 0.0, 1.0 },
      { 0.0, 1.0, 0.0, 1.0 },
      { 0.0, 0.0, 1.0, 1.0 },
      { 1.0, 0.0, 0.0, 1.0 },
      { 1.0, 1.0, 0.0, 1.0 },
      { 1.0, 0.0, 1.0, 1.0 }
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

   for (i=0; i<strlen(letters); i++) {

      glPushMatrix(); {

         glTranslatef(origin[0], origin[1], origin[2]);
         glTranslatef(pos[i][0], pos[i][1], pos[i][2]);
         glRotatef(pos[i][3], 1.0, 0.0, 0.0);
         glRotatef(pos[i][4], 0.0, 1.0, 0.0);
         glRotatef(pos[i][5], 0.0, 0.0, 1.0);

         glColor4fv(colors[i]);
         glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colors[i]);

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

   GLint glc_context;

   GLint master_count;
   const char* master_name = "Times";
   GLint master;

   GLint face_count;
   const char* face_name   = "Bold";
   static GLint glc_font_id;

   GLint result;
   int i, j;

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
               if (!strcmp((const char*)glcGetMasterListc(i, GLC_FACE_LIST, j),
			   face_name)) {
                  master = i;
               }
            }
         }
      }

      if (i < master_count) {
        /* Access the font family. */
        result = glcNewFontFromFamily(glc_font_id, 
                                      glcGetMasterc(master, GLC_FAMILY));
        check_glc_error("glcNewFontFromFamily()");
        if (result == glc_font_id) {
          /* Use the font. */
          glcFont(glc_font_id);
          check_glc_error("glcFont()");

          /* Use the face. */
          glcFontFace(glc_font_id, face_name);
          check_glc_error("glcFontFace()");
        }
      }
#if 0
      /* This only speeds up immediate mode rendering.              */
      /* Don't do this when compiling your own display list because */
      /* the polygons will go into the GLC internal display lists   */
      /* rather than the display list you are trying to construct.  */
      glcEnable(GLC_GL_OBJECTS);
      check_glc_error("glcEnable(GLC_GL_OBJECTS)");
      with_dlist = GL_TRUE;
#else
      glcDisable(GLC_GL_OBJECTS);
      check_glc_error("glcDisable(GLC_GL_OBJECTS)");
      with_dlist = GL_FALSE;
#endif

      /* Draw as polygons for smooth rotation & zoom. */
      glcRenderStyle(GLC_TRIANGLE);
      check_glc_error("glcRenderStyle(GLC_TRIANGLE)");
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
do_persp(GLboolean use_jitter, GLdouble jx, GLdouble jy)
{
   GLdouble dx, dy;
   int w;
   int h;
   GLdouble aspect;
   GLfloat m[4][4];

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
   aspect = (GLdouble)w/(GLdouble)h;
   gluPerspective(40., aspect, 1.0, 300.0);

   /* Now determine where to draw things. */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(dx, dy, 150.0,  /* eye is at (0,0,30) */
     dx, dy, 0.0,      /* center is at (0,0,0) */
     0.0, 1.0, 0.);      /* up is in positive Y direction */
   build_rotmatrix(m, curquat);
   glMultMatrixf(&m[0][0]);
   glScalef(scale, scale, scale);
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
  (void)x;
  (void)y;

   switch (key) {

   case 27:    /* Esc - Quits the program. */
      exit(1);
      break;

   default:
      break;
   } 
}

void
animate(void)
{
  add_quats(lastquat, curquat, curquat);
  glutPostRedisplay();
}

void
my_mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    glutIdleFunc(NULL);
    moving = 1;
    beginx = x;
    beginy = y;
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
      scaling = 1;
    } else {
      scaling = 0;
    }
  }
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    moving = 0;
    glutIdleFunc(animate);
  }
}

void
my_motion(int x, int y)
{
  if (scaling) {
    scale = scale * (1.0 + (((float) (beginy - y)) / height));
    beginx = x;
    beginy = y;
    glutPostRedisplay();
    return;
  }
  if (moving) {
    trackball(lastquat,
      (2.0 * beginx - width) / width,
      (height - 2.0 * beginy) / height,
      (2.0 * x - width) / width,
      (height - 2.0 * y) / height
      );
    beginx = x;
    beginy = y;
    animate();
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
   int duration;

#ifdef BLACK_LETTERS_WHITE_BACKGROUND
   glClearColor(1.0, 1.0, 1.0, 0.0);
#else
   glClearColor(0.0, 0.0, 0.0, 0.0);
#endif

#ifdef ANTIALIASING
   glClear(GL_ACCUM_BUFFER_BIT);

   for (i=0; i<passes; i++) {
      glClear(clearMode);
      do_persp(GL_TRUE, j8[i].x, j8[i].y);
      draw_letters(scale);
      glAccum(GL_ACCUM, 1.0/(GLfloat)passes);
   }
   glAccum(GL_RETURN, 1.0);
   glFlush();
#else
   glClear(clearMode);
   do_persp(GL_FALSE, 0.0, 0.0);
   draw_letters();
#endif

   glutSwapBuffers();
   frame++;
   duration = glutGet(GLUT_ELAPSED_TIME) - msec;
   if (duration > 5000) {
      msec += duration;
      printf("%f fps\n", (float)frame*1000./(float)duration);
      frame = 0;
   }
}

void my_menu(int);
void my_menu_polygon(int);
void my_menu_render(int);

void
create_menu(void)
{
   menuPolygon = glutCreateMenu(my_menu_polygon);
   if (polygonMode == GL_POINT)
      glutAddMenuEntry("+Point", 201);
   else
      glutAddMenuEntry(" Point", 201);
   if (polygonMode == GL_LINE)
      glutAddMenuEntry("+Line", 202);
   else
      glutAddMenuEntry(" Line", 202);
   if (polygonMode == GL_FILL)
      glutAddMenuEntry("+Fill", 203);
   else
      glutAddMenuEntry(" Fill", 203);
   if (polygonMode == GLC_EXTRUDE_QSO)
      glutAddMenuEntry("+Extrude", 204);
   else
      glutAddMenuEntry(" Extrude", 204);

   menuRender = glutCreateMenu(my_menu_render);
   if (renderStyle == GLC_TEXTURE)
      glutAddMenuEntry("+Texture", 301);
   else
      glutAddMenuEntry(" Texture", 301);
   if (renderStyle == GLC_LINE)
      glutAddMenuEntry("+Line", 302);
   else
      glutAddMenuEntry(" Line", 302);
   if (renderStyle == GLC_TRIANGLE)
      glutAddMenuEntry("+Triangle", 303);
   else
      glutAddMenuEntry(" Triangle", 303);

   menu = glutCreateMenu(my_menu);
   if (with_dlist)
      glutAddMenuEntry("Disable display lists", 101);
   else
      glutAddMenuEntry("Enable display lists", 101);
   if (with_light)
      glutAddMenuEntry("Disable light", 102);
   else
      glutAddMenuEntry("Enable light", 102);
   glutAddSubMenu("Render style", menuRender);
   if (renderStyle == GLC_TRIANGLE)
      glutAddSubMenu("Polygon mode", menuPolygon);
   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void
my_menu_polygon(int value)
{
   glcDisable(GLC_EXTRUDE_QSO);
   clearMode = GL_COLOR_BUFFER_BIT;
   glDisable(GL_DEPTH_TEST);
   switch(value) {
   case 201:
      polygonMode = GL_POINT;
      clearMode = GL_COLOR_BUFFER_BIT;
      glDisable(GL_DEPTH_TEST);
      break;
   case 202:
      polygonMode = GL_LINE;
      clearMode = GL_COLOR_BUFFER_BIT;
      glDisable(GL_DEPTH_TEST);
      break;
   case 203:
      polygonMode = GL_FILL;
      clearMode = GL_COLOR_BUFFER_BIT;
      glDisable(GL_DEPTH_TEST);
      break;
   case 204:
      polygonMode = GL_FILL;
      glcEnable(GLC_EXTRUDE_QSO);
      clearMode = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
      glEnable(GL_DEPTH_TEST);
      break;
   }
   glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
   if (value == 204)
      polygonMode = GLC_EXTRUDE_QSO;
   glutDestroyMenu(menu);
   glutDestroyMenu(menuPolygon);
   glutDestroyMenu(menuRender);
   create_menu();
}

void
my_menu_render(int value)
{
   switch(value) {
   case 301:
      renderStyle = GLC_TEXTURE;
      polygonMode = GL_FILL;
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      clearMode = GL_COLOR_BUFFER_BIT;
      glDisable(GL_DEPTH_TEST);
      break;
   case 302:
      renderStyle = GLC_LINE;
      polygonMode = GL_FILL;
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      clearMode = GL_COLOR_BUFFER_BIT;
      glDisable(GL_DEPTH_TEST);
      break;
   case 303:
      renderStyle = GLC_TRIANGLE;
      if (glcIsEnabled(GLC_EXTRUDE_QSO)) {
	clearMode = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
	glEnable(GL_DEPTH_TEST);
      }
      else {
	clearMode = GL_COLOR_BUFFER_BIT;
	glDisable(GL_DEPTH_TEST);
      }
      break;
   }
   glutDestroyMenu(menu);
   glutDestroyMenu(menuPolygon);
   glutDestroyMenu(menuRender);
   create_menu();
   glcRenderStyle(renderStyle);
   if (!with_dlist)
     glcDeleteGLObjects();
   glutPostRedisplay();
}

void
my_menu(int value)
{
   switch(value) {
   case 101:
      with_dlist = !with_dlist;
      break;
   case 102:
      with_light =!with_light;
      if (with_light) {
	 glEnable(GL_NORMALIZE);
	 glEnable(GL_LIGHTING);
	 glEnable(GL_LIGHT0);
      }
      else {
	 glDisable(GL_NORMALIZE);
	 glDisable(GL_LIGHTING);
	 glDisable(GL_LIGHT0);
      }
     break;
   }
   glutDestroyMenu(menu);
   glutDestroyMenu(menuPolygon);
   glutDestroyMenu(menuRender);
   create_menu();
   if (!with_dlist) {
     glcDeleteGLObjects();
     glcDisable(GLC_GL_OBJECTS);
   }
   else
     glcEnable(GLC_GL_OBJECTS);
   glutPostRedisplay();
}

int 
main(int argc, char **argv)
{

   glutInit(&argc, argv);
#ifdef ANTIALIASING
   glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_ACCUM|GLUT_DEPTH);
#else
   glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
#endif

   trackball(curquat, 0.0, 0.0, 0.0, 0.0);
   trackball(lastquat, 0.001, 0.0, 0.0, 0.001);

   glutCreateWindow("QuesoGLC Demo");

   if (argc > 1) {
      sscanf(argv[1], "%f", &scale);
   }
   else {
      scale = 1.0;
   }

   glutDisplayFunc(my_display);
   glutReshapeFunc(my_reshape);
   glutKeyboardFunc(my_handle_key);
   glutMouseFunc(my_mouse);
   glutMotionFunc(my_motion);
   glutIdleFunc(animate);

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);
   with_light = GL_FALSE;

   create_menu();
   access_font();

   msec = glutGet(GLUT_ELAPSED_TIME);

   glutMainLoop();

   return 0;
}
