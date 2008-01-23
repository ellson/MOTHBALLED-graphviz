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

//view data structure
#ifndef VIEWPORT_H
#define VIEWPORT_H
#define bool int
#include <glTemplate.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>
#include "xdot.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pango/pangoft2.h>
#include "cgraph.h"
#include "selection.h"
#include "draw.h"
#define IS_TEST_MODE_ON				0
#define	DEFAULT_MAGNIFIER_WIDTH		300
#define	DEFAULT_MAGNIFIER_HEIGHT	225
#define DEFAULT_MAGNIFIER_KTS		10	//x10
#define DEFAULT_FISHEYE_MAGNIFIER_RADIUS 250;
//=======
#define MAX_ZOOM	-1.000033
#define MIN_ZOOM	-89.00000
#define ZOOM_STEP	1;
//>>>>>>> 1.8
enum
{
  COL_NAME = 0,
  COL_FILENAME,
  NUM_COLS
} ;
//atributes
typedef struct _mouse_attr
{
	int mouse_down;
	int mouse_mode;
	int mouse_X;
	int mouse_Y;
}mouse_attr;


typedef struct _attribute
{
	char Type;
	char* Name;
	char* Default;
	char ApplyTo[4];
	char Engine[5];
	char** ComboValues;
	int ComboValuesCount;
	GtkWidget* attrWidget;

}attribute;

//bind this to cgraph g
typedef struct _custom_graph_data
{
	Agrec_t	h;
	char* GraphName;
	char* GraphFileName;
	int AlwaysShow; //active or not draw it 
	int TopView;	//default 0, 1 for topview data, dots and lines
	int Locked;
	int Engine;	//enum GVEngine{DOT,NEATO,TWOPI,CIRCO,FDP};
	//graph's location, change these to move the whole graph
	int Modified;	//if graph has been modified after loading
	GLfloat offsetx;
	GLfloat offsety;
	GLfloat offsetz;

	Agraph_t** selectedGraphs;		//clusters , subgraphs indeed
	Agnode_t** selectedNodes;
	Agedge_t** selectedEdges;

	int selectedGraphsCount;
	int selectedNodesCount;
	int selectedEdgesCount;
} custom_graph_data;



enum GEunit{GEpixels,GEinches,GEmm};

typedef struct{
	float R;
	float G;
	float B;
	float A;	//Alpha
} RGBColor;

typedef struct _custom_object_data		//has to be attached to every Node, Edge, Graph and cluster
{
	Agrec_t	h;
	int ID;
	char* ObjName;
	int ObjType;
	int Layer;
	int Visible;
	int Locked;
	int Highlighted;
	int Selected;
	int Preselected;
	int NumDataCount;
	float* NumData;
	int StrDataCount;
	char** StrData;
	int selectionflag;
	int param;		//generic purpose param
	int TVRef;		//Topview reference

}custom_object_data;

typedef struct _selection
{
	int Active;		//0 there is no selection need to be applied
	char Type;		//0	single selection , 1 rectangle , 2 rectangleX 
	float X,Y,W,H;	//selection boundries
	int Anti;	//subtract selections if 1
	int AlreadySelected; //for single selections to avoid selecting more than one object
	RGBColor SelectionColor;

}selection;
typedef struct _magnifier
{
	GLfloat x,y;
	GLfloat kts;	//zoom X
	GLfloat GLwidth,GLheight;
	int width,height;	//how big is the magnifier referenced from windows
	int active;
} magnifier;

typedef struct _fisheye_magnifier
{
	GLfloat x,y;//center coords of active circle
	GLfloat distortion_factor;	//distortion factor ,default 1
	int R;	//radius of  the magnifier referenced from windows
	int active;
} fisheye_magnifier;

typedef struct _ViewInfo
{
	GLfloat panx;
	GLfloat pany;
	GLfloat panz;
	GLfloat prevpanx;
	GLfloat prevpany;
	GLfloat prevpanz;

	GLfloat zoom;

	RGBColor bgColor;
	RGBColor penColor;
	RGBColor fillColor;
	RGBColor selectColor;


	GLfloat LineWidth;
	GLfloat grR,grG,grB,grA;	//grid color
	bool gridVisible;	//if the grid sis visible
	GLfloat gridSize;	//grid cell size

	bool bdVisible;	//if borders are visible (boundries of the drawing,
	GLuint bdR,bdG,bdB,bdA;	//border colors
	GLfloat bdxLeft,bdyTop,bdzTop;	//border top coordinates
	GLfloat bdxRight,bdyBottom,bdzBottom; //border bottom coordinates

	//defaults
	enum GEunit unit;	//default pixels :0  
	//***mouse variables
	GLfloat GLx,GLy,GLz;		//real GL coordinates of xwin mouse coords
	GLfloat GLx2,GLy2,GLz2;		//real GL coordinates(second) of win mouse coords
	float clipX1,clipX2,clipY1,clipY2,clipZ1,clipZ2;
	Agraph_t** g;
	int graphCount;		//number of graphs loaded
	int activeGraph; //



	int texture;		//boolean , to disable and enable texturing, if graph does not have an embedded image
	TextTexture* font_textures;	//text textures
	int font_texture_count;	//count of text  textures uploaded to gl engine
	float GLDepth;		//opengl depth value to convert mouse to GL coords

	GtkWidget* pango_widget;
	char* FontName;
	float FontSize;
	//selection
//	int mousedown;	//0 down , 1 up , used for selection boxes
	mouse_attr mouse;
	selection Selection;
	magnifier mg;
	fisheye_magnifier fmg;
	//data attributes are read from graph's attributes DataAttribute1 and DataAttribute2
	char* node_data_attribute1;	//for topview graphs this is the node data attribute to put as label
	char* node_data_attribute2;	//for topview graphs this is the node data attribute to be stored and used for something else

}ViewInfo;

extern char* globalString;
extern ViewInfo	view;
extern Agraph_t* tempG;	//helper graph for default attr values,
extern int SignalBlock;

extern float TopViewPointsX [50000];
extern float TopViewPointsY [50000];

extern float TopViewEdgesHeadX[50000];
extern float TopViewEdgesHeadY[50000];
extern float TopViewEdgesTailX[50000];
extern float TopViewEdgesTailY[50000];

extern int	TopViewNodeCount;
extern int	TopViewEdgeCount;
extern GtkMessageDialog*  Dlg;
extern int respond;

//ss
void init_viewport(ViewInfo* view);
int add_graph_to_viewport_from_file (char* fileName);	//returns 1 if successfull else 0 ++
int add_new_graph_to_viewport();	//returns graph index , otherwise -1
int create_xdot_for_graph(Agraph_t* graph,int keeppos);	//0 failed , 1 successfull ++
void update_graph_params(Agraph_t* graph);	//adds gledit params  ++
extern Agraph_t* loadGraph(char* filename); //dont use directly, use add_graph_to_viewport_from_file instead 
void load_graph_params(Agraph_t* graph);	//run once right after loading graph++
void clear_graph(Agraph_t* graph);	//clears custom data binded,
int save_graph();	//save without prompt
int save_as_graph(); //save with prompt
int save_graph_with_file_name(Agraph_t* graph,char* fileName);	//saves graph with file name,if file name is NULL save as is ++

int do_graph_layout(Agraph_t* graph,int Engine,int keeppos); //changes the layout, all user relocations are reset
void refreshControls(ViewInfo* v);


int attach_object_custom_data_to_graph(Agraph_t* graph);//run once or to reset all data !! prev data is removed
// int init_object_custom_data(void* obj);//creates a custom_object_data and return its pointer.obj is used to clear prev data
int clear_object_custom_data(void* obj);	//frees memory allocated for cutom object data
int add_string_data_to_object_custom_data(void* obj,char* data);
int add_numeric_data_to_object_custom_data(void* obj,float data);
int clear_string_data_from_object_custom_data(void* obj);
int clear_numeric_data_from_object_custom_data(void* obj);

int clear_object_xdot(void* obj);	//clear single object's xdot info
int clear_graph_xdot(Agraph_t* graph);	//clears all xdot  attributes, used especially before layout change

char* get_object_attribute(void* obj,char* attr);	//returns an attribute value no matter what,

void move_node(void* n,float dx,float dy);




void move_nodes(Agraph_t* g);	//move selected nodes 
//helper functions
RGBColor GetRGBColor(char* color);
int SetGdkColor(GdkColor* c,char* color);
#endif
