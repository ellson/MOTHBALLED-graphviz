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
void btnTVEdit_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	printf("btnTVEdit_clicked_cb\n");
}
void btnTVDelete_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	printf("btnTVDelete_clicked_cb\n");
}
void btnTVFilter_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	int respond;

//	gtk_dialog_set_response_sensitive   (glade_xml_get_widget(xml, "dlgOpenGraph"),1,1);
//	gtk_dialog_set_response_sensitive   (glade_xml_get_widget(xml, "dlgOpenGraph"),2,1);
	respond=gtk_dialog_run ((GtkDialog*)glade_xml_get_widget(xml, "dlgTVFilter"));
//	respond=gtk_dialog_run (glade_xml_get_widget(xml, "dlgFilters"));
	gtk_widget_hide(glade_xml_get_widget(xml, "dlgTVFilter"));
	if(respond == 1)
		apply_filter_from_gui();
}
void btnTVFirst_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	tv_nodes_first_page();
}
void btnTVPrevious_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	tv_nodes_prior_page();
}
void btnTVNext_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	tv_nodes_next_page();
}
void btnTVLast_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	tv_nodes_last_page();
}
void btnTVGotopage_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	GtkSpinButton* spn;
	int p;
	spn=(GtkSpinButton*)glade_xml_get_widget(xml, "spnTVGotopage");
	p=gtk_spin_button_get_value_as_int(spn);
	tv_nodes_goto_page(p-1);
}
void btnTVCancel_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	printf("btnTVCancel_clicked_cb\n");
}
void btnTVOK_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	update_TV_data_from_gui();
    gtk_widget_hide(glade_xml_get_widget(xml, "frmTVNodes"));
}
void btnTVReverse_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	printf("btnTVEdit_clicked_cb\n");
}
void cgbTVSelect_toggled_cb (GtkWidget *widget,gpointer user_data)
{
	printf("btnTVEdit_clicked_cb\n");
}



void btnTVFilterApply_clicked_cb (GtkWidget *widget,gpointer user_data)
{
/*	GTK_RESPONSE_OK     = -5,
	GTK_RESPONSE_CANCEL = -6,
	GTK_RESPONSE_CLOSE  = -7,
	GTK_RESPONSE_YES    = -8,
	GTK_RESPONSE_NO     = -9,
	GTK_RESPONSE_APPLY  = -10,
	GTK_RESPONSE_HELP   = -11 */
	
	gtk_dialog_response((GtkDialog*)glade_xml_get_widget(xml, "dlgTVFilter"),GTK_RESPONSE_OK);

}
void btnTVFilterClear_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	gtk_entry_set_text ((GtkEntry*)glade_xml_get_widget(xml,"edtTVFilterMinData1"),"" );
	gtk_entry_set_text ((GtkEntry*) glade_xml_get_widget(xml,"edtTVFilterMaxData1"),"" );
	gtk_entry_set_text ((GtkEntry*) glade_xml_get_widget(xml,"edtTVFilterMinData2"),"" );
	gtk_entry_set_text ((GtkEntry*) glade_xml_get_widget(xml,"edtTVFilterMaxData2"),"" );
	gtk_entry_set_text ((GtkEntry*) glade_xml_get_widget(xml,"edtTVFilterString"),"" );


	gtk_toggle_button_set_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterHigh1"),1);
	gtk_toggle_button_set_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterSel1"),1);
	gtk_toggle_button_set_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterVisible1"),1);
	apply_filter_from_gui();
}

void btnTVSelectAll_clicked_cb (GtkWidget *widget,gpointer user_data)
{
	tv_select_all();
}
void btnTVUnselectAll_clicked_cb(GtkWidget *widget,gpointer user_data)
{
	tv_unselect_all();
}
void btnTVHighlightAll_clicked_cb(GtkWidget *widget,gpointer user_data)
{
	tv_highligh_all();
}
void btnTVUnhighlightAll_clicked_cb(GtkWidget *widget,gpointer user_data)
{
	tv_unhighligh_all();
}


void  btnTVShowAll_clicked_cb(GtkWidget *widget,gpointer user_data)
{
	tv_show_all();
}
void btnTVHideAll_clicked_cb(GtkWidget *widget,gpointer user_data)
{
	tv_hide_all();
}





