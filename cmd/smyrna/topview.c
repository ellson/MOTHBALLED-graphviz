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
#include "glcomptext.h"
#include "glcomptextpng.h"
#include "glcompbutton.h"
#include "glcomppanel.h"
#include "glcomplabel.h"
#include "gltemplate.h"
#include "glutils.h"
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
#ifdef WIN32
#include "regex_win32.h"
#else
#include "regex.h"
#endif
#include "color.h"
#include "colorprocs.h"
static float dx = 0.0;
static float dy = 0.0;
static float dz = 0.0;

   /* Forward declarations */
glCompSet *glcreate_gl_topview_menu(void);
static void set_boundaries(topview * t);
static void set_topview_options(void);
static int draw_topview_label(topview_node * v, float zdepth);
static int draw_topview_edge_label(topview_edge * e, float zdepth);
static int node_visible(topview_node* n);
static int select_topview_node(topview_node * n);
/* static int select_topview_edge(topview_edge * e); */
static int update_topview_node_from_cgraph(topview_node * Node);
static int get_color_from_edge(topview_edge * e);
static int draw_node_hint_boxes(void);
static int pick_node(topview_node * n);

#ifdef UNUSED
static void remove_recs()
{
	Agraph_t* g;
    Agnode_t *v;
	if(view->activeGraph < 0)
		return;
	g=view->g[view->activeGraph];
	for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
	{
		agdelrec(v,"temp_node_record");
	}
}
#endif

void cleartopview(topview * t)
{
	/*free attached records*/
	/*clear nodes */
	free (t->Nodes);
	free (t->Edges);
//    free(t);
}
static void init_element_data(element_data* d)
{
							   
	d->Highlighted=0;
	d->Layer=0;
	d->Visible=1;
	d->Highlighted=0;
	d->Selected=0;
	d->Preselected=0;
	d->NumDataCount=0;
	d->NumData=(float*)0;
	d->StrDataCount=0;
	d->StrData=(char**)0;
	d->selectionflag=0;
	d->param=0;
	d->TVRef=-1;
}	
static int setpositioninfo(float* x,float* y,float* z,char* buf)
{
	int ind=0;
	int commacount=0;
	/*zero all values*/
	*x=0;
	*y=0;
	*z=0;

	/*find out if 2D or 3 d, count commas*/
	if (!buf)
		return 0;
	for (;ind < (int)strlen(buf); ind=ind +1)
	{
		if( buf[ind]==',')
			commacount ++;
	}
	if (commacount == 1) /*2D no z value*/
	{
		sscanf(buf,"%f,%f",x,y);
		z=0;
	}
	else 
		sscanf(buf,"%f,%f,%f",x,y,z);
	return 1;
}
static void setRGBcolor(RGBColor* c,char* colorstr)
{
    gvcolor_t cl;
	/*if node has color attribute*/
	if ((colorstr != '\0') && (strlen(colorstr) > 0))
	{
		colorxlate(colorstr, &cl, RGBA_DOUBLE);
		c->tag=1;	/*tag is used to identofy colros set by the graph , not smyrna*/
	}
	else
	{
		colorxlate(agget(view->g[view->activeGraph],"defaultnodecolor"), &cl, RGBA_DOUBLE);
		c->tag=0;
	}
		c->R = (float) cl.u.RGBA[0];
		c->G = (float) cl.u.RGBA[1];
		c->B = (float) cl.u.RGBA[2];
		c->A = (float) cl.u.RGBA[3];

}




/*update position info from cgraph*/
void settvposinfo(Agraph_t* g,topview* t)
{
    int ind;
    float maxedgelen,len,minedgelen;
	maxedgelen=0;
	minedgelen=MAXFLOAT;
	/*loop nodes*/
	for (ind=0;ind < t->Nodecount ; ind ++)
	{
		if (!setpositioninfo(&t->Nodes[ind].x,&t->Nodes[ind].y,&t->Nodes[ind].z,agget(t->Nodes[ind].Node, "pos")))
			printf ("missing position information of node %s , setting to zero\n",agnameof(t->Nodes[ind].Node));
		/*distorted coordiantes, same with original ones at the beginning*/
		t->Nodes[ind].distorted_x = t->Nodes[ind].x;
		t->Nodes[ind].distorted_y = t->Nodes[ind].y;
		t->Nodes[ind].distorted_z = t->Nodes[ind].z;
	}
	/*loop edges*/
	for (ind=0;ind < t->Edgecount ; ind ++)
	{
		setpositioninfo(&t->Edges[ind].x1,&t->Edges[ind].y1,&t->Edges[ind].z1,agget(t->Edges[ind].Node1->Node, "pos"));
		setpositioninfo(&t->Edges[ind].x2,&t->Edges[ind].y2,&t->Edges[ind].z2,agget(t->Edges[ind].Node2->Node, "pos"));
		len=(float)pow(pow((t->Edges[ind].x2-t->Edges[ind].x1),2)+pow((t->Edges[ind].y2-t->Edges[ind].y1),2),0.5);
		if (len > maxedgelen)
			maxedgelen=len;
		if(len < minedgelen)
			minedgelen=len;
		t->Edges[ind].length=len;
	}
	t->maxedgelen=maxedgelen;
	t->minedgelen=minedgelen;

}
void settvcolorinfo(Agraph_t* g,topview* t)
{
    int ind;
	RGBColor color;
	/*loop nodes*/
	/* float maxvalue=0; */
	char* str;
	for (ind=0;ind < t->Nodecount ; ind ++)
	{
		setRGBcolor(&color,agget(t->Nodes[ind].Node, "color"));
		t->Nodes[ind].Color.R = color.R;t->Nodes[ind].Color.G = color.G;t->Nodes[ind].Color.B = color.B;t->Nodes[ind].Color.A = color.A;t->Edges[ind].Color.tag=color.tag;
	}
	/*loop edges*/
	for (ind=0;ind < t->Edgecount ; ind ++)
	{
			char* color_string = agget(t->Edges[ind].Edge, "color");
			if ((color_string)&& (*color_string != '\0')) 
				setRGBcolor(&color,color_string);
			else{	/*use color theme*/
				getcolorfromschema(view->colschms,t->Edges[ind].length,t->maxedgelen,&color);
				color.tag=0;
			}
			t->Edges[ind].Color.R=color.R;	t->Edges[ind].Color.G=color.G;	t->Edges[ind].Color.B=color.B;	t->Edges[ind].Color.A=color.A;t->Edges[ind].Color.tag=color.tag;
	}
	/*update node size values in case node size is changed*/
	t->init_node_size=t->minedgelen*10/GetOGLDistance(10)*atoi(agget(view->g[view->activeGraph],"nodesize"))/100.0*5.00;
	t->init_zoom=view->zoom;

}
void update_topview(Agraph_t * g, topview * t,int init)
{
	if (init)
		preparetopview(g,t);
	settvposinfo(g,t);
	settvcolorinfo(g,t);
	set_boundaries(t);
	set_update_required(t);
	btnToolZoomFit_clicked(NULL,NULL);

}

void preparetopview(Agraph_t * g, topview * t)
{
    char *d_attr1;
    char *d_attr2;
    Agnode_t *v;
    Agedge_t *e;
    Agsym_t *sym;
    int ind, ind2, data_type_count;	//number of columns for custom view->Topview data ,IP ,HOST, etc
    float maxedgelen,minedgelen,edgelength;


    maxedgelen=0;
    minedgelen=MAXFLOAT;
    edgelength=0;

    ind = 0;
    ind2 = 0;
    gtk_widget_hide(glade_xml_get_widget(xml, "layout6"));	//hide top panel
//      gtk_widget_hide(glade_xml_get_widget(xml, "menubar1")); //hide menu
    data_type_count = 0;
	d_attr1=NULL;
    d_attr1 = agget(g, "nodelabelattribute");
    if (d_attr1) {
	if (!strcmp(d_attr1, "\\N"))
	    sym = 0;
	else if (!(sym = agattr(g, AGNODE, d_attr1, 0)))
	    d_attr1 = 0;
    }
    d_attr2 = agget(g, "DataAttribute2");


	/*initialize node and edge array */
	printf("# of edges:%d\n",agnedges(g));
	printf("# of nodes:%d\n",agnnodes(g));


	t->Edges = N_GNEW(agnedges(g), topview_edge);

    t->Nodes = N_GNEW(agnnodes(g), topview_node);
	t->maxnodedegree=1;

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
	{
		//bind temp record;
		agbindrec(v, "temp_node_record", sizeof(temp_node_record), TRUE);//graph custom data
		/*initialize group index, -1 means no group */
		t->Nodes[ind].GroupIndex = -1;
		t->Nodes[ind].Node = v;
		t->Nodes[ind].data.TVRef=ind;
		((temp_node_record*)AGDATA(v))->TVref=ind;
		init_element_data(&t->Nodes[ind].data);
		t->Nodes[ind].zoom_factor = 1;
		t->Nodes[ind].degree = agdegree(g, v, 1, 1);
		if (t->Nodes[ind].degree > t->maxnodedegree)
			t->maxnodedegree=t->Nodes[ind].degree;
		view->Topview->Nodes[ind].Label=NULL;

		t->Nodes[ind].node_alpha = 1;
		for (e = agfstout(g, v); e; e = agnxtout(g, e)) 
		{
			init_element_data(&t->Edges[ind2].data);/*init edge data*/
			t->Edges[ind2].Edge = e;
			ind2++;
		}
	ind++;
	}


	/*attach edge node references ,  loop one more time,set colors*/
    ind = 0;
    ind2 = 0;
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) 
	{
		for (e=agfstout(g, v); e; e = agnxtout(g, e)) 
		{
			t->Edges[ind2].Node1 =&t->Nodes[((temp_node_record*)AGDATA(agtail(e)))->TVref];
			t->Edges[ind2].Node2 =&t->Nodes[((temp_node_record*)AGDATA(aghead(e)))->TVref];
			ind2++;
		}
		ind++;
    }
	/*set some stats for topview*/
	t->Nodecount = ind;
    t->Edgecount = ind2;


	/*create glcomp menu system*/
	view->widgets =glcreate_gl_topview_menu();


	/*for 3d graphs , camera controlling widget extension*/
	attach_camera_widget(view);

	/*for grouped data , group data viewing buttons extension*/	
//	load_host_buttons(t, g, view->widgets);
	/*set topologilca fisheye to NULL*/
	t->h = '\0';
    if (view->dfltViewType == VT_TOPFISH)
		t->is_top_fisheye = 1;
    else
		t->is_top_fisheye = 0;

	/*reset picked nodes*/
	t->picked_node_count = 0;
    t->picked_nodes = '\0';

}


float set_gl_dot_size(topview * t)
{
	static float sizevc;
	if (view->active_camera==-1)
		sizevc = t->init_node_size /view->zoom*t->init_zoom;
	else
		sizevc=t->init_node_size /view->cameras[view->active_camera]->r*t->init_zoom;

	return sizevc;

}

float calcfontsize(float totaledgelength,int totallabelsize,int edgecount,int totalnodecount)
{
	float avglength=totaledgelength/(float)edgecount;
	float avglabelsize=totallabelsize/(float)totalnodecount;
	return avglength/ avglabelsize;

}
static int begintopviewnodes(Agraph_t* g)
{
	switch (view->defaultnodeshape)
	{
	case 0:
		glPointSize((GLfloat)set_gl_dot_size(view->Topview));		
		glEnable(GL_POINT_SMOOTH);
		glBegin(GL_POINTS);
		break;
	case 1:
		set_gl_dot_size(view->Topview);		
		break;
	default:
		set_gl_dot_size(view->Topview);		
		glBegin(GL_POINTS);

	};
	//reset single selection mechanism
	view->Selection.single_selected_node=(topview_node*)0;
	view->Selection.single_selected_edge=(topview_edge*)0;
	view->Selection.node_distance=-1;
	return 1;


}
static void enddrawcycle(Agraph_t* g)
{
    if (view->Selection.single_selected_edge)	{
	if (!(view->mouse.button== rightmousebutton))	//right click pick mode
	{	//left click single select mode
		if (view->Selection.single_selected_edge->data.Selected == 0) 
		{
			view->Selection.single_selected_edge->data.Selected = 1;
			select_edge(view->Selection.single_selected_edge);
	    } 
		else
		{
			view->Selection.single_selected_edge->data.Selected = 1;
			deselect_edge(view->Selection.single_selected_edge);
	    }
	}
	/* return 1; */
    }
    if (view->Selection.single_selected_node)	
	{
		if (view->mouse.button== rightmousebutton)	//right click pick mode
			pick_node(view->Selection.single_selected_node);
	else 
	{	//left click single select mode
	    if (view->Selection.single_selected_node->data.Selected == 0) 
		{
			view->Selection.single_selected_node->data.Selected = 1;
			select_node(view->Selection.single_selected_node);
	    } 
		else 
		{
			view->Selection.single_selected_node->data.Selected = 1;
			deselect_node(view->Selection.single_selected_node);
	    }
	}
	}

}


static int endtopviewnodes(Agraph_t* g)
{
	switch (view->defaultnodeshape)
	{
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
    float dotsize = 0;
	dotsize=set_gl_dot_size(view->Topview);		//sets the size of the gl points

	set_topview_options();
	begintopviewnodes(g);
	view->visiblenodecount=0;
	for (ind = 0;
	     ((ind < view->Topview->Nodecount) );
	     ind++) 
	{
	    if (((-view->Topview->Nodes[ind].distorted_x / view->zoom > view->clipX1)
		 && (-view->Topview->Nodes[ind].distorted_x / view->zoom <
		     view->clipX2)
		 && (-view->Topview->Nodes[ind].distorted_y / view->zoom >
		     view->clipY1)
		 && (-view->Topview->Nodes[ind].distorted_y / view->zoom <
		     view->clipY2))
			 || (view->active_camera >= 0) ) {
		float zdepth;
		v = &view->Topview->Nodes[ind];
		view->visiblenodecount = view->visiblenodecount + 1;
			if(!view->drawnodes)
				continue;

		if (!node_visible(v))
		    continue;
		select_topview_node(v);
		//UPDATE view->Topview data from cgraph
		if (v->update_required)
		    update_topview_node_from_cgraph(v);
		if (v->data.Selected == 1) {
		    glColor4f(view->selectedNodeColor.R,
			      view->selectedNodeColor.G,
			      view->selectedNodeColor.B,
			      view->selectedNodeColor.A); 
		    ddx = dx;
		    ddy = dy;
		    ddz = dz;
		} else		//get the color from node
		{
		    glColor4f(v->Color.R, v->Color.G, v->Color.B,
				v->node_alpha*view->defaultnodealpha);
		    ddx = 0;
		    ddy = 0;
		    ddz = 0;
		}

		if (v->distorted_x != v->x)
		    zdepth = (float) Z_FORWARD_PLANE;
		else
		    zdepth = (float) Z_BACK_PLANE;

			if ((view->defaultnodeshape==0))
			{
				glVertex3f(v->distorted_x - ddx,
				v->distorted_y - ddy, v->distorted_z - ddz);
			}
			else if (view->defaultnodeshape==1)
			{
				drawCircle(v->distorted_x - ddx,v->distorted_y - ddy,dotsize,0);
			}
	    }
		else
		{
			/* int a=1; */
		}

	}
	endtopviewnodes(g);
    return 1;

}

static void drawtopviewedges(Agraph_t * g)
{
    topview_edge *e;
    float ddx, ddy, ddz;
    float dddx, dddy, dddz;
    int ind = 0;

    glBegin(GL_LINES);
    set_topview_options();
    for (ind = 0; ((ind < view->Topview->Edgecount) && view->drawedges);ind++) 
	{

		if (((view->Topview->Edges[ind].x1 / view->zoom * -1 >
	      view->clipX1)
	     && (view->Topview->Edges[ind].x1 / view->zoom * -1 <
		 view->clipX2)
	     && (view->Topview->Edges[ind].y1 / view->zoom * -1 >
		 view->clipY1)
	     && (view->Topview->Edges[ind].y1 / view->zoom * -1 <
		 view->clipY2))
	    ||
	    ((view->Topview->Edges[ind].x2 / view->zoom * -1 >
	      view->clipX1)
	     && (view->Topview->Edges[ind].x2 / view->zoom * -1 <
		 view->clipX2)
	     && (view->Topview->Edges[ind].y2 / view->zoom * -1 >
		 view->clipY1)
	     && (view->Topview->Edges[ind].y2 / view->zoom * -1 <
		 view->clipY2))
	    || (view->active_camera >= 0)) {
	    e = &view->Topview->Edges[ind];
	    if (!get_color_from_edge(e)) 
			continue;

	    //select_topview_edge(e);
		if (e->Node1->data.Selected == 1) 
		{	//tail is selected
			ddx = dx;
			ddy = dy;
			ddz = 0;
	    } 
		else {
			ddx = 0;
			ddy = 0;
			ddz = 0;
	    }
		if (e->Node2->data.Selected == 1) 
		{	//head
			dddx = dx;
			dddy = dy;
			dddz = 0;
	    } 
		else 
		{
			dddx = 0;
			dddy = 0;
			dddz = 0;
	    }
		glVertex3f(e->Node1->distorted_x - ddx,
				   e->Node1->distorted_y - ddy,
				e->Node1->distorted_z - ddz);
		glVertex3f(e->Node2->distorted_x - dddx,
				   e->Node2->distorted_y - dddy,
			e->Node2->distorted_z - ddz);
	    
	}
    }
    glEnd();


}

static int drawtopviewlabels(Agraph_t * g)
{
    //drawing labels
	int ind = 0;
	topview_node *v;
	float f;

	if ((view->visiblenodecount >view->labelnumberofnodes) || (!view->labelshownodes) ||(!view->drawnodes))
		return 0;
	if (view->Topview->maxnodedegree > 15)
		f=15;
	else
		f=view->Topview->maxnodedegree;
	for (ind = 0; ind < view->Topview->Nodecount; ind++) 
	{
		
		v = &view->Topview->Nodes[ind];

		if( ((float)view->visiblenodecount   > view->labelnumberofnodes * v->degree /  f) && view->labelwithdegree)
			continue;
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

	if ((view->visiblenodecount >view->labelnumberofnodes) || (!view->labelshowedges))
		return 0;
	if (view->Topview->maxnodedegree > 15)
		f=15;
	else
		f=view->Topview->maxnodedegree;
	for (ind = 0; ind < view->Topview->Edgecount; ind++) 
	{
		
		e = &view->Topview->Edges[ind];

		if(
			(((float)view->visiblenodecount   > view->labelnumberofnodes * e->Node1->degree /  f) && view->labelwithdegree)
									&&
			(((float)view->visiblenodecount   > view->labelnumberofnodes * e->Node2->degree /  f) && view->labelwithdegree)
			)
			continue;
		if( (!node_visible(e->Node1)) && (!node_visible(e->Node2)) )
		    continue;
		draw_topview_edge_label(e, 0.001);
	}
	return 1;
}


void drawTopViewGraph(Agraph_t * g)
{
    drawtopviewnodes(g);
	drawtopviewlabels(g);
    drawtopviewedges(g);
	drawtopviewedgelabels(g);
	enddrawcycle(g);


	draw_node_hint_boxes();
    if ((view->Selection.Active > 0) && (!view->SignalBlock)) {
	view->Selection.Active = 0;
	drawTopViewGraph(g);
	view->SignalBlock = 1;
	glexpose();
	view->SignalBlock = 0;
    }
}

static int is_node_picked(topview_node * n)
{
    int ind = 0;
    int found = 0;
    for (; ind < view->Topview->picked_node_count; ind++) {
	if ((view->Topview->picked_nodes[ind] == n) && (!found))
	    return 1;
    }
    return 0;
}

static int remove_from_pick_list(topview_node * n)
{
    int ind = 0;
    int found = 0;
	view->mouse.button=-1;	//reset button click to avoid extra selection and pick chekcs
 
	for (; ind < view->Topview->picked_node_count; ind++) {
	if ((view->Topview->picked_nodes[ind] == n) && (!found))
	    found = 1;
	if ((found) && (ind < (view->Topview->picked_node_count - 1))) {
	    view->Topview->picked_nodes[ind] =
		view->Topview->picked_nodes[ind + 1];
	}
    }
    if (found) {
	view->Topview->picked_node_count--;
	view->Topview->picked_nodes =
	    realloc(view->Topview->picked_nodes,
		    sizeof(topview_node *) *
		    view->Topview->picked_node_count);
	return 1;
    }
    return 0;
}

static int add_to_pick_list(topview_node * n)
{
    view->Topview->picked_node_count++;
    view->Topview->picked_nodes =
	realloc(view->Topview->picked_nodes,
		sizeof(topview_node *) * view->Topview->picked_node_count);
    view->Topview->picked_nodes[view->Topview->picked_node_count - 1] = n;
	view->mouse.button=-1;	//reset button click to avoid extra selection and pick chekcs

    return 1;

}

static int pick_node(topview_node * n)
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

}
static int draw_node_hint_boxes(void)
{
    int ind;
	float fs = GetOGLDistance(12);

	for (ind = 0; ind < view->Topview->picked_node_count; ind++) {
//	int draw_node_hintbox(GLfloat x,GLfloat y,GLfloat z,GLfloat fs,char* text)
		draw_node_hintbox(view->Topview->picked_nodes[ind]->distorted_x,
			  view->Topview->picked_nodes[ind]->distorted_y,
				view->Topview->picked_nodes[ind]->distorted_z,
			  (GLfloat) fs,agnameof(view->Topview->picked_nodes[ind]->Node)
			  )
			  ;
		view->widgets->fontset->fonts[view->widgets->fontset->activefont]->fontheight=fs;
	/*blue font color*/
//	fontwidth=GetOGLDistance(glutBitmapLength(GLUT_BITMAP_HELVETICA_12,agnameof(view->Topview->picked_nodes[ind]->Node)));
	glColor4f(0, 0, 1, 1);
    glprintfglut (GLUT_BITMAP_HELVETICA_12, view->Topview->picked_nodes[ind]->distorted_x,
					(view->Topview->picked_nodes[ind]->distorted_y+fs+fs/(GLfloat)5.0) ,
					agnameof(view->Topview->picked_nodes[ind]->Node));


		       
    }
    return 1;
}


static int select_topview_node(topview_node * n)
{
/*    if (!view->Selection.Active) {
	//implement hint box here
		if (view->mouse.button== rightmousebutton)
		{
			if (pick_node(n))
				view->mouse.button = -1;
		}
		return 0;
    }*/
	if (
		(( view->Selection.Type == 0) && (view->Selection.Active))
		|| 
		(view->mouse.button== rightmousebutton))	//single selection or right click (picking)
	{
		float dist=(float)pow((view->Selection.X-n->distorted_x),2)+(float)pow((view->Selection.Y-n->distorted_y),2);
		if ((view->Selection.node_distance==-1) ||(dist < view->Selection.node_distance))
		{
				view->Selection.node_distance=dist;
				view->Selection.single_selected_node=n;
		}

		return 0;
		
/*		if (OD_Selected(n->Node) == 0)
		{
			OD_Selected(n->Node) = 1;
			select_object(view->g[view->activeGraph], n->Node);
	    } else {
			OD_Selected(n->Node) = 1;
			deselect_object(view->g[view->activeGraph], n->Node);
	    }
	    break;*/

	}
	if(view->Selection.Active==0)
		return 0;
	if (is_point_in_rectangle
	(n->x, n->y, view->Selection.X, view->Selection.Y,
	 view->Selection.W, view->Selection.H)) {

	switch (view->Selection.Type) {

/*
	int Active;			//0 there is no selection need to be applied
    char Type;			//0     single selection , 1 rectangle , 2 rectangleX 
    float X, Y, W, H;		//selection boundries
    int Anti;			//subtract selections if 1
    int AlreadySelected;	//for single selections to avoid selecting more than one object
    RGBColor SelectionColor;
*/


	case 1:
	case 2:
	    if (view->Selection.Anti == 0) {
		select_node(n);
		view->Selection.AlreadySelected = 1;
	    } else {

		deselect_node (n);
		view->Selection.AlreadySelected = 1;
	    }
	    break;

	}
    }
    return 1;
}


#if 0
static int select_topview_edge(topview_edge * e)
{
    
	int r = 0;
	if (
		(( view->Selection.Type == 0) && (view->Selection.Active))
		|| 
		(view->mouse.button== rightmousebutton))	//single selection or right click (picking)
	{

		float dist=distance_to_line(e->x1,e->y1,e->x2,e->y2,view->Selection.X,view->Selection.Y);
		if ((view->Selection.node_distance==-1) ||(dist < view->Selection.node_distance))
		{
				view->Selection.node_distance=dist;
				view->Selection.single_selected_edge=e;
		}

		return 0;
	}
    if (!view->Selection.Active)
		return 0;
    r = (lineintersects(e->x1, e->y1, e->x2, e->y2));
    if (r >= 0) 
	{
		switch (view->Selection.Type) 
		{
		case 0:
			if (OD_Selected(e->Edge) == 0) 
			{
				OD_Selected(e->Edge) = 1;
				select_object(view->g[view->activeGraph], e->Edge);
			}
			else 
			{
				OD_Selected(e->Edge) = 1;
				deselect_object(view->g[view->activeGraph], e->Edge);
			}
		    break;

		}
    }
    return 1;

}
#endif

static int update_topview_node_from_cgraph(topview_node * Node)
{
    char *buf;
    buf = agget(Node->Node, "color");
    if (buf)
	Node->Color = GetRGBColor(buf);
/*	else
	{
		randomize_color(&(Node->Color),2);

		Node->Color.R=view->penColor.R;
		Node->Color.G=view->penColor.G;
		Node->Color.B=view->penColor.B;
		Node->Color.A=view->penColor.A;
	}*/
    Node->update_required = 0;
    return 1;
}

#if 0
int update_topview_edge_from_cgraph(topview_edge * Edge)
{
    //for now just color , maybe i need more later
    char buf[124];
    strcpy(buf,
	   (agget(Edge->Edge, "color") ==
	    NULL) ? "black" : agget(Edge->Edge, "color"));
    if (strlen(buf) > 0)
	Edge->Color = GetRGBColor(buf);
    else {
	Edge->Color.R = view->penColor.R;
	Edge->Color.G = view->penColor.G;
	Edge->Color.B = view->penColor.B;
	Edge->Color.A = view->penColor.A;
    }
    Edge->update_required = 0;
    return 1;
}
#endif



int set_update_required(topview * t)
{
    int i = 0;
    int ilimit;
    ilimit = (t->Nodecount > t->Edgecount) ? t->Nodecount : t->Edgecount;

    for (i = 0; i < ilimit; i++) {
	if (t->Nodecount > i)

	    t->Nodes[i].update_required = 1;
	if (t->Edgecount > i)
	    t->Edges[i].update_required = 1;
    }
    return 1;

}

float calculate_font_size(topview_node * v)
{
	float n;
	n=(float)v->degree+(float)1.00;
	return n;

}

static int draw_topview_label(topview_node * v, float zdepth)
{

    float ddx = 0;
    float ddy = 0;
	char* buf;
	if ((v->distorted_x / view->zoom * -1 > view->clipX1)
	&& (v->distorted_x / view->zoom * -1 < view->clipX2)
	&& (v->distorted_y / view->zoom * -1 > view->clipY1)
	&& (v->distorted_y / view->zoom * -1 < view->clipY2)) 
	{
		if (v->data.Selected == 1) 
		{
		    ddx = dx;
			ddy = dy;
		}
		glColor4f(view->nodelabelcolor.R,view->nodelabelcolor.G,view->nodelabelcolor.B,view->nodelabelcolor.A);
		buf=agget(agraphof(v->Node),"nodelabelattribute");
		if (buf)
			glprintfglut(view->glutfont,(v->distorted_x - ddx),(v->distorted_y - ddy),agget(v->Node,buf));
		return 1;
    } else
		return 0;
}
static int draw_topview_edge_label(topview_edge * e, float zdepth)
{

    float ddx = 0;
    float ddy = 0;
	char* buf;
	float x1,y1,x2,y2,x,y;
	x1=e->Node1->distorted_x;
	y1=e->Node1->distorted_y;
	x2=e->Node2->distorted_x;
	y2=e->Node2->distorted_y;


	if ((x1 / view->zoom * -1 > view->clipX1)
	&& (x1 / view->zoom * -1 < view->clipX2)
	&& (y1 / view->zoom * -1 > view->clipY1)
	&& (y1 / view->zoom * -1 < view->clipY2)) 
	{

		x=(x2-x1)/2.00 + x1;
		y=(y2-y1)/2.00 + y1;
		if (e->data.Selected==1)
		{
		    ddx = dx;
			ddy = dy;
		}
		glColor4f(view->edgelabelcolor.R,view->edgelabelcolor.G,view->edgelabelcolor.B,view->edgelabelcolor.A);
		buf=agget(agraphof(e->Edge),"edgelabelattribute");
		if (buf)
			glprintfglut(view->glutfont,x - ddx,y - ddy,agget(e->Edge,buf));
		return 1;
    } else
		return 0;
}




static void set_topview_options(void)
{

    if ((view->mouse.mouse_mode == 10) && (view->mouse.mouse_down == 1))	//selected, if there is move move it, experimental
    {
	dx = view->GLx - view->GLx2;
	dy = view->GLy - view->GLy2;
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
    GdkColor color;
    int return_value = 0;
    float Alpha = 0;
    GtkHScale *AlphaScale =
	(GtkHScale *) glade_xml_get_widget(xml, "settingsscale2");
    Alpha = (float) gtk_range_get_value((GtkRange *) AlphaScale);

    //check visibility;
    if ((node_visible(e->Node1))
	&& (node_visible(e->Node2)))
	return_value = 1;


    /*if both head and tail nodes are selected use selection color for edges */
	if ((e->Node1->data.Selected) || (e->Node2->data.Selected)) {
	glColor4f(view->selectedEdgeColor.R, view->selectedEdgeColor.G,
		  view->selectedEdgeColor.B, view->selectedEdgeColor.A);
	return return_value;
    }
    /*if both head and tail nodes are highlighted use edge highlight color */
	if ((e->Node1->data.Highlighted)
	&& (e->Node2->data.Highlighted)) {
	glColor4f(view->highlightedEdgeColor.R,
		  view->highlightedEdgeColor.G,
		  view->highlightedEdgeColor.B,
		  view->highlightedEdgeColor.A);
	return return_value;
    }
    /*edge maybe in a group and group may be selected, then use groups's color example:ATT hosts */
    if ((e->Node1->GroupIndex >= 0) || (e->Node2->GroupIndex >= 0)) {
	if (view->Topview->TopviewData->hostactive[e->Node1->GroupIndex] ==
	    1) {
	    gtk_color_button_get_color(view->Topview->TopviewData->
				       gtkhostcolor[e->Node1->GroupIndex],
				       &color);
	    glColor4f((GLfloat) color.red / (GLfloat) 65535.0,
		      (GLfloat) color.green / (GLfloat) 65535.0,
		      (GLfloat) color.blue / (GLfloat) 65535.0,
		      (GLfloat) 1);
	    return return_value;
	} else {
	    if (view->Topview->TopviewData->
		hostactive[e->Node2->GroupIndex] == 1) {
		gtk_color_button_get_color(view->Topview->TopviewData->
					   gtkhostcolor[e->Node2->
							GroupIndex],
					   &color);
		glColor4f((GLfloat) color.red / (GLfloat) 65535.0,
			  (GLfloat) color.green / (GLfloat) 65535.0,
			  (GLfloat) color.blue / (GLfloat) 65535.0,
			  (GLfloat) 1);
		return return_value;
	    }
	}

    }

    /*get edge's color attribute */
	if (e->Color.tag==0)
		glColor4f(e->Color.R,e->Color.G,e->Color.B,Alpha*e->Color.A);
	else
		glColor4f(e->Color.R,e->Color.G,e->Color.B,e->Color.A);
    return return_value;
}

static int node_visible(topview_node * n)
{
	return n->data.Visible;

}

int move_TVnodes(void)
{
    topview_node *v;
    int ind = 0;
    for (ind = 0; ind < view->Topview->Nodecount; ind++) {
	v = &view->Topview->Nodes[ind];
	if (v->data.Selected) {
	    v->distorted_x = v->distorted_x - dx;
	    v->distorted_y = v->distorted_y - dy;
	}
    }
    return 1;
}


#ifdef UNUSED
int load_host_buttons(Agraph_t * g, glCompSet * s)
{
    GtkLayout *layout;
    int btncount = 0;
    int i = 0;
    char buf[255];
    char *str;
    char hostbtncaption[50];
    char hostbtnregex[50];
    char hostbtncolorR[50];
    char hostbtncolorG[50];
    char hostbtncolorB[50];
    char hostbtncolorA[50];
    int X = 10;
    int Y = 25;
    GdkColor color;
    glCompPanel *p;
    glCompButton *b;

    layout = glade_xml_get_widget(xml, "frmHostSelectionFixed");
    str = '\0';
    str = agget(g, "hostbtncount");
    if (str)
	btncount = atoi(str);

//      Graph [hostbtncaption1="AT&T",hostbtnregex1="*.ATT*",hostbtncolorR1="1",hostbtncolorG1="0",hostbtncolorB1="0",hostbtncolorA1="1"];

    hostregex = N_GNEW(btncount, char **);
    gtkhostbtn = N_GNEW(btncount, GtkButton *);
    gtkhostcolor = N_GNEW(btncount, GtkColorButton *);
    gtkhostbtncount = btncount;
    if (btncount > 0) {
	p = glCompPanelNew(25, 75, 165, 400);
	p->data = 2;		//data panel
	p->color.R = 0.80;
	p->color.B = 0, 2;
	glCompSetAddPanel(s, p);
    }
    for (i = 0; i < btncount; i++) {
	sprintf(hostbtncaption, "hostbtncaption%i", i);
	sprintf(hostbtnregex, "hostbtnregex%i", i);
	sprintf(hostbtncolorR, "hostbtncolorR%i", i);
	sprintf(hostbtncolorG, "hostbtncolorG%i", i);
	sprintf(hostbtncolorB, "hostbtncolorB%i", i);
	sprintf(hostbtncolorA, "hostbtncolorA%i", i);
	printf("caption:%s regex:%s Color(%s,%s,%s,%s)\n",
	       agget(g, hostbtncaption),
	       agget(g, hostbtnregex),
	       agget(g, hostbtncolorR),
	       agget(g, hostbtncolorG),
	       agget(g, hostbtncolorB), agget(g, hostbtncolorA));
	hostregex[i] = agget(g, hostbtnregex);

	b = glCompButtonNew(5, 7 + (i + 1) * 36, 150, 35,
			    agget(g, hostbtncaption), '\0', 0, 0);
	b->color.R = atof(agget(g, hostbtncolorR));
	b->color.G = atof(agget(g, hostbtncolorG));
	b->color.B = atof(agget(g, hostbtncolorB));
	b->color.A = 1;
	b->panel = p;
	b->groupid = -1;
	b->callbackfunc = glhost_button_clicked_Slot;
	b->data = i;
	glCompSetAddButton(s, b);

	gtkhostbtn[i] =
	    gtk_button_new_with_label(agget(g, hostbtncaption));
	g_signal_connect((gpointer) gtkhostbtn[i], "clicked",
			 G_CALLBACK(host_button_clicked_Slot), i);

	color.blue = 65535 * atof(agget(g, hostbtncolorB));
	color.red = 65535 * atof(agget(g, hostbtncolorR));
	color.green = 65535 * atof(agget(g, hostbtncolorG));

	gtkhostcolor[i] = gtk_color_button_new_with_color(&color);

	gtk_color_button_set_alpha(gtkhostbtn[i],
				   65535 * atof(agget(g, hostbtncolorA)));


	gtk_layout_put(layout, gtkhostbtn[i], X, Y);
	gtk_widget_set_size_request(gtkhostbtn[i], 200, 35);

	gtk_layout_put(layout, gtkhostcolor[i], X + 225, Y);
	gtk_widget_set_size_request(gtkhostcolor[i], 40, 35);

	gtk_widget_show(gtkhostbtn[i]);
	gtk_widget_show(gtkhostcolor[i]);
	Y = Y + 40;
	hostactive[i] = 0;
    }
    p->height = 15 + (btncount + 1) * 36;
    for (i = 0; i < btncount; i++) {
	prepare_nodes_for_groups(i);
    }
}

static void on_host_alpha_change(GtkWidget * widget, gpointer user_data)
{
    glexpose();
}
#endif

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

#if 0
void test_callback(void)
{
}
#endif

static void menu_click_control(void *p)
{
    glCompSet *s;
    int ind = 0;
    s = ((glCompButton *) p)->parentset;
    for (ind = 0; ind < s->panelcount; ind++) {
	if (s->panels[ind]->data > 0)
	    glCompPanelHide(s->panels[ind]);	//hide all panels
	if (s->panels[ind]->data == 1)	//control panel
	{
	    glCompPanelShow(s->panels[ind]);
	}
    }
}

static void menu_click_data(void *p)
{
    glCompSet *s;
    int ind = 0;
    s = ((glCompButton *) p)->parentset;
    for (ind = 0; ind < s->panelcount; ind++) {
	if (s->panels[ind]->data > 0)
	    glCompPanelHide(s->panels[ind]);	//hide all panels
	if (s->panels[ind]->data == 2)	//data panel
	{
	    glCompPanelShow(s->panels[ind]);
	}
    }
}

static void menu_click_hide(void *p)
{
    glCompSet *s;
    int ind = 0;
    s = ((glCompButton *) p)->parentset;
    for (ind = 0; ind < s->panelcount; ind++) {
	if (s->panels[ind]->data > 0)
	    glCompPanelHide(s->panels[ind]);	//hide all panels
    }
}

static void menu_click_pan(void *p)
{
	switch_Mouse(NULL,MM_PAN);

}

static void menu_click_zoom(void *p)
{

	switch_Mouse(NULL,MM_ZOOM);

}

static void menu_click_fisheye_magnifier(void *p)
{
    view->mouse.mouse_mode = MM_FISHEYE_MAGNIFIER;

}

static void menu_click_zoom_minus(void *p)
{
}

static void menu_click_zoom_plus(void *p)
{
}

#ifdef UNUSED
static void menu_click_alpha_plus(void *p)
{
    if ((view->zoom + ZOOM_STEP) < MAX_ZOOM)
	view->zoom = view->zoom + ZOOM_STEP;
    else
	view->zoom = (float) MAX_ZOOM;

}
#endif

static void menu_click_3d_view(void *p)
{
    glCompSet *s;
    int ind = 0;
    s = ((glCompButton *) p)->parentset;
    for (ind = 0; ind < s->panelcount; ind++) {
	if (s->panels[ind]->data > 0)
	    glCompPanelHide(s->panels[ind]);	//hide all panels
	if (s->panels[ind]->data == 3)	//cameras panel
	{
	    glCompPanelShow(s->panels[ind]);
	}
    }

}

static void menu_switch_to_normal_mode(void *p)
{
    view->Topview->is_top_fisheye = 0;

	g_timer_stop(view->timer);

}

static void menu_switch_to_fisheye(void *p)
{
    if (!view->Topview->h) {
	prepare_topological_fisheye(view->Topview);
	g_timer_start(view->timer);
    }
    view->Topview->is_top_fisheye = 1;
/*    char* str;
	int value;
	str = agget(view->g[view->activeGraph], "topologicalfisheyelabelfinenodes");
	value = (float) atof(str);*/

}

static void menu_click_rotate(void *p)
{
	switch_Mouse(NULL,MM_ROTATE);
	view->mouse.mouse_mode = MM_ROTATE;
}

static void menu_click_rotate_x(void *p)
{
	switch_Mouse(NULL,MM_ROTATE);

	view->mouse.rotate_axis = MOUSE_ROTATE_X;
}

static void menu_click_rotate_y(void *p)
{
	switch_Mouse(NULL,MM_ROTATE);

	view->mouse.rotate_axis = MOUSE_ROTATE_Y;
}

static void menu_click_rotate_xy(void *p)
{
	switch_Mouse(NULL,MM_ROTATE);

	view->mouse.rotate_axis = MOUSE_ROTATE_XY;
}

static void menu_click_rotate_z(void *p)
{
	switch_Mouse(NULL,MM_ROTATE);

	view->mouse.rotate_axis = MOUSE_ROTATE_Z;
}

static void menu_click_center(void *p)
{
	if (view->active_camera == -1)	/*2D mode*/
	{	
		btnToolZoomFit_clicked(NULL,NULL);
	}
	else	/*there is active camera , adjust it to look at the center*/
	{
		view->cameras[view->active_camera]->targetx=0;
		view->cameras[view->active_camera]->targety=0;
		view->cameras[view->active_camera]->r=20;

	}
}

/*1) 3D select or identify.
2) Should 3D nodes have a size? (Strange behavior: some 3D views have large node sizes. Why the difference?)
3) Sanity button - if I get lost in 3D, reset the viewpoint so that I have a good view of the graph
4) Additional selection options when selecting nodes - at present, we do union - nice to have intersection, subtraction
5) User control of alpha, so I can fade out the edges.

I'll see if I can track down the color bug.*/




static char *smyrna_icon_pan;
static char *smyrna_icon_zoom;
static char *smyrna_icon_zoomplus;
static char *smyrna_icon_zoomminus;
static char *smyrna_icon_fisheye;
static char *smyrna_icon_rotate;

glCompSet *glcreate_gl_topview_menu(void)
{

    glCompSet *s = glCompSetNew();
    glCompPanel *p;
    glCompButton *b;
    glCompLabel *l;
	s->fontset=fontset_init();
	/*add a glut font*/
	add_glut_font(s->fontset,GLUT_BITMAP_HELVETICA_12);	
	s->fontset->activefont=0;

	/* GtkRequisition requisition; *//* What??*/
    if (!smyrna_icon_pan) {
#ifdef _WIN32
	smyrna_icon_pan = "c:/pan.raw";
	smyrna_icon_zoom = "c:/zoom.raw";
	smyrna_icon_zoomplus = "c:/zoomplus.raw";
	smyrna_icon_zoomminus = "c:/zoomminus.raw";
	smyrna_icon_fisheye = "c:/fisheye.raw";
	smyrna_icon_rotate = "c:/rotate.raw";
#else
	smyrna_icon_pan = smyrnaPath("pan.raw");
	smyrna_icon_zoom = smyrnaPath("zoom.raw");
	smyrna_icon_zoomplus = smyrnaPath("zoomplus.raw");
	smyrna_icon_zoomminus = smyrnaPath("zoomminus.raw");
	smyrna_icon_fisheye = smyrnaPath("fisheye.raw");
	smyrna_icon_rotate = smyrnaPath("rotate.raw");
#endif
    }

    s->panelcount = 0;
    s->panels = '\0';
    s->buttoncount = 0;
    s->buttons = '\0';
    s->labels = '\0';
    s->labelcount = 0;
    /*panel data 
       0 :small bottom
       1 :control
       2 :data


     */

    //small panel left bottom
    p = glCompPanelNew(25, 25, 325, 40,scientific_y);
    p->data = 0;
    glCompSetAddPanel(s, p);

    b = glCompButtonNew(5, 7, 75, 25, "BROWSE", '\0', 0, 0,scientific_y);
    b->panel = p;
    b->groupid = 1;
    b->customptr = view;
    glCompSetAddButton(s, b);
    b->callbackfunc = menu_click_control;


    b = glCompButtonNew(85, 7, 75, 25, "SHOW", '\0', 0, 0,scientific_y);
    b->panel = p;
    b->customptr = view;
    b->groupid = 1;
    b->callbackfunc = menu_click_data;
    glCompSetAddButton(s, b);


    b = glCompButtonNew(165, 7, 75, 25, "CAMERAS", '\0', 0, 0,scientific_y);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_3d_view;
    glCompSetAddButton(s, b);

    b = glCompButtonNew(245, 7, 75, 25, "HIDE", '\0', 0, 0,scientific_y);
    b->color.R = 1;
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_hide;
    glCompSetAddButton(s, b);

    //Fisheye control panel
//	p = glCompPanelNew(	25, view->h-40, 165, 40,scientific_y);
  //  p->data = 1;		//control panel
  //  glCompSetAddPanel(s, p);

	//control panel
    p = glCompPanelNew(25, 75, 165, 277,scientific_y);
    p->data = 1;		//control panel
    glCompSetAddPanel(s, p);

    //view mode normal button
    b = glCompButtonNew(5, 7, 75, 25, "NORMAL", '\0', 0, 0,scientific_y);
    b->color.R = 0;
    b->color.G = 1;
    b->color.B = (float) 0.1;
    b->customptr = view;
    b->panel = p;
    b->groupid = 2;
    b->callbackfunc = menu_switch_to_normal_mode;

    glCompSetAddButton(s, b);
    //view mode fisheye button
    b = glCompButtonNew(85, 7, 75, 25, "FISHEYE", '\0', 0, 0,scientific_y);
    b->color.R = 0;
    b->color.G = 1;
    b->color.B = (float) 0.1;
    b->customptr = view;
    b->panel = p;
    b->groupid = 2;
    b->callbackfunc = menu_switch_to_fisheye;
    glCompSetAddButton(s, b);

    //rotate
    b = glCompButtonNew(5, 197, 72, 72, "", smyrna_icon_rotate, 72, 72,scientific_y);
    b->groupid = 3;
    b->customptr = view;
    b->panel = p;
    b->callbackfunc = menu_click_rotate;
    glCompSetAddButton(s, b);

    b = glCompButtonNew(80, 251, 40, 20, "X", '\0', 0, 0,scientific_y);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_rotate_x;
    glCompSetAddButton(s, b);
    b = glCompButtonNew(125, 251, 40, 20, "Y", '\0', 0, 0,scientific_y);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_rotate_y;
    glCompSetAddButton(s, b);
    b = glCompButtonNew(80, 231, 40, 20, "XY", '\0', 0, 0,scientific_y);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_rotate_xy;

	glCompSetAddButton(s, b);
    b = glCompButtonNew(125, 231, 40, 20, "Z", '\0', 0, 0,scientific_y);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_rotate_z;
    glCompSetAddButton(s, b);

	//sanity button to center the drawing and fit it in the screen
    b = glCompButtonNew(80, 201, 90, 20, "center", '\0', 0, 0,scientific_y);
    b->customptr = view;
    b->panel = p;
    b->groupid = 0;
    b->callbackfunc = menu_click_center;
	b->color.R=0;
	b->color.G=1;
	b->color.B=0;
    glCompSetAddButton(s, b);




    //pan button
    b = glCompButtonNew(5, 120, 72, 72, "adasasds", smyrna_icon_pan, 72,
			72,scientific_y);
    b->groupid = 3;
    b->customptr = view;
    b->panel = p;
    b->callbackfunc = menu_click_pan;
    glCompSetAddButton(s, b);

    //zoom
    b = glCompButtonNew(85, 120, 72, 72, "adasasds", smyrna_icon_zoom, 72,
			72,scientific_y);
    b->groupid = 3;
    b->customptr = view;
    b->panel = p;
    b->callbackfunc = menu_click_zoom;
    glCompSetAddButton(s, b);
    //zoom +
    b = glCompButtonNew(85, 82, 36, 36, "adasasds", smyrna_icon_zoomplus,
			36, 36,scientific_y);
    b->groupid = 0;
    b->customptr = view;
    b->panel = p;
    b->callbackfunc = menu_click_zoom_plus;
    glCompSetAddButton(s, b);
    //zoom -
    b = glCompButtonNew(121, 82, 36, 36, "adasasds", smyrna_icon_zoomminus,
			36, 36,scientific_y);
    b->groupid = 0;
    b->panel = p;
    b->customptr = view;
    b->callbackfunc = menu_click_zoom_minus;
    glCompSetAddButton(s, b);

    b = glCompButtonNew(5, 45, 72, 72, "adasasds", smyrna_icon_fisheye, 72,
			72,scientific_y);
    b->groupid = 3;
    b->panel = p;
    b->customptr = view;
    b->callbackfunc = menu_click_fisheye_magnifier;
    glCompSetAddButton(s, b);
    //zoom percantage label
    l = glCompLabelNew(100, 45, 24, "100",scientific_y);
    l->panel = p;
    l->fontsizefactor = (float) 0.4;
    glCompSetAddLabel(s, l);
    view->Topview->customptr = l;
    l = glCompLabelNew(93, 65, 20, "zoom",scientific_y);
    l->panel = p;
    l->fontsizefactor = (float) 0.4;
    glCompSetAddLabel(s, l);

    glCompPanelHide(p);


	
	//3d Controls Panel
  /*  p = glCompPanelNew(25,view->h,wid_w,wid_h);
    p->data = 1;		
    glCompSetAddPanel(s, p);*/
	

	return s;

}

#define strcaseeq(a,b)     (*(a)==*(b)&&!strcasecmp(a,b))

gvk_layout 
s2layout (char* s)
{
    if (!s) return GVK_NONE;

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

char* 
layout2s (gvk_layout gvkl)
{
    char* s;
    switch (gvkl) {
    case GVK_NONE :
	s = "";
	break;
    case GVK_DOT :
	s = "dot";
	break;
    case GVK_NEATO :
	s = "neato";
	break;
    case GVK_TWOPI :
	s = "twopi";
	break;
    case GVK_CIRCO :
	s = "circo";
	break;
    case GVK_FDP :
	s = "fdp";
	break;
    case GVK_SFDP :
	s = "sfdp";
	break;
    default :
	s = "";
	break;
    }
    return s;
}

char* 
element2s (gve_element el)
{
    char* s;
    switch (el) {
    case GVE_NONE :
	s = "";
	break;
    case GVE_GRAPH :
	s = "graph";
	break;
    case GVE_CLUSTER :
	s = "cluster";
	break;
    case GVE_NODE :
	s = "node";
	break;
    case GVE_EDGE :
	s = "edge";
	break;
    default :
	s = "";
	break;
    }
    return s;
}

static int node_regex(topview_node * n,char* exp)
{

    regex_t preg;
	char *data =n->Label;
	int return_value=0;
	if (data) 
	{
	    regcomp(&preg, exp, REG_NOSUB);
	    if (regexec(&preg, data, 0, 0, 0) == 0)
			return_value=1;
		else
			return_value=0;
	    regfree(&preg);
	} 
	return return_value;
}

void select_with_regex(char* exp)
{
	topview_node *v;
	int ind=0;
	for (ind = 0; ind < view->Topview->Nodecount; ind++) 
	{
	    v = &view->Topview->Nodes[ind];
	    if (node_visible(v))
		{
			if(node_regex(v,exp))
			{
				v->data.Selected = 1;
				select_node(v);
			}
		}
	}


}



