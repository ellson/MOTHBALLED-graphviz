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

//file
char buf[255];
void mNewSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mNewSlot \n");
}

  void mOpenSlot (GtkWidget *widget,gpointer user_data)
{
	GtkWidget *dialog;
	GtkFileFilter* filter;

	printf("mOpenSlot \n");

	filter=gtk_file_filter_new ();
	gtk_file_filter_add_pattern(filter,"*.dot");
	dialog = gtk_file_chooser_dialog_new ("Open File",
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	gtk_file_chooser_set_filter(dialog,filter);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	  {
		char *filename;
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		add_graph_to_viewport_from_file (filename);
	    g_free (filename);
	}
	
	gtk_widget_destroy (dialog);
	gtk_widget_destroy (filter);
}
  void mSaveSlot (GtkWidget *widget,gpointer user_data)
{
	
	save_graph();	//save without prompt

}
  void mSaveAsSlot (GtkWidget *widget,gpointer user_data)
{
	save_as_graph(); //save with prompt
}
  void mOptionsSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mOptionsSlot\n");
}

  void mQuitSlot (GtkWidget *widget,gpointer user_data)
{
	//1 check all graphs 1 by 1 to see any unsaved
	int respond;
	int gIndex=0;
	int active_graph=view->activeGraph ; //stores the active graph in case quit aborted
	for (gIndex ; gIndex < view->graphCount ; gIndex++)
	{
		view->activeGraph=gIndex;
		if(((custom_graph_data*)AGDATA(view->g[view->activeGraph]))->Modified)
		{
			sprintf(buf,"graph %s has been modified \n , would you like to save it before quitting the the program?",((custom_graph_data*)AGDATA(view->g[view->activeGraph]))->GraphName);
			Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_NONE,
                                  buf);
			gtk_dialog_add_button (Dlg,"YES",100);
			gtk_dialog_add_button (Dlg,"NO",101);
			gtk_dialog_add_button (Dlg,"Cancel",102);
			
			respond=gtk_dialog_run (Dlg);
			if (respond == 100)	//save and continue
			{
				if(!save_graph())
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
  void mCutSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mCutSlot\n");
}

  void mCopySlot (GtkWidget *widget,gpointer user_data)
{
	printf("mCopySlot\n");
}

  void mPasteSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mPasteSlot\n");
}

  void mDeleteSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mDeleteSlot\n");
}



//view
  void mShowToolBoxSlot(GtkWidget *widget,gpointer user_data)
{
		
	    gtk_widget_hide(glade_xml_get_widget(xml, "frmToolBox"));
		gtk_widget_show(glade_xml_get_widget(xml, "frmToolBox"));
		gtk_window_set_keep_above           (glade_xml_get_widget(xml, "frmToolBox"),1);
	
}
void mShowHostSelectionSlot(GtkWidget *widget,gpointer user_data)
{
		
	    gtk_widget_hide(glade_xml_get_widget(xml, "frmHostSelection"));
		gtk_widget_show(glade_xml_get_widget(xml, "frmHostSelection"));
		gtk_window_set_keep_above           (glade_xml_get_widget(xml, "frmHostSelection"),1);
	
}


//Graph
  void mNodeListSlot (GtkWidget *widget,gpointer user_data)
{
	execute_tv_nodes();
}

  void mNewNodeSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mNewNodeSlot\n");
}

  void mNewEdgeSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mNewEdgeSlot\n");
}
  void mNewClusterSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mNewClusterSlot\n");
}

  void mGraphPropertiesSlot (GtkWidget *widget,gpointer user_data)
{
	int respond;
	//there has to be an active graph to open the graph prop page
	if(view->activeGraph > -1)
	{
		load_graph_properties(view->g[view->activeGraph]);//load from graph to gui		
		gtk_dialog_set_response_sensitive   (glade_xml_get_widget(xml, "dlgOpenGraph"),1,1);
		gtk_dialog_set_response_sensitive   (glade_xml_get_widget(xml, "dlgOpenGraph"),2,1);
		respond=gtk_dialog_run (glade_xml_get_widget(xml, "dlgOpenGraph"));
		//need to hide the dialog , again freaking GTK!!!!
		gtk_widget_hide(glade_xml_get_widget(xml, "dlgOpenGraph"));
	}
}
  void mClusterPropertiesSlot (GtkWidget *widget,gpointer user_data)
{
	if(  ((custom_graph_data*)AGDATA(view->g[view->activeGraph]))->selectedGraphsCount > 0)
	{
		gtk_widget_hide(glade_xml_get_widget(xml, "frmObject"));
		gtk_widget_show(glade_xml_get_widget(xml, "frmObject"));
		load_object_properties(1,view->g[view->activeGraph]);
	}
	else
	{
		Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_OK,
                                  "You need to select some clusters first!");
		respond=gtk_dialog_run (Dlg);
		gtk_widget_hide(Dlg);

	}
}
  void mNodePropertiesSlot (GtkWidget *widget,gpointer user_data)
{
	if(  ((custom_graph_data*)AGDATA(view->g[view->activeGraph]))->selectedNodesCount > 0)
	{
		gtk_widget_hide(glade_xml_get_widget(xml, "frmObject"));
		gtk_widget_show(glade_xml_get_widget(xml, "frmObject"));
		load_object_properties(2,view->g[view->activeGraph]);
	}
	else
	{
		Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_OK,
                                  "You need to select some nodes first!");
		respond=gtk_dialog_run (Dlg);
		gtk_widget_hide(Dlg);

	}
}
  void mEdgePropertiesSlot (GtkWidget *widget,gpointer user_data)
{
	if(  ((custom_graph_data*)AGDATA(view->g[view->activeGraph]))->selectedEdgesCount > 0)
	{
		gtk_widget_hide(glade_xml_get_widget(xml, "frmObject"));
		gtk_widget_show(glade_xml_get_widget(xml, "frmObject"));
		load_object_properties(3,view->g[view->activeGraph]);
	}
	else
	{
		Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_OK,
                                  "You need to select some Edges first!");
		respond=gtk_dialog_run (Dlg);
		gtk_widget_hide(Dlg);
	}

}


  void mShowCodeSlot (GtkWidget *widget,gpointer user_data)
{
	printf("mShowCodeSlot\n");
}
  void mDotSlot (GtkWidget *widget,gpointer user_data)
{
	GdkCursor* cursor;
	GdkWindow* w;
	Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_YES_NO,
                                  "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

	respond=gtk_dialog_run (Dlg);
	if (respond == GTK_RESPONSE_YES)	
		do_graph_layout(view->g[view->activeGraph],0,0);
	gtk_object_destroy (Dlg);

	cursor = gdk_cursor_new(GDK_HAND2);
	w=glade_xml_get_widget(xml, "frmMain");
   gdk_window_set_cursor ((GTK_WIDGET(view->drawing_area)->window), cursor);
	gdk_cursor_destroy(cursor);
}

  void mNeatoSlot (GtkWidget *widget,gpointer user_data)
{
	Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_YES_NO,
                                  "This will change the graph layout\n all your position changes will be lost\n Are you sure?");
	respond=gtk_dialog_run (Dlg);
	if (respond == GTK_RESPONSE_YES)	
		do_graph_layout(view->g[view->activeGraph],1,0);
	gtk_object_destroy (Dlg);
}

  void mTwopiSlot (GtkWidget *widget,gpointer user_data)
{
	Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_YES_NO,
                                  "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

	respond=gtk_dialog_run (Dlg);
	if (respond == GTK_RESPONSE_YES)	
		do_graph_layout(view->g[view->activeGraph],2,0);
	gtk_object_destroy (Dlg);

}

  void mCircoSlot (GtkWidget *widget,gpointer user_data)
{
	Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_YES_NO,
                                  "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

	respond=gtk_dialog_run (Dlg);
	if (respond == GTK_RESPONSE_YES)	
		do_graph_layout(view->g[view->activeGraph],3,0);
	gtk_object_destroy (Dlg);
}

  void mFdpSlot (GtkWidget *widget,gpointer user_data)
{
	int ind=0;
	
	Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_YES_NO,
                                  "This will change the graph layout\n all your position changes will be lost\n Are you sure?");

	respond=gtk_dialog_run (Dlg);
	if (respond == GTK_RESPONSE_YES)	
		do_graph_layout(view->g[view->activeGraph],4,0);
	gtk_object_destroy (Dlg);
}

//select
  void  mSelectAllSlot(GtkWidget *widget,gpointer user_data)
{
	select_all(view->g[view->activeGraph]);
}


  void  mUnselectAllSlot(GtkWidget *widget,gpointer user_data)
{
	deselect_all(view->g[view->activeGraph]);
}

  void mSelectAllNodesSlot(GtkWidget *widget,gpointer user_data)
{
	select_all_nodes(view->g[view->activeGraph]);
}

  void mSelectAllEdgesSlot(GtkWidget *widget,gpointer user_data)
{
	select_all_edges(view->g[view->activeGraph]);
}

void mSelectAllClustersSlot(GtkWidget *widget,gpointer user_data)
{
	select_all_graphs(view->g[view->activeGraph]);
}


void mUnselectAllNodesSlot(GtkWidget *widget,gpointer user_data)
{
	deselect_all_nodes(view->g[view->activeGraph]);
}

void mUnselectAllEdgesSlot(GtkWidget *widget,gpointer user_data)
{
	deselect_all_edges(view->g[view->activeGraph]);
}


void mUnselectAllClustersSlot(GtkWidget *widget,gpointer user_data)
{
	deselect_all_graphs(view->g[view->activeGraph]);
}

void mSingleSelectSlot(GtkWidget *widget,gpointer user_data)
{
	printf("mSingleSelectSlot\n");
}

void mSelectAreaSlot(GtkWidget *widget,gpointer user_data)
{
	printf("mSelectAreaSlot\n");
}

void mSelectAreaXSlot(GtkWidget *widget,gpointer user_data)
{
	printf("mSelectAreaXSlot\n");

}

//help
void mAbout(GtkWidget *widget,gpointer user_data)
{
	printf("mAbout\n");
}

void mHelp(GtkWidget *widget,gpointer user_data)
{
	printf("mHelp\n");
}
