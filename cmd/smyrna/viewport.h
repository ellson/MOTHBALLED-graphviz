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

//view data structure
#ifndef VIEWPORT_H
#define VIEWPORT_H
#define bool int
#include "smyrnadefs.h"
#include <gtk/gtk.h>
#include "xdot.h"
#include "cgraph.h"

void init_viewport(ViewInfo * view);
void set_viewport_settings_from_template(ViewInfo * view, Agraph_t *);
void clear_viewport(ViewInfo * view);
int add_graph_to_viewport_from_file(char *fileName);
int save_graph();
int save_as_graph();

int do_graph_layout(Agraph_t * graph, int Engine, int keeppos);
void refreshControls(ViewInfo * v);

void move_node(void *n, float dx, float dy);
void glexpose();
void move_nodes(Agraph_t * g);

viewport_camera* add_camera_to_viewport(ViewInfo * view);
int delete_camera_from_viewport(ViewInfo * view,viewport_camera* c);
int activate_viewport_camera (ViewInfo * view,int cam_index);

 /* helper functions */
extern int setGdkColor(GdkColor * c, char *color);
#endif
