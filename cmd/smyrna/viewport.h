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

void arcmouseRClick(ViewInfo* v);
void arcmouseClick(ViewInfo* v);
void arcmouseDrag(ViewInfo* v);



void init_viewport(ViewInfo * view);
void set_viewport_settings_from_template(ViewInfo * view, Agraph_t *);
void clear_viewport(ViewInfo * view);
void switch_graph(int);
void refreshViewport (int doClear);
int add_graph_to_viewport_from_file(char *fileName);
int add_graph_to_viewport(Agraph_t* graph, char*);
int close_graph(ViewInfo * view,int graphid);
int save_graph(void);
int save_graph_with_file_name(Agraph_t * graph, char *fileName);
int save_as_graph(void);

int do_graph_layout(Agraph_t * graph, int Engine, int keeppos);

void movenode(void *n, float dx, float dy);
void glexpose(void);
void move_nodes(Agraph_t * g);
void please_wait(void);
void please_dont_wait(void);
extern md5_byte_t* get_md5_key(Agraph_t* graph);
void fill_key(md5_byte_t* b,md5_byte_t* data);
colorschemaset* create_color_theme(int themeid);
extern void getcolorfromschema(colorschemaset* sc,float l,float maxl,RGBColor* c);


 /* helper functions */
extern int setGdkColor(GdkColor * c, char *color);
extern char *get_attribute_value(char *attr, ViewInfo * view, Agraph_t * g);
#endif
