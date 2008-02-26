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




#include <stdio.h>

#include "compat.h"
#include <stdlib.h>
#include "gui.h"
#include "abstring.h"
#include <glade/glade.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include "viewport.h"

static char guibuffer[255];	//general purpose buffer

GdkWindow* window1;
GtkWidget *statusbar1;

GladeXML *xml;			//global libglade vars
GtkWidget *gladewidget;

//1 subgraph 2 node 3 edge
int frmObjectTypeIndex;
Agraph_t* frmObjectg;


GtkComboBox* cbSelectGraph;		//combo at top left

GtkWidget* AttrWidgets[MAXIMUM_WIDGET_COUNT];	
GtkWidget* AttrLabels[MAXIMUM_WIDGET_COUNT];	
int attr_widgets_modified[MAXIMUM_WIDGET_COUNT];
int widgetcounter;	//number of attributes counted dynamically, might be removed in the future 
attribute attr[MAXIMUM_WIDGET_COUNT];	



//loads object properties form and does some fixes
//call this function only ones
void create_object_properties()
{
	char** testRows;
	char* data0="TEST0";
	char* data1="TEST1";
	char* data2="TEST2";
	char* data3="TEST3";
	char* data4="TEST4";
	GladeXML *xml;
    GtkWidget *widget;
    xml = glade_xml_new(GTKTOPVIEW_GLADE, NULL, NULL);

	widget = glade_xml_get_widget(xml, "listPoints");
	gtk_clist_set_column_title          (widget,0,"Def");
	gtk_clist_set_column_title          (widget,1,"x");
	gtk_clist_set_column_title          (widget,2,"y");
	gtk_clist_set_column_title          (widget,3,"z");
	gtk_clist_column_titles_show(widget);
	gtk_widget_show (widget);
	widget = glade_xml_get_widget(xml, "win");
	gtk_widget_show (widget);


}
//call this after create_object_properties()
void object_properties_node_init()
{
	GladeXML *xml;
	GtkWidget *widget;
	
}
void object_properties_edge_init()	//customize window for Edges
{
}
void object_properties_cluster_init()	//customize window for Cluster
{

}
void object_properties_graph_init()	//customize window for Graph , this shows the graph default values
{

}


void graph_properties_init(int newgraph)	//initialize little open graph dialog
{
	//newgraph 0 : open graph mode
	//newgraph 1 : new graph mode


	gint result=0;
	xml = glade_xml_new(GTKTOPVIEW_GLADE, NULL, NULL);
	gladewidget = glade_xml_get_widget(xml, "entryGraphFileName");

	//signals
	//OK
	gladewidget = glade_xml_get_widget(xml, "btnOK");


	g_signal_connect ((gpointer) gladewidget, "clicked", G_CALLBACK(dlgOpenGraph_OK_Clicked),
                    &newgraph);



	if(newgraph)
	{
		gladewidget = glade_xml_get_widget(xml, "entryGraphName");
		gtk_entry_set_text (gladewidget,"Untitled");
		gladewidget = glade_xml_get_widget(xml, "entryGraphFileName");
		gtk_entry_set_text (gladewidget,"Untitled.dot");
	}
	else
	{
		gladewidget = glade_xml_get_widget(xml, "entryGraphName");
		gtk_entry_set_text (gladewidget,"");
		gladewidget = glade_xml_get_widget(xml, "entryGraphFileName");
		gtk_entry_set_text (gladewidget,"");
	}
	gladewidget = glade_xml_get_widget(xml, "dlgOpenGraph");
	result = gtk_dialog_run (GTK_DIALOG (gladewidget));
}

GtkComboBox* get_SelectGraph()
{
	GtkComboBox* cb;
	if (!cbSelectGraph)
	{

		cb=gtk_combo_box_new_text();
		gtk_widget_show (cb);
	    gladewidget = glade_xml_get_widget(xml, "layout6");	
	    gtk_box_pack_start (GTK_BOX (gladewidget), cb, FALSE, FALSE, 0);

		
		gtk_layout_put                  (gladewidget,cb,780,3);
		//signal
		g_signal_connect ((gpointer) cb, "changed", G_CALLBACK(graph_select_change),
	                    NULL);


		return cb;
	}
	else
		return cbSelectGraph;
}


void Color_Widget_bg (char* colorstring, GtkWidget *widget)
{
	GdkColor color;
	gdk_color_parse (colorstring, &color);
	  gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, &color);
	  gtk_widget_modify_base(widget, GTK_STATE_NORMAL, &color);
}


#if 0
void Color_Widget_fg (int r, int g, int b, GtkWidget *widget)
{
	GdkColor color;
	gdk_color_parse ("red", &color);
	gtk_widget_modify_fg (widget, GTK_STATE_NORMAL, &color);

}
#endif

void load_graph_properties(Agraph_t* graph)
{
	//dlgOpenGraph , GtkDialog
	gtk_entry_set_text(glade_xml_get_widget(xml, "entryGraphName"),((custom_graph_data*)AGDATA(graph))->GraphName);
	gtk_entry_set_text(glade_xml_get_widget(xml, "entryGraphFileName"),((custom_graph_data*)AGDATA(graph))->GraphFileName);

	gtk_combo_box_set_active (glade_xml_get_widget(xml, "cbLayout"),((custom_graph_data*)AGDATA(graph))->Engine);
	gtk_toggle_button_set_active(glade_xml_get_widget(xml, "chkVisible"),((custom_graph_data*)AGDATA(graph))->AlwaysShow);
	gtk_toggle_button_set_active(glade_xml_get_widget(xml, "chkLocked"),((custom_graph_data*)AGDATA(graph))->Locked);
	gtk_toggle_button_set_active(glade_xml_get_widget(xml, "chkTopView"),((custom_graph_data*)AGDATA(graph))->TopView);
}

int update_graph_properties(Agraph_t* graph) //updates graph from gui
{
	FILE* file;
	int respond=0;
	int id=0;
	//check the graph name	should not be duplicated graph names
	for (id=0;id < view->graphCount ; id ++)
	{
		if (graph != view->g[id])	
		{
			if(strcasecmp (gtk_entry_get_text(glade_xml_get_widget(xml, "entryGraphName")),((custom_graph_data*)AGDATA(view->g[id]))->GraphName ) == 0)
			{
				
							Dlg=gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_WARNING,
                                  GTK_BUTTONS_OK,
                                  "There is another graph with this name!");
						respond=gtk_dialog_run (Dlg);
						gtk_object_destroy (Dlg);
					return 0;

			}

		}
	}

	//check if file is changed
	if (strcasecmp (gtk_entry_get_text(glade_xml_get_widget(xml, "entryGraphFileName")),((custom_graph_data*)AGDATA(graph))->GraphFileName ) != 0)
	{
	
	
		if (file = fopen(gtk_entry_get_text(glade_xml_get_widget(xml, "entryGraphFileName")), "r"))
	    {
			fclose(file);
			Dlg = gtk_message_dialog_new (NULL,
									GTK_DIALOG_MODAL,
									GTK_MESSAGE_QUESTION,
	                                  GTK_BUTTONS_YES_NO,
									"File name you have entered already exists\n,this will cause overwriting on existing file.\nAre you sure?");
				respond=gtk_dialog_run (Dlg);
				gtk_object_destroy (Dlg);
	
				if (respond==GTK_RESPONSE_NO)
					return 0;
		}
		//now check if filename is legal, try to open it to write
	    if (file = fopen(gtk_entry_get_text(glade_xml_get_widget(xml, "entryGraphFileName")), "w"))
	        fclose(file);
		else
		{
								Dlg=gtk_message_dialog_new (NULL,
									GTK_DIALOG_MODAL,
	                                  GTK_MESSAGE_WARNING,
									GTK_BUTTONS_OK,
									"File name is invalid or I/O error!");
				respond=gtk_dialog_run (Dlg);
				gtk_object_destroy (Dlg);

			return 0;
		}
		
	}



	//if it comes so far graph deserves new values



	((custom_graph_data*)AGDATA(graph))->GraphName=gtk_entry_get_text(glade_xml_get_widget(xml, "entryGraphName"));
	((custom_graph_data*)AGDATA(graph))->GraphFileName=gtk_entry_get_text(glade_xml_get_widget(xml, "entryGraphFileName"));

	((custom_graph_data*)AGDATA(graph))->AlwaysShow=gtk_toggle_button_get_active(glade_xml_get_widget(xml, "chkVisible"));
	((custom_graph_data*)AGDATA(graph))->Locked=gtk_toggle_button_get_active(glade_xml_get_widget(xml, "chkLocked"));
	((custom_graph_data*)AGDATA(graph))->TopView=gtk_toggle_button_get_active(glade_xml_get_widget(xml, "chkTopView"));


	//check if the engine has been changed, if so do new layout
	if( ((custom_graph_data*)AGDATA(graph))->Engine != gtk_combo_box_get_active (glade_xml_get_widget(xml, "cbLayout")) )
	{
			Dlg = gtk_message_dialog_new (NULL,
								GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_YES_NO,
                                  "You have changed the layout of the graph,this will change the graph layout\n all your position changes will be lost\n Are you sure?");
						respond=gtk_dialog_run (Dlg);
					if (respond == GTK_RESPONSE_YES)	
						do_graph_layout(graph,gtk_combo_box_get_active (glade_xml_get_widget(xml, "cbLayout")),0);
					gtk_object_destroy (Dlg);
	}
	return 1;
}



int load_object_properties(int typeIndex,Agraph_t* g)	//load  from object to gui;
{
	//typeindex 0:graph 1:cluster 2:Node  3:Edge
	//load attr from first selected object
	GtkLayout* layout;
	GdkColor color;
	FILE * file;
    char buf[100];
	char string [100];
	char *ss;
	int ind=0;
	int Y=45;
	int X=90;
	int Yinc=25;
	int Xinc=30;
	int OriginalX= 90;
	int OriginalY= 45;
	int widget_per_page=21;
	void* obj;
	char line [ 128 ];
	float a,b;
	char* pch;
	layout=glade_xml_get_widget(xml, "layout4");
	frmObjectTypeIndex=typeIndex;
	frmObjectg=g;
	widgetcounter=0;
				//values should be set from first selected object
				//according to object type (typeIndex) set the reference object
		switch (typeIndex)	//typeindex 0 means new object
		{
		case 1:	//graph  sub graph (cluster)
			obj=((custom_graph_data*)AGDATA(g))->selectedGraphs[0];
			break;
		case 2:	//Node
			obj=((custom_graph_data*)AGDATA(g))->selectedNodes[0];
			break;
		case 3:	//Edge
			obj=((custom_graph_data*)AGDATA(g))->selectedEdges[0];
			break;
		}
		for (widgetcounter=0;widgetcounter < MAXIMUM_WIDGET_COUNT ;widgetcounter ++)
		{
			//create the labels and widget here
			attr[widgetcounter].ComboValuesCount=0;
			attr[widgetcounter].ComboValues='\0';

			if (!AttrWidgets[widgetcounter])
			{
				AttrLabels[widgetcounter]=gtk_label_new (attr[widgetcounter].Name);
				switch (attr[widgetcounter].Type)
				{
				case 'F':	//float
					AttrWidgets[widgetcounter]=gtk_spin_button_new_with_range(0,100,0.001);
					g_signal_connect ((gpointer) AttrWidgets[widgetcounter], "value-changed", G_CALLBACK(attr_widgets_modifiedSlot),widgetcounter);
                    
					break;
				case 'C':	//color box
					AttrWidgets[widgetcounter]=gtk_color_button_new();
					gtk_widget_set_size_request(AttrWidgets[widgetcounter],50,23);
					g_signal_connect ((gpointer) AttrWidgets[widgetcounter], "color-set", G_CALLBACK(attr_widgets_modifiedSlot),widgetcounter);

					break;
				default:	//alphanumreric		GTK Entry
					AttrWidgets[widgetcounter]=gtk_entry_new();
					gtk_widget_set_size_request(AttrWidgets[widgetcounter],130,23);
					g_signal_connect ((gpointer) AttrWidgets[widgetcounter], "changed", G_CALLBACK(attr_widgets_modifiedSlot),widgetcounter);

					break;					
				}
				attr[widgetcounter].attrWidget=AttrWidgets[widgetcounter];
			} 
			//locate widget on the GtkLayout* layout
			if (attr[widgetcounter].ApplyTo[typeIndex]==1)
			{
				gtk_layout_put	(layout,AttrWidgets[widgetcounter],X,Y);					
				gtk_layout_put	(layout,AttrLabels[widgetcounter],X-80,Y);					
				gtk_widget_show(AttrWidgets[widgetcounter]);
				gtk_widget_show(AttrLabels[widgetcounter]);
				Y=Y+Yinc;
				switch(attr[widgetcounter].Type)
				{
				case 'F':
					if (agget(obj, attr[widgetcounter].Name))
						gtk_spin_button_set_value    (AttrWidgets[widgetcounter],atof(agget(obj, attr[widgetcounter].Name)) );					
					else
						gtk_spin_button_set_value    (AttrWidgets[widgetcounter],atof(attr[widgetcounter].Default) );					
					break;
				case 'C':
					if (agget(obj, attr[widgetcounter].Name))
						SetGdkColor(&color,agget(obj, attr[widgetcounter].Name));
					else
						SetGdkColor(&color,attr[widgetcounter].Default);

					gtk_color_button_set_color(AttrWidgets[widgetcounter],&color);
					break;
				default:						
					if (agget(obj, attr[widgetcounter].Name))
						gtk_entry_set_text(AttrWidgets[widgetcounter],agget(obj, attr[widgetcounter].Name));				
					else
						gtk_entry_set_text(AttrWidgets[widgetcounter],attr[widgetcounter].Default);				

				}
				gtk_widget_show(AttrWidgets[widgetcounter]);
				gtk_widget_show(AttrLabels[widgetcounter]);
			}
			else
			{
				gtk_widget_hide(AttrWidgets[widgetcounter]);
				gtk_widget_hide(AttrLabels[widgetcounter]);

			}
			if (Y > widget_per_page*Yinc)
			{
				X=320;
				Y=OriginalY;
			}
			attr_widgets_modified[widgetcounter]=0;		//set to unmodified
		}

		
	//first part, common attributes
	sprintf (buf, "%i", ((custom_object_data*)AGDATA(obj))->ID);
	gtk_entry_set_text(glade_xml_get_widget(xml, "objEntryName"),((custom_object_data*)AGDATA(obj))->ObjName);
	gtk_entry_set_text(glade_xml_get_widget(xml, "objEntryLabel"),agnameof(obj));

	gtk_toggle_button_set_active(glade_xml_get_widget(xml, "frmObjectchkVisible"),((custom_object_data*)AGDATA(obj))->Visible);
	gtk_toggle_button_set_active(glade_xml_get_widget(xml, "frmObjectchkLocked"),((custom_object_data*)AGDATA(obj))->Locked);
	gtk_toggle_button_set_active(glade_xml_get_widget(xml, "frmObjectchkHighlighted"),((custom_object_data*)AGDATA(obj))->Highlighted);
	//get the position info // show only one item is selected
	if (
		 ( (((custom_graph_data*)AGDATA(g))->selectedNodesCount == 1) && (typeIndex==2) ) 
										||
		 ( (((custom_graph_data*)AGDATA(g))->selectedEdgesCount == 1) && (typeIndex==3) ) 
										||
		 ((((custom_graph_data*)AGDATA(g))->selectedGraphsCount == 1) && (typeIndex==3) ) 

		 )
		 {
			sprintf (line, "%s", agget(obj,"pos"));
			a=atof(strtok (line,"," ));
			b=atof(strtok (NULL,"," ));
			gtk_spin_button_set_value (glade_xml_get_widget(xml,"frmObjectPosX"), a );					
			gtk_spin_button_set_value (glade_xml_get_widget(xml,"frmObjectPosY"), b );					
			gtk_spin_button_set_value (glade_xml_get_widget(xml,"frmObjectPosZ"), 0 );					
			gtk_widget_show (glade_xml_get_widget(xml,"frmObjectPosX"));
			gtk_widget_show (glade_xml_get_widget(xml,"frmObjectPosY"));
			gtk_widget_show (glade_xml_get_widget(xml,"frmObjectPosZ"));
			gtk_widget_show (glade_xml_get_widget(xml,"frmObjectlabel3"));
			gtk_label_set_text(  glade_xml_get_widget(xml,"frmObjectPosLabelX"),"X:");
			
		 }
	else
	{
			gtk_widget_hide (glade_xml_get_widget(xml,"frmObjectPosX"));
			gtk_widget_hide (glade_xml_get_widget(xml,"frmObjectPosY"));
			gtk_widget_hide (glade_xml_get_widget(xml,"frmObjectPosZ"));
			gtk_widget_hide (glade_xml_get_widget(xml,"frmObjectlabel3"));
			gtk_widget_hide (glade_xml_get_widget(xml,"frmObjectPosLabelY"));
			gtk_widget_hide (glade_xml_get_widget(xml,"frmObjectPosLabelZ"));
			switch (typeIndex)	//typeindex 0 means new object
			{
				case 1:	//graph  sub graph (cluster)
					gtk_label_set_text(glade_xml_get_widget(xml,"frmObjectPosLabelX"),"Changes that you make will be applied to all selected Clusters1");
				break;
				case 2:	//Node
					gtk_label_set_text(glade_xml_get_widget(xml,"frmObjectPosLabelX"),"Changes that you make will be applied to all selected Nodes!");
				break;
				case 3:	//Edge
					gtk_label_set_text(glade_xml_get_widget(xml,"frmObjectPosLabelX"),"Changes that you make will be applied to all selected Edges!");
				break;
			}
	}
	return 1;
}
void update_object_properties(int typeIndex,Agraph_t* g)	//updates objects from gui(node ,edge, cluster)
{
	int ind=0;
	for (ind = 0 ; ind < widgetcounter ; ind ++)
	{
		//if widget has been changed
		if	(attr_widgets_modified[ind] == 1)
		{
			switch (typeIndex)	//typeindex 0 means new object
			{
				case 1:	//graph  sub graph (cluster)
					change_selected_graph_attributes(g,attr[ind].Name,get_attribute_string_value_from_widget(&attr[ind]));
					break;
				case 2:	//Node
					change_selected_node_attributes(g,attr[ind].Name,get_attribute_string_value_from_widget(&attr[ind]));
					break;
				case 3:	//Edge
					change_selected_edge_attributes(g,attr[ind].Name,get_attribute_string_value_from_widget(&attr[ind]));
					break;
			}
		}

	}
}
char* get_attribute_string_value_from_widget(attribute* att)
{
	GdkColor color;
	switch(att->Type)
	{
		case 'F':
			sprintf(guibuffer,"%f",gtk_spin_button_get_value(att->attrWidget));			
			return guibuffer;
			break;
		case 'C':
			gtk_color_button_get_color(att->attrWidget,&color);
			sprintf(guibuffer,"#%x%x%x",color.red/255,color.green/255,color.blue/255);
			return guibuffer;
			break;
		default:						
			strcpy(guibuffer,gtk_entry_get_text(att->attrWidget));
			return guibuffer;
	}
}
void change_selected_graph_attributes (Agraph_t* g,char* attrname,char* attrvalue)
{
	int ind=0;
	agattr(g,AGRAPH,attrname,"");

	for (ind=0;ind <((custom_graph_data*)AGDATA(g))->selectedGraphsCount;ind ++ )
	{
		agset(((custom_graph_data*)AGDATA(g))->selectedGraphs[ind],attrname,attrvalue);		
	}

	
}
void change_selected_node_attributes (Agraph_t* g,char* attrname,char* attrvalue)
{
	int ind=0;
	agattr(g,AGNODE,attrname,"");
	for (ind=0;ind <((custom_graph_data*)AGDATA(g))->selectedNodesCount;ind ++ )
	{
		agset(((custom_graph_data*)AGDATA(g))->selectedNodes[ind],attrname,attrvalue);		
	}
}
void change_selected_edge_attributes (Agraph_t* g,char* attrname,char* attrvalue)
{
	int ind=0;
	agattr(g,AGEDGE,attrname,"");
	for (ind=0;ind <((custom_graph_data*)AGDATA(g))->selectedEdgesCount;ind ++ )
	{
		agset(((custom_graph_data*)AGDATA(g))->selectedEdges[ind],attrname,attrvalue);		

	}
}

void load_attributes()
{
	FILE * file;
    char line[255];
	char *ss;
	char* pch;
	int ind=0;
	int attrcount=0;
	//loads attributes from a text file
	file = fopen (GTKTOPVIEW_ATTRS, "r");
	if ( file != NULL )
	{
		while ( fgets ( line, sizeof line, file ) != NULL )
		{
			pch = strtok (line,",");
			ind=0;
			while (pch != NULL)
			{
				ss=strdup(pch);
//				ABRemove(&ss,'\"');
//				ABRemove(&ss,' ');
				pch = strtok (NULL, ",");
				switch (ind)
				{	
					case 0:
						attr[attrcount].Type=ss[0];
						break;
					case 1:
						attr[attrcount].Name=strdup(ss);
						break;
					case 2:
						attr[attrcount].Default=strdup(ss);
						break;
					case 3:
						if (strstr(ss,"ANY_ELEMENT"))
						{
							attr[attrcount].ApplyTo[0]=1;
							attr[attrcount].ApplyTo[1]=1;
							attr[attrcount].ApplyTo[2]=1;
							attr[attrcount].ApplyTo[3]=1;
						}
						else
						{
							attr[attrcount].ApplyTo[0]=strstr(ss,"GRAPH") ? 1:0;
							attr[attrcount].ApplyTo[1]=strstr(ss,"CLUSTER") ? 1:0;
							attr[attrcount].ApplyTo[2]=strstr(ss,"NODE")? 1:0;
							attr[attrcount].ApplyTo[3]=strstr(ss,"EDGE")? 1:0;
						}
						break;
					case 4:
						if (strstr(ss,"ALL_ENGINES"))
						{
							attr[attrcount].Engine[0]=1;
							attr[attrcount].Engine[1]=1;
							attr[attrcount].Engine[2]=1;
							attr[attrcount].Engine[3]=1;
							attr[attrcount].Engine[4]=1;
						}
						else
						{
							attr[attrcount].Engine[0]=strstr(ss,"DOT")? 1:0;
							attr[attrcount].Engine[1]=strstr(ss,"NEATO")? 1:0;
							attr[attrcount].Engine[2]=strstr(ss,"TWOPI")? 1:0;
							attr[attrcount].Engine[3]=strstr(ss,"CIRCO")? 1:0;
							attr[attrcount].Engine[4]=strstr(ss,"FDP")? 1:0;
						}
						break;
					default:
						attr[attrcount].ComboValues=realloc(attr[attrcount].ComboValues,sizeof(char**)*attr[attrcount].ComboValuesCount);					
						attr[attrcount].ComboValues[attr[attrcount].ComboValuesCount]=strdup(ss);
						attr[attrcount].ComboValuesCount ++;
						break;
				}
				ind++;
			}
			attrcount++;
		}
	}
}




