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

void btnTVDelete_clicked_cb(GtkWidget * widget, gpointer user_data)
{
}
void btnTVFirst_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_nodes_first_page();
}

void btnTVPrevious_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_nodes_prior_page();
}

void btnTVNext_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_nodes_next_page();
}

void btnTVLast_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_nodes_last_page();
}

void btnTVGotopage_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    GtkSpinButton *spn;
    int p;
    spn = (GtkSpinButton *) glade_xml_get_widget(xml, "spnTVGotopage");
    p = gtk_spin_button_get_value_as_int(spn);
    tv_nodes_goto_page(p - 1);
}

void btnTVCancel_clicked_cb(GtkWidget * widget, gpointer user_data)
{
}
void btnTVOK_clicked_cb(GtkWidget * widget, gpointer user_data)
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

void btnTVSelectAll_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_select_all();
}

void btnTVUnselectAll_clicked_cb(GtkWidget * widget, gpointer user_data)
{
    tv_unselect_all();
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
    tv_save_as();
}
