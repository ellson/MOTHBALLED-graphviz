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
#include <GL/gl.h>
#include <GL/glu.h>							
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>
#include <gdk/gdkcursor.h>

void examine_gl_config_attrib (GdkGLConfig *glconfig);
void print_gl_config_attrib (GdkGLConfig *glconfig,const gchar *attrib_str,int attrib,gboolean is_boolean);
static void realize (GtkWidget *widget,gpointer   data);
static gboolean configure_event (GtkWidget *widget,GdkEventConfigure *event, gpointer data);
gboolean expose_event (GtkWidget *widget,GdkEventExpose *event,gpointer data);
static gboolean button_press_event (GtkWidget *widget,GdkEventButton *event,gpointer data);
static gboolean button_release_event (GtkWidget *widget,GdkEventButton *event,gpointer data);
static gboolean motion_notify_event (GtkWidget *widget,GdkEventMotion *event,gpointer data);
static gboolean key_press_event (GtkWidget *widget, GdkEventKey *event,gpointer data);
void switch_Mouse (GtkMenuItem *menuitem,int mouse_mode);
static gboolean button_press_event_popup_menu (GtkWidget *widget,GdkEventButton *event,gpointer data);
extern GdkGLConfig *configure_gl (void);
void create_window (GdkGLConfig *glconfig,GtkWidget* vbox);
#endif
