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

#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>
#include <glade/glade.h>
#include "callbacks.h"
#include "viewport.h"



#ifdef	WIN32	//this shit is needed on WIN32 to get libglade see the callback
#define _BB  __declspec(dllexport)
#else
#define _BB  
#endif

#ifdef _WIN32
#define SMYRNA_GLADE "C:\\Projects\\ATT\\GTK\\smyrna\\lib\\GUI\\smyrna.glade"
#define SMYRNA_ATTRS "C:\\Projects\\ATT\\GTK\\GTKTest2\\attrs.txt"
// #else
// using -DSMYRNA_GLADE from Makefile.am and configure.ac
// using -DSMYRNA_ATTRS from Makefile.am and configure.ac
#endif

#define MAXIMUM_WIDGET_COUNT	97

//GtkWidget *window1;		//main window
extern GdkWindow* window1;
extern GtkWidget *statusbar1;

extern GladeXML *xml;			//global libglade vars
extern GtkWidget *gladewidget;

//1 subgraph 2 node 3 edge
extern int frmObjectTypeIndex;
extern Agraph_t* frmObjectg;


extern GtkComboBox* cbSelectGraph;		//combo at top left

extern GtkWidget* AttrWidgets[MAXIMUM_WIDGET_COUNT];	
extern GtkWidget* AttrLabels[MAXIMUM_WIDGET_COUNT];	
extern int attr_widgets_modified[MAXIMUM_WIDGET_COUNT];
extern int widgetcounter;	//number of attributes counted dynamically, might be removed in the future 
extern attribute attr[MAXIMUM_WIDGET_COUNT];	


void create_object_properties();	//creates general purpose object properties template
void object_properties_node_init();	//customize window for Nodes
void object_properties_edge_init();	//customize window for Edges
void object_properties_cluster_init();	//customize window for Cluster
void object_properties_graph_init();	//customize window for Graph , this shows the graph default values
void graph_properties_init(int newgraph);	//initialize little open graph dialog
GtkComboBox* get_SelectGraph();		//freaking GLADE!!!!!
int update_graph_properties(Agraph_t* graph); //updates graph from gui
void load_graph_properties(Agraph_t* graph);//load from graph to gui

void update_object_properties(int typeIndex,Agraph_t* g);	//updates objects from gui(node ,edge, cluster)
int load_object_properties(int typeIndex,Agraph_t* g);	//load  from object to gui;
void load_attributes();//loads attributes from a text file
void change_selected_graph_attributes (Agraph_t* g,char* attrname,char* attrvalue);
void change_selected_node_attributes (Agraph_t* g,char* attrname,char* attrvalue);
void change_selected_edge_attributes (Agraph_t* g,char* attrname,char* attrvalue);
char* get_attribute_string_value_from_widget(attribute* att);


//GTK helpre functions
//void Color_Widget_bg (int r, int g, int b, GtkWidget *widget);	//change background color 
 void Color_Widget_bg (char* colorstring, GtkWidget *widget);








#endif
