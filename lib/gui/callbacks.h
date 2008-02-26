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

#include <gtk/gtk.h>
#include "toolboxcallbacks.h"
#include "gui.h"





#ifdef	WIN32	//this shit is needed on WIN32 to get libglade see the callback
#define _BB  __declspec(dllexport)
#else
#define _BB  
#endif

_BB void new_graph_clicked (GtkWidget *widget,gpointer user_data);
_BB void open_graph_clicked (GtkWidget *widget,gpointer user_data);
_BB void save_graph_clicked (GtkWidget     *widget,gpointer user_data);
_BB void save_as_graph_clicked (GtkWidget     *widget,gpointer user_data);
_BB void remove_graph_clicked (GtkWidget     *widget,gpointer user_data);
void on_newNode (GtkWidget *button,gpointer user_data);
void dlgOpenGraph_OK_Clicked( GtkWidget *button,gpointer data );
_BB void btn_dot_clicked(GtkWidget *widget,gpointer user_data);
_BB void btn_neato_clicked(GtkWidget *widget,gpointer user_data);
_BB void btn_twopi_clicked(GtkWidget *widget,gpointer user_data);
_BB void btn_circo_clicked(GtkWidget *widget,gpointer user_data);
_BB void btn_fdp_clicked (GtkWidget *widget,gpointer user_data);

_BB void graph_select_change (GtkWidget *widget,gpointer user_data);
_BB void mGraphPropertiesSlot (GtkWidget *widget,gpointer user_data);


//dlgOpenGraph btnOK clicked
_BB void on_dlgOpenGraph_btnOK_clicked (GtkWidget *widget,gpointer user_data);
//dlgOpenGraph btncancelclicked
_BB void on_dlgOpenGraph_btncancel_clicked (GtkWidget *widget,gpointer user_data);

//MENU 


//frm Object OK button
//hides frmObject , set changed values to selected objects
_BB void frmObjectBtnOK_clicked (GtkWidget *widget,gpointer     user_data);

//frm Object Cancel button
//hides frmObject , ignores changed values

_BB void frmObjectBtnCancel_clicked (GtkWidget *widget,gpointer     user_data);


_BB void attr_widgets_modifiedSlot(GtkWidget *widget,gpointer     user_data);

