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

#define _CRT_SECURE_NO_DEPRECATE 1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtk.h>
#include "callbacks.h"
#include "viewport.h"
#include "topview.h"
#include "selection.h"
#include "memory.h"

//Menu Items 

void new_graph_clicked(GtkWidget * widget, gpointer user_data)
{
    g_print("new graph button fired\n");
}

void open_graph_clicked(GtkWidget * widget, gpointer user_data)
{

}
void save_graph_clicked(GtkWidget * widget, gpointer user_data)
{
    if (view->activeGraph > -1)
	save_graph(view->g[view->activeGraph], NULL);
}


void save_as_graph_clicked(GtkWidget * widget, gpointer user_data)
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Save File",
					 NULL,
					 GTK_FILE_CHOOSER_ACTION_SAVE,
					 GTK_STOCK_CANCEL,
					 GTK_RESPONSE_CANCEL,
					 GTK_STOCK_SAVE,
					 GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER
						   (dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
	char *filename;
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	save_graph(view->g[view->activeGraph], filename);
	g_free(filename);
    }
    gtk_widget_destroy(dialog);



}

void remove_graph_clicked(GtkWidget * widget, gpointer user_data)
{
    g_print("remove graph button fired\n");
}


void btn_dot_clicked(GtkWidget * widget, gpointer user_data)
{
    GdkCursor *cursor;
    GdkWindow *w;
    Dlg = (GtkMessageDialog *) gtk_message_dialog_new(NULL,
						      GTK_DIALOG_MODAL,
						      GTK_MESSAGE_QUESTION,
						      GTK_BUTTONS_YES_NO,
						      "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

    respond = gtk_dialog_run((GtkDialog *) Dlg);
    if (respond == GTK_RESPONSE_YES)
	do_graph_layout(view->g[view->activeGraph], GVK_DOT, 0);
    gtk_object_destroy((GtkObject *) Dlg);

    cursor = gdk_cursor_new(GDK_HAND2);
    w = (GdkWindow *) glade_xml_get_widget(xml, "frmMain");
//      gdk_window_set_cursor(w, cursor);
    gdk_window_set_cursor((GTK_WIDGET(view->drawing_area)->window),
			  cursor);
//      gdk_window_set_title((GTK_WIDGET(widget)->window),"adasdasdasdassada");
    gdk_cursor_destroy(cursor);



}

void btn_neato_clicked(GtkWidget * widget, gpointer user_data)
{
    Dlg = (GtkMessageDialog *) gtk_message_dialog_new(NULL,
						      GTK_DIALOG_MODAL,
						      GTK_MESSAGE_QUESTION,
						      GTK_BUTTONS_YES_NO,
						      "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

    respond = gtk_dialog_run((GtkDialog *) Dlg);
    if (respond == GTK_RESPONSE_YES)
	do_graph_layout(view->g[view->activeGraph], GVK_NEATO, 0);
    gtk_object_destroy((GtkObject *) Dlg);
    gtk_button_set_image(GTK_BUTTON
			 (glade_xml_get_widget(xml, "btn_neato")),
			 gtk_image_new_from_file("c:\fonts.png"));

}

void btn_twopi_clicked(GtkWidget * widget, gpointer user_data)
{
    Dlg = (GtkMessageDialog *) gtk_message_dialog_new(NULL,
						      GTK_DIALOG_MODAL,
						      GTK_MESSAGE_QUESTION,
						      GTK_BUTTONS_YES_NO,
						      "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

    respond = gtk_dialog_run((GtkDialog *) Dlg);
    if (respond == GTK_RESPONSE_YES)
	do_graph_layout(view->g[view->activeGraph], GVK_TWOPI, 0);
    gtk_object_destroy((GtkObject *) Dlg);

}

void btn_circo_clicked(GtkWidget * widget, gpointer user_data)
{
    Dlg = (GtkMessageDialog *) gtk_message_dialog_new(NULL,
						      GTK_DIALOG_MODAL,
						      GTK_MESSAGE_QUESTION,
						      GTK_BUTTONS_YES_NO,
						      "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

    respond = gtk_dialog_run((GtkDialog *) Dlg);
    if (respond == GTK_RESPONSE_YES)
	do_graph_layout(view->g[view->activeGraph], GVK_CIRCO, 0);
    gtk_object_destroy((GtkObject *) Dlg);
}

void btn_fdp_clicked(GtkWidget * widget, gpointer user_data)
{

    Dlg = (GtkMessageDialog *) gtk_message_dialog_new(NULL,
						      GTK_DIALOG_MODAL,
						      GTK_MESSAGE_QUESTION,
						      GTK_BUTTONS_YES_NO,
						      "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

    respond = gtk_dialog_run((GtkDialog *) Dlg);
    if (respond == GTK_RESPONSE_YES)
	do_graph_layout(view->g[view->activeGraph], GVK_FDP, 0);
    gtk_object_destroy((GtkObject *) Dlg);


}





//test call back function delete later


#ifdef UNUSED
static void callback(GtkWidget * widget, gpointer data)
{
    g_print("Hello again - %s was pressed\n", (char *) data);
}
#endif


void on_newNode(GtkWidget * button, gpointer user_data)
{
    int *a;
    int *b;
    a = NEW(int);
    b = NEW(int);
}


void dlgOpenGraph_OK_Clicked(GtkWidget * button, gpointer data)
{
    g_print("ok is pressed - %i was pressed\n", *(int *) data);


}


void graph_select_change(GtkWidget * widget, gpointer user_data)
{
    gint active_graph;


    if (!view->SignalBlock) {
	active_graph = gtk_combo_box_get_active((GtkComboBox *) widget);

	if (active_graph > -1) {
	    view->activeGraph = active_graph;
	    refreshControls(view);
	}
    }
}

//dlgOpenGraph btnOK clicked
void on_dlgOpenGraph_btnOK_clicked(GtkWidget * widget, gpointer user_data)
{

/*	GTK_RESPONSE_OK     = -5,
	GTK_RESPONSE_CANCEL = -6,
	GTK_RESPONSE_CLOSE  = -7,
	GTK_RESPONSE_YES    = -8,
	GTK_RESPONSE_NO     = -9,
	GTK_RESPONSE_APPLY  = -10,
	GTK_RESPONSE_HELP   = -11 */

    if (update_graph_properties(view->g[view->activeGraph]))
	gtk_dialog_response((GtkDialog *)
			    glade_xml_get_widget(xml, "dlgOpenGraph"),
			    GTK_RESPONSE_OK);
}

//dlgOpenGraph btncancelclicked
void on_dlgOpenGraph_btncancel_clicked(GtkWidget * widget,
				       gpointer user_data)
{
    gtk_dialog_response((GtkDialog *)
			glade_xml_get_widget(xml, "dlgOpenGraph"),
			GTK_RESPONSE_CANCEL);

}







void attr_widgets_modifiedSlot(GtkWidget * widget, gpointer user_data)
{
    attr_widgets_modified[(int) user_data] = 1;
    g_print("attr changed signal..incoming data : %i\n", (int) user_data);

}



void frmObject_set_scroll(GtkWidget * widget, gpointer user_data)
{
    g_print("scroll resize required\n");

}

void frmObjectBtnOK_clicked(GtkWidget * widget, gpointer user_data)
{
    //call function to update object values
    update_object_properties(frmObjectTypeIndex, frmObjectg);
    if (GD_TopView(view->g[view->activeGraph]) == 0)
	do_graph_layout(view->g[view->activeGraph], 0, 1);
    else {
	set_update_required(view->Topview);
	deselect_all(view->g[view->activeGraph]);
    }

    gtk_widget_hide(glade_xml_get_widget(xml, "frmObject"));
}

void frmObjectBtnCancel_clicked(GtkWidget * widget, gpointer user_data)
{
    gtk_widget_hide(glade_xml_get_widget(xml, "frmObject"));

}
