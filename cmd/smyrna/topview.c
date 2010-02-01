/* $Id$Revision: */
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
#include "glcompui.h"
#include "gltemplate.h"
#include "topview.h"
#include "math.h"
#include "memory.h"
#include "btree.h"
#include "viewport.h"
#include "viewportcamera.h"
#include "draw.h"
#include "selection.h"
#include "topviewdata.h"
#include "hier.h"
#include "arith.h"
#include "topfisheyeview.h"
#include "beacon.h"
#include "pointset.h"
#include "glpangofont.h"
#ifdef WIN32
#include "regex_win32.h"
#else
#include "regex.h"
#endif
#include "color.h"
#include "colorprocs.h"
#include <assert.h>
#include "frmobjectui.h"
#include "hotkeymap.h"
static float dx = 0.0;
static float dy = 0.0;

   /* Forward declarations */
static void set_boundaries(topview * t);
static void set_topview_options(void);
static int draw_topview_label(topview_node * v, float zdepth);
static int draw_topview_edge_label(topview_edge * e, float zdepth);
static int node_visible(topview_node * n);
static int get_color_from_edge(topview_edge * e);
static void draw_tv_xdot(topview* t);
static void draw_xdot(xdot* x,float base_z);
static int drawtopviewnodes(Agraph_t * g);
static void drawtopviewedges(Agraph_t * g);
void tesstest();



void cleartopview(topview * t)
{
    free(t->Nodes);
	free_attr_list(t->attributes);
    free(t->Edges);

}
static void init_element_data(element_data * d)
{

    d->Layer = 0;
    d->Visible = 1;
    d->Selected = 0;
    d->NumDataCount = 0;
    d->NumData = (float *) 0;
    d->StrDataCount = 0;
    d->StrData = (char **) 0;
    d->param = 0;
    d->TVRef = -1;
}

static void setpositioninfo(float *x, float *y, float *z, char *buf)
{

    /*zero all values */
    char* a;
    static char bf[512];
    strcpy(bf,buf);
    *x = 0;
    *y = 0;
    *z = 0;
    a=strtok(bf,",");
    if(a)
    *x=atof(a);
    a=strtok(NULL,",");
    if(a)
    *y=atof(a);
    a=strtok(NULL,",");
    if(a)
	*z=atof(a);



//    sscanf(buf, "%f,%f,%f", x, y, z);
}

static void setglCompColor(glCompColor * c, char *colorstr)
{
    gvcolor_t cl;
    float A=1;
    /*if node has color attribute */
    if ((colorstr != '\0') && (strlen(colorstr) > 0)) {
	colorxlate(colorstr, &cl, RGBA_DOUBLE);
	c->tag = 1;		/*tag is used to identofy colros set by the graph , not smyrna */
    } else {
	colorxlate(agget(view->g[view->activeGraph], "defaultnodecolor"),
		   &cl, RGBA_DOUBLE);
	
	c->tag = 0;
	A= view->defaultnodealpha;
    }
    c->R = (float) cl.u.RGBA[0];
    c->G = (float) cl.u.RGBA[1];
    c->B = (float) cl.u.RGBA[2];
    c->A = (float) cl.u.RGBA[3]*view->defaultnodealpha;
 
}

#undef DIST2
#undef DIST
#define DIST2(x,y) (((x)*(x))+((y)*(y)))
#define DIST(x,y) (sqrt(DIST2(x,y)))


/*update position info from cgraph*/

/* mapbool:
 */
static int mapbool(char *p, int defv)
{
    if (p == NULL || (*p == '\0'))
	return defv;
    if (!strcasecmp(p, "false"))
	return 0;
    if (!strcasecmp(p, "true"))
	return 1;
    return atoi(p);
}

/* boolAttr:
 * if object has attribute returns its value, else returns 0*/
/*for atttribute values which has no meaning for a 0 or 1 value 0 iz returned,
function is error safe
*/
static int boolAttr(void *obj, Agsym_t * attr, int defv)
{
    if (attr == NULL)
	return defv;
    else
	return mapbool(agxget(obj, attr), defv);
}

static int visible(void *obj, Agsym_t * vattr, Agsym_t * sattr)
{
    if (vattr)
	return boolAttr(obj, vattr, 1);
    else if (sattr) {
	if (strcmp(agxget(obj, sattr), "invis"))
	    return 1;
	else
	    return 0;
    } else
	return 1;
}

static int needs_refresh(ViewInfo* v)
{
    return ((view->refresh.color) || (view->refresh.nodesize) || (view->refresh.pos) || (view->refresh.selection) || (view->refresh.visibility));
}




void settvcolorinfo(Agraph_t * g, topview * t)
{
    float maxedgelen, len, minedgelen, totallen;
    float xmin, xmax, ymin, ymax;
    int ind;
    glCompColor color;
    char *color_string;
    char* tempStr;
    topview_node *np;
    topview_edge *ep;
    Agsym_t *sel = agattr(g, AGNODE, "selected", 0);
    Agsym_t *vis = agattr(g, AGNODE, "visible", 0);
    Agsym_t *sty = agattr(g, AGNODE, "style", 0);
    Agsym_t *ecolor = agattr(g, AGEDGE, "color", 0);
    Agsym_t *edgeid = agattr(g, AGEDGE, "edgeid", 0);
    Agsym_t *poss = agattr(g, AGNODE, "pos", 0);
    /* int setpos=1; */
    assert(poss);
    maxedgelen = 0;
    xmax = ymax = -MAXFLOAT;
    xmin = ymin = minedgelen = MAXFLOAT;
    totallen = 0;
    if (!needs_refresh(view))
	return;

    /*loop nodes */
    for (ind = 0; ind < t->Nodecount; ind++) 
    {
	np = t->Nodes + ind;
	if(view->refresh.color)
	{
	    setglCompColor(&color, agget(np->Node, "color"));
	    np->Color = color;
	}

	/*while in the loop why dont we set some smyrna settings from graph? selected , highlighted , visible */
	if(view->refresh.selection)
	    np->data.Selected = boolAttr(np->Node, sel, 0);
	if(view->refresh.visibility)
		np->data.Visible = visible(np->Node, vis, sty);
	if(view->refresh.nodesize)
	{
	    t->Nodes[ind].size = 0;
	    tempStr=agget(t->Nodes[ind].Node, "size");
	    if(tempStr)
	    {
		if (strlen(tempStr) > 0)	/*set node size */
		    t->Nodes[ind].size = 0.01*atof(tempStr);
	    }

	}
	if (t->Nodes[ind].degree > t->maxnodedegree)
	    t->maxnodedegree = t->Nodes[ind].degree;


	if(view->refresh.pos)
	{
		setpositioninfo(&np->x, &np->y, &np->z, agxget(np->Node, poss));
		/*distorted coordiates, same with original ones at the beginning */
		np->distorted_x = np->x;
		np->distorted_y = np->y;
		np->distorted_z = np->z;
		xmax = MAX(xmax, np->x);
		xmin = MIN(xmin, np->x);
		ymax = MAX(ymax, np->y);
		ymin = MIN(ymin, np->y);
	}
    }

    /*loop edges */
    sel = agattr(g, AGEDGE, "selected", 0);
    vis = agattr(g, AGEDGE, "visible", 0);
    sty = agattr(g, AGEDGE, "style", 0);
    setMultiedges(g, "edgeid");
    edgeid = agattr(g, AGEDGE, "edgeid", 0);
    /*set multi edges */
    for (ind = 0; ind < t->Edgecount; ind++) 
    {
	ep = t->Edges + ind;
	if(view->refresh.pos)
	{
	    ep->x1 = ep->Node1->x;
	    ep->y1 = ep->Node1->y;
	    ep->z1 = ep->Node1->z;
	    ep->x2 = ep->Node2->x;
	    ep->y2 = ep->Node2->y;
	    ep->z2 = ep->Node2->z;
	    len = (float) DIST(ep->x2 - ep->x1, ep->y2 - ep->y1);
	    totallen = totallen + len;
	    if (len > maxedgelen)
		    maxedgelen = len;
	    if (len < minedgelen)
    	        minedgelen = len;
	    ep->length = len;
	}

	if(view->refresh.color)
	{
		if (ecolor && (color_string = agxget(ep->Edge, ecolor))
    		    && (*color_string != '\0')&& (strlen(color_string)>0))
			setglCompColor(&color, color_string);
		else {			/*use color theme */
		    getcolorfromschema(view->colschms, ep->length, t->maxedgelen,&color);
		    color.tag = 0;}
		ep->Color = color;

	}
	ep->data.edgeid = boolAttr(ep->Edge, edgeid, 0);
    	if(view->refresh.selection)
	    ep->data.Selected = boolAttr(ep->Edge, sel, 0);
	if(view->refresh.visibility)
	    ep->data.Visible = visible(ep->Edge, vis, sty);

    }
	if(view->refresh.pos)
	{
	    t->maxedgelen = maxedgelen;
	    t->minedgelen = minedgelen;
	    t->avgedgelength = totallen / (float) t->Edgecount;
	}


}


static xdot* parseXdotwithattrs(void *e)
{
	
	xdot* xDot=NULL;
	xDot=parseXDotFOn (agget(e,"_draw_" ), OpFns,sizeof(sdot_op), xDot);
	xDot=parseXDotFOn (agget(e,"_ldraw_" ), OpFns,sizeof(sdot_op), xDot);
	xDot=parseXDotFOn (agget(e,"_hdraw_" ), OpFns,sizeof(sdot_op), xDot);
	xDot=parseXDotFOn (agget(e,"_tdraw_" ), OpFns,sizeof(sdot_op), xDot);
	xDot=parseXDotFOn (agget(e,"_hldraw_" ), OpFns,sizeof(sdot_op), xDot);
	xDot=parseXDotFOn (agget(e,"_tldraw_" ), OpFns,sizeof(sdot_op), xDot);
	return xDot;

}

void settvxdot(Agraph_t * g, topview * t)
{
    /*
    look for xdot attributes and parse them if there is any 
    */
    topview_node *np;
    topview_edge *ep;
    int ind;
	t->xDot=parseXdotwithattrs(view->g[view->activeGraph]);
	for (ind = 0; ind < t->Nodecount; ind++) 
	{
		np = &t->Nodes[ind];
		np->xDot=parseXdotwithattrs(np->Node);
    }
    for (ind = 0; ind < t->Edgecount; ind++) 
	{
		ep = &t->Edges[ind];
		ep->xDot=parseXdotwithattrs(ep->Edge);
    }
}
void init_node_size(Agraph_t * g, topview * t)
{
    float vsize;
    int percent;
    percent = atoi(agget(g, "nodesize"));
    if (percent == 0)
	percent = 0.000001;
    vsize =
	0.05 * sqrt((view->bdxRight - view->bdxLeft) *
		    (view->bdyTop - view->bdyBottom));
    t->init_node_size =	vsize * 2  * percent / 100.0 /
	sqrt(t->Nodecount);
/*    if (t->init_node_size < 1)
	t->init_node_size=1;*/
//    t->init_zoom = view->zoom;
    t->init_zoom = -20;

}

void _init_node_size(Agraph_t * g, topview * t)
{
    float vsize;
    int percent;
    percent = atoi(agget(g, "nodesize"));
    if (percent == 0)
	percent = 0.000001;
    vsize =
	0.05 * sqrt((view->bdxRight - view->bdxLeft) *
		    (view->bdyTop - view->bdyBottom));
    t->init_node_size =
	vsize * 2 / GetOGLDistance(2) * percent / 100.0 /
	sqrt(t->Nodecount);
    if (t->init_node_size < 1)
	t->init_node_size=1;
    t->init_zoom = view->zoom;
}


static void reset_refresh(ViewInfo* v)
{
    v->refresh.color=0;
    v->refresh.nodesize=0;
    v->refresh.pos=0;
    v->refresh.selection=0;
    v->refresh.visibility=0;

}

static int drawtopviewlabels(Agraph_t * g)
{
    //drawing labels
    int ind = 0;
    topview_node *v;
    float f;

    if (((view->visiblenodecount > view->labelnumberofnodes)
	 && (view->active_camera == -1))
	|| (!view->labelshownodes) || (!view->drawnodes))
	return 0;
    if (view->Topview->maxnodedegree > 15)
	f = 15;
    else
	f = view->Topview->maxnodedegree;
    for (ind = 0; ind < view->Topview->Nodecount; ind++) {

	v = &view->Topview->Nodes[ind];

	if (view->active_camera == -1) {
	    if (((float) view->visiblenodecount >
		 view->labelnumberofnodes * v->degree / f)
		&& view->labelwithdegree)
		continue;
	}
	if (!node_visible(v))
	    continue;
	draw_topview_label(v, 1);
    }
    return 1;
}

static int drawtopviewedgelabels(Agraph_t * g)
{
    //drawing labels
    int ind = 0;
    topview_edge *e;
    float f;

    if ((view->visiblenodecount > view->labelnumberofnodes)
	|| (!view->labelshowedges))
	return 0;
    if (view->Topview->maxnodedegree > 15)
	f = 15;
    else
	f = view->Topview->maxnodedegree;
    for (ind = 0; ind < view->Topview->Edgecount; ind++) {

	e = &view->Topview->Edges[ind];

	if ((((float) view->visiblenodecount >
	      view->labelnumberofnodes * e->Node1->degree / f)
	     && view->labelwithdegree)
	    &&
	    (((float) view->visiblenodecount >
	      view->labelnumberofnodes * e->Node2->degree / f)
	     && view->labelwithdegree)
	    )
	    continue;
	if ((!node_visible(e->Node1)) && (!node_visible(e->Node2)))
	    continue;
	draw_topview_edge_label(e, 0.001);
    }
    return 1;
}


#if 0
static void create_DL()
{
    Agraph_t* g=view->g[view->activeGraph];
    glNewList(1,GL_COMPILE); 
      drawtopviewnodes(g);
        drawtopviewlabels(g);
	drawtopviewedges(g);
	drawtopviewedgelabels(g);
/*	renderNodes(g);
	renderEdges(g);*/
    glEndList();

}
#endif

void update_topview(Agraph_t * g, topview * t, int init)
{

    if (init)
    {
	preparetopview(g, t);
	t->xdotId=-1;
    }
    settvcolorinfo(g, t);
    set_boundaries(t);
    settvxdot(view->g[view->activeGraph], view->Topview);
    reset_refresh(view);
    if (init)/*one time call to calculate right colors*/
    {
	view->refresh.color=1;
	view->refresh.pos=1;
	settvcolorinfo(g, t);
        reset_refresh(view);
        set_boundaries(t);
    }
       init_node_size(g, t);


    /*This is a temp code , need to be removed after Xue's demo */
#if UNUSED
    info_file = agget(g, "demo_file");
    if ((info_file != NULL) && (strlen(info_file) != 0)) {
	agxbuf xbuf;
	agxbinit(&xbuf, 512, xbuffer);

	f = fopen(info_file, "r");
	if (info_file) {
	    while (fgets(buf, BUFSIZ, f))
		agxbput(&xbuf, buf);
	    agxbput(&xbuf, "");
	    str = agxbuse(&xbuf);
	    append_textview((GtkTextView *)
			    glade_xml_get_widget(xml, "mainconsole"), str,
			    strlen(str));
	}
    }


    /*end of temp code */
#endif

    if (view->SignalBlock)
	btnToolZoomFit_clicked(NULL, NULL);
//    create_DL();

}


void preparetopview(Agraph_t * g, topview * t)
{
    char *d_attr1;
    char *d_attr2;
    Agnode_t *v;
    Agedge_t *e;
    Agsym_t *sym;
    int ind, ind2, data_type_count;
    float maxedgelen, minedgelen, edgelength;


    maxedgelen = 0;
    minedgelen = MAXFLOAT;
    edgelength = 0;

    ind = 0;
    ind2 = 0;
    data_type_count = 0;
    d_attr1 = NULL;
    d_attr1 = agget(g, "nodelabelattribute");
    if (d_attr1) {
	if (!strcmp(d_attr1, "\\N"))
	    sym = 0;
	else if (!(sym = agattr(g, AGNODE, d_attr1, 0)))
	    d_attr1 = 0;
    }
    d_attr2 = agget(g, "DataAttribute2");


    /*initialize node and edge array */
    t->Edges = N_GNEW(agnedges(g), topview_edge);
    t->Nodes = N_GNEW(agnnodes(g), topview_node);
    t->maxnodedegree = 1;

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	//bind temp record;
	agbindrec(v, "temp_node_record", sizeof(temp_node_record), TRUE);	//graph custom data
	/*initialize group index, -1 means no group */
	t->Nodes[ind].Node = v;
	t->Nodes[ind].data.TVRef = ind;
	((temp_node_record *) AGDATA(v))->TVref = ind;
	init_element_data(&t->Nodes[ind].data);
	t->Nodes[ind].zoom_factor = 1;
	t->Nodes[ind].degree = agdegree(g, v, 1, 1);
	t->Nodes[ind].size=0;
	view->Topview->Nodes[ind].Label = NULL;

	t->Nodes[ind].node_alpha = 1;
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) {
	    init_element_data(&t->Edges[ind2].data);	/*init edge data */
	    t->Edges[ind2].Edge = e;
	    ind2++;
	}
	ind++;
    }


    /*attach edge node references ,  loop one more time,set colors */
    ind = 0;
    ind2 = 0;
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) {
	    t->Edges[ind2].Node1 =
		&t->Nodes[((temp_node_record *) AGDATA(agtail(e)))->TVref];
	    t->Edges[ind2].Node2 =
		&t->Nodes[((temp_node_record *) AGDATA(aghead(e)))->TVref];
	    ind2++;
	}
	ind++;
    }
    /*set some stats for topview */
    t->Nodecount = ind;
    t->Edgecount = ind2;

    /*create glcomp menu system */
    view->widgets = glcreate_gl_topview_menu();
    /*for grouped data , group data viewing buttons extension */
//      load_host_buttons(t, g, view->widgets);
    /*set topologilca fisheye to NULL */
    t->fisheyeParams.h = '\0';
    if (view->dfltViewType == VT_TOPFISH)
	t->fisheyeParams.active = 1;
    else
	t->fisheyeParams.active = 0;

    /*reset picked nodes */
    t->picked_node_count = 0;
    t->picked_nodes = '\0';
    t->picked_edge_count = 0;
    t->picked_edges = '\0';

	/*create attribute list*/
	t->attributes=load_attr_list(view->g[view->activeGraph]);
	t->filtered_attr_list=NULL;
}


static float set_gl_dot_size(topview * t)
{
    float sizevc;
    if (view->active_camera == -1)
	sizevc = t->init_node_size / view->zoom * t->init_zoom;
    else
	sizevc =
	    t->init_node_size / view->cameras[view->active_camera]->r *
	    t->init_zoom;

    if (sizevc < 1)
	sizevc = 1;
    return sizevc;

}

/*
	draws multi edges , single edges
	this function assumes     glBegin(GL_LINES) has been called 
*/
static void draw_edge(double x1, double y1, double z1, double x2,
		      double y2, double z2, int deg, topview_edge * e)
{
    double alpha, R, ITERANGLE;
    double X1, Y1, X2, Y2;

    if (deg) {
	R = e->length / 20.0;
	if ((deg / 2) * 2 != deg)	/*odd */
	    ITERANGLE = (deg) * 15.00 * -1;
	else
	    ITERANGLE = (deg) * 15.00;
	ITERANGLE = DEG2RAD * ITERANGLE;

	alpha = atan((y2 - y1) / (x2 - x1));
	if (x1 > x2)
	    ITERANGLE = 180 * DEG2RAD - ITERANGLE;
	X1 = R * cos(alpha - ITERANGLE) + x1;
	Y1 = R * sin(alpha - ITERANGLE) + y1;
	X2 = R * cos(alpha - (180 * DEG2RAD - ITERANGLE)) + x2;
	Y2 = R * sin(alpha - (180 * DEG2RAD - ITERANGLE)) + y2;
	glVertex3f(x1, y1, z1);
	glVertex3f(X1, Y1, z1);
	glVertex3f(X1, Y1, z1);
	glVertex3f(X2, Y2, z2);
	glVertex3f(X2, Y2, z2);
	glVertex3f(x2, y2, z2);
    } else {
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);

    }

}


static int begintopviewnodes(Agraph_t * g, float dotsz)
{
    switch (view->defaultnodeshape) {
    case 0:
	glPointSize((GLfloat) dotsz);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	break;
    case 1:
	/* set_gl_dot_size(view->Topview); FIX - command with no effect          */
	break;
    default:
	/* set_gl_dot_size(view->Topview); FIX - command with no effect          */
	glBegin(GL_POINTS);

    };
    //reset single selection mechanism
    view->Selection.single_selected_node = (topview_node *) 0;
    view->Selection.single_selected_edge = (topview_edge *) 0;
    view->Selection.node_distance = -1;
    return 1;


}
static void enddrawcycle(Agraph_t * g)
{
    if (view->Selection.single_selected_edge) {
	if (!(view->mouse.t == rightmousebutton))	//right click pick mode
	{			//left click single select mode
	    if (view->Selection.single_selected_edge->data.Selected == 0) {
		view->Selection.single_selected_edge->data.Selected = 1;
		select_edge(view->Selection.single_selected_edge);
	    } else {
		view->Selection.single_selected_edge->data.Selected = 1;
		deselect_edge(view->Selection.single_selected_edge);
	    }
	}
	/* return 1; */
    }
    if (view->Selection.single_selected_node) {
	if (view->mouse.t == rightmousebutton)
	    //right click pick mode
	    ;
	/*              pick_node(view->Selection.single_selected_node); */
	else {			//left click single select mode
	    if (view->Selection.single_selected_node->data.Selected == 0) {
		view->Selection.single_selected_node->data.Selected = 1;
		select_node(view->Selection.single_selected_node);
	    } else {
		view->Selection.single_selected_node->data.Selected = 1;
		deselect_node(view->Selection.single_selected_node);
	    }
	}
    }

}


static int endtopviewnodes(Agraph_t * g)
{
    switch (view->defaultnodeshape) {
    case 0:
	glEnd();
	glDisable(GL_POINT_SMOOTH);
	break;
    case 1:
	break;
    default:
	glEnd();
	break;

    };

    return 1;
}




static int drawtopviewnodes(Agraph_t * g)
{
    topview_node *v;
    float ddx, ddy, ddz;
    int ind = 0;
    float dotsize = set_gl_dot_size(view->Topview);	//sets the size of the gl points
    set_topview_options();
    begintopviewnodes(g, dotsize);
    view->visiblenodecount = 0;
    for (ind = 0; ind < view->Topview->Nodecount; ind++) {
	v = view->Topview->Nodes + ind;
/*	if (((-v->distorted_x / view->zoom >= view->clipX1)
	     && (-v->distorted_x / view->zoom <= view->clipX2)
	     && (-v->distorted_y / view->zoom >= view->clipY1)
	     && (-v->distorted_y / view->zoom <= view->clipY2))
	    || (view->active_camera >= 0))*/
	    if(1)
	    {
	    float zdepth;
	    view->visiblenodecount = view->visiblenodecount + 1;
	    if (!view->drawnodes || !node_visible(v))
		continue;

	    /*check for each node if it needs to be selected or picked */
	    //UPDATE view->Topview data from cgraph
	    /* if (v->update_required) */
	    /* update_topview_node_from_cgraph(v); */
	    if (v->data.Selected == 1) {
		glColor4f(view->selectedNodeColor.R,
			  view->selectedNodeColor.G,
			  view->selectedNodeColor.B,
			  view->selectedNodeColor.A);
	    } else {		//get the color from node
		glColor4f(v->Color.R, v->Color.G, v->Color.B,
			  v->Color.A);
		ddx = 0;
		ddy = 0;
		ddz = 0;
	    }
	    if (v->distorted_x != v->x)
		zdepth = (float) Z_FORWARD_PLANE;
	    else
		zdepth = (float) Z_BACK_PLANE;

	    if ((view->defaultnodeshape == 0)) {
		glVertex3f(v->distorted_x,
			   v->distorted_y, v->distorted_z);
	    } else if (view->defaultnodeshape == 1) {
		if (v->size > 0)
		    drawCircle(v->distorted_x , v->distorted_y ,
			       v->size * view->Topview->init_node_size,
			       v->distorted_z );
		else
		    drawCircle(v->distorted_x , v->distorted_y,
			       view->Topview->init_node_size,
			       v->distorted_z);
	    }
	} else {
	    /* int a=1; */
	}
    }
    endtopviewnodes(g);
    return 1;

}
static int edgevisible(topview_edge* e)
{
    static float x1,y1,x2,y2;
    static glCompPoint a;
    static glCompPoint b;
    static glCompPoint c;
    static glCompPoint d;
    return 1;

    x1=e->x1/ view->zoom * -1;
    x2=e->y1/ view->zoom * -1;
    y1=e->y1/ view->zoom * -1;
    y2=e->y2/ view->zoom * -1;

    if (
	    ((x1> view->clipX1) && (x1 < view->clipX2) && (y1 > view->clipY1) && (y1 < view->clipY2))
		    || 
	    ((x2 > view->clipX1)&& (x2< view->clipX2)&& (y2 > view->clipY1)	&& (y2 < view->clipY2))
	    	    || 
	    (view->active_camera >= 0)
    
       )	
	    return 1;
    if(view->edgerendertype == 0)
    {
	a.x=x1;a.y=y1;a.z=0;
	b.x=x2;b.y=y2;b.z=0;
	c.x=view->clipX1;c.y=view->clipY1;c.z=0;
	c.x=view->clipX1;c.y=view->clipY2;c.z=0;
	if(lines_intersect (&a, &b, &c,&d))
	    return 1;
	c.x=view->clipX2;c.y=view->clipY1;c.z=0;
	c.x=view->clipX2;c.y=view->clipY2;c.z=0;
	if(lines_intersect (&a, &b, &c,&d))
	    return 1;
	c.x=view->clipX1;c.y=view->clipY1;c.z=0;
	c.x=view->clipX2;c.y=view->clipY1;c.z=0;
	if(lines_intersect (&a, &b, &c,&d))
	    return 1;
	c.x=view->clipX1;c.y=view->clipY2;c.z=0;
	c.x=view->clipX2;c.y=view->clipY2;c.z=0;
	if(lines_intersect (&a, &b, &c,&d))
	    return 1;
	return 0;

    }
    return 0;



}

static void drawtopviewedges(Agraph_t * g)
{
    topview_edge *e;
    float ddx, ddy, ddz;
    float dddx, dddy, dddz;
    int ind = 0;
    if (!view->drawedges)
	return;
    glBegin(GL_LINES);
    set_topview_options();
    for (ind = 0; ind < view->Topview->Edgecount; ind++) 
    {
	e = view->Topview->Edges + ind;
	if(edgevisible(e))
	{
	    if (!get_color_from_edge(e))
		continue;
	    //select_topview_edge(e);
	    if (e->Node1->data.Selected == 1) {	//tail is selected
		ddx = dx;
		ddy = dy;
		ddz = 0;
	    } else {
		ddx = 0;
		ddy = 0;
		ddz = 0;
	    }
	    if (e->Node2->data.Selected == 1) {	//head
		dddx = dx;
		dddy = dy;
		dddz = 0;
	    } else {
		dddx = 0;
		dddy = 0;
		dddz = 0;
	    }

	    /*glVertex3f(e->Node1->distorted_x - ddx,
	       e->Node1->distorted_y - ddy,
	       e->Node1->distorted_z - ddz);
	       e->Node2->distorted_x - dddx,
	       e->Node2->distorted_y - dddy,
	       e->Node2->distorted_z - ddz */
	    draw_edge(e->Node1->distorted_x ,
		      e->Node1->distorted_y ,
		      e->Node1->distorted_z ,
		      e->Node2->distorted_x ,
		      e->Node2->distorted_y ,
		      e->Node2->distorted_z , e->data.edgeid, e);



	}
    }
    glEnd();

}


void drawTopViewGraph(Agraph_t * g)
{
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

//    glDisable(GL_DEPTH_TEST);
//    glCallList (1);
    drawtopviewnodes(g);
    drawtopviewlabels(g);
    drawtopviewedges(g);
    drawtopviewedgelabels(g);
    enddrawcycle(g);
    draw_tv_xdot(view->Topview);
  //  draw_node_hint_boxes();
//    tesstest();
//    glEnable(GL_DEPTH_TEST);


    if ((view->Selection.Active > 0) && (!view->SignalBlock)) {
	view->Selection.Active = 0;
	drawTopViewGraph(g);
	view->SignalBlock = 1;
	glexpose();
	view->SignalBlock = 0;
    }

}



/*static int pick_node(topview_node * n)
{
	if (!is_node_picked(n)) 
	{
		if (add_to_pick_list(n)) 
		{
			return 1;
	    }
	    return 0;
	}
	else 
	{
		if (remove_from_pick_list(n)) 
		{
			return 1;
		}
	    return 0;
	}
    return 0;

}*/





#ifdef UNUSED
static int select_topview_edge(topview_edge * e)
{

    int r = 0;
    if (((view->Selection.Type == 0) && (view->Selection.Active))
	|| (view->mouse.button == rightmousebutton))	//single selection or right click (picking)
    {

	float dist =
	    distance_to_line(e->x1, e->y1, e->x2, e->y2, view->Selection.X,
			     view->Selection.Y);
	if ((view->Selection.node_distance == -1)
	    || (dist < view->Selection.node_distance)) {
	    view->Selection.node_distance = dist;
	    view->Selection.single_selected_edge = e;
	}

	return 0;
    }
    if (!view->Selection.Active)
	return 0;
    r = (lineintersects(e->x1, e->y1, e->x2, e->y2));
    if (r >= 0) {
	switch (view->Selection.Type) {
	case 0:
	    if (OD_Selected(e->Edge) == 0) {
		OD_Selected(e->Edge) = 1;
		select_object(view->g[view->activeGraph], e->Edge);
	    } else {
		OD_Selected(e->Edge) = 1;
		deselect_object(view->g[view->activeGraph], e->Edge);
	    }
	    break;

	}
    }
    return 1;

}
#endif



float calculate_font_size(topview_node * v)
{
    float n;
    n = (float) v->degree + (float) 1.00;
    return n;

}

static int draw_topview_label(topview_node * v, float zdepth)
{

    float ddx = 0;
    float ddy = 0;
    char *buf;
    if (((v->distorted_x / view->zoom * -1 > view->clipX1)
	 && (v->distorted_x / view->zoom * -1 < view->clipX2)
	 && (v->distorted_y / view->zoom * -1 > view->clipY1)
	 && (v->distorted_y / view->zoom * -1 < view->clipY2))
	|| (view->active_camera >= 0)) {
	if (v->data.Selected == 1) {
	    ddx = dx;
	    ddy = dy;
	}
	glColor4f(view->nodelabelcolor.R, view->nodelabelcolor.G,
		  view->nodelabelcolor.B, view->nodelabelcolor.A);
	buf = agget(agraphof(v->Node), "nodelabelattribute");
	if (buf)
	    glprintfglut(view->glutfont, (v->distorted_x - ddx),
			 (v->distorted_y - ddy), v->distorted_z,
			 agget(v->Node, buf));
	return 1;
    } else
	return 0;
}
static int draw_topview_edge_label(topview_edge * e, float zdepth)
{

    float ddx = 0;
    float ddy = 0;
    char *buf;
    float x1, y1, z1, x2, y2, z2, x, y, z;
    x1 = e->Node1->distorted_x;
    y1 = e->Node1->distorted_y;
    x2 = e->Node2->distorted_x;
    y2 = e->Node2->distorted_y;
    z1 = e->Node1->distorted_z;
    z2 = e->Node2->distorted_z;


    if ((x1 / view->zoom * -1 > view->clipX1)
	&& (x1 / view->zoom * -1 < view->clipX2)
	&& (y1 / view->zoom * -1 > view->clipY1)
	&& (y1 / view->zoom * -1 < view->clipY2)) {

	x = (x2 - x1) / 2.00 + x1;
	y = (y2 - y1) / 2.00 + y1;
	z = (z2 - z1) / 2.00 + z1;
	if (e->data.Selected == 1) {
	    ddx = dx;
	    ddy = dy;
	}
	glColor4f(view->edgelabelcolor.R, view->edgelabelcolor.G,
		  view->edgelabelcolor.B, view->edgelabelcolor.A);
	buf = agget(agraphof(e->Edge), "edgelabelattribute");
	if (buf)
	    glprintfglut(view->glutfont, x - ddx, y - ddy, z,
			 agget(e->Edge, buf));
	return 1;
    } else
	return 0;
}




static void set_topview_options(void)
{

    int a=get_mode(view);
    if ((a == 10) && (view->mouse.down == 1))	//selected, if there is move move it, experimental
    {
	dx = view->mouse.GLinitPos.x - view->mouse.GLfinalPos.x;
	dy = view->mouse.GLinitPos.y - view->mouse.GLfinalPos.y;
    } else {
	dx = 0;
	dy = 0;
    }

}

/*refreshes limits of the graph call it when node locations are changed*/
static void set_boundaries(topview * t)
{

    int ind = 0;
    float left, right, top, bottom;
    left = t->Nodes[0].distorted_x;
    right = t->Nodes[0].distorted_x;
    top = t->Nodes[0].distorted_y;
    bottom = t->Nodes[0].distorted_y;
    for (ind = 0; ind < t->Nodecount; ind++) {
	if (left > t->Nodes[ind].distorted_x)
	    left = t->Nodes[ind].distorted_x;
	if (right < t->Nodes[ind].distorted_x)
	    right = t->Nodes[ind].distorted_x;
	if (bottom > t->Nodes[ind].distorted_y)
	    bottom = t->Nodes[ind].distorted_y;
	if (top < t->Nodes[ind].distorted_y)
	    top = t->Nodes[ind].distorted_y;
    }
    view->bdxLeft = left;
    view->bdyTop = top;
    view->bdxRight = right;
    view->bdyBottom = bottom;

    view->bdzTop = 0;
    view->bdzBottom = 0;

}

static int get_color_from_edge(topview_edge * e)
{
    int return_value = 1;
    float Alpha = 0;
    GtkHScale *AlphaScale =
	(GtkHScale *) glade_xml_get_widget(xml, "settingsscale2");
    Alpha = (float) gtk_range_get_value((GtkRange *) AlphaScale);

    //check visibility;
/*
    if ((node_visible(e->Node1))
	&& (node_visible(e->Node2)))
*/
    /* FIX - if edge is not visible, why not return immediately? */
    if (!e->data.Visible)
	return_value = 0;


        /*if both head and tail nodes are selected use selection color for edges */
    if(e->data.Selected)
//    if ((e->Node1->data.Selected) || (e->Node2->data.Selected)) 
    {
	glColor4f(view->selectedEdgeColor.R, view->selectedEdgeColor.G,
		  view->selectedEdgeColor.B, view->selectedEdgeColor.A);
	return return_value;
    }
    /*get edge's color attribute */
    if (e->Color.tag == 0)
	glColor4f(e->Color.R, e->Color.G, e->Color.B, Alpha * e->Color.A);
    else
	glColor4f(e->Color.R, e->Color.G, e->Color.B, e->Color.A);
    return return_value;
}

static int node_visible(topview_node * n)
{
    return n->data.Visible;

}

int move_TVnodes(void)
{
    float delX,delY;
    topview_node *v;
    static int flag=0;
    int ind = 0;
    if (!flag)
    {
//	printf ("dragx:%f dragy:%f\n",view->mouse.dragX,view->mouse.dragY);
	flag=1;
	return 0;
    }

    delX = GetOGLDistance((int) view->mouse.dragX);
    delY = GetOGLDistance((int) view->mouse.dragY);
    for (ind = 0; ind < view->Topview->Nodecount; ind++) {
	v = &view->Topview->Nodes[ind];
	if (v->data.Selected) {
	    v->distorted_x = v->distorted_x + delX;
	    v->distorted_y = v->distorted_y - delY;
	    v->x = v->x + delX;
	    v->y = v->y - delY;
	}
    }
    return 1;
}



void local_zoom(topview * t)
{
    int i;
    double delx, dely, w, h, tempx, tempy;
    w = view->mg.width;
    h = view->mg.height;
    for (i = 1; i < t->Nodecount; i++) {
	if (is_point_in_rectangle
	    (t->Nodes[i].x, t->Nodes[i].y, view->mg.x - view->mg.GLwidth,
	     view->mg.y - view->mg.GLheight, view->mg.GLwidth * 2,
	     view->mg.GLheight * 2)) {
	    delx = t->Nodes[i].x - view->mg.x;
	    dely = t->Nodes[i].y - view->mg.y;
	    tempx = view->mg.x + delx * view->mg.kts;
	    tempy = view->mg.y + dely * view->mg.kts;
	    if (is_point_in_rectangle
		((GLfloat) tempx, (GLfloat) tempy,
		 view->mg.x - view->mg.GLwidth,
		 view->mg.y - view->mg.GLheight,
		 view->mg.GLwidth * (GLfloat) 2,
		 view->mg.GLheight * (GLfloat) 2)) {
		t->Nodes[i].distorted_x =
		    view->mg.x + (GLfloat) delx *view->mg.kts;
		t->Nodes[i].distorted_y =
		    view->mg.y + (GLfloat) dely *view->mg.kts;
		t->Nodes[i].zoom_factor = view->mg.kts;
	    } else		//get intersections and move nodes to edges of magnifier box
	    {
		t->Nodes[i].distorted_x = t->Nodes[i].x;
		t->Nodes[i].distorted_y = t->Nodes[i].y;
		t->Nodes[i].zoom_factor = 1;
	    }

	} else {
	    t->Nodes[i].distorted_x = t->Nodes[i].x;
	    t->Nodes[i].distorted_y = t->Nodes[i].y;
	    t->Nodes[i].zoom_factor = 1;
	}
    }
}



void originate_distorded_coordinates(topview * t)
{
    //sets original coordinates values to distorded coords. this happens when lieft mouse click is released in geometrical fisyehey mode
    int i;
    for (i = 1; i < t->Nodecount; i++) {
	t->Nodes[i].distorted_x = t->Nodes[i].x;
	t->Nodes[i].distorted_y = t->Nodes[i].y;
	t->Nodes[i].zoom_factor = 1;
    }
}


#define strcaseeq(a,b)     (*(a)==*(b)&&!strcasecmp(a,b))

gvk_layout s2layout(char *s)
{
    if (!s)
	return GVK_NONE;

    if (strcaseeq(s, "dot"))
	return GVK_DOT;
    else if (strcaseeq(s, "neato"))
	return GVK_NEATO;
    else if (strcaseeq(s, "twopi"))
	return GVK_TWOPI;
    else if (strcaseeq(s, "circo"))
	return GVK_CIRCO;
    else if (strcaseeq(s, "fdp"))
	return GVK_FDP;
    else if (strcaseeq(s, "sfdp"))
	return GVK_SFDP;
    else
	return GVK_NONE;

}

char *layout2s(gvk_layout gvkl)
{
    char *s;
    switch (gvkl) {
    case GVK_NONE:
	s = "";
	break;
    case GVK_DOT:
	s = "dot";
	break;
    case GVK_NEATO:
	s = "neato";
	break;
    case GVK_TWOPI:
	s = "twopi";
	break;
    case GVK_CIRCO:
	s = "circo";
	break;
    case GVK_FDP:
	s = "fdp";
	break;
    case GVK_SFDP:
	s = "sfdp";
	break;
    default:
	s = "";
	break;
    }
    return s;
}

char *element2s(gve_element el)
{
    char *s;
    switch (el) {
    case GVE_NONE:
	s = "";
	break;
    case GVE_GRAPH:
	s = "graph";
	break;
    case GVE_CLUSTER:
	s = "cluster";
	break;
    case GVE_NODE:
	s = "node";
	break;
    case GVE_EDGE:
	s = "edge";
	break;
    default:
	s = "";
	break;
    }
    return s;
}

static int node_regex(topview_node * n, char *exp)
{

    regex_t preg;
    char *data = n->Label;
    int return_value = 0;
    if (data) {
	regcomp(&preg, exp, REG_NOSUB);
	if (regexec(&preg, data, 0, 0, 0) == 0)
	    return_value = 1;
	else
	    return_value = 0;
	regfree(&preg);
    }
    return return_value;
}

void select_with_regex(char *exp)
{
    topview_node *v;
    int ind = 0;
    for (ind = 0; ind < view->Topview->Nodecount; ind++) {
	v = &view->Topview->Nodes[ind];
	if (node_visible(v)) {
	    if (node_regex(v, exp)) {
		v->data.Selected = 1;
		select_node(v);
	    }
	}
    }
}


static void draw_xdot(xdot* x,float base_z)
{
	int i;
	sdot_op *op;
	if (!x)
		return;

	view->Topview->global_z=base_z;

	op=(sdot_op*)x->ops;
	for (i=0; i < x->cnt; i++,op++)
	{
		if(op->op.drawfunc)
			op->op.drawfunc(&op->op,0);
	}


}


static void draw_tv_xdot(topview* t)
{

/*    glNewList(1,GL_COMPILE); 
      drawtopviewnodes(g);
        drawtopviewlabels(g);
	drawtopviewedges(g);
	drawtopviewedgelabels(g);
	renderNodes(g);
	renderEdges(g);*/

    int j;
    float basez=0;

    if(t->xdotId==-1)
    {
	t->xdotId=glGenLists(1);
	glNewList(1,GL_COMPILE); 
	glNewList(t->xdotId,GL_COMPILE);
    	    draw_xdot(t->xDot,basez);
	    basez= basez+0.01;
	    for (j=0; j < t->Nodecount; j++)
	    {
		    draw_xdot(t->Nodes[j].xDot,basez);
		    basez = basez+0.001;
	    }
	    for (j=0; j < t->Edgecount; j++)
	    {
		    draw_xdot(t->Edges[j].xDot,basez);

	    }
    	glEndList();
    }
    else
	glCallList(t->xdotId);

/*    if((t->xDot)&&(t->xDot->ops->kind ==xd_font))
    {
	draw_xdot(t->xDot,basez);
	basez= basez+0.01;
    }
    for (j=0; j < t->Nodecount; j++)
    {
	if(t->Nodes[j].xDot->ops->kind==xd_font)
	{
	    draw_xdot(t->Nodes[j].xDot,basez);
	    basez = basez+0.001;
	}
    }
    for (j=0; j < t->Edgecount; j++)
    {
	if(t->Edges[j].xDot->ops->kind==xd_font)
	    draw_xdot(t->Edges[j].xDot,basez);
    }*/

}

void setMultiedges(Agraph_t * g, char *attrname)
{
    Agsym_t *attr = agattr(g, AGEDGE, attrname, 0);
    Agnode_t *n;
    Agedge_t *e;
    PointMap *map = newPM();
    int tid, hid, u, v, idx;
    char buf[128];

    if (!attr)
	attr = agattr(g, AGEDGE, attrname, "0");

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	tid = AGID(n);
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    hid = AGID(AGHEAD(e));
	    if (tid < hid) {
		u = tid;
		v = hid;
	    } else {
		u = hid;
		v = tid;
	    }
	    idx = insertPM(map, u, v, 0);
	    sprintf(buf, "%d", idx);
	    agxset(e, attr, buf);
	    updatePM(map, u, v, idx + 1);
	}
    }
    freePM(map);
}

void tessBeginCB(GLenum which)
{
    glBegin(which);
}



void tessEndCB()
{
    glEnd();
}
void errorCallback(GLenum errorCode)
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf (stderr, "Tessellation Error: %s\n", estring);
   exit (0);
}

#if 0
void tesstest()
{

    static GLUtesselator *tobj;


    GLdouble rect[4][3] = {0.0, 0.0, 0.0,
                          200.0, 50.0, 0.0,
                          200.0, 200.0, 0.0,
                          0, 0, 0.0};
   GLdouble tri[3][3] = {0.0, 0.0, 0.0,
                         125.0, 175.0, 0.0,
                         175.0, 0.0, 0.0};
   GLdouble star[5][6] = {250.0, 50.0, 0.0, 1.0, 0.0, 1.0,
                          325.0, 200.0, 0.0, 1.0, 1.0, 0.0,
                          400.0, 50.0, 0.0, 0.0, 1.0, 1.0,
                          250.0, 150.0, 0.0, 1.0, 0.0, 0.0,
                          400.0, 150.0, 0.0, 0.0, 1.0, 0.0};

    if(!tobj)
	tobj=gluNewTess(); // create a tessellator
    if(!tobj) return;  // failed to create tessellation object, return 0

//   gluTessCallback(tobj, GLU_TESS_VERTEX,(GLvoid (*) ()) &glVertex3dv);

    gluTessCallback(tobj, GLU_TESS_BEGIN, (void (*)())tessBeginCB);
    gluTessCallback(tobj, GLU_TESS_END, (void (*)())tessEndCB);


   glShadeModel(GL_FLAT);
   gluTessBeginPolygon(tobj, NULL);
      gluTessBeginContour(tobj);
         gluTessVertex(tobj, rect[0], rect[0]);
         gluTessVertex(tobj, rect[1], rect[1]);
         gluTessVertex(tobj, rect[2], rect[2]);
         gluTessVertex(tobj, rect[3], rect[3]);
      gluTessEndContour(tobj);
      gluTessBeginContour(tobj);
         gluTessVertex(tobj, tri[0], tri[0]);
         gluTessVertex(tobj, tri[1], tri[1]);
         gluTessVertex(tobj, tri[2], tri[2]);
      gluTessEndContour(tobj);
   gluTessEndPolygon(tobj);

   glShadeModel(GL_SMOOTH);
   gluTessProperty(tobj, GLU_TESS_WINDING_RULE,
                   GLU_TESS_WINDING_POSITIVE);
   gluTessBeginPolygon(tobj, NULL);
      gluTessBeginContour(tobj);
         gluTessVertex(tobj, star[0], star[0]);
         gluTessVertex(tobj, star[1], star[1]);
         gluTessVertex(tobj, star[2], star[2]);
         gluTessVertex(tobj, star[3], star[3]);
         gluTessVertex(tobj, star[4], star[4]);
      gluTessEndContour(tobj);
   gluTessEndPolygon(tobj);
}
#endif
