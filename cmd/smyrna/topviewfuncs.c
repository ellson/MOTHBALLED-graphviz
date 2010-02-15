#include "topviewfuncs.h"
#include "cgraph.h"
#include "smyrna_utils.h"
#include "colorprocs.h"
#include "draw.h"
#include "glcompui.h"
#include "frmobjectui.h"
#include "xdot.h"
#include "glutils.h"
#include "selectionfuncs.h"

#define WITH_CGRAPH 1
static xdot *parseXdotwithattrs(void *e);
static Agsym_t* pos_attr=(Agsym_t*)0;




static void set_boundaries(Agraph_t * g, topview * t)
{
    Agnode_t *v;
    static glCompPoint pos;
    float left, right, top, bottom;
    int id=0;
    if(!pos_attr)
	pos_attr=agattr(g, AGNODE,"pos",0);
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	pos=getPointFromStr(agxget(v, pos_attr));
	if(id==0)
	{
	    left=pos.x;
	    right=pos.x;
	    top=pos.y;
	    bottom=pos.y;
	}
	if (left > pos.x)
	    left = pos.x;
	if (right < pos.x)
	    right = pos.x;
	if (bottom > pos.y)
	    bottom = pos.y;
	if (top < pos.y)
	    top = pos.y;
	id++;
    }
    view->bdxLeft = left;
    view->bdyTop = top;
    view->bdxRight = right;
    view->bdyBottom = bottom;

    view->bdzTop = 0;
    view->bdzBottom = 0;



}

static float init_node_size(Agraph_t * g, topview * t)
{
    float vsize;
    int percent;
    float sz;
    percent = atoi(agget(g, "nodesize"));
    if (percent == 0)
	percent = 0.000001;
    vsize =
	0.05 * sqrt((view->bdxRight - view->bdxLeft) *
		    (view->bdyTop - view->bdyBottom));
    sz =	vsize * 2  * percent / 100.0 /
	sqrt(t->Nodecount);
/*    if (t->init_node_size < 1)
	t->init_node_size=1;*/
//    t->init_zoom = view->zoom;
    t->init_zoom = -20;
    return  sz;

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



static glCompPoint getEdgeHead(Agedge_t * edge)
{   
    return getPointFromStr(agget(aghead(edge),"pos"));
}
static glCompPoint getEdgeTail(Agedge_t *  edge)
{
    return getPointFromStr(agget(agtail(edge),"pos"));
}
static GLfloat getEdgeLength(Agedge_t *  edge)
{
    GLfloat rv=0;	
    glCompPoint A,B;
    A=getEdgeTail(edge);
    B=getEdgeHead(edge);
    rv=(A.x-B.x)*(A.x-B.x) + (A.y-B.y)*(A.y-B.y) +(A.z-B.z)*(A.z-B.z);
    rv=sqrt(rv);
    return rv;
}
static void glCompColorxlate(glCompColor* c,char* str)
{
        static gvcolor_t cl;
	colorxlate(str, &cl, RGBA_DOUBLE);
	c->R=cl.u.RGBA[0];
	c->G=cl.u.RGBA[1];
	c->B=cl.u.RGBA[2];
	c->A=cl.u.RGBA[3];
}

int object_color(void* obj,glCompColor* c)
{
    static gvcolor_t cl;
    Agraph_t* g=view->g[view->activeGraph];
    int return_value = 1;
    int objType;
    float Alpha = 1;
    char* bf;

    objType=AGTYPE(obj);

    if(objType==AGEDGE)
	Alpha=getAttrFloat(g,obj,"defaultedgealpha",1);
    if(objType==AGNODE)
	Alpha=getAttrFloat(g,obj,"defaultnodealpha",1);
    if(!getAttrBool(g,obj,"visible",1))
	return 0;
    if(getAttrBool(g,obj,"selected",0))
    {
	setColor(c,view->selectedEdgeColor.R, view->selectedEdgeColor.G,view->selectedEdgeColor.B, view->selectedEdgeColor.A);
	return return_value;
    }
    /*get edge's color attribute */
    bf=getAttrStr(g,obj,"color",NULL);
    if((bf)&&(strlen(bf)>0))
    {
	colorxlate(bf, &cl, RGBA_DOUBLE);
	c->R=cl.u.RGBA[0];
	c->G=cl.u.RGBA[1];
	c->B=cl.u.RGBA[2];
	c->A=cl.u.RGBA[3];

    }
    else
    {
	if(objType==AGEDGE)
	    getcolorfromschema(view->colschms, getEdgeLength(obj), view->Topview->maxedgelen,c);
	else
	{
	    colorxlate(agget(g, "defaultnodecolor"),&cl, RGBA_DOUBLE);
		c->R=cl.u.RGBA[0];
	    c->G=cl.u.RGBA[1];
	    c->B=cl.u.RGBA[2];
	    c->A=cl.u.RGBA[3];
	}

    }
    return return_value;
}




/*
	draws multi edges , single edges
	this function assumes     glBegin(GL_LINES) has been called 
*/
static void draw_edge(glCompPoint* posT,glCompPoint* posH, GLfloat length,int deg)
{
    double alpha, R, ITERANGLE;
    double X1, Y1, X2, Y2;

    if (deg) {
	R = length / 20.0;
	if ((deg / 2) * 2 != deg)	/*odd */
	    ITERANGLE = (deg) * 15.00 * -1;
	else
	    ITERANGLE = (deg) * 15.00;
	ITERANGLE = DEG2RAD * ITERANGLE;

	alpha = atan((posH->y - posT->y) / (posH->x - posT->x));
	if (posT->x > posH->x)
	    ITERANGLE = 180 * DEG2RAD - ITERANGLE;
	X1 = R * cos(alpha - ITERANGLE) + posT->x;
	Y1 = R * sin(alpha - ITERANGLE) + posT->y;
	X2 = R * cos(alpha - (180 * DEG2RAD - ITERANGLE)) + posH->x;
	Y2 = R * sin(alpha - (180 * DEG2RAD - ITERANGLE)) + posH->y;
	glVertex3f(posT->x, posT->y, posT->z);
	glVertex3f(X1, Y1, posT->z);
	glVertex3f(X1, Y1, posT->z);
	glVertex3f(X2, Y2, posH->z);
	glVertex3f(X2, Y2, posH->z);
	glVertex3f(posH->x, posH->y, posH->z);
    } else {
	glVertex3f(posT->x, posT->y, posT->z);
	glVertex3f(posH->x, posH->y, posH->z);

    }

}
void renderSelectedNodes(Agraph_t * g)
{
    Agnode_t *v;
    static xdot * x;
        static glCompPoint pos;


    static int defaultNodeShape=0;
    static GLfloat nodeSize=0;
    if(!defaultNodeShape)
	defaultNodeShape=getAttrBool(g,g,"defaultnodeshape",0);
    if(defaultNodeShape==0)
	glBegin(GL_POINTS);

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	if(!((nodeRec*)(aggetrec(v,"nodeRec",0)))->selected);
	    continue;
	x=parseXdotwithattrs(v);
	draw_xdot(x,0);
	if(x)
	    freeXDot (x);
    }


    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	if(!((nodeRec*)(aggetrec(v,"nodeRec",0)))->selected)
	    continue;
	glColor4f(1,0,0,1);	    
	pos=((nodeRec*)(aggetrec(v,"nodeRec",0)))->A;
	nodeSize=((nodeRec*)(aggetrec(v,"nodeRec",0)))->size;

	if (defaultNodeShape == 0) 
	    glVertex3f(pos.x,pos.y,pos.z);
	else if (defaultNodeShape == 1) 

	    drawCircle(pos.x,pos.y,nodeSize,pos.z);
	if(((nodeRec*)(aggetrec(v,"nodeRec",0)))->printLabel==1)
            glprintfglut(view->glutfont,pos.x+nodeSize,pos.y+nodeSize,pos.z,agnameof(v));

    }
    if(defaultNodeShape==0)
	glEnd();
}



void renderNodes(Agraph_t * g)
{
    Agnode_t *v;
    static glCompPoint pos;
    static Agsym_t* size_attr=(Agsym_t*)0;
    static Agsym_t* selected_attr=(Agsym_t*)0;
    static int defaultNodeShape=0;
    static GLfloat nodeSize=0;
    static glCompColor c;
    static xdot * x;


    if(!defaultNodeShape)
	defaultNodeShape=getAttrBool(g,g,"defaultnodeshape",0);
    if(!pos_attr)
	pos_attr=agattr(g, AGNODE,"pos",0);
    if(!size_attr)
	size_attr=agattr(g, AGNODE,"size",0);
    if(!selected_attr)
	selected_attr=agattr(g, AGNODE,"selected",0);

    if(defaultNodeShape==0)
	glBegin(GL_POINTS);

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	    if(!object_color(v,&c))
		continue;
	    x=parseXdotwithattrs(v);
	    draw_xdot(x,0);


	    if(x)
		freeXDot (x);
    }




    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	if(!object_color(v,&c))
	{
	    ((nodeRec*)(aggetrec(v,"nodeRec",0)))->visible=0;
	    continue;
	}
	else
	    ((nodeRec*)(aggetrec(v,"nodeRec",0)))->visible=1;


	if(l_int(v, selected_attr,0))
	{
	    ((nodeRec*)(aggetrec(v,"nodeRec",0)))->selected=1;
	    continue;
	}
	glColor4f(c.R,c.G,c.B,c.A);	    
	pos=getPointFromStr(agxget(v, pos_attr));
	nodeSize=(GLfloat)l_float(v, size_attr,0);

	((nodeRec*)(aggetrec(v,"nodeRec",0)))->A=pos;

        if (nodeSize > 0)
	    nodeSize=nodeSize*view->Topview->init_node_size;
	else
	    nodeSize=view->Topview->init_node_size;
	if(defaultNodeShape==0)
	    nodeSize=1;
	((nodeRec*)(aggetrec(v,"nodeRec",0)))->size=nodeSize;
	if (defaultNodeShape == 0) 
	    glVertex3f(pos.x,pos.y,pos.z);
	else if (defaultNodeShape == 1) 
	    drawCircle(pos.x,pos.y,nodeSize,pos.z);
    }
    if(defaultNodeShape==0)
	glEnd();
}


void renderSelectedEdges(Agraph_t * g)
{

    Agedge_t *e;
    Agnode_t *v;
    static xdot * x;
    static glCompPoint posT;	/*Tail position*/
    static glCompPoint posH;	/*Head position*/
    static glCompColor c;
    /*xdots tend to be drawn as background shapes,that is why they are being rendered before edges*/

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
	{
	    if(!((edgeRec*)(aggetrec(e,"edgeRec",0)))->selected)
		continue;
	    if(!object_color(e,&c))
		continue;

	    x=parseXdotwithattrs(e);
	    draw_xdot(x,0);
	    if(x)
		freeXDot (x);
	}
    }




    glBegin(GL_LINES);
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
	{
	    if(!((edgeRec*)(aggetrec(e,"edgeRec",0)))->selected)
		continue;

	    if(!object_color(e,&c))
		continue;
	    glColor4f(1,0,0,1);	    
	    posT=((edgeRec*)(aggetrec(e,"edgeRec",0)))->posTail;
	    posH=((edgeRec*)(aggetrec(e,"edgeRec",0)))->posHead;
	    posT.z +=0.01;
	    posH.z +=0.01;
	    draw_edge(&posT,&posH,getEdgeLength(e),0);
	}
    }
    glEnd();
}




void renderEdges(Agraph_t * g)
{

    Agedge_t *e;
    Agnode_t *v;
    static xdot * x;
    static glCompPoint posT;	/*Tail position*/
    static glCompPoint posH;	/*Head position*/
    static glCompColor c;
    if(!pos_attr)
	pos_attr=agattr(g, AGNODE,"pos",0);
    /*xdots tend to be drawn as background shapes,that is why they are being rendered before edges*/

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
	{
	    if(!object_color(e,&c))
	    {
		((edgeRec*)(aggetrec(e,"edgeRec",0)))->visible=0;
		continue;
	    }
	    else
		((edgeRec*)(aggetrec(e,"edgeRec",0)))->visible=1;

	    if(((edgeRec*)(aggetrec(e,"edgeRec",0)))->selected)
		continue;

	    x=parseXdotwithattrs(e);
	    draw_xdot(x,0);


	    if(x)
		freeXDot (x);
	}
    }




    glBegin(GL_LINES);
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
	{
	    if(!object_color(e,&c))
		continue;
	    if(((edgeRec*)(aggetrec(e,"edgeRec",0)))->selected)
		continue;
	    glColor4f(c.R,c.G,c.B,c.A);	    
	    posT=getPointFromStr(agxget(agtail(e), pos_attr));
	    posH=getPointFromStr(agxget(aghead(e), pos_attr));
	    draw_edge(&posT,&posH,getEdgeLength(e),0);
    	    ((edgeRec*)(aggetrec(e,"edgeRec",0)))->posTail=posT;
	    ((edgeRec*)(aggetrec(e,"edgeRec",0)))->posHead=posH;


	}
    }
    glEnd();
}

void renderNodeLabels(Agraph_t * g)
{
    Agnode_t *v;
    static glCompPoint pos;
    static Agsym_t* data_attr=(Agsym_t*)0;
    static Agsym_t* l_color_attr=(Agsym_t*)0;
    static GLfloat nodeSize=0;
    static glCompColor c;
    if(!data_attr)
	data_attr=agattr(g, AGNODE,agget(g, "nodelabelattribute"),0);
    if(!l_color_attr)
	l_color_attr=agattr(g, AGRAPH,"nodelabelcolor",0);




    glCompColorxlate(&c,agxget(g,l_color_attr));


    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	 if(((nodeRec*)(aggetrec(v,"nodeRec",0)))->visible==0)
	    continue;
	 if(((nodeRec*)(aggetrec(v,"nodeRec",0)))->selected==1)
	    continue;

	pos=((nodeRec*)(aggetrec(v,"nodeRec",0)))->A;
	nodeSize=((nodeRec*)(aggetrec(v,"nodeRec",0)))->size;
	glColor4f(c.R,c.G,c.B,c.A);
        if (nodeSize > 0)
	    nodeSize=nodeSize*view->Topview->init_node_size;
	else
	    nodeSize=view->Topview->init_node_size;
	if(!data_attr)
            glprintfglut(view->glutfont,pos.x+nodeSize,pos.y+nodeSize,pos.z,agnameof(v));
	else
	    glprintfglut(view->glutfont,pos.x+nodeSize,pos.y+nodeSize,pos.z,agxget(v,data_attr));
    }
}
void renderEdgeLabels(Agraph_t * g)
{
    Agedge_t *e;
    Agnode_t *v;
    static glCompPoint posT;
    static glCompPoint posH;
    static Agsym_t* data_attr=(Agsym_t*)0;
    static Agsym_t* l_color_attr=(Agsym_t*)0;
    static GLfloat nodeSize=0;
    static glCompColor c;
    GLfloat x,y,z;

    if(!data_attr)
	data_attr=agattr(g, AGNODE,agget(g, "edgelabelattribute"),0);
    if(!l_color_attr)
	l_color_attr=agattr(g, AGRAPH,"edgelabelcolor",0);

    glCompColorxlate(&c,agxget(g,l_color_attr));

    if(!data_attr)
	return;

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
	{

	    if(((edgeRec*)(aggetrec(v,"nodeRec",0)))->visible==0)
		continue;

	    posT=((edgeRec*)(aggetrec(e,"edgeRec",0)))->posTail;
	    posH=((edgeRec*)(aggetrec(e,"edgeRec",0)))->posHead;
	    glColor4f(c.R,c.G,c.B,c.A);
	    x=posH.x+(posT.x-posH.x)/2;
	    y=posH.y+(posT.y-posH.y)/2;
	    z=posH.z+(posT.z-posH.z)/2;
	    glprintfglut(view->glutfont,x,y,z,agxget(e,data_attr));

	}
    }
}






static xdot *parseXdotwithattrs(void *e)
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

void cacheNodes(Agraph_t * g,topview* t)
{
    if(t->cache.node_id!=-1)	/*clean existing cache*/
	glDeleteLists(t->cache.node_id,1);
    t->cache.node_id=glGenLists(1);
    glNewList(t->cache.node_id,GL_COMPILE);
    renderNodes(g);
    glEndList();




}
void cacheEdges(Agraph_t * g,topview* t)
{
    if(t->cache.edge_id!=-1)	/*clean existing cache*/
	glDeleteLists(t->cache.edge_id,1);
    t->cache.edge_id=glGenLists(1);
    glNewList(t->cache.edge_id,GL_COMPILE);
    renderEdges(g);
    glEndList();


}
void cacheSelectedEdges(Agraph_t * g,topview* t)
{
    if(t->cache.seledge_id!=-1)	/*clean existing cache*/
	glDeleteLists(t->cache.seledge_id,1);
    t->cache.seledge_id=glGenLists(1);
    glNewList(t->cache.seledge_id,GL_COMPILE);
    renderSelectedEdges(g);
    glEndList();


}
void cacheSelectedNodes(Agraph_t * g,topview* t)
{
    if(t->cache.selnode_id!=-1)	/*clean existing cache*/
	glDeleteLists(t->cache.selnode_id,1);
    t->cache.selnode_id=glGenLists(1);
    glNewList(t->cache.selnode_id,GL_COMPILE);
    renderSelectedNodes(g);
    glEndList();
}
void cacheNodeLabels(Agraph_t * g,topview* t)
{
    if(t->cache.nodelabel_id!=-1)	/*clean existing cache*/
	glDeleteLists(t->cache.nodelabel_id,1);
    t->cache.nodelabel_id=glGenLists(1);
    glNewList(t->cache.nodelabel_id,GL_COMPILE);
    renderNodeLabels(g);
    glEndList();
}
void cacheEdgeLabels(Agraph_t * g,topview* t)
{
    if(t->cache.edgelabel_id!=-1)	/*clean existing cache*/
	glDeleteLists(t->cache.edgelabel_id,1);
    t->cache.edgelabel_id=glGenLists(1);
    glNewList(t->cache.edgelabel_id,GL_COMPILE);
    renderEdgeLabels(g);
    glEndList();
}

void updateSmGraph(Agraph_t * g,topview* t)
{
    Agnode_t *v;
    Agedge_t *e;
    float eLength=0;
    float totalELength=0;

    t->Nodecount=0;
    t->Edgecount=0;
    t->avgedgelength=0;
    t->maxedgelen=0;
    t->minedgelen=-1;

    t->picked_node_count = 0;
    t->picked_nodes = '\0';
    t->picked_edge_count = 0;
    t->picked_edges = '\0';
    t->global_z=0;



    if(!t)
	return ;
    /*Node Loop*/
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
	{
	    t->Edgecount++;
	    eLength=getEdgeLength(e);
	    if((t->minedgelen == -1) || (t->minedgelen > eLength))
		t->minedgelen=eLength;
	    if(eLength > t->maxedgelen)
		t->maxedgelen=eLength;
	    totalELength += eLength;
	}
	t->Nodecount++;

    }
    aginit(g, AGNODE, "nodeRec", sizeof(nodeRec), 0);
    aginit(g, AGEDGE, "edgeRec", sizeof(edgeRec), 0);

    set_boundaries(g,t);
    t->avgedgelength = totalELength / t->Edgecount;
    t->init_node_size=init_node_size(g, t);
    view->Topview=t;
    cacheNodes(g,t);
    cacheEdges(g,t);
    cacheSelectedNodes(g,t);
    cacheSelectedEdges(g,t);
    cacheNodeLabels(g,t);
    cacheEdgeLabels(g,t);
}
topview* initSmGraph(Agraph_t * g)
{
    topview* rv=(topview*)malloc(sizeof(topview));
    rv->maxnodedegree = 1;
        
    /*create glcomp menu system */
    view->widgets = glcreate_gl_topview_menu();

    /*create attribute list*/
    rv->attributes=load_attr_list(view->g[view->activeGraph]);
    rv->filtered_attr_list=NULL;

    /*set topologilca fisheye to NULL */
    rv->fisheyeParams.h = '\0';

    if (view->dfltViewType == VT_TOPFISH)
	rv->fisheyeParams.active = 1;
    else
	rv->fisheyeParams.active = 0;
    rv->cache.node_id=-1;
    rv->cache.selnode_id=-1;
    rv->cache.edge_id=-1;
    rv->cache.seledge_id=-1;
    updateSmGraph(g,rv);
    return rv;
}

void renderSmGraph(Agraph_t * g,topview* t)
{

    if(view->drawnodes)
    {
	glCallList(t->cache.node_id);
        glCallList(t->cache.selnode_id);
        if(view->drawnodelabels)
	{
	    if(view->zoom*-1 <	t->fitin_zoom /(float)view->labelnumberofnodes*-1) 
		glCallList(t->cache.nodelabel_id);
	}

    }

    if(view->drawedges)
    {
	glCallList(t->cache.edge_id);
        glCallList(t->cache.seledge_id);
        if(view->drawedgelabels)
	{
	    if(view->zoom*-1 <	t->fitin_zoom /(float)view->labelnumberofnodes*-1) 
		    glCallList(t->cache.edgelabel_id);

	}
    }

}

void freeSmGraph(Agraph_t * g,topview* t)
{
    return ;
}
void move_TVnodes()
{
    return;
}



