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

#ifdef _WIN32
#include <windows.h>
#endif
#include "compat.h"
#include "viewport.h"
#include "draw.h"
#include "color.h"
#include <glade/glade.h>
#include "gui.h"
#include "string.h"
#include "topview.h"
#include "gltemplate.h"
#include "colorprocs.h"
#include "topviewsettings.h"



#define countof( array ) ( sizeof( array )/sizeof( array[0] ) )

ViewInfo *view;
/*these two global variables should be wrapped in somethign else*/
GtkMessageDialog *Dlg;
int respond;

void clear_viewport(ViewInfo * view)
{
    int ind = 0;
    /*free topview if there is one */
    if (view->graphCount) {
	cleartopview(view->Topview);
	/*all cgraph graphs should be freed */
	for (ind = 0; ind < view->graphCount; ind++) {
	    agclose(view->g[ind]);
	}
	/*frees itself */
	free(view);
    }
}
void init_viewport(ViewInfo * view)
{

    FILE *input_file;

	/*loading default visual attributes*/
    input_file = fopen(DEFAULT_ATTRIBUTES_TEMPLATE_DOT_FILE, "r");
    if (input_file == NULL)
	{
		g_print("default attributes template graph is not found! Program is being terminated....");
		exit(-1);
	}

	else if (!(view->default_attributes = agread(input_file, NIL(Agdisc_t *))))
	{
		g_print("Could not load default attributes template graph! Program is being terminated....");
		exit(-1);
	}



	//init graphs
    view->g = NULL;		//no graph, gl screen should check it
    view->graphCount = 0;	//and disable interactivity if count is zero


    view->bdxLeft = 0;
    view->bdxRight = 500;
    view->bdyBottom = 0;
    view->bdyTop = 500;
    view->bdzBottom = 0;
    view->bdzTop = 0;

    view->borderColor.R = 1;
    view->borderColor.G = 0;
    view->borderColor.B = 0;
    view->borderColor.A = 1;

    view->bdVisible =1;	//show borders red

    view->gridSize = 10;
    view->gridColor.R = 0.5;
    view->gridColor.G = 0.5;
    view->gridColor.B = 0.5;
    view->gridColor.A = 1;
    view->gridVisible = 0;	//show grids in light gray

    //mouse mode=pan
    view->mouse.mouse_mode = 0;
    //pen color
    view->penColor.R = 0;
    view->penColor.G = 0;
    view->penColor.B = 0;
    view->penColor.A = 1;

    view->fillColor.R = 1;
    view->fillColor.G = 0;
    view->fillColor.B = 0;
    view->fillColor.A = 1;
    //background color , default white
    view->bgColor.R = 1;
    view->bgColor.G = 1;
    view->bgColor.B = 1;
    view->bgColor.A = 1;

    //selected objets are drawn with this color
    view->selectedNodeColor.R = 1;
    view->selectedNodeColor.G = 0;
    view->selectedNodeColor.B = 0;
    view->selectedNodeColor.A = 1;

    //default line width;
    view->LineWidth = 1;

    //default view settings , camera is not active
    view->GLDepth = 1;		//should be set before GetFixedOGLPos(int x, int y,float kts) funtion is used!!!!
    view->panx = 0;
    view->pany = 0;
    view->panz = 0;

    view->prevpanx = 0;
    view->prevpany = 0;


    view->zoom = -20;
    view->texture = 1;
    view->FontSize = 52;

    view->topviewusermode = TOP_VIEW_USER_NOVICE_MODE;	//for demo
    view->mg.active = 0;
    view->mg.x = 0;
    view->mg.y = 0;
    view->mg.width = DEFAULT_MAGNIFIER_WIDTH;
    view->mg.height = DEFAULT_MAGNIFIER_HEIGHT;
    view->mg.kts = DEFAULT_MAGNIFIER_KTS;
    view->fmg.R = DEFAULT_FISHEYE_MAGNIFIER_RADIUS;
    view->fmg.active = 0;
    view->mouse.mouse_down = 0;
    view->activeGraph = -1;
    view->SignalBlock = 0;
    view->Selection.Active = 0;
    view->Selection.SelectionColor.R = 0.5;
    view->Selection.SelectionColor.G = (float) 0.2;
    view->Selection.SelectionColor.B = 1;
    view->Selection.SelectionColor.A = 1;
    view->Selection.Anti = 0;
    view->Topview = malloc(sizeof(topview));
    view->Topview->topviewmenu = '\0';
	set_viewport_settings_from_template  (view,view->default_attributes);

}
static char* get_attribute_value(char* attr,ViewInfo * view,Agraph_t *g)
{
	char* buf;
	buf=agget(g,attr);
	if ((!buf)||(strcmp(buf,"")==0))
		buf=agget(view->default_attributes,attr);
	return buf;

}
void set_viewport_settings_from_template  (ViewInfo * view,Agraph_t *g)
{
    gvcolor_t cl;
	char* buf;
	/*graph [antialiasing=1,
		bgcolor="#ffffff",
		bordercolor="#38eb29",
		bordercoloralpha="1",
		bordervisible=1,
		defaultfontname=1,
		defaultfontsize=52,
		gridcolor="#6033d8",
		gridcoloralpha="1",
		gridvisible=1,
		highlightededgecolor="#c41b50",
		highlightededgecoloralpha="1",
		highlightednodecolor="#d1cd24",
		highlightednodecoloralpha="1",
		defaultlinewidth=1
		nodesizewithdegree=1,
		randomizeedgecolors=1,
		randomizenodecolors=1,
		selectededgecolor="#ffc0cb",
		selectededgecoloralpha="1",
		selectednodecolor="#8ce61d",
		selectednodecoloralpha
		gridcoloralpha="1",
		defaultmagnifierwidth="300",
		defaultmagnifierheight="200",
		defaultmagnifierkts="5",
		defaultfisheyemagnifierradius="250"
		usermode=1
	];
	border color*/
	colorxlate(get_attribute_value("bordercolor",view,g), &cl, RGBA_DOUBLE);
	view->borderColor.R = (float)cl.u.RGBA[0];
    view->borderColor.G = (float)cl.u.RGBA[1];
    view->borderColor.B = (float)cl.u.RGBA[2];

	view->borderColor.A = (float)atof(get_attribute_value("bordercoloralpha",view,g));


	view->bdVisible = (float)atof(get_attribute_value("bordervisible",view,g));
	

	buf=get_attribute_value("gridcolor",view,g);
	colorxlate(buf, &cl, RGBA_DOUBLE);
	view->gridColor.R = (float)cl.u.RGBA[0];
    view->gridColor.G = (float)cl.u.RGBA[1];
    view->gridColor.B = (float)cl.u.RGBA[2];
	view->gridColor.A = (float)atof(get_attribute_value("gridcoloralpha",view,g));

	
	
	view->gridSize = (float)atof(buf=get_attribute_value("gridsize",view,g));

	view->gridVisible = atoi(get_attribute_value("gridvisible",view,g));

    //mouse mode=pan

    //background color , default white
	colorxlate(get_attribute_value("bgcolor",view,g), &cl, RGBA_DOUBLE);

	view->bgColor.R = (float)cl.u.RGBA[0];
    view->bgColor.G = (float)cl.u.RGBA[1];
    view->bgColor.B =(float) cl.u.RGBA[2];
    view->bgColor.A = 1;

    //selected nodes are drawn with this color
	colorxlate(get_attribute_value("selectednodecolor",view,g), &cl, RGBA_DOUBLE);
	view->selectedNodeColor.R = (float)cl.u.RGBA[0];
    view->selectedNodeColor.G = (float)cl.u.RGBA[1];
    view->selectedNodeColor.B = (float)cl.u.RGBA[2];
    view->selectedNodeColor.A = (float)atof(get_attribute_value("selectednodecoloralpha",view,g));
	//selected edge are drawn with this color
	colorxlate(get_attribute_value("selectededgecolor",view,g), &cl, RGBA_DOUBLE);
	view->selectedEdgeColor.R = (float)cl.u.RGBA[0];
    view->selectedEdgeColor.G = (float)cl.u.RGBA[1];
    view->selectedEdgeColor.B = (float)cl.u.RGBA[2];
    view->selectedEdgeColor.A = (float)atof(get_attribute_value("selectededgecoloralpha",view,g));


	colorxlate(get_attribute_value("highlightednodecolor",view,g), &cl, RGBA_DOUBLE);
	view->highlightedNodeColor.R = (float)cl.u.RGBA[0];
    view->highlightedNodeColor.G = (float)cl.u.RGBA[1];
    view->highlightedNodeColor.B = (float)cl.u.RGBA[2];
    view->highlightedNodeColor.A = (float)atof(get_attribute_value("highlightednodecoloralpha",view,g));

	buf=agget(g,"highlightededgecolor");
	colorxlate(get_attribute_value("highlightededgecolor",view,g), &cl, RGBA_DOUBLE);
	view->highlightedEdgeColor.R = (float)cl.u.RGBA[0];
    view->highlightedEdgeColor.G = (float)cl.u.RGBA[1];
    view->highlightedEdgeColor.B = (float)cl.u.RGBA[2];
    view->highlightedEdgeColor.A = (float)atof(get_attribute_value("highlightededgecoloralpha",view,g));


/*

but here i am, on the road again
here i am, up on the stage
here i go, playing the star again
there i go, turn the page

*/

	
	
	/*default line width*/
    view->LineWidth = (float)atof(get_attribute_value("defaultlinewidth",view,g));
	view->FontSize = atoi(get_attribute_value("defaultfontsize",view,g));

	view->topviewusermode = atoi(get_attribute_value("usermode",view,g));
	get_attribute_value("defaultmagnifierwidth",view,g);
	view->mg.width = atoi(get_attribute_value("defaultmagnifierwidth",view,g));
	view->mg.height = atoi(get_attribute_value("defaultmagnifierheight",view,g));

	view->mg.kts = (float)atof(get_attribute_value("defaultmagnifierkts",view,g));

	view->fmg.R = atoi(get_attribute_value("defaultfisheyemagnifierradius",view,g));

	glClearColor(view->bgColor.R, view->bgColor.G, view->bgColor.B, view->bgColor.A);	//background color

}


int add_graph_to_viewport_from_file(char *fileName)
{
    //returns 1 if successfull else 0
    Agraph_t *graph;
    graph = loadGraph(fileName);
    if (graph) {
	view->graphCount = view->graphCount + 1;
	view->g =
	    (Agraph_t **) realloc(view->g,
				  sizeof(Agraph_t *) * view->graphCount);
	view->g[view->graphCount - 1] = graph;
	view->activeGraph = view->graphCount - 1;
	//GUI update , graph combo box on top-right should be updated
	load_settings_from_graph(view->default_attributes);
	refreshControls(view);
	return 1;
    } else
	return 0;

}


int add_new_graph_to_viewport()
{
    //returns graph index , otherwise -1
    Agraph_t *graph;
    graph = (Agraph_t *) malloc(sizeof(Agraph_t));
    if (graph) {
	view->graphCount = view->graphCount + 1;
	view->g[view->graphCount - 1] = graph;
	return (view->graphCount - 1);
    } else
	return -1;
}


void refreshControls(ViewInfo * v)
{

    int i = 0;
    GtkComboBox *widget;
    widget = get_SelectGraph();
    //load graph names to combobox
    for (i = 0; i < v->graphCount; i++) {
	gtk_combo_box_append_text(widget,
				  ((custom_graph_data
				    *) (AGDATA(v->g[i])))->GraphFileName);
    }
    view->SignalBlock = 1;	//HACK
    gtk_combo_box_set_active(widget, view->activeGraph);
    view->SignalBlock = 0;


    //change button colors
    Color_Widget_bg("gray", glade_xml_get_widget(xml, "btnDot"));
    Color_Widget_bg("gray", glade_xml_get_widget(xml, "btnNeato"));
    Color_Widget_bg("gray", glade_xml_get_widget(xml, "btnTwopi"));
    Color_Widget_bg("gray", glade_xml_get_widget(xml, "btnCirco"));
    Color_Widget_bg("gray", glade_xml_get_widget(xml, "btnFdp"));


    switch (((custom_graph_data *) (AGDATA(view->g[view->activeGraph])))->
	    Engine) {
    case 0:
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnDot"));
	break;

    case 1:
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnNeato"));
	break;

    case 2:
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnTwopi"));
	break;

    case 3:
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnCirco"));
	break;

    case 4:
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnFdp"));
	break;


    }
    glexpose();





}


void update_graph_params(Agraph_t * graph)	//adds gledit params 
{


    char tempString[100];
    agattr(graph, AGRAPH, "GraphFileName",
	   ((custom_graph_data *) (AGDATA(graph)))->GraphFileName);
    agattr(graph, AGRAPH, "GraphName",
	   ((custom_graph_data *) (AGDATA(graph)))->GraphName);
    sprintf(tempString, "%i",
	    ((custom_graph_data *) (AGDATA(graph)))->AlwaysShow);
    agattr(graph, AGRAPH, "AlwaysShow", tempString);
    sprintf(tempString, "%i",
	    ((custom_graph_data *) (AGDATA(graph)))->TopView);
    agattr(graph, AGRAPH, "TopView", tempString);
    sprintf(tempString, "%i",
	    ((custom_graph_data *) (AGDATA(graph)))->Locked);
    agattr(graph, AGRAPH, "Locked", tempString);
    sprintf(tempString, "%i",
	    ((custom_graph_data *) (AGDATA(graph)))->Engine);
    agattr(graph, AGRAPH, "Engine", tempString);

}

void load_graph_params(Agraph_t * graph)	//run once right after loading graph
{
    //file may or may have not gl edit attributes
    //first defaults are set in loading function
    //here file is checked for previously saved gledit attributes
    if (agget(graph, "GraphName"))	//Graph Name
    {
	((custom_graph_data *) AGDATA(graph))->GraphName =
	    (char *) malloc((strlen(agget(graph, "GraphName")) + 1) *
			    sizeof(char));
	strcpy(((custom_graph_data *) AGDATA(graph))->GraphName,
	       agget(graph, "GraphName"));
    }
    if (agget(graph, "AlwaysShow"))	//Graph Name
	((custom_graph_data *) AGDATA(graph))->AlwaysShow =
	    atoi(agget(graph, "AlwaysShow"));
    else
	((custom_graph_data *) AGDATA(graph))->AlwaysShow = 0;

    if (agget(graph, "TopView"))	//Graph Name
	((custom_graph_data *) AGDATA(graph))->TopView =
	    atoi(agget(graph, "TopView"));
    else
	((custom_graph_data *) AGDATA(graph))->TopView = 0;
    if (agget(graph, "Locked"))	//Graph Name
	((custom_graph_data *) AGDATA(graph))->Locked =
	    atoi(agget(graph, "Locked"));
    else
	((custom_graph_data *) AGDATA(graph))->Locked = 0;
    if (agget(graph, "Engine"))	//Graph Name
	((custom_graph_data *) AGDATA(graph))->Engine =
	    atoi(agget(graph, "Engine"));
    else
	((custom_graph_data *) AGDATA(graph))->Engine = 0;	//DOT

    ((custom_graph_data *) AGDATA(graph))->Modified = 0;	//not modified yet
    ((custom_graph_data *) AGDATA(graph))->selectedEdges = '\0';
    ((custom_graph_data *) AGDATA(graph))->selectedNodes = '\0';
    ((custom_graph_data *) AGDATA(graph))->selectedGraphs = '\0';

    ((custom_graph_data *) AGDATA(graph))->selectedNodesCount = 0;
    ((custom_graph_data *) AGDATA(graph))->selectedEdgesCount = 0;
    ((custom_graph_data *) AGDATA(graph))->selectedGraphsCount = 0;



/*		if(agget((void*)g, "xdotversion"))	//xdot exists
			((custom_graph_data*)AGDATA(g))->view->Topview=0; //need to check xdot version attribute
		else		//we dont know if it is view->Topview or simply a graph with no xdot, for testing i ll use view->Topview
			((custom_graph_data*)AGDATA(g))->view->Topview=1;  */





}

int save_graph()		//save without prompt
{
    //check if there is an active graph
    if (view->activeGraph > -1) {
	//check if active graph has a file name
	if (((custom_graph_data *) AGDATA(view->g[view->activeGraph]))->
	    GraphFileName) {
	    return save_graph_with_file_name(view->g[view->activeGraph],
					     ((custom_graph_data *)
					      AGDATA(view->
						     g[view->
						       activeGraph]))->
					     GraphFileName);
	} else
	    return save_as_graph();
    }
    return 1;

}

int save_as_graph()		//save with prompt
{
    //check if there is an active graph
    if (view->activeGraph > -1) {
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
	    filename =
		gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	    save_graph_with_file_name(view->g[view->activeGraph],
				      filename);
	    g_free(filename);
	    gtk_widget_destroy(dialog);

	    return 1;
	} else {
	    gtk_widget_destroy(dialog);
	    return 0;
	}
    }
    return 0;
}
int save_graph_with_file_name(Agraph_t * graph, char *fileName)	//saves graph with file name,if file name is NULL save as is ++
{
    //if file name is NULL save to graph's filename
    //else use file name, this implements save as..
    FILE *output_file;
    update_graph_params(graph);
    if (fileName)
	output_file = fopen(fileName, "w");
    else {
	if (((custom_graph_data *) (AGDATA(graph)))->GraphFileName)
	    output_file =
		fopen(((custom_graph_data *) (AGDATA(graph)))->
		      GraphFileName, "w");
	else {
	    g_print("there is no file name to save! Programmer error\n");
	    return 0;
	}
    }
    if (output_file == NULL) {
	g_print("Cannot create file \n");
	return 0;
    } else if (agwrite(graph, (void *) output_file)) {
	g_print("%s sucessfully saved \n", fileName);
	return 1;
    }
    return 0;
}

int create_xdot_for_graph(Agraph_t * graph, int keeppos)
{
    //0 failed , 1 successfull
    //save graph to __temp.dot
    //run dot/neato whatever to create the xdot version __temp.xdot
    //delete temp files
    //use this function to do layouts too   
    int r = 0;
    FILE *output_file;
    update_graph_params(graph);
#ifdef _WIN32
    if (output_file = fopen("c:/__tempfile.dot", "w"))
#else
    if ((output_file = fopen("/tmp/__tempfile.dot", "w")))
#endif
    {
	clear_graph_xdot(graph);
	agwrite(graph, (void *) output_file);	//save graph
	fclose(output_file);	//close file desc

	if (keeppos == 0) {
#ifdef _WIN32
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 0)
		system
		    ("\"C:/Program Files/Graphviz2.15/bin/dot.exe\" -Txdot -Kdot c:/__tempfile.dot -oc:/__tempfile.xdot");
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 1)
		system
		    ("\"C:/Program Files/Graphviz2.15/bin/dot.exe\" -Txdot -Kneato c:/__tempfile.dot -oc:/__tempfile.xdot");
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 2)
		system
		    ("\"C:/Program Files/Graphviz2.15/bin/dot.exe\" -Txdot -Ktwopi c:/__tempfile.dot -oc:/__tempfile.xdot");
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 3)
		system
		    ("\"C:/Program Files/Graphviz2.15/bin/dot.exe\" -Txdot -Kcirco c:/__tempfile.dot -oc:/__tempfile.xdot");
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 4)
		system
		    ("\"C:/Program Files/Graphviz2.15/bin/dot.exe\" -Txdot -Kfdp c:/__tempfile.dot -oc:/__tempfile.xdot");
#else
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 0)
		system
		    ("\"dot\" -Txdot -Kdot /tmp/__tempfile.dot -o/tmp/__tempfile.xdot");
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 1)
		system
		    ("\"dot\" -Txdot -Kneato /tmp/__tempfile.dot -o/tmp/__tempfile.xdot");
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 2)
		system
		    ("\"dot\" -Txdot -Ktwopi /tmp/__tempfile.dot -o/tmp/__tempfile.xdot");
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 3)
		system
		    ("\"dot\" -Txdot -Kcirco /tmp/__tempfile.dot -o/tmp/__tempfile.xdot");
	    if (((custom_graph_data *) AGDATA(graph))->Engine == 4)
		system
		    ("\"dot\" -Txdot -Kfdp /tmp/__tempfile.dot -o/tmp/__tempfile.xdot");
#endif
	}
	//no position change
	else
#ifdef _WIN32
	    system
		("\"C:/Program Files/Graphviz2.15/bin/neato.exe\" -n2 -Txdot  c:/__tempfile.dot -oc:/__tempfile.xdot");
#else
	    system
		("\"neato\" -n2 -Txdot  /tmp/__tempfile.dot -o/tmp/__tempfile.xdot");
#endif

	if (r)			//load the new graph and change file name
	{
	    clear_graph(graph);	//i am not sure about this, need to talk to North
	    return TRUE;

	}
    } else
	return FALSE;

    return 0;

}
int do_graph_layout(Agraph_t * graph, int Engine, int keeppos)	//changes the layout, all user relocations are reset unless keeppos is set to 1
{
/*	Agnode_t *v;
	Agedge_t *e;
	Agsym_t *attr;
	Dict_t *d;
	int cnt;*/
//      mydata *p;
    FILE *input_file;
    char *_filename =
	(char *)
	malloc((strlen
		(((custom_graph_data
		   *) (AGDATA(view->g[view->activeGraph])))->
		 GraphFileName) + 1) * sizeof(char));
    strcpy(_filename,
	   ((custom_graph_data *) AGDATA(view->g[view->activeGraph]))->
	   GraphFileName);


    ((custom_graph_data *) AGDATA(view->g[view->activeGraph]))->Engine =
	Engine;
    create_xdot_for_graph(view->g[view->activeGraph], keeppos);

#ifdef _WIN32
    input_file = fopen("c:/__tempfile.xdot", "r");
#else
    input_file = fopen("/tmp/__tempfile.xdot", "r");
#endif
    clear_graph(view->g[view->activeGraph]);
    agclose(view->g[view->activeGraph]);
    if (input_file == NULL)
	g_print("temp file Cannot open n");
    else if ((view->g[view->activeGraph] =
	      agread(input_file, NIL(Agdisc_t *)))) {
	fclose(input_file);
	//attaching rec for graph fields
	attach_object_custom_data_to_graph(view->g[view->activeGraph]);
	//set real file name
	((custom_graph_data *) AGDATA(view->g[view->activeGraph]))->
	    GraphFileName =
	    (char *) malloc((strlen(_filename) + 1) * sizeof(char));
	load_graph_params(view->g[view->activeGraph]);	//init glparams
	strcpy(((custom_graph_data *) AGDATA(view->g[view->activeGraph]))->
	       GraphFileName, _filename);
	free(_filename);
	//set engine
	((custom_graph_data *) AGDATA(view->g[view->activeGraph]))->
	    Engine = Engine;
	((custom_graph_data *) AGDATA(view->g[view->activeGraph]))->
	    Modified = 1;
	refreshControls(view);
	return 1;
    } else {
	return 0;
    }


    return 0;

}

void clear_graph(Agraph_t * graph)
//clears custom data binded
{


}

Agraph_t *loadGraph(char *filename)
{
    Agraph_t *g;
//      mydata *p;
    FILE *input_file;
    input_file = fopen(filename, "r");
    if (input_file == NULL)
	g_print("Cannot open %s\n", filename);
    else if ((g = agread(input_file, NIL(Agdisc_t *)))) {
	attach_object_custom_data_to_graph(g);
	load_graph_params(g);

	if ((!agget(g, "xdotversion"))
	    && ((agget(g, "TopView") == "0")
		|| !agget(g, "TopView")
	    )

	    )
	{
	    create_xdot_for_graph(g, 0);
	    fclose(input_file);
#ifdef _WIN32
	    input_file = fopen("c:/__tempfile.xdot", "r");
#else
	    input_file = fopen("/tmp/__tempfile.xdot", "r");
#endif
	    while (input_file == NULL)	//HACK!!!!
	    {
		input_file = fopen("c:/__tempfile.xdot", "r");
//                      g_print("Cannot open xdot  error %si\n",strerror(errno));

	    }
	    g = agread(input_file, NIL(Agdisc_t *));
	    g_print("xdot is being loaded\n");
	    //attaching rec for graph fields
	    attach_object_custom_data_to_graph(g);
	    load_graph_params(g);
	    //      fclose(input_file);
	}
	((custom_graph_data *) AGDATA(g))->GraphFileName =
	    (char *) malloc((strlen(filename) + 1) * sizeof(char));
	//attaching rec for graph objects
	strcpy(((custom_graph_data *) AGDATA(g))->GraphFileName, filename);
	/*if(strcasecmp(agget(g, "TopView"),"1")==0)
	   {
	   if(
	   TopviewNodeCount > 0)
	   {
	   Dlg=gtk_message_dialog_new (NULL,
	   GTK_DIALOG_MODAL,
	   GTK_MESSAGE_WARNING,
	   GTK_BUTTONS_OK,
	   "For Performance issues , this program does not support multiple Topview graphs!");
	   respond=gtk_dialog_run (Dlg);
	   gtk_object_destroy (Dlg);
	   agclose(g);
	   return 0;
	   }
	   else
	   PrepareTopview(g);
	   } */
	preparetopview(g, view->Topview);
	return g;
    } else {
	return 0;
    }
    return 0;
}
int clear_object_xdot(void *obj)
{
    if (obj) {
	if (agattrsym(obj, "_draw_"))
	    agset(obj, "_draw_", "");
	if (agattrsym(obj, "_ldraw_"))
	    agset(obj, "_ldraw_", "");
	if (agattrsym(obj, "_hdraw_"))
	    agset(obj, "_hdraw_", "");
	if (agattrsym(obj, "_tdraw_"))
	    agset(obj, "_tdraw_", "");
	if (agattrsym(obj, "_hldraw_"))
	    agset(obj, "_hldraw_", "");
	if (agattrsym(obj, "_tldraw_"))
	    agset(obj, "_tldraw_", "");
	return 1;
    }
    return 0;
}


int clear_graph_xdot(Agraph_t * graph)	//clears all xdot  attributes, used especially before layout change
{
    Agnode_t *n;
    Agedge_t *e;
    Agraph_t *s;


    clear_object_xdot(graph);
    n = agfstnode(graph);

    for (s = agfstsubg(graph); s; s = agnxtsubg(s))
	clear_object_xdot(s);

    for (n = agfstnode(graph); n; n = agnxtnode(graph, n)) {
	clear_object_xdot(n);
	for (e = agfstout(graph, n); e; e = agnxtout(graph, e)) {
	    clear_object_xdot(e);
	}
    }
    return 1;


}

/*

Object Custom Data Functions

*/


int attach_object_custom_data_to_graph(Agraph_t * graph)
{
    Agnode_t *n;
    Agedge_t *e;
    Agraph_t *s;

    agbindrec(graph, "custom_graph_data", sizeof(custom_graph_data), TRUE);	//graph custom data
    init_object_custom_data(graph, graph);	//attach to graph itself

    n = agfstnode(graph);

    for (s = agfstsubg(graph); s; s = agnxtsubg(s))
	init_object_custom_data(graph, s);	//attach to subgraph 

    for (n = agfstnode(graph); n; n = agnxtnode(graph, n)) {
	init_object_custom_data(graph, n);	//attach to node
	for (e = agfstout(graph, n); e; e = agnxtout(graph, e)) {
	    init_object_custom_data(graph, e);	//attach to edge
	}
    }
    return 1;

}


int init_object_custom_data(Agraph_t * graph, void *obj)	//creates a custom_object_data 
{
    agdelrec(graph, "custom_object_data");
    agbindrec(obj, "custom_object_data", sizeof(custom_object_data), TRUE);
    ((custom_object_data *) AGDATA(obj))->ID = 0;
    ((custom_object_data *) AGDATA(obj))->ObjName = NULL;
    ((custom_object_data *) AGDATA(obj))->ObjType = AGTYPE(obj);
    ((custom_object_data *) AGDATA(obj))->Layer = -1;
    ((custom_object_data *) AGDATA(obj))->Visible = 1;
    ((custom_object_data *) AGDATA(obj))->Locked = 0;
    ((custom_object_data *) AGDATA(obj))->Highlighted = 0;
    ((custom_object_data *) AGDATA(obj))->NumDataCount = 0;
    ((custom_object_data *) AGDATA(obj))->NumData = NULL;
    ((custom_object_data *) AGDATA(obj))->StrDataCount = 0;
    ((custom_object_data *) AGDATA(obj))->StrData = NULL;
    return 1;
}


int clear_object_custom_data(void *obj)	//frees memory allocated for cutom object data
{
    return ((clear_string_data_from_object_custom_data(obj))
	    || (clear_numeric_data_from_object_custom_data(obj)));
}

int add_string_data_to_object_custom_data(void *obj, char *data)
{
    if ((obj != NULL) && (data != NULL)) {
	((custom_object_data *) AGDATA(obj))->StrData =
	    realloc(((custom_object_data *) AGDATA(obj))->StrData,
		    sizeof(char *) *
		    (((custom_object_data *) AGDATA(obj))->StrDataCount +
		     1));
	((custom_object_data *) AGDATA(obj))->
	    StrData[((custom_object_data *) AGDATA(obj))->StrDataCount] =
	    malloc((strlen(data) + 1) * sizeof(char));
	strcpy(((custom_object_data *) AGDATA(obj))->
	       StrData[((custom_object_data *) AGDATA(obj))->StrDataCount],
	       data);
	((custom_object_data *) AGDATA(obj))->StrDataCount++;
	return 1;
    }
    return 0;
}

int add_numeric_data_to_object_custom_data(void *obj, float data)
{
    if (obj != NULL) {
	((custom_object_data *) AGDATA(obj))->NumData =
	    realloc(((custom_object_data *) AGDATA(obj))->StrData,
		    sizeof(char *) *
		    (((custom_object_data *) AGDATA(obj))->NumDataCount +
		     1));
	((custom_object_data *) AGDATA(obj))->
	    NumData[((custom_object_data *) AGDATA(obj))->NumDataCount] =
	    data;
	((custom_object_data *) AGDATA(obj))->NumDataCount++;
	return 1;
    }
    return 0;
}

int clear_string_data_from_object_custom_data(void *obj)
{
    if (obj != NULL) {
	int ind = 0;
	for (ind = 0;
	     ind < ((custom_object_data *) AGDATA(obj))->StrDataCount;
	     ind++) {
	    free(((custom_object_data *) AGDATA(obj))->StrData[ind]);
	}
	free(((custom_object_data *) AGDATA(obj))->StrData);
	return 1;
    }
    return 0;
}

int clear_numeric_data_from_object_custom_data(void *obj)
{
    if (obj != NULL) {
	free(((custom_object_data *) AGDATA(obj))->NumData);
	return 1;
    }
    return 0;
}
void move_node(void *obj, float dx, float dy)
{
    char buf[512];
    char buf2[512];
    char *pch;
    int a = 0;
    int i = 0;
    if ((agget(obj, "pos")) && (AGTYPE(obj) == AGNODE)) {
	//tokenize 
	strcpy(buf, agget(obj, "pos"));

	pch = strtok(buf, ",");
	while (pch != NULL) {
	    if (i == 0)
		a = sprintf(buf2 + a, "%i,", atoi(pch) - (int) dx);
	    else
		a = sprintf(buf2 + a, "%i,", atoi(pch) - (int) dy);
	    pch = strtok(NULL, ",");
	    i++;
	}
	buf2[strlen(buf2) - 1] = '\0';
	agset(obj, "pos", buf2);
    }
}

static char *move_xdot(void *obj, xdot * x, int dx, int dy, int dz)
{
    int i = 0;
    int j = 0;
    /* int a=0; */
    /* char* pch; */
    /* int pos[MAXIMUM_POS_COUNT];  //maximum pos count hopefully does not exceed 100 */
    if (!x)
	return "\0";

    for (i = 0; i < x->cnt; i++) {
	switch (x->ops[i].kind) {
	case xd_filled_polygon:
	case xd_unfilled_polygon:
	case xd_filled_bezier:
	case xd_unfilled_bezier:
	case xd_polyline:
	    for (j = 0; j < x->ops[i].u.polygon.cnt; j++) {
		x->ops[i].u.polygon.pts[j].x =
		    x->ops[i].u.polygon.pts[j].x - dx;
		x->ops[i].u.polygon.pts[j].y =
		    x->ops[i].u.polygon.pts[j].y - dy;
		x->ops[i].u.polygon.pts[j].z =
		    x->ops[i].u.polygon.pts[j].z - dz;
	    }
	    break;
	case xd_filled_ellipse:
	case xd_unfilled_ellipse:
	    x->ops[i].u.ellipse.x = x->ops[i].u.ellipse.x - dx;
	    x->ops[i].u.ellipse.y = x->ops[i].u.ellipse.y - dy;
	    //                      x->ops[i].u.ellipse.z=x->ops[i].u.ellipse.z-dz;
	    break;
	case xd_text:
	    x->ops[i].u.text.x = x->ops[i].u.text.x - dx;
	    x->ops[i].u.text.y = x->ops[i].u.text.y - dy;
	    //                      x->ops[i].u.text.z=x->ops[i].u.text.z-dz;
	    break;
	case xd_image:
	    x->ops[i].u.image.pos.x = x->ops[i].u.image.pos.x - dx;
	    x->ops[i].u.image.pos.y = x->ops[i].u.image.pos.y - dy;
	    //                      x->ops[i].u.image.pos.z=x->ops[i].u.image.pos.z-dz;
	    break;
	default:
	    break;
	}
    }
    view->GLx = view->GLx2;
    view->GLy = view->GLy2;
    return sprintXDot(x);


}

#ifdef UNUSED
static char *offset_spline(xdot * x, float dx, float dy, float headx,
			   float heady)
{
    int i = 0;
    Agnode_t *headn, tailn;
    Agedge_t *e;
    e = x->obj;			//assume they are all edges, check function name
    headn = aghead(e);
    tailn = agtail(e);

    for (i = 0; i < x->cnt; i++)	//more than 1 splines ,possible
    {
	switch (x->ops[i].kind) {
	case xd_filled_polygon:
	case xd_unfilled_polygon:
	case xd_filled_bezier:
	case xd_unfilled_bezier:
	case xd_polyline:
	    if (((custom_object_data *) AGDATA((headn)->obj))->Selected ==
		1)
		&&((custom_object_data *) AGDATA((tailn)->obj))->
		    Selected == 1)) {
		for (j = 0; j < x->ops[i].u.polygon.cnt; j++) {

		    x->ops[i].u.polygon.pts[j].x =
			x->ops[i].u.polygon.pts[j].x + dx;
		    x->ops[i].u.polygon.pts[j].y =
			x->ops[i].u.polygon.pts[j].y + dy;
		    x->ops[i].u.polygon.pts[j].z =
			x->ops[i].u.polygon.pts[j].z + dz;
		}
		}
	    break;
	}
    }
    return 0;
}
#endif

void move_nodes(Agraph_t * g)	//move selected nodes 
{
    Agnode_t *obj;

    float dx, dy;
    xdot *bf;
    int i = 0;
    dx = view->GLx - view->GLx2;
    dy = view->GLy - view->GLy2;

    if (((custom_graph_data *) AGDATA(view->g[view->activeGraph]))->
	TopView == 0) {
	for (i = 0;
	     i < ((custom_graph_data *) AGDATA(g))->selectedNodesCount;
	     i++) {
	    obj = ((custom_graph_data *) AGDATA(g))->selectedNodes[i];
	    bf = parseXDot(agget(obj, "_draw_"));
	    agset(obj, "_draw_",
		  move_xdot(obj, bf, (int) dx, (int) dy, 0));
	    free(bf);
	    bf = parseXDot(agget(obj, "_ldraw_"));
	    agset(obj, "_ldraw_",
		  move_xdot(obj, bf, (int) dx, (int) dy, 0));
	    free(bf);
	    move_node(obj, dx, dy);
	    //iterate edges
	    /*for (e = agfstout(g,obj) ; e ; e = agnxtout (g,e))
	       {
	       bf=parseXDot (agget(e,"_tdraw_"));
	       agset(e,"_tdraw_",move_xdot(e,bf,(int)dx,(int)dy,0.00));
	       free(bf);
	       bf=parseXDot (agget(e,"_tldraw_"));
	       agset(e,"_tldraw_",move_xdot(e,bf,(int)dx,(int)dy,0.00));
	       free(bf);
	       bf=parseXDot (agget(e,"_draw_"));
	       agset(e,"_draw_",offset_spline(bf,(int)dx,(int)dy,0.00,0.00,0.00));
	       free(bf);
	       bf=parseXDot (agget(e,"_ldraw_"));
	       agset(e,"_ldraw_",offset_spline(bf,(int)dx,(int)dy,0.00,0.00,0.00));
	       free (bf);
	       } */
	    /*              for (e = agfstin(g,obj) ; e ; e = agnxtin (g,e))
	       {
	       free(bf);
	       bf=parseXDot (agget(e,"_hdraw_"));
	       agset(e,"_hdraw_",move_xdot(e,bf,(int)dx,(int)dy,0.00));
	       free(bf);
	       bf=parseXDot (agget(e,"_hldraw_"));
	       agset(e,"_hldraw_",move_xdot(e,bf,(int)dx,(int)dy,0.00));
	       free(bf);
	       bf=parseXDot (agget(e,"_draw_"));
	       agset(e,"_draw_",offset_spline(e,bf,(int)dx,(int)dy,0.00,0.00,0.00));
	       free(bf);
	       bf=parseXDot (agget(e,"_ldraw_"));
	       agset(e,"_ldraw_",offset_spline(e,bf,(int)dx,(int)dy,0.00,0.00,0.00));
	       } */
	}
    }
}




int SetGdkColor(GdkColor * c, char *color) {
    gvcolor_t cl;
    if (color != '\0') {
	colorxlate(color, &cl, RGBA_DOUBLE);
	c->red = (int) (cl.u.RGBA[0] * 65535.0);
	c->green = (int) (cl.u.RGBA[1] * 65535.0);
	c->blue = (int) (cl.u.RGBA[2] * 65535.0);
	return 1;
    } else
	return 0;

}

void glexpose() {
    expose_event(view->drawing_area, NULL, NULL);
}



