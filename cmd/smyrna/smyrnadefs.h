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

#ifndef SMYRNADEFS_H
#define SMYRNADEFS_H

#ifdef WIN32
#ifndef NO_WIN_HEADER
#include "windows.h"
#endif
//#define       SMYRNA_GLADE "C:/Projects/graphviz2/share/gui/smyrna.glade"
#define	SMYRNA_ATTRS "c:/graphviz-ms/graphviz2/share/gui/attrs.txt"
#endif
#ifdef	WIN32			//this  is needed on WIN32 to get libglade see the callback
#define _BB  __declspec(dllexport)
#else
#define _BB
#endif

#include "xdot.h"
#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gtk/gtkgl.h>
#include "cgraph.h"
#include "glcompset.h"
#include "hier.h"
#include "md5.h"
#include "glutils.h"
#include "arcball.h"

#define IS_TEST_MODE_ON							0
#define	DEFAULT_MAGNIFIER_WIDTH					300
#define	DEFAULT_MAGNIFIER_HEIGHT				225
#define DEFAULT_MAGNIFIER_KTS					3	//x3
#define DEFAULT_FISHEYE_MAGNIFIER_RADIUS		250
#define TOP_VIEW_USER_ADVANCED_MODE				0
#define TOP_VIEW_USER_NOVICE_MODE				1
#define GL_VIEWPORT_FACTOR						100
//mouse modes
#define MM_PAN					0
#define MM_ZOOM					1
#define MM_ROTATE				2
#define MM_SINGLE_SELECT		3
#define MM_RECTANGULAR_SELECT	4
#define MM_RECTANGULAR_X_SELECT	5
#define MM_MOVE					10
#define MM_MAGNIFIER			20
#define MM_FISHEYE_MAGNIFIER	21
#define GLOBAL_Z_OFFSET			0.001

#define MAX_ZOOM	500
#define MIN_ZOOM	0.005
#define ZOOM_STEPS	100

#define ZOOM_STEP	0.5
#define DEG2RAD  G_PI/180
#define RAD2DEG	  1/0.017453292519943

#define UNHIGHLIGHTED_ALPHA	0.3
#define Z_FORWARD_PLANE -0.00201
#define Z_MIDDLE_PLANE 0.0000
#define Z_BACK_PLANE -0.00199

#define NODE_ZOOM_LIMIT	-25.3
#define NODE_CIRCLE_LIMIT	-7.3

#define GL_DOTSIZE_CONSTANT -18
#define DOUBLE_IT 10.00


#define SPHERE_SLICE_COUNT 6
#define DOT_SIZE_CORRECTION_FAC 0.3

typedef enum { nodshapedot,nodeshapecircle} node_shape;
typedef enum { leftmousebutton,rightmousebutton,thirdmousebutton} clicked_mouse_button;
typedef enum { MOUSE_ROTATE_X,MOUSE_ROTATE_Y,MOUSE_ROTATE_XY,MOUSE_ROTATE_Z} mouse_rotate_axis;

typedef struct {
    float R;
    float G;
    float B;
    float A;			//Alpha
	int tag;	/*custom data field*/
} RGBColor;

#define MAX_BTN_CNT 50
typedef struct{
	float perc;
	RGBColor c;
	int smooth;

}colorschema;

typedef struct{
	char* script;
	char* args;
	char* attr_name; /*attribute name to identify script in the graph*/
}gvprscript;

typedef struct{
	int schemacount;
	colorschema* s;
}colorschemaset;



typedef enum { 
    VT_NONE,
    VT_XDOT,
    VT_TOPVIEW,
    VT_TOPFISH,
} viewtype_t;


typedef enum { 
    GVE_NONE = -1, 
    GVE_GRAPH,
    GVE_CLUSTER,
    GVE_NODE,
    GVE_EDGE   /* keep last */
} gve_element;

typedef enum { 
    GVK_NONE = -1, 
    GVK_DOT,
    GVK_NEATO,
    GVK_TWOPI,
    GVK_CIRCO,
    GVK_FDP,   
    GVK_SFDP/* keep last */
} gvk_layout;

typedef struct {
	int anglex;
	int angley;
	int anglez;
} rotation;

typedef struct {
	int anglex;
	int angley;
	int anglez;
} gl3DNav;


typedef struct {
    GtkButton **gtkhostbtn;
    int gtkhostbtncount;
    GtkColorButton **gtkhostcolor;
    int hostactive[MAX_BTN_CNT];  //temporary static, convert to dynamic,realloc
    char **hostregex;
} topviewdata;

typedef struct xdot_set xdot_set;

//bind this to cgraph g
/*
#define GD_data(g) ((custom_graph_data*)AGDATA(g))
#define GD_TopView(g) (GD_data(g)->TopView)
#define GD_Engine(g) (GD_data(g)->Engine)
#define GD_GraphFileName(g) (GD_data(g)->GraphFileName)
#define GD_GraphName(g) (GD_data(g)->GraphName)
#define GD_AlwaysShow(g) (GD_data(g)->AlwaysShow)
#define GD_Locked(g) (GD_data(g)->Locked)
#define GD_Modified(g) (GD_data(g)->Modified)
#define GD_selectedGraphs(g) (GD_data(g)->selectedGraphs)
#define GD_selectedGraphsCount(g) (GD_data(g)->selectedGraphsCount)
#define GD_selectedNodesCount(g) (GD_data(g)->selectedNodesCount)
#define GD_selectedNodes(g) (GD_data(g)->selectedNodes)
#define GD_selectedEdgesCount(g) (GD_data(g)->selectedEdgesCount)
#define GD_selectedEdges(g) (GD_data(g)->selectedEdges)
*/


typedef enum { GEpixels, GEinches, GEmm } GEunit;

typedef struct _object_data	//has to be attached to every Node, Edge, Graph and cluster
{
    Agrec_t h;
    int ID;
    char *ObjName;
    int ObjType;
    int Layer;
    int Visible;
    int Locked;
    int Highlighted;
    int Selected;
    int Preselected;
    int NumDataCount;
    float *NumData;
    int StrDataCount;
    char **StrData;
    int selectionflag;
    int param;			//generic purpose param
    int TVRef;			//Topview reference
	int edgeid;	/*for only edges,  > 0  multiedges*/

} element_data;
typedef struct _temp_node_record	//helper record to identofy head and tail of edges
{
    Agrec_t h;
    int ID;
    int TVref; //topview data structure reference
}temp_node_record;

/*#define OD_ID(p) (((custom_object_data*)AGDATA(p))->ID)
#define OD_ObjName(p) (((custom_object_data*)AGDATA(p))->ObjName)
#define OD_ObjType(p) (((custom_object_data*)AGDATA(p))->ObjType)
#define OD_Layer(p) (((custom_object_data*)AGDATA(p))->Layer)
#define OD_Visible(p) (p->data.Visible)
#define OD_Locked(p) (((custom_object_data*)AGDATA(p))->Locked)
#define OD_Highlighted(p) (((custom_object_data*)AGDATA(p))->Highlighted)
#define OD_NumDataCount(p) (((custom_object_data*)AGDATA(p))->NumDataCount)
#define OD_NumData(p) (((custom_object_data*)AGDATA(p))->NumData)
#define OD_StrDataCount(p) (((custom_object_data*)AGDATA(p))->StrDataCount)
#define OD_StrData(p) (((custom_object_data*)AGDATA(p))->StrData)
#define OD_Selected(p) (((custom_object_data*)AGDATA(p))->Selected)
#define OD_Preselected(p) (((custom_object_data*)AGDATA(p))->Preselected)
#define OD_SelFlag(p) (((custom_object_data*)AGDATA(p))->selectionflag)
#define OD_TVRef(p) (((custom_object_data*)AGDATA(p))->TVRef)*/

#define OD_Visible(p) (p.data.Visible)
#define OD_Locked(p) (p.data.Locked)
#define OD_Highlighted(p) (p.data.Highlighted)



typedef enum { CAM_PERSPECTIVE,CAM_ORTHO} cam_t;

typedef struct _viewport_camera{
	float x;
	float y;
	float z;

	float targetx;
	float targety;
	float targetz;
	int index;
	float anglexy;
	float anglexyz;

	float anglex;
	float angley;
	float anglez;


	float camera_vectorx;
	float camera_vectory;
	float camera_vectorz;
	float r;


	cam_t type; //
} viewport_camera;



typedef struct {
    Agnode_t *Node;
    /*original coordinates */
    float x;
    float y;
    float z;
    /*coordinates to draw */
    float distorted_x;
    float distorted_y;
    float distorted_z;
    float zoom_factor;
    int in_fish_eye;		//boolean value if to apply fisheye
    RGBColor Color;
    RGBColor GroupColor;
    int GroupIndex;		//default -1;
    int update_required;
    char *Label;
    char *Label2;
    int degree;
    float node_alpha;
    int valid;
	element_data data;
	float size;
} topview_node;

typedef struct {
//    topview_node *Tnode;		//Tail node
//    topview_node *Hnode;		//Tail node
    Agedge_t *Edge;		//edge itself
    float x1;
    float y1;
    float z1;
    float x2;
    float y2;
    float z2;
	float length;
    topview_node *Node1; //Tail
    topview_node *Node2; //Head
    RGBColor Color;
    int update_required;
	element_data data;

} topview_edge;
typedef struct _graph_data {
    Agrec_t h;
    char *GraphFileName;
    //graph's location, change these to move the whole graph
    int Modified;		//if graph has been modified after loading
    float offsetx;
    float offsety;
    float offsetz;
} graph_data;

typedef struct {
    topview_node *Nodes;
    topview_edge *Edges;
    int Nodecount;
    int Edgecount;
    int limits[4];
    topviewdata *TopviewData;
    void *customptr;
    Hierarchy *h;
	glCompColor srcColor;	//fine node colors of topfisheye
	glCompColor tarColor;   //supernode colors of fisheye
    int is_top_fisheye;	//1 draw hierarchy 0 draw regular topview
    focus_t* fs;
    struct {
	reposition_t repos;
	levelparms_t level;
	hierparms_t hier;
    } parms;
	int animate;
	topview_node** picked_nodes;
	int picked_node_count;
	topview_edge** picked_edges;
	int picked_edge_count;

	graph_data Graphdata;
	int maxnodedegree;
	float maxedgelen;
	float minedgelen;
	float avgedgelength;
	float init_node_size;	//raster size of node
	float init_zoom;
	float fitin_zoom;
	xdot_set* xdot_list;    /*xdot attached to whole graph*/

} topview;



enum {
    COL_NAME = 0,
    COL_FILENAME,
    NUM_COLS
};


typedef struct _mouse_attr {
    int mouse_down;
    int mouse_mode;
	int pick;
    float mouse_X;
    float mouse_Y;
	float begin_x;
	float begin_y;
	float dx;
	float dy;	
	float GLX;/*opengl coordiantes for right click*/
	float GLY;
	float GLZ;
	mouse_rotate_axis rotate_axis;	
	clicked_mouse_button button;
} mouse_attr;



typedef struct _attribute {
    char Type;
    char *Name;
    char *Default;
    char ApplyTo[GVE_EDGE+1];
    char Engine[GVK_FDP+1];
    char **ComboValues;
    int ComboValuesCount;
    GtkWidget *attrWidget;

} attribute;

typedef struct _selection {
    int Active;			//0 there is no selection need to be applied
    char Type;			//0     single selection , 1 rectangle , 2 rectangleX 
	int PickingType;	//0 normal, union,2 subtract 3 intersection
	float X, Y, W, H;		//selection boundries
    int Anti;			//subtract selections if 1
    int AlreadySelected;	//for single selections to avoid selecting more than one object
    RGBColor SelectionColor;
	float node_distance; //to get the closest node , this value is updated for each node, distance between selection coords and node coords, smallest gets to be selected
	topview_node* single_selected_node; //pointer to selected node in a single node/edge selection cycle,after each node loop this value is checked and if it is in the limits that node is selected or deselected
										//before the node/edge loop this value is nulled
	topview_edge* single_selected_edge;//pointer to selected/picked edge

} selection;
typedef struct _magnifier {
    float x, y;
    float kts;			//zoom X
    float GLwidth, GLheight;
    int width, height;		//how big is the magnifier referenced from windows
    int active;
} magnifier;

typedef struct _fisheye_magnifier {
    float x, y;			//center coords of active circle
    float distortion_factor;	//distortion factor ,default 1
    int R;			//radius of  the magnifier 
	int constantR;	//radius of  the magnifier referenced from windows
    int active;
    int fisheye_distortion_fac;
} fisheye_magnifier;



typedef struct _ViewInfo
{
	/*view variables*/
	float panx;
	float pany;
	float panz;
	float prevpanx;
	float prevpany;
	float prevpanz;
	float zoom;

	/*clipping coordinates, to avoid unnecesarry rendering*/
	float clipX1,clipX2,clipY1,clipY2,clipZ1,clipZ2;

	/*background color*/
	RGBColor bgColor;
	/*default pen color*/
	RGBColor penColor;
	/*default fill color*/
	RGBColor fillColor;
	/*highlighted Node Color*/
	RGBColor highlightedNodeColor;
	/*highlighted Edge Color*/
	RGBColor highlightedEdgeColor;
	/*grid color*/
	RGBColor gridColor;	//grid color
	/*border color*/
	RGBColor borderColor;
	/*selected node color*/
	RGBColor selectedNodeColor;
	/*selected edge color*/
	RGBColor selectedEdgeColor;
	/*default node alpha*/
	float defaultnodealpha;
	/*default edge alpha*/
	float defaultedgealpha;
	/*default node shape*/
	int defaultnodeshape;

	/*default line width*/
	float LineWidth;
	
	/*grid is drawn if this variable is 1*/
	int gridVisible;	
	/*grid cell size in gl coords system*/
	float gridSize;	//grid cell size

	/*draws a border in border colors if it is 1*/
	int bdVisible;	//if borders are visible (boundries of the drawing,
	/*border coordinates, needs to be calculated for each graph*/

	/*randomize node colors or use default node color*/
	int rndNodeColor;

	/*randomize edge colors or use default edge color*/
	int rndEdgeColor;
	/*Font Size*/
	float FontSize;


	float bdxLeft,bdyTop,bdzTop;	
	float bdxRight,bdyBottom,bdzBottom; 

	/*reserved , not being used yet*/
	GEunit unit;	//default pixels :0  

	/*variable to hold mouse coordinates temporarily*/
	float GLx,GLy,GLz;		
	/*this is second  set of mouse coordinates holder for, it is needed to draw a rectangle with mouse*/
	float GLx2,GLy2,GLz2;		

	/*screen window size in 2d*/	
	int w,h;
	/*graph pointer to hold loaded graphs*/
	Agraph_t** g;
	/*number of graphs loaded*/
	int graphCount;		
	/*active graph*/
	int activeGraph; 

	/*texture data*/
	int texture;	/*1 texturing enabled, 0 disabled*/	
	/*opengl depth value to convert mouse to GL coords*/
	float GLDepth;		

	/*stores the info about status of mouse,pressed? what button ? where?*/
	mouse_attr mouse;

	/*selection object,refer to smyrnadefs.h for more info*/
	selection Selection;

	/*rectangular magnifier object*/
	magnifier mg;
	/*fisheye magnifier object*/
	fisheye_magnifier fmg;

	viewport_camera** cameras;
	int camera_count; //number of cameras
	int active_camera;
	viewport_camera* selected_camera;	//selected camera does not have to nec. be active one 

	/*data attributes are read from graph's attributes DataAttribute1 and DataAttribute2*/
	char* node_data_attribute1;	/*for topview graphs this is the node data attribute to put as label*/
	char* node_data_attribute2;	/*for topview graphs this is the node data attribute to be stored and used for something else*/

	/*0 advanced users with editing options 1 nonice users just navigate (glmenu system)*/
	int topviewusermode;	
	/*this should not be confused with graphviz node shapes, it can be dot or circles (dots are rendered mych faster, circle looks handsome, if graph is ulta large go with dot*/
//	node_shape nodeshape;
	/*if true and nodeshape is nodeshapecircle , radius of nodes changes with degree*/
	int nodesizewithdegree;

	/*open gl canvas, used to be a globa variable before looks better wrapped in viewinfo*/
	GtkWidget *drawing_area; 

	/*some boolean variable for variety hacks used in the software*/
	int SignalBlock;

	/*Topview data structure, refer topview.h for more info*/
	topview* Topview;
	Agraph_t* default_attributes;
	/*timer for animations*/
	GTimer* timer;
	/*this timer is session timer and always active*/
	GTimer* timer2;
	int active_frame;
	int total_frames;
	int frame_length;
	/*lately added*/
	int drawnodes;
	int drawedges;
	int drawlabels;
	int drawnodelabels;
	int drawedgelabels;

	/*labelling properties*/
	void* glutfont;
	glCompColor nodelabelcolor;
	glCompColor edgelabelcolor;
	int labelwithdegree;
	int labelnumberofnodes;
	int labelshownodes;
	int labelshowedges;

	viewtype_t dfltViewType;
	gvk_layout dfltEngine;
	GtkTextBuffer* consoleText;
	float FontSizeConst;
    glCompSet *widgets;	//for novice user open gl menu
	int visiblenodecount;	/*helper variable to know the number of the nodes being rendered, good data to optimize speed*/
	md5_byte_t orig_key[16] ;	/*md5 result for original graph*/
	md5_byte_t final_key[16] ;	/*md5 result right before graph is saved*/
	char* initFileName;	//file name from command line
	int initFile;
	colorschemaset* colschms;
	char* template_file;
	char* glade_file;
	char* attr_file;
	int flush;
	line interpol;
	gvprscript* scripts;
	int script_count;  /*# of scripts*/
	GtkComboBox* graphComboBox;/*pointer to graph combo box at top right*/
	ArcBall_t* arcball;

} ViewInfo;
/*rotation steps*/




extern ViewInfo *view;
extern GtkMessageDialog *Dlg;
extern int respond;
extern char* smyrnaPath (char* suffix);
extern char* smyrnaGlade;

extern void glexpose(void);

extern char* layout2s (gvk_layout gvkl);
extern gvk_layout s2layout (char* s);
extern char* element2s (gve_element);
extern unsigned char SmrynaVerbose;

#endif
