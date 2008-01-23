/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

/*
	this code is used to set up a opngl window and set
	some basic features (panning zooming and rotating)
	Viewport.h provides a higher level control such as drawing primitives
*/
#ifndef GL_TEMPLATE_H
#define GL_TEMPLATE_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>							// Header File For The GLu32 Library
// #include <GL/glaux.h>	
#include "materials.h"

#include "viewport.h"

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>
#include <gdk/gdkcursor.h>

// #include "guifunctions.h"
#include "trackball.h"
#include "cgraph.h"
#include "xdot.h"
#include <glTexFont.h>


#define DIG_2_RAD (G_PI / 180.0)
#define RAD_2_DIG (180.0 / G_PI)
#define ANIMATE_THRESHOLD 25.0
#define VIEW_SCALE_MAX 2.0
#define VIEW_SCALE_MIN 0.5
#define NUM_SHAPES 9
static float DEG2RAD = 3.14159/180;
static gboolean animate = FALSE;
static void toggle_animation (GtkWidget *widget);

// solid shapes
static const GLuint shape_cube         = 0;
static const GLuint shape_sphere       = 1;
static const GLuint shape_cone         = 2;
static const GLuint shape_torus        = 3;
static const GLuint shape_tetrahedron  = 4;
static const GLuint shape_octahedron   = 5;
static const GLuint shape_dodecahedron = 6;
static const GLuint shape_icosahedron  = 7;
static const GLuint shape_teapot       = 8;
//2D shapes
static const GLuint shape_dot		   = 9;
static const GLuint shape_polygon      = 10;
static const GLuint shape_ellipse      = 11;
static const GLuint shape_spline       = 12;
static const GLuint shape_text         = 13;
static GLuint shape_list_base = 0;
static GLuint shape_current = 8;
static MaterialProp *mat_current = &mat_silver;
static float view_quat_diff[4] = { 0.0, 0.0, 0.0, 1.0 };
static float view_quat[4] = { 0.0, 0.0, 0.0, 1.0 };
static float view_scale = 1.0;
//mouse modes
#define MM_PAN					0
#define MM_ZOOM					1
#define MM_ROTATE				2
#define MM_SINGLE_SELECT		3
#define MM_RECTANGULAR_SELECT	4
#define MM_RECTANGULAR_X_SELECT	5
#define MM_MOVE					10
#define MM_MAGNIFIER			20
#define MM_FISHEYE_MAGNIFIER	21

void loadDLL (char* file);
extern xdot* testxdot;
extern GtkWidget *drawing_area;

static void init_view (void);
void examine_gl_config_attrib (GdkGLConfig *glconfig);
void print_gl_config_attrib (GdkGLConfig *glconfig,const gchar *attrib_str,int attrib,gboolean is_boolean);
static void realize (GtkWidget *widget,gpointer   data);
static gboolean configure_event (GtkWidget *widget,GdkEventConfigure *event, gpointer data);
gboolean expose_event (GtkWidget *widget,GdkEventExpose *event,gpointer data);


static gboolean button_press_event (GtkWidget *widget,GdkEventButton *event,gpointer data);
static gboolean button_release_event (GtkWidget *widget,GdkEventButton *event,gpointer data);
static gboolean motion_notify_event (GtkWidget *widget,GdkEventMotion *event,gpointer data);
static gboolean key_press_event (GtkWidget *widget, GdkEventKey *event,gpointer data);
static gboolean idle (GtkWidget *widget);

static void idle_add (GtkWidget *widget);
static void idle_remove (GtkWidget *widget);
static gboolean map_event (GtkWidget *widget,GdkEvent *event,gpointer   data);
static gboolean unmap_event (GtkWidget *widget,GdkEvent  *event,gpointer data);
static gboolean visibility_notify_event (GtkWidget *widget,GdkEventVisibility *event,gpointer data);
static void toggle_animation (GtkWidget *widget);
static void change_shape (GtkMenuItem  *menuitem,const GLuint *shape);
//static void change_material (GtkMenuItem *menuitem,MaterialProp *mat);
void switch_Mouse (GtkMenuItem *menuitem,int mouse_mode);
static gboolean button_press_event_popup_menu (GtkWidget *widget,GdkEventButton *event,gpointer data);
static GtkWidget *create_popup_menu (GtkWidget *drawing_area);
extern 
GdkGLConfig *configure_gl (void);
void create_window (GdkGLConfig *glconfig,GtkWidget* vbox);
int GetOGLPos(int x, int y);//this piece of code returns the opengl coordinates of mouse coordinates
int GetFixedOGLPos(int x, int y,float kts);
int GetOGLPosRef(int x, int y,float* X,float* Y,float* Z);
float GetOGLDistance(int l);
void loadDLL (char* file);	//for loading plugins , requires ltdl (libtool)
void DrawView();//draws view

#endif
