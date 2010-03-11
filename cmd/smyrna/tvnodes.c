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

/* #include "topview.h" */
#include "tvnodes.h"
#include "btree.h"
#include "viewport.h"
#include "topviewfuncs.h"

#include "memory.h"

typedef struct{
    GType type;
    char* name;
    int editable;
}gridCol;
typedef struct
{
    int count;
    gridCol** columns;
    GtkTreeStore *store;

}grid;

/*
	call this function to create a subgraph from filtered nodes and maybe edges
*/

int create_save_subgraph_from_filter(char *filename)
{

    int i = 0;
    Agraph_t *subg = agsubg(view->g[view->activeGraph], "temp", 1);
    FILE *outputfile;
    for (i = 0; i < view->Topview->Nodecount; i++) {
	if (view->Topview->Nodes[i].valid == 1) {
	    agsubnode(subg, view->Topview->Nodes[i].Node, 1);
	}
    }

    if ((outputfile = fopen(filename, "w"))) {
	int ret = agwrite(subg, outputfile);
	fclose (outputfile);
	if (ret) {
	    agdelsubg(view->g[view->activeGraph], subg);
	    return 1;
	} else {
	    agdelsubg(view->g[view->activeGraph], subg);
	    return 0;
	}
    } else {
	agdelsubg(view->g[view->activeGraph], subg);
	return 0;
    }


}


static void set_visibility(Agraph_t* g,int visibility)
{

    Agnode_t *v;
    static char bf1[2];
    static char* bf2;
    static Agsym_t* visible_attr=(Agsym_t*)0;
    static Agsym_t* selected_attr=(Agsym_t*)0;
    if(!visible_attr)
	visible_attr=agattr(g, AGNODE,"visible","1");
    if(!selected_attr)
	selected_attr=agattr(g, AGNODE,"selected",0);
    sprintf(bf1,"%d",visibility);
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	bf2=agxget(v,selected_attr);
	if((!bf2) || (strcmp(bf2,"0")==0))
		continue;
	agxset(v,visible_attr,bf1);
    }

}

int tv_show_all(void)
{
    set_visibility(view->g[view->activeGraph],1);
    updateSmGraph(view->g[view->activeGraph],view->Topview);
    return 1;
}

int tv_hide_all(void)
{
    set_visibility(view->g[view->activeGraph],0);
    updateSmGraph(view->g[view->activeGraph],view->Topview);

    return 1;
}
int tv_save_as(void)
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

	create_save_subgraph_from_filter(filename);
	g_free(filename);
	gtk_widget_destroy(dialog);

	return 1;
    } else {
	gtk_widget_destroy(dialog);
	return 0;
    }
    return 0;
}



static void create_text_column(char* Title,GtkTreeView* tree,int asso,int editable)
{
   PangoColor c;
    GtkTreeViewColumn *column;
    GtkCellRendererText *renderer;


   renderer = (GtkCellRendererText*)gtk_cell_renderer_text_new ();
    ((GtkCellRenderer*)renderer)->mode=GTK_CELL_RENDERER_MODE_EDITABLE;
   renderer->editable=editable;
   c.blue=0;
   c.green=1;
   c.red=0;
   renderer->foreground=c;


   column = gtk_tree_view_column_new_with_attributes (Title,(GtkCellRenderer*)renderer,"text",asso,NULL);

   gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
   gtk_tree_view_column_set_resizable  (column,1);

}
static void create_toggle_column(char* Title,GtkTreeView* tree,int asso,int editable)
{
   GtkTreeViewColumn *column;
    GtkCellRendererToggle* renderer;
   renderer =(GtkCellRendererToggle*)gtk_cell_renderer_toggle_new ();
   renderer->activatable=editable;
   g_object_set (G_OBJECT (renderer),"foreground", "red",NULL);

   column =  gtk_tree_view_column_new_with_attributes (Title,(GtkCellRenderer*)renderer,"active",asso,NULL);
   gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
    gtk_tree_view_column_set_resizable  (column,1);

}
static int boolStrMap(char* str)
{
    if (strcmp(str,"1") ||strcmp(str,"true")|| strcmp(str,"TRUE") || strcmp(str,"True"))
	return 1;
    return 0;

}





void populate_data(Agraph_t* g,grid* grid)
{
    Agnode_t *v;
    int id=0;
    GtkTreeIter iter;
    static GValue valueData;
    static GValue* value;

    char* bf;
    value=&valueData;

    //GValue* g_value_init (GValue *value,GType g_type);
/*    if(!value)
	value=(GValue*)malloc(sizeof(GValue));*/
    value=g_value_init (value,G_TYPE_STRING);

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	bf=agget(v,"selected");
	if((!bf) || (strcmp(bf,"0")==0))
	continue;
	gtk_tree_store_append (grid->store, &iter, NULL);

	for (id=1;id < grid->count;id ++)
	{
	    g_value_unset (value);
	    value=g_value_init (value,grid->columns[id]->type);
	    if(id==1)
		bf=agnameof(v);
	    else
	    bf=agget(v,grid->columns[id]->name);
	    if((!bf) && (id !=2))
		continue;

	    switch(grid->columns[id]->type)
    	    {
	    case G_TYPE_BOOLEAN:
		    if(bf)
		    {
			if((strcmp(bf,"1")==0) || (strcmp(bf,"true")==0) || (strcmp(bf,"True")==0))
			    g_value_set_boolean (value, 1);
			else
			    g_value_set_boolean (value, 0);
		    }
		    else
		    {
			if(id==2)
			    g_value_set_boolean (value, 1);
		    }

		break;
	    default:
		    g_value_set_string (value, bf);

	    }
	    gtk_tree_store_set_value(grid->store,&iter,id,value);
        }

    }

}







GtkTreeStore* update_tree_store(GtkTreeStore* store,int ncolumns,GType *types)
{
    if ((ncolumns ==0) || (types==NULL))
	return NULL;
    if(store)
    {
	gtk_tree_store_clear(store);
	g_object_unref(store);
    }
	
    store=gtk_tree_store_newv (ncolumns,types);
    return store;
}


static void create_column(gridCol* c,GtkTreeView *tree,int id)
{
    if (!c)
	return;
    switch (c->type)
    {
    case G_TYPE_STRING:
    case G_TYPE_INT:
	create_text_column(c->name,tree,id,c->editable);
	break;
    case G_TYPE_BOOLEAN:
	create_toggle_column(c->name,tree,id,c->editable);
	break;
    default:
	create_text_column(c->name,tree,id,c->editable);
    }
}




GtkTreeView* update_tree (GtkTreeView *tree,grid* g)
{

    GtkTreeStore* store=NULL;
    GtkTreeViewColumn* column;
    GType *types;
    int id=0;
    if(tree!=NULL)
    {
        while(column=gtk_tree_view_get_column(tree,0))	/*clear all columns*/
	    gtk_tree_view_remove_column(tree,column);
	store=(GtkTreeStore*)gtk_tree_view_get_model(tree);
    }
    else
    {
	tree=(GtkTreeView*)gtk_tree_view_new();
        gtk_widget_show((GtkWidget*)tree);

	gtk_container_add((GtkContainer*)glade_xml_get_widget(xml, "scrolledwindow9"),(GtkWidget*)tree);

    }
    if(g->count > 0)
    {
	types=(GType*)malloc(g->count*sizeof(GType));
	for (id=0;id < g->count;id++)
	    types[id]=g->columns[id]->type;
        store=update_tree_store(store,g->count,types);
        gtk_tree_view_set_model(tree,(GtkTreeModel*)store);
	/*insert columns*/
	for (id=0;id < g->count;id ++)
	    create_column(g->columns[id],tree,id);
    }
    g->store=store;
    return tree;


}
static void add_column(grid* g,char* name,int editable,GType g_type)
{
    g->columns=(gridCol**)realloc(g->columns,sizeof(gridCol*)*(g->count+1));
    g->columns[g->count]=(gridCol*)malloc(sizeof(gridCol));
    g->columns[g->count]->editable=editable;
    g->columns[g->count]->name=strdup(name);
    g->columns[g->count]->type=g_type;
    g->count++;
}
static void clear_grid(grid* g)
{
    int id=0;
    if (g->count >0)
    {
        for (id=0;id < g->count ; id++)
	{
	    free(g->columns[id]->name);
	    free (g->columns[id]);
	}
	
    }

}
static grid* initGrid()
{
    grid* gr;
    gr=(grid*)malloc(sizeof(grid));
    gr->columns=NULL;
    gr->count=0;
    return gr;
}



grid* update_colums(grid* g,char* str)
{
    /*free memory for existing c*/
    char* a;
    if(g)
        clear_grid(g);
    else
	g=initGrid();
    add_column(g,"ID",0,G_TYPE_STRING);
    add_column(g,"Name",0,G_TYPE_STRING);
    add_column(g,"visible",0,G_TYPE_BOOLEAN);
    if(!str)
	return g;
    
    a=strtok(str,",");
    add_column(g,a,1,G_TYPE_STRING);
    while ((a=strtok(NULL,",")))
        add_column(g,a,1,G_TYPE_STRING);
    return g;
}
void setup_tree (Agraph_t* g)
{
    /*
    G_TYPE_STRING:
    G_TYPE_INT:
    G_TYPE_BOOLEAN:
    */
    static char* buf=NULL;
    static GtkTreeStore *store=NULL;
    static GtkTreeView *tree=NULL;
    grid* gr=NULL;
    buf=agget(g,"datacolumns");

//    tree=(GtkTreeView *) glade_xml_get_widget(xml, "treeview1");
    gr=update_colums(gr,buf);
    tree=update_tree (tree,gr);
    populate_data(g,gr);
}

