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

#ifndef VIEWPORTCAMERA_H
#define VIEWPORTCAMERA_H

#include "smyrnadefs.h"
#include "glcompset.h"

viewport_camera* add_camera_to_viewport(ViewInfo * view);
void set_camera_x_y(viewport_camera* c);
int delete_camera_from_viewport(ViewInfo * view,viewport_camera* c);
int activate_viewport_camera (ViewInfo * view,int cam_index);
int refresh_viewport_camera (ViewInfo * view);
void attach_camera_widget(ViewInfo * view);
int show_camera_settings(viewport_camera* c);
int save_camera_settings(viewport_camera* c);
_BB void dlgcameraokbutton_clicked_cb(GtkWidget * widget, gpointer user_data);
_BB void dlgcameracancelbutton_clicked_cb(GtkWidget * widget, gpointer user_data);

#endif
