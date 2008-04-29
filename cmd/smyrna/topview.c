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
#include "glTexFont.h"
#include "glTexFontTGA.h"
#include "glTexFontDefs.h"
#include "glTexFontInclude.h"
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
#include "topfisheyeview.h"
static float dx = 0.0;
static float dy = 0.0;
static float dz = 0.0;

   /* Forward declarations */
static glCompSet *glcreate_gl_topview_menu();
static void set_boundaries(topview * t);
static void set_topview_options();
static int draw_topview_label(topview_node * v, float zdepth);
static int node_visible(Agnode_t * n);
static int select_topview_node(topview_node * n);
static int select_topview_edge(topview_edge * e);
static int update_topview_node_from_cgraph(topview_node * Node);
static int get_color_from_edge(topview_edge * e);
static int draw_node_hint_boxes();

void cleartopview(topview * t)
{
    /*clear nodes */
    free(t->Nodes);
    /*clear edges */
    free(t->Edges);
    /*clear gl menu */
    glCompSetClear(t->topviewmenu);
    free(t);
}

void preparetopview(Agraph_t * g, topview * t)
{
    char *str;
    char *d_attr1;
    char *d_attr2;
    float a, b, c;
    Agnode_t *v;
    Agedge_t *e;
    Agsym_t *sym;
    int ind, ind2, data_type_count;	//number of columns for custom view->Topview data ,IP ,HOST, etc
    char buf[256];
    RGBColor color;
    ind = 0;
    ind2 = 0;
    gtk_widget_hide(glade_xml_get_widget(xml, "layout6"));	//hide top panel
//      gtk_widget_hide(glade_xml_get_widget(xml, "menubar1")); //hide menu
    data_type_count = 0;
    d_attr1 = agget(g, "DataAttribute1");
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

    printf("# of edges :%i\n", agnnodes(g));
    printf("# of edges :%i\n", agnedges(g));

    /* malloc topviewdata */
    t->TopviewData = NEW(topviewdata);

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	//set node TV reference
	OD_TVRef(v) = ind;	//view->Topview reference
	strcpy(buf, agget(v, "pos"));
	if (strlen(buf)) {
	    a = (float) atof(strtok(buf, ","));
	    b = (float) atof(strtok(NULL, ","));
	    str = strtok(NULL, ",");
	    if (str)
		c = (float) atof(str);
	    else
		c = (float) 0.0;
	}
	/*initialize group index, -1 means no group */
	t->Nodes[ind].GroupIndex = -1;
	t->Nodes[ind].Node = v;
	if (agget(t->Nodes[ind].Node, "color")) {
	    color = GetRGBColor(agget(t->Nodes[ind].Node, "color"));
	    t->Nodes[ind].Color.R = color.R;
	    t->Nodes[ind].Color.G = color.G;
	    t->Nodes[ind].Color.B = color.B;
	    t->Nodes[ind].Color.A = color.A;
	} else
	    randomize_color(&(t->Nodes[ind].Color), 2);
	t->Nodes[ind].x = a;
	t->Nodes[ind].y = b;
	t->Nodes[ind].z = c;
	t->Nodes[ind].distorted_x = a;
	t->Nodes[ind].distorted_y = b;
	t->Nodes[ind].distorted_z = c;

	t->Nodes[ind].zoom_factor = 1;
	t->Nodes[ind].degree = agdegree(g, v, 1, 1);
	t->Nodes[ind].node_alpha = 1;
	//    (float) log((double) t->Nodes[ind].degree + (double) 0.3);
	if (d_attr1) {
	    if (sym)
		str = agxget(v, sym);
	    else
		str = agnameof(v);
	    t->Nodes[ind].Label = strdup(str);
	} else
	    t->Nodes[ind].Label = '\0';
	if (d_attr2) {
	    str = agget(v, d_attr2);
	    if (str) {
		t->Nodes[ind].Label2 = strdup(str);
	    }
	} else
	    t->Nodes[ind].Label2 = '\0';

	for (e = agfstout(g, v); e; e = agnxtout(g, e)) {
	    t->Edges[ind2].Hnode = aghead(e);
	    t->Edges[ind2].Tnode = agtail(e);
	    t->Edges[ind2].Edge = e;
	    strcpy(buf, agget(aghead(e), "pos"));
	    if (strlen(buf)) {
		a = (float) atof(strtok(buf, ","));
		b = (float) atof(strtok(NULL, ","));
		str = strtok(NULL, ",");
		if (str)
		    c = (float) atof(str);
		else
		    c = (float) 0.0;

		t->Edges[ind2].x1 = a;
		t->Edges[ind2].y1 = b;
		t->Edges[ind2].z1 = b;
	    }
	    strcpy(buf, agget(agtail(e), "pos"));
	    if (strlen(buf)) {
		a = (float) atof(strtok(buf, ","));
		b = (float) atof(strtok(NULL, ","));
		str = strtok(NULL, ",");
		if (str)
		    c = (float) atof(str);
		else
		    c = (float) 0.0;
		t->Edges[ind2].x2 = a;
		t->Edges[ind2].y2 = b;
		t->Edges[ind2].z2 = c;
	    }
	    ind2++;
	}
	ind++;
    }
    //attach edge node references   loop one more time
    ind = 0;
    ind2 = 0;
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	//set node TV reference
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) {
	    t->Edges[ind2].Node1 =
		&t->Nodes[OD_TVRef(t->Edges[ind2].Tnode)];
	    t->Edges[ind2].Node2 =
		&t->Nodes[OD_TVRef(t->Edges[ind2].Hnode)];
	    ind2++;

	}
	ind++;
    }
    t->Nodecount = ind;
    t->Edgecount = ind2;
    view->fmg.fisheye_distortion_fac = 5;	//need to be hooked to a widget
    set_boundaries(t);
    set_update_required(t);
    t->topviewmenu = glcreate_gl_topview_menu();
    attach_camera_widget(view);
    load_host_buttons(t, g, t->topviewmenu);
    t->h = '\0';
    t->is_top_fisheye = 0;
    t->picked_node_count = 0;
    t->picked_nodes = '\0';
}

static int drawtopviewnodes(Agraph_t * g)
{
    topview_node *v;
    float ddx, ddy, ddz;
    int ind = 0;
    float dotsize = 0;
    if (view->zoom > NODE_ZOOM_LIMIT) {
	dotsize = 5 / view->zoom * -1;
	if (dotsize > 1)
	    glPointSize(dotsize);
	else
	    glPointSize(1);


	//draw nodes
	set_topview_options();
	glBegin(GL_POINTS);
	for (ind = 0;
	     ((ind < view->Topview->Nodecount) && (view->drawnodes));
	     ind++) {
	    if (((-view->Topview->Nodes[ind].x / view->zoom > view->clipX1)
		 && (-view->Topview->Nodes[ind].x / view->zoom <
		     view->clipX2)
		 && (-view->Topview->Nodes[ind].y / view->zoom >
		     view->clipY1)
		 && (-view->Topview->Nodes[ind].y / view->zoom <
		     view->clipY2))
		|| (view->active_camera >= 0)) {
		float zdepth;
		v = &view->Topview->Nodes[ind];
		if (!node_visible(v->Node))
		    break;

		select_topview_node(v);
		//UPDATE view->Topview data from cgraph
		if (v->update_required)
		    update_topview_node_from_cgraph(v);
		if (OD_Selected(v->Node) == 1) {
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
			      v->node_alpha);
		    ddx = 0;
		    ddy = 0;
		    ddz = 0;
		}

		if (v->distorted_x != v->x)
		    zdepth = (float) Z_FORWARD_PLANE;
		else
		    zdepth = (float) Z_BACK_PLANE;
		glVertex3f(v->distorted_x - ddx,
			   v->distorted_y - ddy, v->distorted_z - ddz);
	    }
	}
	glEnd();
    }
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
    for (ind = 0; ((ind < view->Topview->Edgecount) && view->drawedges);
	 ind++) {
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
	    select_topview_edge(e);
	    if (OD_Selected(e->Node1->Node) == 1) {	//tail is selected
		ddx = dx;
		ddy = dy;
		ddz = 0;
	    } else {
		ddx = 0;
		ddy = 0;
		ddz = 0;
	    }
	    if (OD_Selected(e->Node2->Node) == 1) {	//head
		dddx = dx;
		dddy = dy;
		dddz = 0;
	    } else {
		dddx = 0;
		dddy = 0;
		dddz = 0;
	    }
	    if (get_color_from_edge(e)) {
		glVertex3f(e->Node1->distorted_x - ddx,
			   e->Node1->distorted_y - ddy,
			   e->Node1->distorted_z - ddz);
		glVertex3f(e->Node2->distorted_x - dddx,
			   e->Node2->distorted_y - dddy,
			   e->Node2->distorted_z - ddz);
	    }
	}
    }
    glEnd();


}

static int drawtopviewlabels(Agraph_t * g)
{
    //drawing labels
    int ind = 0;
    if (view->drawlabels) {
	topview_node *v;
	for (ind = 0; ind < view->Topview->Nodecount; ind++) {
	    v = &view->Topview->Nodes[ind];
	    if (!node_visible(v->Node))
		break;
	    draw_topview_label(v, 1);
	}
	return 1;
    }
    return 0;

}


void drawTopViewGraph(Agraph_t * g)
{
    drawtopviewnodes(g);
    drawtopviewlabels(g);
    draw_node_hint_boxes();
    drawtopviewedges(g);
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
    return 1;

}

static int pick_node(topview_node * n)
{
    static int closest_dif = 3;
    float a, b;
    a = ABS(n->distorted_x - view->GLx);
    b = ABS(n->distorted_y - view->GLy);
    a = (float) pow((a * a + b * b), (float) 0.5);
    if (a < closest_dif) {
	if (!is_node_picked(n)) {
	    if (add_to_pick_list(n)) {
		printf("node picked ,name:%s\n", agnameof(n->Node));
		return 1;
	    }
	    return 0;
	} else {
	    if (remove_from_pick_list(n)) {
		printf("node has been unpicked ,name:%s\n",
		       agnameof(n->Node));
		return 1;
	    }
	    return 0;
	}
    }
    return 0;

}
static int draw_node_hint_boxes()
{
    int ind;
    int fs = 12;
    for (ind = 0; ind < view->Topview->picked_node_count; ind++) {
	draw_node_hintbox(view->Topview->picked_nodes[ind]->distorted_x,
			  view->Topview->picked_nodes[ind]->distorted_y,
			  (GLfloat) fs, (GLfloat) 1, (GLfloat) 1,
			  (GLfloat) (strlen
				     (agnameof
				      (view->Topview->picked_nodes[ind]->
				       Node)) / 2), (GLfloat) ind);
	fontSize(fs);
	fontColorA(0, 0, 1, 1);
	fontDrawString((int)
		       (view->Topview->picked_nodes[ind]->distorted_x -
			fs / 3 + 1 - fs),
		       (int) (view->Topview->picked_nodes[ind]->
			      distorted_y + fs + 1),
		       agnameof(view->Topview->picked_nodes[ind]->Node),
		       fs *
		       strlen(agnameof
			      (view->Topview->picked_nodes[ind]->Node)) /
		       2);
    }
    return 1;
}


static int select_topview_node(topview_node * n)
{
    if (!view->Selection.Active) {
	//implement hint box here
	if (view->mouse.pick) {
	    if (pick_node(n))
		view->mouse.pick = 0;
	}
	return 0;
    }
    if (is_point_in_rectangle
	(n->x, n->y, view->Selection.X, view->Selection.Y,
	 view->Selection.W, view->Selection.H)) {

	switch (view->Selection.Type) {
	case 0:
/* FIX
 * Why is Selected being set to 1 in both cases?
 */
	    if (OD_Selected(n->Node) == 0) {
		OD_Selected(n->Node) = 1;
		select_object(view->g[view->activeGraph], n->Node);
	    } else {
		OD_Selected(n->Node) = 1;
		deselect_object(view->g[view->activeGraph], n->Node);
	    }
	    break;

	case 1:
	case 2:
	    if (view->Selection.Anti == 0) {
		select_object(view->g[view->activeGraph], n->Node);
		view->Selection.AlreadySelected = 1;
	    } else {
		deselect_object(view->g[view->activeGraph], n->Node);
		view->Selection.AlreadySelected = 1;
	    }
	    break;

	}
    }
    return 1;
}



static int select_topview_edge(topview_edge * e)
{
    int r = 0;
    if (!view->Selection.Active)
	return 0;
    r = (lineintersects(e->x1, e->y1, e->x2, e->y2));
    if (r >= 0) {

/* FIX
 * Why is Selected being set to 1 in both cases?
 */
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

static int update_topview_node_from_cgraph(topview_node * Node)
{
    //for now just color, maybe i need more later
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

static int draw_topview_label(topview_node * v, float zdepth)
{

    float fs = 0;
    float ddx = 0;
    float ddy = 0;
    if (!v->Label)
	return 0;
    if ((view->zoom * -1 / v->degree / v->zoom_factor) > 2)
	return 0;
    if ((v->distorted_x / view->zoom * -1 > view->clipX1)
	&& (v->distorted_x / view->zoom * -1 < view->clipX2)
	&& (v->distorted_y / view->zoom * -1 > view->clipY1)
	&& (v->distorted_y / view->zoom * -1 < view->clipY2)) {

	fs = (v->degree ==
	      1) ? (float) (log((double) v->degree +
				1) *
			    (double) 7) : (float) (log((double) v->degree +
						       (double) 0.5) *
						   (double) 7);
	fs = fs * v->zoom_factor;
	if (OD_Selected(v->Node) == 1) {
	    ddx = dx;
	    ddy = dy;
	}

	fontSize((int) fs);
	if ((log((float) v->degree) * -0.6 * view->zoom) > 0)
	    fontColorA((float) log((double) v->degree + (double) 1),
		       view->penColor.G, view->penColor.B,
		       view->penColor.A / (float) log((double) v->degree) *
		       (float) -0.6 * (float) view->zoom);
	else
	    fontColorA((float) log((double) v->degree + (double) 1),
		       view->penColor.G, view->penColor.B, 1);

	fontDrawString((int) (v->distorted_x - ddx),
		       (int) (v->distorted_y - ddy), v->Label,
		       (int) (fs * 5));

	return 1;
    } else
	return 0;
}



static void set_topview_options()
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

    printf("graph borders:(%f,%f) (%f,%f)\n", view->bdxLeft,
	   view->bdyBottom, view->bdxRight, view->bdyTop);
}

static int get_color_from_edge(topview_edge * e)
{
    RGBColor c;
    GdkColor color;
    char *color_string;
    int return_value = 0;
    float Alpha = 0;
    GtkHScale *AlphaScale =
	(GtkHScale *) glade_xml_get_widget(xml, "frmHostAlphaScale");
    Alpha = (float) gtk_range_get_value((GtkRange *) AlphaScale);

    //check visibility;
    if ((node_visible(e->Node1->Node))
	&& (node_visible(e->Node2->Node)))
	return_value = 1;


    /*if both head and tail nodes are selected use selection color for edges */
    if ((OD_Selected(e->Node1->Node)) && (OD_Selected(e->Node2->Node))) {
	glColor4f(view->selectedEdgeColor.R, view->selectedEdgeColor.G,
		  view->selectedEdgeColor.B, view->selectedEdgeColor.A);
	return return_value;
    }
    /*if both head and tail nodes are highlighted use edge highlight color */

    if ((OD_Highlighted(e->Node1->Node))
	&& (OD_Highlighted(e->Node2->Node))) {
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
    color_string = agget(e->Edge, "color");
    if (color_string) {
	c = GetRGBColor(color_string);
	glColor4f(c.R, c.G, c.B, Alpha);
    } else
	glColor4f(e->Node1->Color.R, e->Node1->Color.G, e->Node1->Color.B,
		  Alpha);
    return return_value;
}

static int node_visible(Agnode_t * n)
{
    return OD_Visible(n);
}

int move_TVnodes()
{
    topview_node *v;
    int ind = 0;
    for (ind = 0; ind < view->Topview->Nodecount; ind++) {
	v = &view->Topview->Nodes[ind];
	if (OD_Selected(v->Node)) {
	    v->x = v->x - dx;
	    v->y = v->y - dy;
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
void test_callback()
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
    view->mouse.mouse_mode = MM_PAN;

}

static void menu_click_zoom(void *p)
{
    view->mouse.mouse_mode = MM_ZOOM;

}

static void menu_click_fisheye_magnifier(void *p)
{
    view->mouse.mouse_mode = MM_FISHEYE_MAGNIFIER;

}

static void menu_click_zoom_minus(void *p)
{
    if ((view->zoom - ZOOM_STEP) > MIN_ZOOM)
	view->zoom = view->zoom - ZOOM_STEP;
    else
	view->zoom = MIN_ZOOM;
}

static void menu_click_zoom_plus(void *p)
{
    if ((view->zoom + ZOOM_STEP) < MAX_ZOOM)
	view->zoom = view->zoom + ZOOM_STEP;
    else
	view->zoom = (float) MAX_ZOOM;

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
	please_wait();
	prepare_topological_fisheye(view->Topview);
	view->Topview->animate = 1;
	g_timer_start(view->timer);
	please_dont_wait();
    }
    view->Topview->is_top_fisheye = 1;
}

static void menu_click_rotate(void *p)
{
    view->mouse.mouse_mode = MM_ROTATE;
}

static void menu_click_rotate_x(void *p)
{
    view->mouse.rotate_axis = MOUSE_ROTATE_X;
}

static void menu_click_rotate_y(void *p)
{
    view->mouse.rotate_axis = MOUSE_ROTATE_Y;
}

static void menu_click_rotate_xy(void *p)
{
    view->mouse.rotate_axis = MOUSE_ROTATE_XY;
}

static void menu_click_rotate_z(void *p)
{
    view->mouse.rotate_axis = MOUSE_ROTATE_Z;
}

static char *smyrna_icon_pan;
static char *smyrna_icon_zoom;
static char *smyrna_icon_zoomplus;
static char *smyrna_icon_zoomminus;
static char *smyrna_icon_fisheye;
static char *smyrna_icon_rotate;

static glCompSet *glcreate_gl_topview_menu()
{

    glCompSet *s = NEW(glCompSet);
    glCompPanel *p;
    glCompButton *b;
    glCompLabel *l;

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
    p = glCompPanelNew(25, 25, 325, 40);
    p->data = 0;
    glCompSetAddPanel(s, p);

    b = glCompButtonNew(5, 7, 75, 25, "BROWSE", '\0', 0, 0);
    b->panel = p;
    b->groupid = 1;
    b->customptr = view;
    glCompSetAddButton(s, b);
    b->callbackfunc = menu_click_control;


    b = glCompButtonNew(85, 7, 75, 25, "SHOW", '\0', 0, 0);
    b->panel = p;
    b->customptr = view;
    b->groupid = 1;
    b->callbackfunc = menu_click_data;
    glCompSetAddButton(s, b);


    b = glCompButtonNew(165, 7, 75, 25, "CAMERAS", '\0', 0, 0);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_3d_view;
    glCompSetAddButton(s, b);

    b = glCompButtonNew(245, 7, 75, 25, "HIDE", '\0', 0, 0);
    b->color.R = 1;
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_hide;
    glCompSetAddButton(s, b);


    //control panel
    p = glCompPanelNew(25, 75, 165, 277);
    p->data = 1;		//control panel
    glCompSetAddPanel(s, p);

    //view mode normal button
    b = glCompButtonNew(5, 7, 75, 25, "NORMAL", '\0', 0, 0);
    b->color.R = 0;
    b->color.G = 1;
    b->color.B = (float) 0.1;
    b->customptr = view;
    b->panel = p;
    b->groupid = 2;
    b->callbackfunc = menu_switch_to_normal_mode;

    glCompSetAddButton(s, b);
    //view mode fisheye button
    b = glCompButtonNew(85, 7, 75, 25, "FISHEYE", '\0', 0, 0);
    b->color.R = 0;
    b->color.G = 1;
    b->color.B = (float) 0.1;
    b->customptr = view;
    b->panel = p;
    b->groupid = 2;
    b->callbackfunc = menu_switch_to_fisheye;
    glCompSetAddButton(s, b);

    //rotate
    b = glCompButtonNew(5, 197, 72, 72, "", smyrna_icon_rotate, 72, 72);
    b->groupid = 3;
    b->customptr = view;
    b->panel = p;
    b->callbackfunc = menu_click_rotate;
    glCompSetAddButton(s, b);

    b = glCompButtonNew(80, 251, 40, 20, "X", '\0', 0, 0);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_rotate_x;
    glCompSetAddButton(s, b);
    b = glCompButtonNew(125, 251, 40, 20, "Y", '\0', 0, 0);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_rotate_y;
    glCompSetAddButton(s, b);
    b = glCompButtonNew(80, 231, 40, 20, "XY", '\0', 0, 0);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_rotate_xy;
    glCompSetAddButton(s, b);
    b = glCompButtonNew(125, 231, 40, 20, "Z", '\0', 0, 0);
    b->customptr = view;
    b->panel = p;
    b->groupid = 1;
    b->callbackfunc = menu_click_rotate_z;
    glCompSetAddButton(s, b);


    //pan button
    b = glCompButtonNew(5, 120, 72, 72, "adasasds", smyrna_icon_pan, 72,
			72);
    b->groupid = 3;
    b->customptr = view;
    b->panel = p;
    b->callbackfunc = menu_click_pan;
    glCompSetAddButton(s, b);
    //zoom
    b = glCompButtonNew(85, 120, 72, 72, "adasasds", smyrna_icon_zoom, 72,
			72);
    b->groupid = 3;
    b->customptr = view;
    b->panel = p;
    b->callbackfunc = menu_click_zoom;
    glCompSetAddButton(s, b);
    //zoom +
    b = glCompButtonNew(85, 82, 36, 36, "adasasds", smyrna_icon_zoomplus,
			36, 36);
    b->groupid = 0;
    b->customptr = view;
    b->panel = p;
    b->callbackfunc = menu_click_zoom_plus;
    glCompSetAddButton(s, b);
    //zoom -
    b = glCompButtonNew(121, 82, 36, 36, "adasasds", smyrna_icon_zoomminus,
			36, 36);
    b->groupid = 0;
    b->panel = p;
    b->customptr = view;
    b->callbackfunc = menu_click_zoom_minus;
    glCompSetAddButton(s, b);

    b = glCompButtonNew(5, 45, 72, 72, "adasasds", smyrna_icon_fisheye, 72,
			72);
    b->groupid = 3;
    b->panel = p;
    b->customptr = view;
    b->callbackfunc = menu_click_fisheye_magnifier;
    glCompSetAddButton(s, b);
    //zoom percantage label
    l = glCompLabelNew(100, 45, 24, "100");
    l->panel = p;
    l->fontsizefactor = (float) 0.4;
    glCompSetAddLabel(s, l);
    view->Topview->customptr = l;
    l = glCompLabelNew(93, 65, 20, "zoom");
    l->panel = p;
    l->fontsizefactor = (float) 0.4;
    glCompSetAddLabel(s, l);

    glCompPanelHide(p);

/*	//Data Panel
	p=glCompPanelNew(25,75,165,400);
	p->data=2;		//data panel
	glCompSetAddPanel(s,p);
	//alpha plus button
	b=glCompButtonNew(75,3,36,36,"",smyrna_icon_zoomplus,36,36);
	b->groupid=0;
	b->panel=p;
	b->callbackfunc=menu_click_alpha_plus;
	glCompSetAddButton(s,b);
	//alpha minus button
	b=glCompButtonNew(113,3,36,36,"",smyrna_icon_zoomminus,36,36);
	b->groupid=0;
	b->panel=p;
	b->callbackfunc=menu_click_alpha_plus;
	glCompSetAddButton(s,b);

	l=glCompLabelNew(5,8,18,"ALPHA");
	l->panel=p;
	l->fontsizefactor=0.4;
	glCompSetAddLabel(s,l);*/


    return s;

}
