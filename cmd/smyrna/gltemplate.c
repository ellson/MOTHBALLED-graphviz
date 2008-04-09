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

#include <stdlib.h>
#include <math.h>
#include "gui.h"
#include "viewport.h"
#include "topview.h"
#include "gltemplate.h"
#include "glutils.h"
#include "glexpose.h"
#include "glmenu.h"
#include "selection.h"
#include "glcompset.h"
#include "viewportcamera.h"
static float begin_x = 0.0;
static float begin_y = 0.0;
static float dx = 0.0;
static float dy = 0.0;

/*
	test opengl parameters, configuration.Run this function to see machine's open gl capabilities
	params:gtk gl config class ,gtk takes care of all these tests
	return value:none
*/
void examine_gl_config_attrib(GdkGLConfig * glconfig)
{
    g_print("\nOpenGL visual configurations :\n\n");
    g_print("gdk_gl_config_is_rgba (glconfig) = %s\n",
	    gdk_gl_config_is_rgba(glconfig) ? "TRUE" : "FALSE");
    g_print("gdk_gl_config_is_double_buffered (glconfig) = %s\n",
	    gdk_gl_config_is_double_buffered(glconfig) ? "TRUE" : "FALSE");
    g_print("gdk_gl_config_is_stereo (glconfig) = %s\n",
	    gdk_gl_config_is_stereo(glconfig) ? "TRUE" : "FALSE");
    g_print("gdk_gl_config_has_alpha (glconfig) = %s\n",
	    gdk_gl_config_has_alpha(glconfig) ? "TRUE" : "FALSE");
    g_print("gdk_gl_config_has_depth_buffer (glconfig) = %s\n",
	    gdk_gl_config_has_depth_buffer(glconfig) ? "TRUE" : "FALSE");
    g_print("gdk_gl_config_has_stencil_buffer (glconfig) = %s\n",
	    gdk_gl_config_has_stencil_buffer(glconfig) ? "TRUE" : "FALSE");
    g_print("gdk_gl_config_has_accum_buffer (glconfig) = %s\n",
	    gdk_gl_config_has_accum_buffer(glconfig) ? "TRUE" : "FALSE");
    g_print("\n");
    print_gl_config_attrib(glconfig, "GDK_GL_USE_GL", GDK_GL_USE_GL, TRUE);
    print_gl_config_attrib(glconfig, "GDK_GL_BUFFER_SIZE",
			   GDK_GL_BUFFER_SIZE, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_LEVEL", GDK_GL_LEVEL, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_RGBA", GDK_GL_RGBA, TRUE);
    print_gl_config_attrib(glconfig, "GDK_GL_DOUBLEBUFFER",
			   GDK_GL_DOUBLEBUFFER, TRUE);
    print_gl_config_attrib(glconfig, "GDK_GL_STEREO", GDK_GL_STEREO, TRUE);
    print_gl_config_attrib(glconfig, "GDK_GL_AUX_BUFFERS",
			   GDK_GL_AUX_BUFFERS, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_RED_SIZE", GDK_GL_RED_SIZE,
			   FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_GREEN_SIZE",
			   GDK_GL_GREEN_SIZE, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_BLUE_SIZE", GDK_GL_BLUE_SIZE,
			   FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_ALPHA_SIZE",
			   GDK_GL_ALPHA_SIZE, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_DEPTH_SIZE",
			   GDK_GL_DEPTH_SIZE, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_STENCIL_SIZE",
			   GDK_GL_STENCIL_SIZE, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_ACCUM_RED_SIZE",
			   GDK_GL_ACCUM_RED_SIZE, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_ACCUM_GREEN_SIZE",
			   GDK_GL_ACCUM_GREEN_SIZE, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_ACCUM_BLUE_SIZE",
			   GDK_GL_ACCUM_BLUE_SIZE, FALSE);
    print_gl_config_attrib(glconfig, "GDK_GL_ACCUM_ALPHA_SIZE",
			   GDK_GL_ACCUM_ALPHA_SIZE, FALSE);
    g_print("\n");
}

/*
	test single opengl parameter, all visual , it doesnt return a value
	params:gtk gl config class , attribute name and id,if boolean expected send is_boolean true
	return value:none
*/
void print_gl_config_attrib(GdkGLConfig * glconfig,
			    const gchar * attrib_str,
			    int attrib, gboolean is_boolean)
{
    int value;

    g_print("%s = ", attrib_str);
    if (gdk_gl_config_get_attrib(glconfig, attrib, &value)) {
	if (is_boolean)
	    g_print("%s\n", value == TRUE ? "TRUE" : "FALSE");
	else
	    g_print("%d\n", value);
    } else
	g_print("*** Cannot get %s attribute value\n", attrib_str);
}

/*
	initialize the gl , run only once!!
	params:gtk opgn gl canvas and optional data pointer
	return value:none
*/
static void realize(GtkWidget * widget, gpointer data)
{

    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    /*smyrna does not use any ligthting affects but can be turned on for more effects in the future */
    GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat position[] = { 0.0, 3.0, 3.0, 0.0 };

    GLfloat lmodel_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat local_view[] = { 0.0 };

    char* smyrna_font;

#ifdef WIN32
#define SMYRNA_FONT "c:/arial.tga"
// #else
// using -DSMYRNA_FONT from Makefile.am and configure.ac
#endif

    if ((smyrna_font = smyrnaPath ("gui/arial.tga"))) {
	g_print("loading font....%i\n", fontLoad(smyrna_font));
	free (smyrna_font);
    }
    else g_print("loading font....%i\n", fontLoad(SMYRNA_FONT));

  /*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	return;

    glClearColor(view->bgColor.R, view->bgColor.G, view->bgColor.B, view->bgColor.A);	//background color
    glClearDepth(1.0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

    glFrontFace(GL_CW);
// glEnable (GL_LIGHTING);
//  glEnable (GL_LIGHT0);
//  glEnable (GL_AUTO_NORMAL);
//  glEnable (GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
//  glEnable(GL_LINE_SMOOTH);

    gdk_gl_drawable_gl_end(gldrawable);
  /*** OpenGL END ***/
    return;
}

/*
	set up GL for window size changes, run this when necesary (when window size or monitor resolution is changed)
	params:gtk opgn gl canvas , GdkEventConfigure object to retrieve window dimensions and custom data
	return value:true or false, fails (false) if cannot init gl
*/
static gboolean configure_event(GtkWidget * widget,
				GdkEventConfigure * event, gpointer data)
{
    int vPort[4];
    float aspect;
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
    view->w = widget->allocation.width;
    view->h = widget->allocation.height;
	/*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	return FALSE;
    glViewport(0, 0, view->w, view->h);
    /* get current viewport */
    glGetIntegerv(GL_VIEWPORT, vPort);
    /* setup various opengl things that we need */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (view->w > view->h) {
	aspect = (float) view->w / (float) view->h;
	glOrtho(-aspect * GL_VIEWPORT_FACTOR, aspect * GL_VIEWPORT_FACTOR, GL_VIEWPORT_FACTOR*-1, GL_VIEWPORT_FACTOR, -1500, 1500);
    } else {
	aspect = (float) view->h / (float) view->w;
	glOrtho(GL_VIEWPORT_FACTOR*-1, GL_VIEWPORT_FACTOR, -aspect * GL_VIEWPORT_FACTOR, aspect * GL_VIEWPORT_FACTOR, -1500, 1500);
    }

    glMatrixMode(GL_MODELVIEW);
    gdk_gl_drawable_gl_end(gldrawable);
	/*** OpenGL END ***/

    return TRUE;
}

/*
	expose function.real drawing takes place here, 
	params:gtk opgn gl canvas , GdkEventExpose object and custom data
	return value:true or false, fails (false) if cannot init gl
*/
gboolean expose_event(GtkWidget * widget, GdkEventExpose * event,
		      gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
	/*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	return FALSE;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glexpose_main(view);	//draw all stuff
    /* Swap buffers */
    if (gdk_gl_drawable_is_double_buffered(gldrawable))
	gdk_gl_drawable_swap_buffers(gldrawable);
    else
	glFlush();
    gdk_gl_drawable_gl_end(gldrawable);
  /*** OpenGL END ***/
    return TRUE;
}

/*
	when a mouse button is clicked this function is called
	params:gtk opgn gl canvas , GdkEventButton object and custom data
	return value:true or false, fails (false) if cannot init gl
*/
static gboolean button_press_event(GtkWidget * widget,
				   GdkEventButton * event, gpointer data)
{
    if (view->graphCount) {
	if (glCompSetClick
	    (view->Topview->topviewmenu, (int) event->x, (int) event->y))
	    expose_event(view->drawing_area, NULL, NULL);
    }

    begin_x = (float) event->x;
    begin_y = (float) event->y;



    if (event->button == 1)	//left click
    {
		view->prevpanx = view->panx;
		view->prevpany = view->pany;

		view->mouse.mouse_down = 1;
	if (GetOGLPosRef
	    ((int) begin_x, (int) begin_y, &(view->GLx), &(view->GLy),
	     &(view->GLz))) {
	    if (view->mouse.mouse_mode == MM_SINGLE_SELECT)	//single select
	    {
		view->Selection.Active = 1;
		view->Selection.Type = 0;
		view->Selection.AlreadySelected = 0;
		view->Selection.X = view->GLx - SINGLE_SELECTION_WIDTH / 2;
		view->Selection.Y = view->GLy - SINGLE_SELECTION_WIDTH / 2;
		view->Selection.W = SINGLE_SELECTION_WIDTH;
		view->Selection.H = SINGLE_SELECTION_WIDTH;
		expose_event(view->drawing_area, NULL, NULL);
	    }
	}
    }
    return FALSE;
}

/*
	when a mouse button is released(always after click) this function is called
	params:gtk opgn gl canvas , GdkEventButton object and custom data
	return value:true or false, fails (false) if cannot init gl
*/
static gboolean button_release_event(GtkWidget * widget,
				     GdkEventButton * event, gpointer data)
{
    if (event->button == 1)	//left click release
    {
	if (glCompSetRelease
	    (view->Topview->topviewmenu, (int) event->x_root,
	     (int) event->y_root))
	    expose_event(view->drawing_area, NULL, NULL);

	view->mouse.mouse_down = 0;
	if ((view->mouse.mouse_mode == MM_RECTANGULAR_SELECT)
	    || (view->mouse.mouse_mode == MM_RECTANGULAR_X_SELECT)) {
	    if (view->GLx <= view->GLx2)
		view->Selection.X = view->GLx;
	    else
		view->Selection.X = view->GLx2;
	    if (view->GLy <= view->GLy2)
		view->Selection.Y = view->GLy;
	    else
		view->Selection.Y = view->GLy2;

	    view->Selection.W = view->GLx2 - view->GLx;
	    if (view->Selection.W < 0)
		view->Selection.W = view->Selection.W * -1;
	    view->Selection.H = view->GLy2 - view->GLy;
	    if (view->Selection.H < 0)
		view->Selection.H = view->Selection.H * -1;
	    if (view->mouse.mouse_mode == 4)
		view->Selection.Type = 1;
	    else
		view->Selection.Type = 2;
	    view->Selection.Active = 1;
	    expose_event(view->drawing_area, NULL, NULL);
	}
	if (view->mouse.mouse_mode == MM_MOVE) {
	    if (((custom_graph_data *)
		 AGDATA(view->g[view->activeGraph]))->TopView == 0)
		move_nodes(view->g[view->activeGraph]);
	    else
		move_TVnodes();
	}

	if ((view->mouse.mouse_mode == MM_FISHEYE_MAGNIFIER) || (view->mouse.mouse_mode == MM_MAGNIFIER))	//fisheye mag mouse release, stop distortion
	{
	    originate_distorded_coordinates(view->Topview);
	    expose_event(view->drawing_area, NULL, NULL);
	}
    }
    dx = 0.0;
    dy = 0.0;
    return FALSE;
}

/*
	when  mouse is moved over glcanvas this function is called
	params:gtk opgn gl canvas , GdkEventMotion object and custom data
	return value:always TRUE !!!
*/
static gboolean motion_notify_event(GtkWidget * widget,
				    GdkEventMotion * event, gpointer data)
{
    /* float w = (float)widget->allocation.width; */
    /* float h = (float)widget->allocation.height; */
    float x = (float) event->x;
    float y = (float) event->y;
    char buf[50];


	float gldx,gldy;
	gboolean redraw = FALSE;
    dx = x - begin_x;
    dy = y - begin_y;
    /*panning */
    if ((event->state & GDK_BUTTON1_MASK)
	&& (view->mouse.mouse_mode == MM_PAN))
	{
			gldx=GetOGLDistance(dx)/view->zoom*-1;
			gldy=GetOGLDistance(dy)/view->zoom*-1;
		if(view->active_camera ==-1)
		{
			view->panx=view->panx-gldx;
			view->pany=view->pany+gldy;
		}
		else
		{
			view->cameras[view->active_camera]->angley-=dy/5;
			view->cameras[view->active_camera]->anglex-=dx/5;
//			set_camera_x_y(view->cameras[view->active_camera]);
		}
		redraw = TRUE;
	}
    /*zooming */
    if ((event->state & GDK_BUTTON1_MASK)
	&& (view->mouse.mouse_mode == MM_ZOOM)) {
	float x;
	float real_zoom;
	if(view->active_camera==-1)
		real_zoom=view->zoom + dx / 10 * (view->zoom * -1 / 20);
	else
		real_zoom=(view->cameras[view->active_camera]->r + dx / 10 * (view->cameras[view->active_camera]->r  / 20))*-1;

	if (real_zoom > MAX_ZOOM)
		real_zoom = (float) MAX_ZOOM;
	if (real_zoom < MIN_ZOOM)
	    real_zoom = (float) MIN_ZOOM;

	if(view->active_camera==-1)
		view->zoom = real_zoom;
	else{
		view->cameras[view->active_camera]->r=real_zoom*-1;
		set_camera_x_y(view->cameras[view->active_camera]);
	}


	/*set label to new zoom value */
	x = ((float) 100.0 - (float) 1.0) * (view->zoom -
					     (float) MIN_ZOOM) /
	    ((float) MAX_ZOOM - (float) MIN_ZOOM) + (float) 1.0;
	sprintf(buf, "%i", (int) x);
	glCompLabelSetText((glCompLabel *) view->Topview->customptr, buf);
	redraw = TRUE;
    }

    /*selection rect */
    if ((event->state & GDK_BUTTON1_MASK)
	&& ((view->mouse.mouse_mode == MM_RECTANGULAR_SELECT)
	    || (view->mouse.mouse_mode == 5))) {
	GetFixedOGLPos((int) x, (int) y, view->GLDepth, &(view->GLx2),
		       &(view->GLy2), &(view->GLz2));
	redraw = TRUE;
    }
    if ((event->state & GDK_BUTTON1_MASK)
	&& (view->mouse.mouse_mode == MM_MOVE)) {
	GetFixedOGLPos((int) x, (int) y, view->GLDepth, &(view->GLx2),
		       &(view->GLy2), &(view->GLz2));
	redraw = TRUE;
    }
    if ((event->state & GDK_BUTTON1_MASK)
	&& ((view->mouse.mouse_mode == MM_MAGNIFIER)
	    || (view->mouse.mouse_mode == MM_FISHEYE_MAGNIFIER))) {
	view->mouse.mouse_X = (int) x;
	view->mouse.mouse_Y = (int) y;
	redraw = TRUE;
    }


    begin_x = x;
    begin_y = y;


    if (redraw)
	gdk_window_invalidate_rect(widget->window, &widget->allocation,
				   FALSE);
    return TRUE;
}

/*
	when a key is pressed this function is called
	params:gtk opgn gl canvas , GdkEventKey(to retrieve which key is pressed) object and custom data
	return value:true or false, fails (false) if listed keys (in switch) are not pressed
*/
#ifdef UNUSED
static gboolean key_press_event(GtkWidget * widget, GdkEventKey * event,
				gpointer data)
{
    switch (event->keyval) {
    case GDK_Escape:
	gtk_main_quit();
	break;
    default:
	return FALSE;
    }
    return TRUE;
}
#endif

/*
	call back for mouse mode changes,
	params:GtkMenuItem is not used, mouse_mode is the new mouse_mode
	return value:none
*/
void switch_Mouse(GtkMenuItem * menuitem, int mouse_mode)
{
    view->mouse.mouse_mode = mouse_mode;
}


/*
	call back for mouse right click, this function activates the gtk right click pop up menu
	params:widget to shop popup , event handler to check click type and custom data
	return value:true or false, fails (false) if listed keys (in switch) are not pressed
*/
static gboolean button_press_event_popup_menu(GtkWidget * widget,
					      GdkEventButton * event,
					      gpointer data)
{
    if (event->button == 3) {
	/* Popup menu. */
	gtk_menu_popup(GTK_MENU(widget), NULL, NULL, NULL, NULL,
		       event->button, event->time);
	return TRUE;
    }
    return FALSE;
}


/*
	configures various opengl settings
	params:none
	return value:GdkGLConfig object
*/
GdkGLConfig *configure_gl(void)
{
    GdkGLConfig *glconfig;
    /* Try double-buffered visual */
    glconfig = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGB |
					 GDK_GL_MODE_DEPTH |
					 GDK_GL_MODE_DOUBLE);
    if (glconfig == NULL) {
	g_print("\n*** Cannot find the double-buffered visual.\n");
	g_print("\n*** Trying single-buffered visual.\n");

	/* Try single-buffered visual */
	glconfig = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGB |
					     GDK_GL_MODE_DEPTH);
	if (glconfig == NULL) {
	    g_print("*** No appropriate OpenGL-capable visual found.\n");
	    exit(1);
	}
    }

    return glconfig;
}

/*
	run this function only once to create and customize gtk based opengl canvas
	all signal, socket connections are done here for opengl interractions
	params:gl config object, gtk container widget for opengl canvas
	return value:none
*/
void create_window(GdkGLConfig * glconfig, GtkWidget * vbox)
{
    gint major, minor;

    GtkWidget *menu;
    /*
     * Query OpenGL extension version.
     */

    gdk_gl_query_version(&major, &minor);
    g_print("\nOpenGL extension version - %d.%d\n", major, minor);

    /* Try double-buffered visual */

    if (IS_TEST_MODE_ON)	//printf some gl values, to test if your system has opengl stuff
	examine_gl_config_attrib(glconfig);
    /* Drawing area for drawing OpenGL scene. */
    view->drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(view->drawing_area, 300, 300);
    /* Set OpenGL-capability to the widget. */
    gtk_widget_set_gl_capability(view->drawing_area,
				 glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);

    gtk_widget_add_events(view->drawing_area,
			  GDK_BUTTON1_MOTION_MASK |
			  GDK_BUTTON2_MOTION_MASK |
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_VISIBILITY_NOTIFY_MASK);

    g_signal_connect_after(G_OBJECT(view->drawing_area), "realize",
			   G_CALLBACK(realize), NULL);
    g_signal_connect(G_OBJECT(view->drawing_area), "configure_event",
		     G_CALLBACK(configure_event), NULL);
    g_signal_connect(G_OBJECT(view->drawing_area), "expose_event",
		     G_CALLBACK(expose_event), NULL);

    g_signal_connect(G_OBJECT(view->drawing_area), "button_press_event",
		     G_CALLBACK(button_press_event), NULL);
    g_signal_connect(G_OBJECT(view->drawing_area), "button_release_event",
		     G_CALLBACK(button_release_event), NULL);
    g_signal_connect(G_OBJECT(view->drawing_area), "motion_notify_event",
		     G_CALLBACK(motion_notify_event), NULL);


    gtk_box_pack_start(GTK_BOX(vbox), view->drawing_area, TRUE, TRUE, 0);

    gtk_widget_show(view->drawing_area);

    /* Popup menu. */

    menu = create_popup_menu(view->drawing_area);

    /* Signal handler */
    g_signal_connect_swapped(G_OBJECT(view->drawing_area),
			     "button_press_event",
			     G_CALLBACK(button_press_event_popup_menu),
			     menu);

}
