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

#include "datalistcallbacks.h"
#include "tvnodes.h"
void btnTVEdit_clicked_cb(GtkWidget * widget, gpointer user_data)
{
}


void btnTVCancel_clicked_cb(GtkWidget * widget, gpointer user_data)
{
}
void btnTVOK_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    gtk_widget_hide(glade_xml_get_widget(xml, "frmTVNodes"));
}

void on_TVNodes_close (GtkWidget * widget, gpointer user_data)
{
    gtk_widget_hide(glade_xml_get_widget(xml, "frmTVNodes"));
}

void btnTVReverse_clicked_cb(GtkWidget * widget, gpointer user_data)
{
}
void cgbTVSelect_toggled_cb(GtkWidget * widget, gpointer user_data)
{
}
void cgbTVVisible_toggled_cb(GtkWidget * widget, gpointer user_data)
{

}
void cgbTVHighlighted_toggled_cb(GtkWidget * widget, gpointer user_data)
{

}



void btnTVFilterApply_clicked_cb(GtkWidget * widget, gpointer user_data)
{
/*	GTK_RESPONSE_OK     = -5,
	GTK_RESPONSE_CANCEL = -6,
	GTK_RESPONSE_CLOSE  = -7,
	GTK_RESPONSE_YES    = -8,
	GTK_RESPONSE_NO     = -9,
	GTK_RESPONSE_APPLY  = -10,
	GTK_RESPONSE_HELP   = -11 */

    gtk_dialog_response((GtkDialog *)
			glade_xml_get_widget(xml, "dlgTVFilter"),
			GTK_RESPONSE_OK);

}


void btnTVShowAll_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_show_all();
}

void btnTVHideAll_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_hide_all();
}

void btnTVSaveAs_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_save_as(0);
}

void btnTVSaveWith_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_save_as(1);
}
