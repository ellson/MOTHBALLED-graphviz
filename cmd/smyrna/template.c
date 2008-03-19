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



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <gtk/gtkgl.h>
/*** Use OpenGL extensions. ***/
/* #include <gdk/gdkglglext.h> */

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>


/**************************************************************************
 * The following section contains all the macro definitions.
 **************************************************************************/

/***
 *** Change these three macros to customise the
 *** default width and height of the drawing
 *** area, plus the default title of the window.
 ***/
#define DEFAULT_WIDTH  200
#define DEFAULT_HEIGHT 200
#define TIMEOUT_INTERVAL 10


/**************************************************************************
 * Global variable declarations.
 **************************************************************************/



/**************************************************************************
 * The following section contains the function prototype declarations.
 **************************************************************************/

#ifdef UNUSED
static void timeout_add(GtkWidget * widget);
static void timeout_remove(GtkWidget * widget);

static void toggle_animation(GtkWidget * widget);

static GdkGLConfig *configure_gl(void);

static GtkWidget *create_popup_menu(GtkWidget * drawing_area);
static GtkWidget *create_window(GdkGLConfig * glconfig);
#endif


/**************************************************************************
 * The following section contains all the callback function definitions.
 **************************************************************************/

/***
 *** The "realize" signal handler. All the OpenGL initialization
 *** should be performed here, such as default background colour,
 *** certain states etc.
 ***/
#ifdef UNUSED
static void realize(GtkWidget * widget, gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    g_print("%s: \"realize\"\n", gtk_widget_get_name(widget));

  /*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	return;

  /*** Fill in the details here. ***/

    gdk_gl_drawable_gl_end(gldrawable);
  /*** OpenGL END ***/
}
#endif

#ifdef UNUSED
/***
 *** The "configure_event" signal handler. Any processing required when
 *** the OpenGL-capable drawing area is re-configured should be done here.
 *** Almost always it will be used to resize the OpenGL viewport when
 *** the window is resized.
 ***/
static gboolean
configure_event(GtkWidget * widget,
		GdkEventConfigure * event, gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    GLfloat w = widget->allocation.width;
    GLfloat h = widget->allocation.height;

    g_print("%s: \"configure_event\"\n", gtk_widget_get_name(widget));

  /*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	return FALSE;

  /*** Fill in the details here. ***/

    glViewport(0, 0, w, h);

    gdk_gl_drawable_gl_end(gldrawable);
  /*** OpenGL END ***/

    return TRUE;
}

/***
 *** The "expose_event" signal handler. All the OpenGL re-drawing should
 *** be done here. This is repeatedly called as the painting routine
 *** every time the 'expose'/'draw' event is signalled.
 ***/
static gboolean
expose_event(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    g_print("%s: \"expose_event\"\n", gtk_widget_get_name(widget));

  /*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	return FALSE;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*** Fill in the details here. ***/

    /* Swap buffers */
    if (gdk_gl_drawable_is_double_buffered(gldrawable))
	gdk_gl_drawable_swap_buffers(gldrawable);
    else
	glFlush();

    gdk_gl_drawable_gl_end(gldrawable);
  /*** OpenGL END ***/

    return TRUE;
}

/***
 *** The timeout function. Often in animations,
 *** timeout functions are suitable for continous
 *** frame updates.
 ***/
static gboolean timeout(GtkWidget * widget)
{
    g_print(".");

  /*** Fill in the details here ***/

    /* Invalidate the whole window. */
    gdk_window_invalidate_rect(widget->window, &widget->allocation, FALSE);

    /* Update synchronously. */
    gdk_window_process_updates(widget->window, FALSE);

    return TRUE;
}

/***
 *** The "unrealize" signal handler. Any processing required when
 *** the OpenGL-capable window is unrealized should be done here.
 ***/
static void unrealize(GtkWidget * widget, gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    g_print("%s: \"unrealize\"\n", gtk_widget_get_name(widget));

  /*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	return;

  /*** Fill in the details here. ***/

    gdk_gl_drawable_gl_end(gldrawable);
  /*** OpenGL END ***/
}

/***
 *** The "motion_notify_event" signal handler. Any processing required when
 *** the OpenGL-capable drawing area is under drag motion should be done here.
 ***/
static gboolean
motion_notify_event(GtkWidget * widget,
		    GdkEventMotion * event, gpointer data)
{
    g_print("%s: \"motion_notify_event\": button",
	    gtk_widget_get_name(widget));

  /*** Fill in the details here. ***/

    if (event->state & GDK_BUTTON1_MASK) {
	g_print(" 1");
    }

    if (event->state & GDK_BUTTON2_MASK) {
	g_print(" 2");
    }

    if (event->state & GDK_BUTTON3_MASK) {
	g_print(" 3");
    }

    g_print("\n");

    return FALSE;
}

/***
 *** The "button_press_event" signal handler. Any processing required when
 *** mouse buttons (only left and middle buttons) are pressed on the OpenGL-
 *** capable drawing area should be done here.
 ***/
static gboolean
button_press_event(GtkWidget * widget,
		   GdkEventButton * event, gpointer data)
{
    g_print("%s: \"button_press_event\": ", gtk_widget_get_name(widget));

    if (event->button == 1) {
      /*** Fill in the details here. ***/
	g_print("button 1\n");

	return TRUE;
    }

    if (event->button == 2) {
      /*** Fill in the details here. ***/
	g_print("button 2\n");

	return TRUE;
    }

    g_print("\n");

    return FALSE;
}

/* For popup menu. */
static gboolean
button_press_event_popup_menu(GtkWidget * widget,
			      GdkEventButton * event, gpointer data)
{
    g_print("%s: \"button_press_event_popup\": ",
	    gtk_widget_get_name(widget));

    if (event->button == 3) {
	g_print("button 3\n");

	/* Popup menu. */
	gtk_menu_popup(GTK_MENU(widget), NULL, NULL, NULL, NULL,
		       event->button, event->time);
	return TRUE;
    }

    g_print("\n");

    return FALSE;
}

/***
 *** The "key_press_event" signal handler. Any processing required when key
 *** presses occur should be done here.
 ***/
static gboolean
key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer data)
{
    g_print("%s: \"key_press_event\": ", gtk_widget_get_name(widget));

    switch (event->keyval) {
      /*** Fill in the details here. ***/

    case GDK_a:
	g_print("a key\n");
//      toggle_animation (widget);
	break;

    case GDK_Escape:
	g_print("Escape key\n");
	gtk_main_quit();
	break;

    default:
	g_print("\n");
	return FALSE;
    }

    return TRUE;
}


/**************************************************************************
 * The following section contains the timeout function management routines.
 **************************************************************************/

/***
 *** Helper functions to add or remove the timeout function.
 ***/

static guint timeout_id = 0;

static void timeout_add(GtkWidget * widget)
{
    if (timeout_id == 0) {
	timeout_id = g_timeout_add(TIMEOUT_INTERVAL,
				   (GSourceFunc) timeout, widget);
    }
}

static void timeout_remove(GtkWidget * widget)
{
    if (timeout_id != 0) {
	g_source_remove(timeout_id);
	timeout_id = 0;
    }
}

/***
 *** The "map_event" signal handler. Any processing required when the
 *** OpenGL-capable drawing area is mapped should be done here.
 ***/
static gboolean
map_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    g_print("%s: \"map_event\":\n", gtk_widget_get_name(widget));
    return TRUE;
}

/***
 *** The "unmap_event" signal handler. Any processing required when the
 *** OpenGL-capable drawing area is unmapped should be done here.
 ***/
static gboolean
unmap_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    g_print("%s: \"unmap_event\":\n", gtk_widget_get_name(widget));
    timeout_remove(widget);

    return TRUE;
}

/***
 *** The "visibility_notify_event" signal handler. Any processing required
 *** when the OpenGL-capable drawing area is visually obscured should be
 *** done here.
 ***/
static gboolean
visibility_notify_event(GtkWidget * widget,
			GdkEventVisibility * event, gpointer data)
{

    return TRUE;
}
#endif



/**************************************************************************
 * The following section contains the GUI building function definitions.
 **************************************************************************/

/***
 *** Creates the popup menu to be displayed.
 ***/
#ifdef UNUSED
static GtkWidget *create_popup_menu(GtkWidget * drawing_area)
{
    GtkWidget *menu;
    GtkWidget *menu_item;

    menu = gtk_menu_new();

    /* Toggle animation */
    menu_item = gtk_menu_item_new_with_label("Toggle Animation");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
//  g_signal_connect_swapped (G_OBJECT (menu_item), "activate",
//                          G_CALLBACK (toggle_animation), drawing_area);
    gtk_widget_show(menu_item);

    /* Quit */
    menu_item = gtk_menu_item_new_with_label("Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate",
		     G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show(menu_item);

    return menu;
}
#endif

/***
 *** Creates the simple application window with one
 *** drawing area that has an OpenGL-capable visual.
 ***/
#ifdef UNUSED
static GtkWidget *create_window(GdkGLConfig * glconfig)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *drawing_area;
    GtkWidget *menu;
    GtkWidget *button;

    /*
     * Top-level window.
     */

//  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//  gtk_window_set_title (GTK_WINDOW (window), DEFAULT_TITLE);

    /* Get automatically redrawn if any of their children changed allocation. */
    gtk_container_set_reallocate_redraws(GTK_CONTAINER(window), TRUE);

    /* Connect signal handlers to the window */
    g_signal_connect(G_OBJECT(window), "delete_event",
		     G_CALLBACK(gtk_main_quit), NULL);

    /*
     * VBox.
     */

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

    /*
     * Drawing area to draw OpenGL scene.
     */

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, DEFAULT_WIDTH,
				DEFAULT_HEIGHT);

    /* Set OpenGL-capability to the widget */
    gtk_widget_set_gl_capability(drawing_area,
				 glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);

    gtk_widget_add_events(drawing_area,
			  GDK_BUTTON1_MOTION_MASK |
			  GDK_BUTTON2_MOTION_MASK |
			  GDK_BUTTON_PRESS_MASK |
			  GDK_VISIBILITY_NOTIFY_MASK);

    /* Connect signal handlers to the drawing area */
    g_signal_connect_after(G_OBJECT(drawing_area), "realize",
			   G_CALLBACK(realize), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "configure_event",
		     G_CALLBACK(configure_event), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "expose_event",
		     G_CALLBACK(expose_event), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "unrealize",
		     G_CALLBACK(unrealize), NULL);

    g_signal_connect(G_OBJECT(drawing_area), "motion_notify_event",
		     G_CALLBACK(motion_notify_event), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "button_press_event",
		     G_CALLBACK(button_press_event), NULL);

    /* key_press_event handler for top-level window */
    g_signal_connect_swapped(G_OBJECT(window), "key_press_event",
			     G_CALLBACK(key_press_event), drawing_area);

    /* For timeout function. */
    g_signal_connect(G_OBJECT(drawing_area), "map_event",
		     G_CALLBACK(map_event), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "unmap_event",
		     G_CALLBACK(unmap_event), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "visibility_notify_event",
		     G_CALLBACK(visibility_notify_event), NULL);

    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);

    gtk_widget_show(drawing_area);

    /*
     * Popup menu.
     */

    menu = create_popup_menu(drawing_area);

    g_signal_connect_swapped(G_OBJECT(drawing_area), "button_press_event",
			     G_CALLBACK(button_press_event_popup_menu),
			     menu);

    /*
     * Simple quit button.
     */

    button = gtk_button_new_with_label("Quit");

    g_signal_connect(G_OBJECT(button), "clicked",
		     G_CALLBACK(gtk_main_quit), NULL);

    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    gtk_widget_show(button);

    return window;
}
#endif


/**************************************************************************
 * The following section contains utility function definitions.
 **************************************************************************/

/***
 *** Configure the OpenGL framebuffer.
 ***/
#ifdef UNUSED
static GdkGLConfig *configure_gl(void)
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
#endif
