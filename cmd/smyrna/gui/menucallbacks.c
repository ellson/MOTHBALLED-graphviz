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

#include "menucallbacks.h"
#include "viewport.h"
#include "tvnodes.h"
#include "selection.h"
#include "topviewsettings.h"

//file
char buf[255];
void mNewSlot(GtkWidget * widget, gpointer user_data)
{
}

void mOpenSlot(GtkWidget * widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkFileFilter *filter;


    filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.dot");
    dialog = gtk_file_chooser_dialog_new("Open File",
					 NULL,
					 GTK_FILE_CHOOSER_ACTION_OPEN,
					 GTK_STOCK_CANCEL,
					 GTK_RESPONSE_CANCEL,
					 GTK_STOCK_OPEN,
					 GTK_RESPONSE_ACCEPT, NULL);

    gtk_file_chooser_set_filter((GtkFileChooser *) dialog, filter);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
	char *filename;
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

	add_graph_to_viewport_from_file(filename);
	g_free(filename);
	}

    gtk_widget_destroy(dialog);
    gtk_widget_destroy((GtkWidget *) filter);
}

void mSaveSlot(GtkWidget * widget, gpointer user_data)
{

    save_graph();		//save without prompt

}

void mSaveAsSlot(GtkWidget * widget, gpointer user_data)
{
    save_as_graph();		//save with prompt
}

void mOptionsSlot(GtkWidget * widget, gpointer user_data)
{
}

void mQuitSlot(GtkWidget * widget, gpointer user_data)
{
    //1 check all graphs 1 by 1 to see any unsaved
    int respond;
    int gIndex;
#ifdef UNUSED
    int active_graph = view->activeGraph;	//stores the active graph in case quit aborted
#endif
    for (gIndex = 0; gIndex < view->graphCount; gIndex++) {
	view->activeGraph = gIndex;
	if (GD_Modified(view->g[view->activeGraph])) {
	    sprintf(buf,
		    "graph %s has been modified \n , would you like to save it before quitting the the program?",
		     GD_GraphName(view->g[view->activeGraph]));
	    Dlg =
		(GtkMessageDialog *) gtk_message_dialog_new(NULL,
							    GTK_DIALOG_MODAL,
							    GTK_MESSAGE_QUESTION,
							    GTK_BUTTONS_NONE,
							    buf);
	    gtk_dialog_add_button((GtkDialog *) Dlg, "YES", 100);
	    gtk_dialog_add_button((GtkDialog *) Dlg, "NO", 101);
	    gtk_dialog_add_button((GtkDialog *) Dlg, "Cancel", 102);

	    respond = gtk_dialog_run((GtkDialog *) Dlg);
	    if (respond == 100)	//save and continue
	    {
		if (!save_graph())
		    break;
	    }
	    if (respond == 102)	//save and continue
	    {
		break;
	    }
	}
    }
    gtk_main_quit();
}


//edit
void mCutSlot(GtkWidget * widget, gpointer user_data)
{
}

void mCopySlot(GtkWidget * widget, gpointer user_data)
{
}

void mPasteSlot(GtkWidget * widget, gpointer user_data)
{
}

void mDeleteSlot(GtkWidget * widget, gpointer user_data)
{
}
void mTopviewSettingsSlot(GtkWidget * widget, gpointer user_data)
{
    show_settings_form();
}



//view
void mShowToolBoxSlot(GtkWidget * widget, gpointer user_data)
{

    gtk_widget_hide(glade_xml_get_widget(xml, "frmToolBox"));
    gtk_widget_show(glade_xml_get_widget(xml, "frmToolBox"));
    gtk_window_set_keep_above((GtkWindow *)
			      glade_xml_get_widget(xml, "frmToolBox"), 1);

}

void mShowHostSelectionSlot(GtkWidget * widget, gpointer user_data)
{

    gtk_widget_hide(glade_xml_get_widget(xml, "frmHostSelection"));
    gtk_widget_show(glade_xml_get_widget(xml, "frmHostSelection"));
    gtk_window_set_keep_above((GtkWindow *)
			      glade_xml_get_widget(xml,
						   "frmHostSelection"), 1);

}

void mMenuZoom(GtkWidget * widget, gpointer user_data)
{
    view->mouse.mouse_mode = MM_ZOOM;
}

void mMenuPan(GtkWidget * widget, gpointer user_data)
{
    view->mouse.mouse_mode = MM_PAN;
}


//Graph
void mNodeListSlot(GtkWidget * widget, gpointer user_data)
{
    execute_tv_nodes();
}

void mNewNodeSlot(GtkWidget * widget, gpointer user_data)
{
}

void mNewEdgeSlot(GtkWidget * widget, gpointer user_data)
{
}
void mNewClusterSlot(GtkWidget * widget, gpointer user_data)
{
}

void mGraphPropertiesSlot(GtkWidget * widget, gpointer user_data)
{
    int respond;
    //there has to be an active graph to open the graph prop page
    if (view->activeGraph > -1) {
	load_graph_properties(view->g[view->activeGraph]);	//load from graph to gui              
	gtk_dialog_set_response_sensitive((GtkDialog *)
					  glade_xml_get_widget(xml,
							       "dlgOpenGraph"),
					  1, 1);
	gtk_dialog_set_response_sensitive((GtkDialog *)
					  glade_xml_get_widget(xml,
							       "dlgOpenGraph"),
					  2, 1);
	respond =
	    gtk_dialog_run((GtkDialog *)
			   glade_xml_get_widget(xml, "dlgOpenGraph"));
	//need to hide the dialog , again freaking GTK!!!!
	gtk_widget_hide(glade_xml_get_widget(xml, "dlgOpenGraph"));
    }
}
static void mPropertiesSlot(int count, gve_element element)
{
    if (count > 0) {
	gtk_widget_hide(glade_xml_get_widget(xml, "frmObject"));
	gtk_widget_show(glade_xml_get_widget(xml, "frmObject"));
	load_object_properties(element, view->g[view->activeGraph]);
    } else {
	char buf[BUFSIZ];
	sprintf (buf, "You need to select some %ss first!",element2s(element)); 
	Dlg = (GtkMessageDialog *) gtk_message_dialog_new(NULL,
							  GTK_DIALOG_MODAL,
							  GTK_MESSAGE_QUESTION,
							  GTK_BUTTONS_OK,
							  buf);
	respond = gtk_dialog_run((GtkDialog *) Dlg);
	gtk_widget_hide((GtkWidget *) Dlg);

    }
}
void mClusterPropertiesSlot(GtkWidget * widget, gpointer user_data)
{
    mPropertiesSlot (GD_selectedGraphsCount(view->g[view->activeGraph]),GVE_CLUSTER);
}
void mNodePropertiesSlot(GtkWidget * widget, gpointer user_data)
{
    mPropertiesSlot (GD_selectedNodesCount(view->g[view->activeGraph]), GVE_NODE);
}
void mEdgePropertiesSlot(GtkWidget * widget, gpointer user_data)
{
    mPropertiesSlot (GD_selectedEdgesCount(view->g[view->activeGraph]), GVE_EDGE);
}

void mShowCodeSlot(GtkWidget * widget, gpointer user_data)
{
}
static mSlot (GtkWidget * widget, gpointer user_data, gvk_layout layout, int doCursor)
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
	do_graph_layout(view->g[view->activeGraph], layout, 0);
    gtk_object_destroy((GtkObject *) Dlg);

    if (!doCursor) return;

    cursor = gdk_cursor_new(GDK_HAND2);
    w = (GdkWindow *) glade_xml_get_widget(xml, "frmMain");
    gdk_window_set_cursor((GdkWindow *) view->drawing_area->window,
			  cursor);
    gdk_cursor_destroy(cursor);
}

void mDotSlot(GtkWidget * widget, gpointer user_data)
{
    mSlot (widget, user_data, GVK_DOT, 1);
}

void mNeatoSlot(GtkWidget * widget, gpointer user_data)
{
    mSlot (widget, user_data, GVK_NEATO, 0);
}

void mTwopiSlot(GtkWidget * widget, gpointer user_data)
{
    mSlot (widget, user_data, GVK_TWOPI, 0);
}

void mCircoSlot(GtkWidget * widget, gpointer user_data)
{
    mSlot (widget, user_data, GVK_CIRCO, 0);
}

void mFdpSlot(GtkWidget * widget, gpointer user_data)
{
    mSlot (widget, user_data, GVK_FDP, 0);
}

//select
void mSelectAllSlot(GtkWidget * widget, gpointer user_data)
{
    select_all(view->g[view->activeGraph]);
}


void mUnselectAllSlot(GtkWidget * widget, gpointer user_data)
{
    deselect_all(view->g[view->activeGraph]);
}

void mSelectAllNodesSlot(GtkWidget * widget, gpointer user_data)
{
    select_all_nodes(view->g[view->activeGraph]);
}

void mSelectAllEdgesSlot(GtkWidget * widget, gpointer user_data)
{
    select_all_edges(view->g[view->activeGraph]);
}

void mSelectAllClustersSlot(GtkWidget * widget, gpointer user_data)
{
    select_all_graphs(view->g[view->activeGraph]);
}


void mUnselectAllNodesSlot(GtkWidget * widget, gpointer user_data)
{
    deselect_all_nodes(view->g[view->activeGraph]);
}

void mUnselectAllEdgesSlot(GtkWidget * widget, gpointer user_data)
{
    deselect_all_edges(view->g[view->activeGraph]);
}


void mUnselectAllClustersSlot(GtkWidget * widget, gpointer user_data)
{
    deselect_all_graphs(view->g[view->activeGraph]);
}

void mSingleSelectSlot(GtkWidget * widget, gpointer user_data)
{
    view->mouse.mouse_mode = MM_SINGLE_SELECT;
}

void mSelectAreaSlot(GtkWidget * widget, gpointer user_data)
{
    view->mouse.mouse_mode = MM_RECTANGULAR_SELECT;
}

void mSelectAreaXSlot(GtkWidget * widget, gpointer user_data)
{
    view->mouse.mouse_mode = MM_RECTANGULAR_X_SELECT;
}

//help
void mAbout(GtkWidget * widget, gpointer user_data)
{
}

void mHelp(GtkWidget * widget, gpointer user_data)
{
}
