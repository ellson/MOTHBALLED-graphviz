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

//Top view
#ifndef TOPVIEW_H
#define TOPVIEW_H
#define MAX_TOP_VIEW_NODE_COUNT		1000000
#define MAX_TOP_VIEW_EDGE_COUNT		1000000
#define UNHIGHLIGHTED_ALPHA	0.3

#include "viewport.h"
#include "gui.h"
#include "hierarchy.h"
#include "tvnodes.h"

#ifdef	WIN32	//this shit is needed on WIN32 to get libglade see the callback
#define _BB  __declspec(dllexport)
#else
#define _BB  
#endif


typedef struct{
	Agnode_t* Node;
	GLfloat x;
	GLfloat y;
	GLfloat z;

	GLfloat distorted_x;		//geometric fisheye coords
	GLfloat distorted_y;
	GLfloat distorted_z;
	float zoom_factor;
	int in_fish_eye;	//boolean value if to apply fisheye

	RGBColor Color;
	RGBColor GroupColor;
	int GroupIndex; //default -1;
	int update_required;
	char* Label;
	char* Label2;
	int degree;
	float node_alpha;
	int valid;

}topview_node;

typedef struct{
	Agnode_t* Tnode;	//Tail node
	Agnode_t* Hnode;	//Tail node
	Agedge_t *Edge;		//edge itself
	GLfloat x1;
	GLfloat y1;
	GLfloat z1;
	GLfloat x2;
	GLfloat y2;
	GLfloat z2;
	topview_node* Node1;
	topview_node* Node2;
	RGBColor Color;
	int update_required;

}topview_edge;
typedef struct {
	topview_node Nodes[MAX_TOP_VIEW_NODE_COUNT];
	topview_edge Edges[MAX_TOP_VIEW_EDGE_COUNT];
	int Nodecount;
	int Edgecount;
	int limits[4];
} topview;
extern topview Topview;
extern void PrepareTopview(Agraph_t *g);
int select_topview_node(topview_node *n);
int select_topview_edge(topview_edge *e);
int update_topview_node_from_cgraph(topview_node* Node);
int update_topview_edge_from_cgraph(topview_edge* Edge);
int set_update_required(topview* t);
int draw_topview_label(topview_node* v);
int randomize_color(RGBColor* c,int brightness);
void drawCircle(float x,float y,float radius);
void set_topview_options();
void set_boundries();
int get_color_from_edge(topview_edge *e);
int node_visible(Agnode_t* n);
int move_TVnodes();
int draw_navigation_menu();
int load_host_buttons(Agraph_t *g);
void originate_distorded_coordinates(topview* t);
int prepare_nodes_for_groups(int groupindex);
int validate_group_node(tv_node* TV_Node,char* regex_string);
int click_group_button(int groupindex);
_BB void host_button_clicked_Slot(GtkWidget *widget,gpointer     user_data);
_BB void on_host_alpha_change (GtkWidget *widget,gpointer     user_data);
//global gui pointers for buttons and color selection boxes
extern GtkButton** gtkhostbtn;
extern int gtkhostbtncount;
extern GtkColorButton** gtkhostcolor;
extern int hostactive[50];	//temporary static, convert to dynamic,realloc
extern char** hostregex;
double dist(double x1, double y1, double x2, double y2);
double G(double x);
extern int fisheye_distortion_fac;

extern Hierarchy* makeHier (topview*, vtx_data*, double*, double*);
#endif
