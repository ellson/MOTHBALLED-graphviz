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

extern void init_viewport(ViewInfo * view);
extern void set_viewport_settings_from_template(ViewInfo * view, Agraph_t *);
extern void clear_viewport(ViewInfo * view);
extern int add_graph_to_viewport_from_file(char *fileName);
extern int save_graph();
extern int save_as_graph();

extern int do_graph_layout(Agraph_t * graph, int Engine, int keeppos);
extern void refreshControls(ViewInfo * v);

extern void move_node(void *n, float dx, float dy);
extern void glexpose();
extern void move_nodes(Agraph_t * g);

 /* helper functions */
extern int setGdkColor(GdkColor * c, char *color);
#endif
