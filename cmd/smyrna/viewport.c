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
#include <io.h>
#else
#include "unistd.h"
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
#include "memory.h"
#include "topviewsettings.h"

  /* Forward declarations */
static int init_object_custom_data(Agraph_t * graph, void *obj);

#define countof( array ) ( sizeof( array )/sizeof( array[0] ) )

ViewInfo *view;
/* these two global variables should be wrapped in something else */
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

static char *get_attribute_value(char *attr, ViewInfo * view, Agraph_t * g)
{
    char *buf;
    buf = agget(g, attr);
    if ((!buf) || (*buf == '\0'))
	buf = agget(view->default_attributes, attr);
    return buf;
}

void
set_viewport_settings_from_template(ViewInfo * view, Agraph_t * g)
{
    gvcolor_t cl;
    char *buf;
#if 0
    graph[antialiasing = 1,
	  bgcolor = "#ffffff",
	  bordercolor = "#38eb29",
	  bordercoloralpha = "1",
	  bordervisible = "1",
	  defaultfontname = "1",
	  defaultfontsize = "52",
	  gridcolor = "#6033d8",
	  gridcoloralpha = "1",
	  gridvisible = "1",
	  highlightededgecolor = "#c41b50",
	  highlightededgecoloralpha = "1",
	  highlightednodecolor = "#d1cd24",
	  highlightednodecoloralpha = "1",
	  defaultlinewidth = "1"
	  nodesizewithdegree = "1",
	  randomizeedgecolors = "1",
	  randomizenodecolors = "1",
	  selectededgecolor = "#ffc0cb",
	  selectededgecoloralpha = "1",
	  selectednodecolor = "#8ce61d",
	  selectednodecoloralpha = "1",
	  gridcoloralpha = "1",
	  defaultmagnifierwidth = "300",
	  defaultmagnifierheight = "200",
	  defaultmagnifierkts = "5",
	  defaultfisheyemagnifierradius = "250"
	  defaultfisheyemagnifierdistort = "5",
	  usermode = "1",
	  topologicalfisheyefinenodes = "50",
	  topologicalfisheyecoarseningfactor = "2.5",
	  topologicalfisheyedistortionfactor = "1",
	  topologicalfisheyedist2limit = "1",
	  topologicalfisheyeanimate = "1",
	  topologicalfisheyelabelfinenodes = "1",
	  topologicalfisheyecolornodes = "1",
	  topologicalfisheyecoloredges = "1",
	  topologicalfisheyelabelfocus = "1",
	  topologicalfisheyefinestcolor = "red",
	  topologicalfisheyecoarsestcolor = "green"];

    border color
#endif
     colorxlate(get_attribute_value("bordercolor", view, g), &cl,
		RGBA_DOUBLE);
    view->borderColor.R = (float) cl.u.RGBA[0];
    view->borderColor.G = (float) cl.u.RGBA[1];
    view->borderColor.B = (float) cl.u.RGBA[2];

    view->borderColor.A =
	(float) atof(get_attribute_value("bordercoloralpha", view, g));

    view->bdVisible =
	 atoi(get_attribute_value("bordervisible", view, g));

    buf = get_attribute_value("gridcolor", view, g);
    colorxlate(buf, &cl, RGBA_DOUBLE);
    view->gridColor.R = (float) cl.u.RGBA[0];
    view->gridColor.G = (float) cl.u.RGBA[1];
    view->gridColor.B = (float) cl.u.RGBA[2];
    view->gridColor.A =
	(float) atof(get_attribute_value("gridcoloralpha", view, g));

    view->gridSize = (float) atof(buf =
				  get_attribute_value("gridsize", view,
						      g));

    view->gridVisible = atoi(get_attribute_value("gridvisible", view, g));

    //mouse mode=pan

    //background color , default white
    colorxlate(get_attribute_value("bgcolor", view, g), &cl, RGBA_DOUBLE);

    view->bgColor.R = (float) cl.u.RGBA[0];
    view->bgColor.G = (float) cl.u.RGBA[1];
    view->bgColor.B = (float) cl.u.RGBA[2];
    view->bgColor.A = (float)1;

    //selected nodes are drawn with this color
    colorxlate(get_attribute_value("selectednodecolor", view, g), &cl,
	       RGBA_DOUBLE);
    view->selectedNodeColor.R = (float) cl.u.RGBA[0];
    view->selectedNodeColor.G = (float) cl.u.RGBA[1];
    view->selectedNodeColor.B = (float) cl.u.RGBA[2];
    view->selectedNodeColor.A =
	(float)
	atof(get_attribute_value("selectednodecoloralpha", view, g));
    //selected edge are drawn with this color
    colorxlate(get_attribute_value("selectededgecolor", view, g), &cl,
	       RGBA_DOUBLE);
    view->selectedEdgeColor.R = (float) cl.u.RGBA[0];
    view->selectedEdgeColor.G = (float) cl.u.RGBA[1];
    view->selectedEdgeColor.B = (float) cl.u.RGBA[2];
    view->selectedEdgeColor.A =
	(float)
	atof(get_attribute_value("selectededgecoloralpha", view, g));

    colorxlate(get_attribute_value("highlightednodecolor", view, g), &cl,
	       RGBA_DOUBLE);
    view->highlightedNodeColor.R = (float) cl.u.RGBA[0];
    view->highlightedNodeColor.G = (float) cl.u.RGBA[1];
    view->highlightedNodeColor.B = (float) cl.u.RGBA[2];
    view->highlightedNodeColor.A =
	(float)
	atof(get_attribute_value("highlightednodecoloralpha", view, g));

    buf = agget(g, "highlightededgecolor");
    colorxlate(get_attribute_value("highlightededgecolor", view, g), &cl,
	       RGBA_DOUBLE);
    view->highlightedEdgeColor.R = (float) cl.u.RGBA[0];
    view->highlightedEdgeColor.G = (float) cl.u.RGBA[1];
    view->highlightedEdgeColor.B = (float) cl.u.RGBA[2];
    view->highlightedEdgeColor.A =
	(float)
	atof(get_attribute_value("highlightededgecoloralpha", view, g));


/*

but here i am, on the road again
here i am, up on the stage
here i go, playing the star again
there i go, turn the page

*/

    /*default line width */
    view->LineWidth =
	(float) atof(get_attribute_value("defaultlinewidth", view, g));
    view->FontSize = atoi(get_attribute_value("defaultfontsize", view, g));

    view->topviewusermode = atoi(get_attribute_value("usermode", view, g));
    get_attribute_value("defaultmagnifierwidth", view, g);
    view->mg.width =
	atoi(get_attribute_value("defaultmagnifierwidth", view, g));
    view->mg.height =
	atoi(get_attribute_value("defaultmagnifierheight", view, g));

    view->mg.kts =
	(float) atof(get_attribute_value("defaultmagnifierkts", view, g));

    view->fmg.constantR =
	atoi(get_attribute_value
	     ("defaultfisheyemagnifierradius", view, g));

    view->fmg.fisheye_distortion_fac =
	atoi(get_attribute_value
	     ("defaultfisheyemagnifierdistort", view, g));

//FIXME: I don't think an openGL function can be called before it
  //     is initialized.
#ifdef _WIN32
    glClearColor(view->bgColor.R, view->bgColor.G, view->bgColor.B, view->bgColor.A);	//background color
#endif
}

void init_viewport(ViewInfo * view)
{
    char *template_file;
    FILE *input_file;

    /*loading default visual attributes */
#ifdef _WIN32
    template_file = DEFAULT_ATTRIBUTES_TEMPLATE_DOT_FILE;
#else
    template_file = smyrnaPath ("gui/template.dot");
#endif
    input_file = fopen(template_file, "r");
    if (!input_file) {
	fprintf (stderr, "default attributes template graph file \"%s\" not found\n", template_file);
	exit(-1);
    } else if (!(view->default_attributes = agread(input_file, 0))) {
	fprintf (stderr, "could not load default attributes template graph file \"%s\"\n", template_file);
	exit(-1);
    }
#ifndef _WIN32
    free (template_file);
#endif

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

    view->bdVisible = 1;	//show borders red

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
    view->fmg.constantR = DEFAULT_FISHEYE_MAGNIFIER_RADIUS;
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
    view->Topview = GNEW(topview);
    view->Topview->topviewmenu = '\0';
	view->cameras='\0';;
	view->camera_count=0;
	view->active_camera=-1;

    set_viewport_settings_from_template(view, view->default_attributes);
}


/* load_graph_params:
 * run once right after loading graph
 */
static void load_graph_params(Agraph_t * graph)
{
    char* s;
    //file may or may have not gl edit attributes
    //first defaults are set in loading function
    //here file is checked for previously saved gledit attributes
    if ((s == agget(graph, "GraphName")))	//Graph Name
    {
	GD_GraphName(graph) = N_GNEW(strlen(s)+1,char);
	strcpy(GD_GraphName(graph), s);
    }
    if ((s = agget(graph, "AlwaysShow")))
	GD_AlwaysShow(graph) = atoi(s);
    else
	GD_AlwaysShow(graph) = 0;

    if ((s = agget(graph, "TopView")))
	GD_TopView(graph) = atoi(s);
    else
	GD_TopView(graph) = 0;
    if ((s = agget(graph, "Locked")))
	GD_Locked(graph) = atoi(s);
    else
	GD_Locked(graph) = 0;
    if ((s = agget(graph, "Engine")))
	GD_Engine(graph) = atoi(s);
    else
	GD_Engine(graph) = 0;

    GD_Modified(graph) = 0;	//not modified yet
    GD_selectedEdges(graph) = NULL;
    GD_selectedNodes(graph) = NULL;
    GD_selectedGraphs(graph) = NULL;

    GD_selectedNodesCount(graph) = 0;
    GD_selectedEdgesCount(graph) = 0;
    GD_selectedGraphsCount(graph) = 0;

#if 0
    if((s = agget(graph, "xdotversion")))
	((custom_graph_data*)AGDATA(g))->view->Topview=0; //need to check xdot version attribute
    else	//we dont know if it is view->Topview or simply a graph with no xdot, for testing i ll use view->Topview
	((custom_graph_data*)AGDATA(g))->view->Topview=1;
#endif

}

/* attach_object_custom_data_to_graph:
 * run once or to reset all data !! prev data is removed
 */
static int attach_object_custom_data_to_graph(Agraph_t * graph)
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

/* update_graph_params:
 * adds gledit params
 */
static void update_graph_params(Agraph_t * graph)
{
    char tempString[100];

    agattr(graph, AGRAPH, "GraphFileName", GD_GraphFileName(graph));
    agattr(graph, AGRAPH, "GraphName", GD_GraphName(graph));
    sprintf(tempString, "%i", GD_AlwaysShow(graph));
    agattr(graph, AGRAPH, "AlwaysShow", tempString);
    sprintf(tempString, "%i", GD_TopView(graph));
    agattr(graph, AGRAPH, "TopView", tempString);
    sprintf(tempString, "%i", GD_Locked(graph));
    agattr(graph, AGRAPH, "Locked", tempString);
    sprintf(tempString, "%i", GD_Engine(graph));
    agattr(graph, AGRAPH, "Engine", tempString);

}

/* clear_object_xdot:
 * clear single object's xdot info
 */ 
static int clear_object_xdot(void *obj)
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

/* clear_graph_xdot:
 * clears all xdot  attributes, used especially before layout change
 */
static int clear_graph_xdot(Agraph_t * graph)
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

/* clear_graph:
 * clears custom data binded
 * FIXME: memory leak - free allocated storage
 */
static void clear_graph(Agraph_t * graph)
{

}

/* create_xdot_for_graph:
 * Returns (malloced) temp filename for output data
 * or NULL on error.
 * Calling program needs to remove file and free storage:
 *    fname = create_xdot_for_graph (...)
 *       ... use fname ...
 *    unlink (fname);
 *    free (fname);
 */
#define FMT "%s%s -Txdot%s %s -o%s"

static char* create_xdot_for_graph(Agraph_t * graph, int keeppos)
{
    static char buf[BUFSIZ];
    static char* cmd = buf;
    static int buflen = BUFSIZ;
    int len;
    int r = 0;
    FILE *output_file;
    char* dotfile;
    char* xdotfile;
    char* fix;
    char* alg;
    char* path;

    update_graph_params(graph);
    if (!(dotfile = tempnam(0,"_dot"))) return 0;
    if (!(xdotfile = tempnam(0,"_xdot"))) {
	free (dotfile);
	return 0;
    }
    if (!(output_file = fopen(dotfile, "w"))) {
	free (xdotfile);
	free (dotfile);
	return 0;
    }

    clear_graph_xdot(graph);
    agwrite(graph, (void *) output_file);
    fclose(output_file);

    if (keeppos) {
	fix = " -n2";
	alg = " -Kneato";
    }
    else {
	fix = "";
	switch (GD_Engine(graph)) {
	case GVK_DOT :
	    alg = " -Kdot";
	    break;
	case GVK_NEATO :
	    alg = " -Kneato";
	    break;
	case GVK_TWOPI :
	    alg = " -Ktwopi";
	    break;
	case GVK_CIRCO :
	    alg = " -Kcirco";
	    break;
	case GVK_FDP :
	    alg = " -Kfdp";
	    break;
	default :
	    unlink (dotfile);
	    free (dotfile);
	    return 0;
	    break;
	}
    } 

#ifdef _WIN32
    path = "\"C:/Program Files/Graphviz2.15/bin/dot.exe\" ";
#else
    path = "dot ";
#endif
    
    len = strlen(path) + strlen(fix) + strlen(alg) + strlen(dotfile) +
	strlen(xdotfile) + sizeof(FMT);
    if (len > buflen) {
	buflen = len + BUFSIZ;
	if (cmd == buf) cmd = N_GNEW(buflen, char);
	else cmd = RALLOC (buflen, cmd, char);
    }
    sprintf (cmd, FMT, path, fix, alg, dotfile, xdotfile);
    r = system (cmd);
    unlink (dotfile);
    free (dotfile);

    if (r) { // something went wrong
	unlink (xdotfile);
	free (xdotfile);
	return 0;
    }
    else return xdotfile;
}

static int mapbool(char *p)
{
    if (p == NULL)
        return 0;
    if (!strcasecmp(p, "false"))
        return 0;
    if (!strcasecmp(p, "true"))
        return 1;
    return atoi(p);
}

/* loadGraph:
 * dont use directly, use add_graph_to_viewport_from_file instead
 */
static Agraph_t *loadGraph(char *filename)
{
    Agraph_t *g;
    FILE *input_file;
    char *infile;
    if (!(input_file = fopen(filename, "r"))) {
	g_print("Cannot open %s\n", filename);
	return 0;
    }
    if (!(g = agread(input_file, NIL(Agdisc_t *)))) {
	g_print("Cannot read graph in  %s\n", filename);
	fclose (input_file);
	return 0;
    }

	/* If xdot is not available in g and TopView is false,
         * run layout with -Txdot
         */
    if ((!agget(g, "xdotversion")) && !mapbool (agget(g, "TopView"))) {
	if (!(infile = create_xdot_for_graph(g, 0))) return 0;

	while (!(input_file = fopen(infile, "r"))) {	//HACK!!!!
//                   g_print("Cannot open xdot  error %si\n",strerror(errno));

	}
	agclose (g);
	g = agread(input_file, NIL(Agdisc_t *));
	g_print ("xdot is being loaded\n");
	fclose (input_file);
	unlink (infile);  // Remove temp file
	free (infile);    // Free storage for temp file name
    }

    attach_object_custom_data_to_graph(g);
    load_graph_params(g);

    GD_GraphFileName(g) = strdup (filename);

#if 0
    if(strcasecmp(agget(g, "TopView"),"1")==0) {
	if(TopviewNodeCount > 0) {
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
	else {
	    PrepareTopview(g);
	}
    }
#endif
    preparetopview(g, view->Topview);
    return g;
}

/* add_graph_to_viewport_from_file:
 * returns 1 if successfull else 0
 */
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

#if 0
/* add_new_graph_to_viewport:
 * returns graph index , otherwise -1
 */
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
#endif

void refreshControls(ViewInfo * v)
{

    int i = 0;
    GtkComboBox *widget;
    widget = get_SelectGraph();
    //load graph names to combobox
    for (i = 0; i < v->graphCount; i++) {
	gtk_combo_box_append_text(widget, GD_GraphFileName(v->g[i]));
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


    switch (GD_Engine(view->g[view->activeGraph])) {
    case GVK_DOT :
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnDot"));
	break;
    case GVK_NEATO :
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnNeato"));
	break;
    case GVK_TWOPI :
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnTwopi"));
	break;
    case GVK_CIRCO :
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnCirco"));
	break;
    case GVK_FDP :
	Color_Widget_bg("red", glade_xml_get_widget(xml, "btnFdp"));
	break;
    }
    glexpose();
}

/* save_graph_with_file_name:
 * saves graph with file name; if file name is NULL save as is
 */
static int save_graph_with_file_name(Agraph_t * graph, char *fileName)
{
    FILE *output_file;
    update_graph_params(graph);
    if (fileName)
	output_file = fopen(fileName, "w");
    else if (GD_GraphFileName(graph))
	output_file = fopen(GD_GraphFileName(graph), "w");
    else {
	g_print("there is no file name to save! Programmer error\n");
	return 0;
    }

    if (output_file == NULL) {
	g_print("Cannot create file \n");
	return 0;
    } 

    if (agwrite(graph, (void *) output_file)) {
	g_print("%s successfully saved \n", fileName);
	return 1;
    }
    return 0;
}

/* save_graph:
 * save without prompt
 */
int save_graph()
{
    //check if there is an active graph
    if (view->activeGraph > -1) {
	//check if active graph has a file name
	if (GD_GraphFileName(view->g[view->activeGraph])) {
	    return save_graph_with_file_name(
			view->g[view->activeGraph],
			GD_GraphFileName(view->g[view-> activeGraph]));
	} else
	    return save_as_graph();
    }
    return 1;

}

/* save_as_graph:
 * save with prompt
 */
int save_as_graph()
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

/* do_graph_layout:
 * changes the layout. all user relocations are reset unless 
 * keeppos is set to 1
 * FIXME: the graph parameter is never used.
 */
int do_graph_layout(Agraph_t * graph, int Engine, int keeppos)
{
#if 0
    Agnode_t *v;
    Agedge_t *e;
    Agsym_t *attr;
    Dict_t *d;
    int cnt;
    mydata *p;
#endif
    FILE *input_file;
    char* infile;
    Agraph_t* oldg = view->g[view->activeGraph];
    Agraph_t* newg;

    GD_Engine(oldg) = Engine;
    create_xdot_for_graph(oldg, keeppos);
    if (!(infile = create_xdot_for_graph(oldg, keeppos))) return 0;
    while (!(input_file = fopen(infile, "r"))) {	//HACK!!!!
	/* g_print("Cannot open xdot  error %si\n",strerror(errno)); */
    }

    newg = agread(input_file, NIL(Agdisc_t *));
    fclose (input_file);
    unlink (infile);
    free (infile);

    if (!newg) return 0;
	//attaching rec for graph fields
    attach_object_custom_data_to_graph(newg);
	//set real file name
    GD_GraphFileName(newg) = GD_GraphFileName(oldg); 
    load_graph_params(newg);	//init glparams
	//set engine
    GD_Engine(newg) = GD_Engine(oldg); 
    GD_Modified(newg) = 1;

    clear_graph(oldg);
    agclose(oldg);
    view->g[view->activeGraph] = newg;

    refreshControls(view);

    return 1;
}

#if 0
void listg(Agraph_t * g)
{
    Agnode_t *v;
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	fprintf(stderr, "%s\n", agnameof(v));
    }
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

#endif
		/*
 * Object Custom Data Functions
 */

/* init_object_custom_data:
 * creates a custom_object_data
 */
static int init_object_custom_data(Agraph_t * graph, void *obj)
{
    agdelrec(graph, "custom_object_data");
    agbindrec(obj, "custom_object_data", sizeof(custom_object_data), TRUE);
    OD_ID(obj) = 0;
    OD_ObjName(obj) = NULL;
    OD_ObjType(obj) = AGTYPE(obj);
    OD_Layer(obj) = -1;
    OD_Visible(obj) = 1;
    OD_Locked(obj) = 0;
    OD_Highlighted(obj) = 0;
    OD_NumDataCount(obj) = 0;
    OD_NumData(obj) = NULL;
    OD_StrDataCount(obj) = 0;
    OD_StrData(obj) = NULL;
    return 1;
}

#if 0
static int clear_string_data_from_object_custom_data(void *obj)
{
    if (obj != NULL) {
	int ind = 0;
	for (ind = 0; ind < OD_StrDataCount(obj); ind++) {
	    free(OD_StrData(obj)[ind]);
	}
	free(OD_StrData(obj);
	return 1;
    }
    return 0;
}

/* clear_object_custom_data:
 * frees memory allocated for cutom object data
 */
static int clear_numeric_data_from_object_custom_data(void *obj)
{
    if (obj != NULL) {
	free(OD_NumData(obj);
	return 1;
    }
    return 0;
}

static int clear_object_custom_data(void *obj)
{
    return ((clear_string_data_from_object_custom_data(obj))
	    || (clear_numeric_data_from_object_custom_data(obj)));
}

static int add_string_data_to_object_custom_data(void *obj, char *data)
{
    if ((obj != NULL) && (data != NULL)) {
	OD_StrData(obj) = RALLOC(OD_StrDataCount(obj)+1,OD_StrData(obj), char*);
	OD_StrData(obj)[OD_StrDataCount(obj)] = N_GNEW((strlen(data)+1), char);
	strcpy(OD_StrData(obj)[OD_StrDataCount(obj)], data);
	OD_StrDataCount(obj)++;
	return 1;
    }
    return 0;
}

int add_numeric_data_to_object_custom_data(void *obj, float data)
{
    if (obj != NULL) {
	OD_NumData(obj) = RALLOC(OD_NumDataCount(obj)+1,OD_NumData(obj), char*);
	OD_NumData(obj)[OD_NumDataCount(obj)] = data;
	OD_NumDataCount(obj)++;
	return 1;
    }
    return 0;
}
#endif

/* move_node:
 */ 
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
	    if (OD_Selected((headn)->obj) && OD_Selected((tailn)->obj)) {
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

/* move_nodes:
 * move selected nodes 
 */
void move_nodes(Agraph_t * g)
{
    Agnode_t *obj;

    float dx, dy;
    xdot *bf;
    int i = 0;
    dx = view->GLx - view->GLx2;
    dy = view->GLy - view->GLy2;

    if (GD_TopView(view->g[view->activeGraph]) == 0) {
	for (i = 0; i < GD_selectedNodesCount(g); i++) {
	    obj = GD_selectedNodes(g)[i];
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

int setGdkColor(GdkColor * c, char *color) {
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
