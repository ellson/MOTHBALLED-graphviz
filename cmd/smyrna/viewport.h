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




void init_viewport(ViewInfo* view);
void clear_viewport(ViewInfo* view);
int add_graph_to_viewport_from_file (char* fileName);	//returns 1 if successfull else 0 ++
int add_new_graph_to_viewport();	//returns graph index , otherwise -1
int create_xdot_for_graph(Agraph_t* graph,int keeppos);	//0 failed , 1 successfull ++
void update_graph_params(Agraph_t* graph);	//adds gledit params  ++
Agraph_t* loadGraph(char* filename); //dont use directly, use add_graph_to_viewport_from_file instead 
void load_graph_params(Agraph_t* graph);	//run once right after loading graph++
void clear_graph(Agraph_t* graph);	//clears custom data binded,
int save_graph();	//save without prompt
int save_as_graph(); //save with prompt
int save_graph_with_file_name(Agraph_t* graph,char* fileName);	//saves graph with file name,if file name is NULL save as is ++

int do_graph_layout(Agraph_t* graph,int Engine,int keeppos); //changes the layout, all user relocations are reset
void refreshControls(ViewInfo* v);


int attach_object_custom_data_to_graph(Agraph_t* graph);//run once or to reset all data !! prev data is removed
int clear_object_custom_data(void* obj);	//frees memory allocated for custom object data
int add_string_data_to_object_custom_data(void* obj,char* data);
int add_numeric_data_to_object_custom_data(void* obj,float data);
int clear_string_data_from_object_custom_data(void* obj);
int clear_numeric_data_from_object_custom_data(void* obj);

int clear_object_xdot(void* obj);	//clear single object's xdot info
int clear_graph_xdot(Agraph_t* graph);	//clears all xdot  attributes, used especially before layout change

char* get_object_attribute(void* obj,char* attr);	//returns an attribute value no matter what,

void move_node(void* n,float dx,float dy);
void glexpose();
void move_nodes(Agraph_t* g);	//move selected nodes 
//helper functions
int SetGdkColor(GdkColor* c,char* color);
#endif
