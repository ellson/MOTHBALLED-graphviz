/* 
   glutfonts.c - Rendering GLUT's built-in fonts. 

   cc -o glutfonts glutfonts.c -lglut -lGLU -lGL -lXmu -lX11 -lm
*/

/* 
   Copyright (c) Gerard Lanois, 1998.

   This program is freely distributable without licensing fees and is
   provided without guarantee or warrantee expressed or implied. This
   program is -not- in the public domain. 
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "glut.h"

typedef enum {
   MODE_BITMAP,
   MODE_STROKE
} mode_type;

static mode_type mode;
static int font_index;

void
print_bitmap_string(void* font, char* s)
{
   if (s && strlen(s)) {
      while (*s) {
         glutBitmapCharacter(font, *s);
         s++;
      }
   }
}

void
print_stroke_string(void* font, char* s)
{
   if (s && strlen(s)) {
      while (*s) {
         glutStrokeCharacter(font, *s);
         s++;
      }
   }
}
void 
my_init()
{
   mode = MODE_BITMAP;
   font_index = 0;
}

void 
my_reshape(int w, int h)
{
   GLdouble size;
   GLdouble aspect;

   /* Use the whole window. */
   glViewport(0, 0, w, h);

   /* We are going to do some 2-D orthographic drawing. */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   size = (GLdouble)((w >= h) ? w : h) / 2.0;
   if (w <= h) {
      aspect = (GLdouble)h/(GLdouble)w;
      glOrtho(-size, size, -size*aspect, size*aspect, -100000.0, 100000.0);
   }
   else {
      aspect = (GLdouble)w/(GLdouble)h;
      glOrtho(-size*aspect, size*aspect, -size, size, -100000.0, 100000.0);
   }

   /* Make the world and window coordinates coincide so that 1.0 in */
   /* model space equals one pixel in window space.                 */
   glScaled(aspect, aspect, 1.0);

   /* Now determine where to draw things. */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

}

void 
my_handle_key(GLubyte key, GLint x, GLint y)
{
   switch (key) {

   case 27:    /* Esc - Quits the program. */
      printf("done.\n");
      exit(1);
      break;

   case ' ':    /* Space - toggles mode.     */
      mode = (mode == MODE_BITMAP) ? MODE_STROKE : MODE_BITMAP;
      font_index = 0;
      glutPostRedisplay();
      break;

   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
      if (mode == MODE_BITMAP || key == '1' || key == '2') {
        font_index = key - '1';
      }
      glutPostRedisplay();
      break;

   default:
      break;
   } 
}

void
draw_stuff()
{
   char string[8][256];
   unsigned int i, j;
   unsigned int count;

   void* bitmap_fonts[7] = {
      GLUT_BITMAP_9_BY_15,
      GLUT_BITMAP_8_BY_13,
      GLUT_BITMAP_TIMES_ROMAN_10,
      GLUT_BITMAP_TIMES_ROMAN_24,
      GLUT_BITMAP_HELVETICA_10,
      GLUT_BITMAP_HELVETICA_12,
      GLUT_BITMAP_HELVETICA_18     
   };

   char* bitmap_font_names[7] = {
      "GLUT_BITMAP_9_BY_15",
      "GLUT_BITMAP_8_BY_13",
      "GLUT_BITMAP_TIMES_ROMAN_10",
      "GLUT_BITMAP_TIMES_ROMAN_24",
      "GLUT_BITMAP_HELVETICA_10",
      "GLUT_BITMAP_HELVETICA_12",
      "GLUT_BITMAP_HELVETICA_18"     
   };

   void* stroke_fonts[2] = {
      GLUT_STROKE_ROMAN,
      GLUT_STROKE_MONO_ROMAN
   };

   void* stroke_font_names[2] = {
      "GLUT_STROKE_ROMAN",
      "GLUT_STROKE_MONO_ROMAN"
   };

   GLfloat x, y, ystep, yild, stroke_scale;
   count = 0;
    my_init();


   /* Set up some strings with the characters to draw. */
   for (i=1; i < 32; i++) { /* Skip zero - it's the null terminator! */
      string[0][count] = i;
      count++;
   }
   string[0][count] = '\0';

   for (i=32; i < 64; i++) {
      string[1][count] = i;
      count++;
   }
   string[1][count] = '\0';

   count = 0;
   for (i=64; i < 96; i++) {
      string[2][count] = i;
      count++;
   }
   string[2][count] = '\0';

   count = 0;
   for (i=96; i < 128; i++) {
      string[3][count] = i;
      count++;
   }
   string[3][count] = '\0';

   count = 0;
   for (i=128; i < 160; i++) {
      string[4][count] = i;
      count++;
   }
   string[4][count] = '\0';

   count = 0;
   for (i=160; i < 192; i++) {
      string[5][count] = i;
      count++;
   }
   string[5][count] = '\0';

   count = 0;
   for (i=192; i < 224; i++) {
      string[6][count] = i;
      count++;
   }
   string[6][count] = '\0';

   count = 0;
   for (i=224; i < 256; i++) {
      string[7][count] = i;
      count++;
   }
   string[7][count] = '\0';


   /* Draw the strings, according to the current mode and font. */
   glColor4f(0.0, 1.0, 0.0, 1);
   x = -225.0;
   y = 70.0;
   ystep  = 500.0;
   yild   = 20.0;
   if (mode == MODE_BITMAP) {
      glRasterPos2f(-150, y+1.25*yild);
	  print_bitmap_string(bitmap_fonts[4],"Smyrna Graph Visualizer ver 0.99");
   }
   else {
      stroke_scale = 0.1f;
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix(); {
         glTranslatef(x, y+1.25*yild, 0.0);
         glScalef(stroke_scale, stroke_scale, stroke_scale);
         print_stroke_string(
            stroke_fonts[0], "Smyrna Graphviz Visualizer Font test 001");
      } glPopMatrix();
   }
}

void 
my_display(void)
{

   /* Clear the window. */
   glClearColor(1.0, 1.0, 1.0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT);

   draw_stuff();

   glutSwapBuffers();
}

int 
__main(int argc, char **argv)
{

   glutInitWindowSize(500, 250);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);

   glutCreateWindow("GLUT fonts");

   my_init();

   glutDisplayFunc(my_display);
   glutReshapeFunc(my_reshape);
   glutKeyboardFunc(my_handle_key);

   glutMainLoop();

   return 0;
}






















