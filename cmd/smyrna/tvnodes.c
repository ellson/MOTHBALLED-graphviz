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

#include "topview.h"
#include "tvnodes.h"
#include "btree.h"
#include "viewport.h"
#include "selection.h"
tv_nodes TV_Nodes;
static char buf [255];
int MP_Flag=0;

tv_filter* create_tv_filter()
{
	tv_filter* f;
	f=(tv_filter*)malloc(sizeof(tv_filter));
	clear_tv_filter(f);
	return f;
}
void init_tv_nodes(tv_nodes* TV_Nodes)
{
	int i;
	TV_Nodes->filtered=0;
	TV_Nodes->activepage=-1;
	TV_Nodes->firstnodeid=0;
	TV_Nodes->pagecount=0;
	TV_Nodes->recordperpage=22;
	TV_Nodes->page_data_index=0;
	TV_Nodes->Y_Gap=25;
	TV_Nodes->Y=12;
	TV_Nodes->initial_Y=12;
	TV_Nodes->chkSelected_X=5;
	TV_Nodes->IDLabel_X=10;
	TV_Nodes->chkVisible_X=55;
	TV_Nodes->chkHighlighted_X=85;
	TV_Nodes->Data1_X=105;
	TV_Nodes->Data2_X=305;
	TV_Nodes->page_data_node_index=0;
	reset_page_History();

	//null gui elemetns
	for (i=0;i < MAX_NODE_PER_PAGE ;i ++)
	{
		TV_Nodes->TV_Node[i].chkHighlighted=NULL;
		TV_Nodes->TV_Node[i].chkSelected=NULL;
		TV_Nodes->TV_Node[i].chkVisible=NULL;
		TV_Nodes->TV_Node[i].Data1=NULL;
		TV_Nodes->TV_Node[i].Data2=NULL;
		TV_Nodes->TV_Node[i].IDLabel=NULL;
	}
	TV_Nodes->initialized=1;
}

void clear_tv_filter(tv_filter* TV_Filter)
{
	TV_Filter->highlighted=-1;
	TV_Filter->visible=-1;
	TV_Filter->min_data1=NULL;
	TV_Filter->max_data1=NULL;
	TV_Filter->min_data2=NULL;
	TV_Filter->max_data2=NULL;
	
	TV_Nodes.filtered=0;

}

extern int set_filter(tv_filter* TV_Filter,char* MinData1,char* MaxData1,char* MinData2,char* MaxData2,char* Filter_String,int selected,int visible,int highlighted)
{
	TV_Filter->selected=selected;
	TV_Filter->highlighted=highlighted;
	TV_Filter->visible=visible;
	TV_Filter->min_data1=MinData1;
	TV_Filter->max_data1=MaxData1;
	TV_Filter->min_data2=MinData2;
	TV_Filter->max_data2=MaxData2;
	TV_Filter->filter_string=Filter_String;
	TV_Nodes.filtered=1;
	return 1;
}
int reverse_selection()
{
	int i=0;
	for (i; i < view->Topview->Nodecount ; i ++)
	{
		if (((custom_object_data*)AGDATA(view->Topview->Nodes[i].Node))->Selected )
			deselect_node(view->g[view->activeGraph],view->Topview->Nodes[i].Node);
		else
			select_node(view->g[view->activeGraph],view->Topview->Nodes[i].Node);
	}
	for (i=0; i < view->Topview->Edgecount ; i ++)
	{
		if (((custom_object_data*)AGDATA(view->Topview->Edges[i].Edge))->Selected )
			deselect_edge(view->g[view->activeGraph],view->Topview->Edges[i].Edge);
		else
			select_edge(view->g[view->activeGraph],view->Topview->Edges[i].Edge);
	}
}
int validate_node(tv_node* TV_Node)
{
	static btree_node* n=0;
	char* data_attr1;
	char* data_attr2;
	char* buf;
//		n=tree_from_filter_string("([IP=\"^10.*\",min=\"0\",max=\"0\"])");
	// get attributes from graph
	data_attr1=agget(view->g[view->activeGraph],"DataAttribute1");
	data_attr2=agget(view->g[view->activeGraph],"DataAttribute2");


	if (TV_Nodes.filtered)
	{
		int valid=1;
		if((MP_Flag==1) || (!n))
		{
			n=tree_from_filter_string(TV_Nodes.filter.filter_string);
			MP_Flag=0;
			buf=agget(view->Topview->Nodes[TV_Node->index].Node,data_attr1);
			if(buf)
			{
				if (strcmp(TV_Nodes.filter.min_data1,buf))
					valid=0;
			}
		}
		if (data_attr1 && TV_Nodes.filter.max_data1 && agget(view->Topview->Nodes[TV_Node->index].Node,data_attr1))
		{
			if (strcmp(agget(view->Topview->Nodes[TV_Node->index].Node,data_attr1),TV_Nodes.filter.min_data1))
				valid=0;
		}
		//string data checks attr2
		if (data_attr2 && TV_Nodes.filter.min_data2 && agget(view->Topview->Nodes[TV_Node->index].Node,data_attr2))
		{
			if (strcmp(TV_Nodes.filter.min_data2,  agget(view->Topview->Nodes[TV_Node->index].Node,data_attr2)   ))
				valid=0;
		}
		if (data_attr2 && TV_Nodes.filter.max_data2 && agget(view->Topview->Nodes[TV_Node->index].Node,data_attr2))
		{
			if (agget(view->Topview->Nodes[TV_Node->index].Node,data_attr2),TV_Nodes.filter.min_data2)
				valid=0;
		}
		if (strlen(TV_Nodes.filter.filter_string)>0)
			valid=evaluate_expresions (TV_Node,n);
		//if show only highlighted 
		if (TV_Nodes.filter.highlighted >= 0)	
		{
			if(((custom_object_data*)AGDATA(view->Topview->Nodes[TV_Node->index].Node))->Highlighted!= TV_Nodes.filter.highlighted)
				valid=0;
		}
		//if show only visibles
		if (TV_Nodes.filter.visible >= 0)	
		{
			if(((custom_object_data*)AGDATA(view->Topview->Nodes[TV_Node->index].Node))->Visible!= TV_Nodes.filter.visible)
				valid=0;
		}
		//if show only selected
		if (TV_Nodes.filter.selected >= 0)	
		{
			if(((custom_object_data*)AGDATA(view->Topview->Nodes[TV_Node->index].Node))->Selected!= TV_Nodes.filter.selected)
				valid=0;
		}
		return valid;
	}
	else
		return 1;
}

int update_node_gui_objects(tv_node* TV_Node)
{
	char* data_attr1;
	char* data_attr2;
	char buf[255];
	GtkLayout* layout;

	// get attributes from graph
	data_attr1=agget(view->g[view->activeGraph],"DataAttribute1");
	data_attr2=agget(view->g[view->activeGraph],"DataAttribute2");

	//create if objects are null	
	layout=(GtkLayout*)glade_xml_get_widget(xml, "layoutTVData");
	//select box
	if (!TV_Node->chkSelected)
	{
		TV_Node->chkSelected=(GtkCheckButton*)gtk_check_button_new();
		gtk_layout_put	(layout,(GtkWidget*)TV_Node->chkSelected,LOCATION_X_CHKSELECTED,TV_Nodes.Y);
	}
	gtk_widget_show((GtkWidget*)TV_Node->chkSelected);
	gtk_toggle_button_set_active((GtkToggleButton*)TV_Node->chkSelected,((custom_object_data*)AGDATA(view->Topview->Nodes[TV_Node->index].Node))->Selected);

	//Id Label
	if (!TV_Node->IDLabel)
	{
		TV_Node->IDLabel=(GtkLabel*)gtk_label_new("");
		gtk_layout_put	(layout,(GtkWidget*)TV_Node->IDLabel,LOCATION_X_IDLABEL,TV_Nodes.Y);
	}
	sprintf(buf, "%d", TV_Node->index);
	gtk_label_set_text (TV_Node->IDLabel,buf);
	gtk_widget_show((GtkWidget*)TV_Node->IDLabel);

	//visible
	if (!TV_Node->chkVisible)
	{		
		TV_Node->chkVisible=(GtkCheckButton*)gtk_check_button_new();
		gtk_layout_put	(layout,(GtkWidget*)TV_Node->chkVisible,LOCATION_X_CHKVISIBLE,TV_Nodes.Y);
	}

	gtk_widget_show((GtkWidget*)TV_Node->chkVisible);
	gtk_toggle_button_set_active((GtkToggleButton*)TV_Node->chkVisible,((custom_object_data*)AGDATA(view->Topview->Nodes[TV_Node->index].Node))->Visible);
	//highlighted
	if (!TV_Node->chkHighlighted)
	{
		TV_Node->chkHighlighted=(GtkCheckButton*)gtk_check_button_new();
		gtk_layout_put	(layout,(GtkWidget*)TV_Node->chkHighlighted,LOCATION_X_CHKHIGHLIGHTED,TV_Nodes.Y);
	}
	gtk_widget_show((GtkWidget*)TV_Node->chkHighlighted);
	gtk_toggle_button_set_active((GtkToggleButton*)TV_Node->chkHighlighted,((custom_object_data*)AGDATA(view->Topview->Nodes[TV_Node->index].Node))->Highlighted);

	
	//DATA 1
	if (!TV_Node->Data1)
	{
		TV_Node->Data1=(GtkEntry*)gtk_entry_new();
		gtk_layout_put	(layout,(GtkWidget*)TV_Node->Data1,LOCATION_X_DATA1,TV_Nodes.Y);
		gtk_widget_set_size_request((GtkWidget*)TV_Node->Data1,300,23);

	}
	if(data_attr1)
	{
		gtk_entry_set_text (TV_Node->Data1,agget(view->Topview->Nodes[TV_Node->index].Node,data_attr1));
	}
	else
		gtk_entry_set_text (TV_Node->Data1,"");
	gtk_widget_show((GtkWidget*)TV_Node->Data1);

	//DATA 2
	if (!TV_Node->Data2)
	{
		TV_Node->Data2=(GtkEntry*)gtk_entry_new();
		gtk_layout_put	(layout,(GtkWidget*)TV_Node->Data2,LOCATION_X_DATA2,TV_Nodes.Y);
		gtk_widget_set_size_request((GtkWidget*)TV_Node->Data2,300,23);
	}
	if(data_attr2)
	{
		gtk_entry_set_text (TV_Node->Data2,agget(view->Topview->Nodes[TV_Node->index].Node,data_attr2));
	}
	else
		gtk_entry_set_text (TV_Node->Data2,"");
	gtk_widget_show((GtkWidget*)TV_Node->Data2);
	return 1;

}


extern int 	tv_nodes_last_page()
{
	if (TV_Nodes.activepage<TV_Nodes.pagecount)
		return tv_nodes_goto_page(TV_Nodes.pagecount);
	else
		return 0;
}
extern int 	tv_nodes_first_page()
{
	if (TV_Nodes.activepage>0)
		return tv_nodes_goto_page(0);
	else
		return 0;
}

extern int tv_nodes_goto_page(int page)
{
	GtkSpinButton* spn;
	tv_node* tvn;
	GtkLabel* lblTVPage;

	if ((page >=0) && page <= TV_Nodes.pagecount)
	{
		if(TV_Nodes.general_purpose_flag==1)
		{
			update_TV_data_from_gui();
			TV_Nodes.general_purpose_flag=0;
		}
		TV_Nodes.activepage=page;
		TV_Nodes.page_data_node_index=TV_Nodes.page_history[TV_Nodes.activepage];
		TV_Nodes.page_data_index=0;
		TV_Nodes.firstnodeid=TV_Nodes.page_data_node_index;
		TV_Nodes.Y=TV_Nodes.initial_Y;
		hide_data_widgets();
	}
	else
		return 0;
	while ((TV_Nodes.page_data_index < TV_Nodes.recordperpage)&&(TV_Nodes.page_data_node_index < view->Topview->Nodecount))
	{
		tvn=&TV_Nodes.TV_Node[TV_Nodes.page_data_index];
		tvn->index=TV_Nodes.page_data_node_index;
		if(view->Topview->Nodes[TV_Nodes.page_data_node_index].valid==1)
		{
			TV_Nodes.page_data_index++;
			update_node_gui_objects(tvn);
			TV_Nodes.Y=TV_Nodes.Y+TV_Nodes.Y_Gap;
		}
   		TV_Nodes.page_data_node_index++;
	}
	lblTVPage=(GtkLabel*)glade_xml_get_widget(xml, "lblTVPage");
	sprintf(buf,"(%i / %i)", TV_Nodes.activepage+1,TV_Nodes.pagecount+1);
	gtk_label_set_text(lblTVPage,buf);
	spn=(GtkSpinButton*)glade_xml_get_widget(xml, "spnTVGotopage");
	gtk_spin_button_set_value(spn,page+1);




	return 1;
}

extern int tv_nodes_prior_page()
{
	if (TV_Nodes.activepage >0)
	{
		return tv_nodes_goto_page(TV_Nodes.activepage -1);
	}
	else
		return 0;
}


extern int tv_nodes_next_page()
{
	if (TV_Nodes.activepage < TV_Nodes.pagecount)
	{
		return tv_nodes_goto_page(TV_Nodes.activepage +1);
	}
	else
		return 0;
}
extern void execute_tv_nodes()
{

	    gtk_widget_hide(glade_xml_get_widget(xml, "frmTVNodes"));
		gtk_widget_show(glade_xml_get_widget(xml, "frmTVNodes"));
//		gtk_window_set_keep_above           (glade_xml_get_widget(xml, "frmTVNodes"),1);
		if(TV_Nodes.initialized < 1)
		{
			init_tv_nodes(&TV_Nodes);
		}
		//test filter
		prepare_page_history();
		TV_Nodes.general_purpose_flag=1;
		tv_nodes_next_page();
}


static int hide_data_widgets()
{
	int i;
	for (i=0;i < MAX_NODE_PER_PAGE ;i ++)
	{
		if (TV_Nodes.TV_Node[i].chkHighlighted) 
		{		
			gtk_widget_hide((GtkWidget*)TV_Nodes.TV_Node[i].chkHighlighted);
			gtk_widget_hide((GtkWidget*)TV_Nodes.TV_Node[i].chkSelected);
			gtk_widget_hide((GtkWidget*)TV_Nodes.TV_Node[i].chkVisible);
			gtk_widget_hide((GtkWidget*)TV_Nodes.TV_Node[i].Data1);
			gtk_widget_hide((GtkWidget*)TV_Nodes.TV_Node[i].Data2);
			gtk_widget_hide((GtkWidget*)TV_Nodes.TV_Node[i].IDLabel);
		}
	}
	return 1;
	

}
extern int reset_page_History()
{
/*	if(TV_Nodes.page_history_count > 0)
	{
		free(TV_Nodes.page_history);
		TV_Nodes.page_history_count=0;
	}*/
	TV_Nodes.page_history_count=0;
	TV_Nodes.page_history=realloc(TV_Nodes.page_history,sizeof(int) * TV_Nodes.page_history_count);
	return 1;


}
static int push_to_page_history(int index)
{
	TV_Nodes.page_history_count++;
	TV_Nodes.page_history=realloc(TV_Nodes.page_history,sizeof(int) * TV_Nodes.page_history_count);
	TV_Nodes.page_history[TV_Nodes.page_history_count-1]=index;
	return 1;
}
static int pop_from_page_history()
{
	if(TV_Nodes.page_history_count > 0)
	{
		int return_value;
		return_value=TV_Nodes.page_history[TV_Nodes.page_history_count-1];
		TV_Nodes.page_history_count--;
		TV_Nodes.page_history=realloc(TV_Nodes.page_history,sizeof(int) * TV_Nodes.page_history_count);
		return return_value;
	}
	return 0;
}

extern int prepare_page_history()
{
	GtkLabel* lblTVPage;
	GtkSpinButton* spn;
	int i=0;
	int count=0;
	tv_node tvn;
	TV_Nodes.pagecount=0;
	TV_Nodes.activepage=-1;
	reset_page_History();
	push_to_page_history(0);
	
	for (i;i < view->Topview->Nodecount ; i++)
	{
		tvn.index=i;
		if(validate_node(&tvn))
		{
			count ++;
			view->Topview->Nodes[i].valid=1;
		}
		else
			view->Topview->Nodes[i].valid=0;
		if (count== TV_Nodes.recordperpage)
		{
			push_to_page_history(i+1);
			TV_Nodes.pagecount++;
			count=0;
		}
	}
	spn=(GtkSpinButton*)glade_xml_get_widget(xml, "spnTVGotopage");
	gtk_spin_button_set_value(spn,0);
	gtk_spin_button_set_range(spn,0,TV_Nodes.pagecount+1);


	lblTVPage=(GtkLabel*)glade_xml_get_widget(xml, "lblTVPage");
	sprintf(buf,"(%i / %i)", 1,TV_Nodes.pagecount+1);
	gtk_label_set_text(lblTVPage,buf);
	set_data_attributes();
	return 1;

}
static int set_data_attributes()
{
	GtkLabel* lblData1;
	GtkLabel* lblData2;
	char* data_attr1;
	char* data_attr2;
	// get attributes from graph
	data_attr1=agget(view->g[view->activeGraph],"DataAttribute1");
	data_attr2=agget(view->g[view->activeGraph],"DataAttribute2");
	if (!data_attr1)
	{
		agattr(view->g[view->activeGraph],AGRAPH,"DataAttribute1","DATA1");
		agattr(view->g[view->activeGraph],AGNODE,"DATA1","");
	}
	if (!data_attr2)
	{
		agattr(view->g[view->activeGraph],AGRAPH,"DataAttribute2","DATA2");
		agattr(view->g[view->activeGraph],AGNODE,"DATA2","");
	}

	data_attr1=agget(view->g[view->activeGraph],"DataAttribute1");
	data_attr2=agget(view->g[view->activeGraph],"DataAttribute2");

	lblData1=(GtkLabel*)glade_xml_get_widget(xml, "lblTVData1");
	lblData2=(GtkLabel*)glade_xml_get_widget(xml, "lblTVData2");
	gtk_label_set_text (lblData1,data_attr1);
	gtk_label_set_text (lblData2,data_attr2);
	return 1;

}

extern int update_TV_data_from_gui()
{
	int i=0;
	int index=0;
	char* data_attr1;
	char* data_attr2;
	// get attributes from graph
	data_attr1=agget(view->g[view->activeGraph],"DataAttribute1");
	data_attr2=agget(view->g[view->activeGraph],"DataAttribute2");

	for (i;i < TV_Nodes.recordperpage ; i++)
	{
		index=TV_Nodes.TV_Node[i].index;
		if (index < view->Topview->Nodecount)
		{
			// apply if selected
			if(gtk_toggle_button_get_active((GtkToggleButton*)TV_Nodes.TV_Node[i].chkSelected))
			{
				if (!((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Selected)
					select_node(view->g[view->activeGraph],view->Topview->Nodes[index].Node);					
			}
			else
			{
				if (((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Selected)
					deselect_node(view->g[view->activeGraph],view->Topview->Nodes[index].Node);					
			}
			// apply if Visible
			if(gtk_toggle_button_get_active((GtkToggleButton*)TV_Nodes.TV_Node[i].chkVisible))
			{
				if (!((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Visible)
					((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Visible=1;
			}
			else
			{
				if (((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Visible)
					((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Visible=0;
			}
			// apply if Highlighted
			if(gtk_toggle_button_get_active((GtkToggleButton*)TV_Nodes.TV_Node[i].chkHighlighted))
			{
				if (!((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Highlighted)
					((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Highlighted=1;
			}
			else
			{
				if (((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Highlighted)
					((custom_object_data*)AGDATA(view->Topview->Nodes[index].Node))->Highlighted=0;
			}
			//Data1 
			agset((void*)view->Topview->Nodes[index].Node,data_attr1,(char*)gtk_entry_get_text (TV_Nodes.TV_Node[i].Data1));
			//Data2 
			agset(view->Topview->Nodes[index].Node,data_attr2,(char*)gtk_entry_get_text (TV_Nodes.TV_Node[i].Data2));

		}
	}
		return 1;


}

extern int apply_filter_from_gui()
{
	int selected;
	int visible;
	int highlighted;

	if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterSel1")))	
			selected=-1;
		if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterSel2")))	
			selected=1;
		if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterSel3")))	
			selected=0;

		if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterVisible1")))	
			visible=-1;
		if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterVisible2")))	
			visible=1;
		if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterVisible3")))	
			visible=0;

		if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterHigh1")))	
			highlighted=-1;
		if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterHigh2")))	
			highlighted=1;
		if(gtk_toggle_button_get_active((GtkToggleButton*)glade_xml_get_widget(xml,"rbTVFilterHigh3")))	
			highlighted=0;
		set_filter(&TV_Nodes.filter,
		(char*)gtk_entry_get_text ((GtkEntry*)glade_xml_get_widget(xml,"edtTVFilterMinData1") ),
		(char*)gtk_entry_get_text ((GtkEntry*)glade_xml_get_widget(xml,"edtTVFilterMaxData1") ),
		(char*)gtk_entry_get_text ((GtkEntry*)glade_xml_get_widget(xml,"edtTVFilterMinData2") ),
		(char*)gtk_entry_get_text ((GtkEntry*)glade_xml_get_widget(xml,"edtTVFilterMaxData2") ),
		(char*)gtk_entry_get_text ((GtkEntry*)glade_xml_get_widget(xml,"edtTVFilterString") ),
			selected,visible,highlighted);
		MP_Flag=1;
		prepare_page_history();
		tv_nodes_next_page();
		return 1;
}


extern int tv_select_all()
{

	tv_node tvn;
	int i=0;
	for (i;i < view->Topview->Nodecount ; i++)
	{
		tvn.index=i;
		if(validate_node(&tvn))
		{
			select_node(view->g[view->activeGraph],view->Topview->Nodes[i].Node);
		}
	}
	apply_filter_from_gui();
	return 1;

}
extern int tv_unselect_all()
{

	tv_node tvn;
	int i=0;
	for (i;i < view->Topview->Nodecount ; i++)
	{
		tvn.index=i;
		if(validate_node(&tvn))
		{
			deselect_node(view->g[view->activeGraph],view->Topview->Nodes[i].Node);
		}
	}
	apply_filter_from_gui();
	return 1;

}
extern int tv_highligh_all()
{
	tv_node tvn;
	int i=0;
	for (i;i < view->Topview->Nodecount ; i++)
	{
		tvn.index=i;
		if(validate_node(&tvn))
		{
			((custom_object_data*)AGDATA(view->Topview->Nodes[i].Node))->Highlighted=1;
		}
	}
	apply_filter_from_gui();
	return 1;


}
extern int tv_unhighligh_all()
{
	tv_node tvn;
	int i=0;
	for (i;i < view->Topview->Nodecount ; i++)
	{
		tvn.index=i;
		if(validate_node(&tvn))
		{
			((custom_object_data*)AGDATA(view->Topview->Nodes[i].Node))->Highlighted=0;
		}
	}
	apply_filter_from_gui();
	return 1;

}
extern int tv_show_all()
{
	tv_node tvn;
	int i=0;
	for (i;i < view->Topview->Nodecount ; i++)
	{
		tvn.index=i;
		if(validate_node(&tvn))
		{
			((custom_object_data*)AGDATA(view->Topview->Nodes[i].Node))->Visible=1;
		}
	}
	apply_filter_from_gui();
	return 1;



}
extern int tv_hide_all()
{
	tv_node tvn;
	int i=0;
	for (i;i < view->Topview->Nodecount ; i++)
	{
		tvn.index=i;
		if(validate_node(&tvn))
		{
			((custom_object_data*)AGDATA(view->Topview->Nodes[i].Node))->Visible=0;
		}
	}
	apply_filter_from_gui();
	return 1;


}






