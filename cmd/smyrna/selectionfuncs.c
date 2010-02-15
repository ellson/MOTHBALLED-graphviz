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

#include "selectionfuncs.h"
#include "topviewfuncs.h"
#include "smyrna_utils.h"

static void select_node(Agraph_t* g,Agnode_t*  obj,int reverse)
{
    static Agsym_t* sel_attr=(Agsym_t*)0;
    if(!sel_attr)
	sel_attr=agattr(g, AGNODE,"selected","0");
    if(!reverse)
    {
        agxset(obj,sel_attr,"1");
        ((nodeRec*)(aggetrec(obj,"nodeRec",0)))->selected=1;
    }
    else
    {
	if(((nodeRec*)(aggetrec(obj,"nodeRec",0)))->selected==1)
	{
            agxset(obj,sel_attr,"0");
	    ((nodeRec*)(aggetrec(obj,"nodeRec",0)))->selected=0;
	    ((edgeRec*)(aggetrec(obj,"nodeRec",0)))->printLabel=0;


	}
	else
	{
            agxset(obj,sel_attr,"1");
	    ((nodeRec*)(aggetrec(obj,"nodeRec",0)))->selected=1;

	}

    }


}
static void select_edge(Agraph_t* g,Agedge_t*  obj,int reverse)
{
    static Agsym_t* sel_attr=(Agsym_t*)0;
    if(!sel_attr)
	sel_attr=agattr(g, AGEDGE,"selected","0");
    if (!reverse)
    {
	agxset(obj,sel_attr,"1");
	((edgeRec*)(aggetrec(obj,"edgeRec",0)))->selected=1;
    }
    else
    {
	if(((edgeRec*)(aggetrec(obj,"edgeRec",0)))->selected==1)
	{
	    agxset(obj,sel_attr,"0");
	    ((edgeRec*)(aggetrec(obj,"edgeRec",0)))->selected=0;
	    ((edgeRec*)(aggetrec(obj,"edgeRec",0)))->printLabel=0;
	}
	else
	{
	    agxset(obj,sel_attr,"1");
	    ((edgeRec*)(aggetrec(obj,"edgeRec",0)))->selected=1;
	}
    }


}


static void pick_objects_in_rect(Agraph_t* g,GLfloat x1,GLfloat y1,GLfloat x2,GLfloat y2)
{
    static Agnode_t *v;
    static Agedge_t *e;
    static glCompPoint posT;
    static glCompPoint posH;
    static glCompPoint posN;
     
     for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	posN=((nodeRec*)(aggetrec(v,"nodeRec",0)))->A;
	if(is_point_in_rectangle(posN.x,posN.y,x1,y1,x2-x1,y2-y1))
	    select_node(g,v,0);
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
	{
	    posT=((edgeRec*)(aggetrec(e,"edgeRec",0)))->posTail;
	    posH=((edgeRec*)(aggetrec(e,"edgeRec",0)))->posHead;
    	    if(is_point_in_rectangle(posT.x,posT.y,x1,y1,x2-x1,y2-y1))
		if(is_point_in_rectangle(posH.x,posH.y,x1,y1,x2-x1,y2-y1))
		    select_edge(g,e,0);
	}
    }
}



static void* pick_object(Agraph_t* g,glCompPoint p)
{
    static Agsym_t* size_attr=(Agsym_t*)0;
    static Agsym_t* pos_attr=(Agsym_t*)0;

    static Agnode_t *v;
     static Agedge_t *e;
     static glCompPoint posT;
     static glCompPoint posH;
     static glCompPoint posN;
     float dist=999999999;
     static GLfloat nd=0; /*node distance to point*/
     static GLfloat ed=0; /*edge distance to point*/
     static GLfloat nodeSize=0;
     void* rv=(void*)0;
     nd=0; /*node distance to point*/
     ed=0; /*edge distance to point*/

    if(!size_attr)
	size_attr=agattr(g, AGNODE,"size",0);

    if(!pos_attr)
	pos_attr=agattr(g, AGNODE,"pos",0);

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
    {
	posN=((nodeRec*)(aggetrec(v,"nodeRec",0)))->A;
	nodeSize=(GLfloat)l_float(v, size_attr,0);
	if (nodeSize > 0)
    	    nodeSize=nodeSize * view->Topview->init_node_size;
        else
	    nodeSize=view->Topview->init_node_size;
	nd=distBetweenPts(posN,p,nodeSize);
	if( nd < dist )
	{
	    rv=v;dist=nd;
	}

	for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
	{
	    posT=((edgeRec*)(aggetrec(e,"edgeRec",0)))->posTail;
	    posH=((edgeRec*)(aggetrec(e,"edgeRec",0)))->posHead;
	    ed=point_to_lineseg_dist(p, posT,posH);
	    if( ed < dist ) {rv=e;dist=ed;}
	}
    }
    return rv;
}

void pick_object_xyz(Agraph_t* g,topview* t,GLfloat x,GLfloat y,GLfloat z) 
{
    static glCompPoint p;
    void* a;
    p.x=x;p.y=y;p.z=z;
    a=pick_object(g,p);
    if (!a)
	return;
    if(agobjkind(a)==AGNODE)
    {
	
	select_node(g,a,1);	
	((nodeRec*)(aggetrec(a,"nodeRec",0)))->printLabel=1;

	cacheSelectedNodes(g,t);

    }
    if(agobjkind(a)==AGEDGE)
    {
	select_edge(g,a,1);	
	cacheSelectedEdges(g,t);
	((edgeRec*)(aggetrec(a,"edgeRec",0)))->printLabel=1;
    }


}
void pick_objects_rect(Agraph_t* g) 
{

    static GLfloat x1;
    static GLfloat y1;
    static GLfloat x2;
    static GLfloat y2;
    if(view->mouse.GLfinalPos.x > view->mouse.GLinitPos.x)
    {
        x1=view->mouse.GLinitPos.x;
	x2=view->mouse.GLfinalPos.x;
    }
    else
    {
        x2=view->mouse.GLinitPos.x;
	x1=view->mouse.GLfinalPos.x;

    }
    if(view->mouse.GLfinalPos.y > view->mouse.GLinitPos.y)
    {
        y1=view->mouse.GLinitPos.y;
	y2=view->mouse.GLfinalPos.y;
    }
    else
    {
        y2=view->mouse.GLinitPos.y;
	y1=view->mouse.GLfinalPos.y;
    }
    pick_objects_in_rect(g,x1,y1,x2,y2);
    cacheSelectedNodes(g,view->Topview);
    cacheSelectedEdges(g,view->Topview);
}


