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
void close_graph(ViewInfo * view,int graphid);
int save_graph(void);
int save_graph_with_file_name(Agraph_t * graph, char *fileName);
int save_as_graph(void);

int do_graph_layout(Agraph_t * graph, int Engine, int keeppos);

void movenode(void *n, float dx, float dy);
void glexpose(void);
int gl_main_expose(void);
void move_nodes(Agraph_t * g);
void please_wait(void);
void please_dont_wait(void);


 /* helper functions */
extern int setGdkColor(GdkColor * c, char *color);
#endif
