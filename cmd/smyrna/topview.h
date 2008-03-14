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

#ifndef TOPVIEW_H
#define TOPVIEW_H


#include "smyrnadefs.h"
#ifdef	WIN32	//this is needed on WIN32 to get libglade see the callback
#define _BB  __declspec(dllexport)
#else
#define _BB  
#endif

void cleartopview(topview* t);
void preparetopview(Agraph_t *g,topview* t);
void drawTopViewGraph(Agraph_t *g);
int select_topview_node(topview_node *n);
int select_topview_edge(topview_edge *e);
int update_topview_node_from_cgraph(topview_node* Node);
int update_topview_edge_from_cgraph(topview_edge* Edge);
int set_update_required(topview* t);
int draw_topview_label(topview_node* v,float zdepth);
void set_topview_options();
void set_boundries(topview* t);
int get_color_from_edge(topview_edge *e);
int node_visible(Agnode_t* n);
int move_TVnodes();
void local_zoom(topview* t);
void originate_distorded_coordinates(topview* t);
_BB void on_host_alpha_change (GtkWidget *widget,gpointer     user_data);
double dist(double x1, double y1, double x2, double y2);
double G(double x);
glCompSet* glcreate_gl_topview_menu();
void fisheye_polar(double x_focus, double y_focus,topview* t);
void prepare_topological_fisheye(topview* t);


#endif
