/* $Id$Revision: */
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

#ifndef FRMOBJECTUI_H
#define FRMOBJECTUI_H

#include "smyrnadefs.h"


/*
	creates a new attr_list
	attr_list is a basic stack implementation
	with alphanumeric sorting functions 
	that uses quicksort

*/

#define ATTR_NOTEBOOK_IDX 6

extern attr_list* attr_list_new(Agraph_t * g,int with_widgets );
extern void attr_list_add(attr_list* l,attr_t* a);
extern void free_attr_list(attr_list* l);
extern void filter_attributes(char* prefix,topview* t);

_BB void on_txtAttr_changed(GtkWidget * widget, gpointer user_data);
_BB void attr_label_motion(GtkWidget * widget,GdkEventMotion * event, gpointer data);
_BB void on_attrApplyBtn_clicked (GtkWidget * widget, gpointer user_data);
_BB void on_attrAddBtn_clicked (GtkWidget * widget, gpointer user_data);

extern attr_list* load_attr_list(Agraph_t* g);
extern void showAttrsWidget(topview* t);

#endif
