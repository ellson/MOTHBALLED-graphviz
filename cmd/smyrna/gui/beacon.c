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

#include "beacon.h"
#include "viewport.h"
#include "selection.h"
#include "gltemplate.h"
#include "glutils.h"
#include "toolboxcallbacks.h"
#include "sfstr.h"

static void print_object(void* obj);

static int remove_edge_from_pick_list(topview_edge * e)
{
    int ind = 0;
    int found = 0;
    for (; ind < view->Topview->picked_edge_count; ind++) {
	if ((view->Topview->picked_edges[ind] == e) && (!found))
	    found = 1;
	if ((found) && (ind < (view->Topview->picked_edge_count - 1))) {
	    view->Topview->picked_edges[ind] =
		view->Topview->picked_edges[ind + 1];
	}
    }
    if (found) {
	view->Topview->picked_edge_count--;
	view->Topview->picked_edges =
	    realloc(view->Topview->picked_edges,
		    sizeof(topview_edge *) *
		    view->Topview->picked_edge_count);
	deselect_edge(e);
	return 1;
    }
    return 0;
}

static int add_edge_to_pick_list(topview_edge * e)
{
    view->Topview->picked_edge_count++;
    view->Topview->picked_edges =
	realloc(view->Topview->picked_edges,
		sizeof(topview_edge *) * view->Topview->picked_edge_count);
    view->Topview->picked_edges[view->Topview->picked_edge_count - 1] = e;
    select_edge(e);
    print_object((void*) e->Edge);	

    return 1;
}

static int is_edge_picked(topview_edge * e)
{
    int ind = 0;
    int found = 0;
    for (; ind < view->Topview->picked_edge_count; ind++) {
	if ((view->Topview->picked_edges[ind] == e) && (!found))
	    return 1;
    }
    return 0;
}


void pick_node_from_coords(float x, float y, float z)
{
    topview_node *n;
    topview_node *sn;		/*selected node , closest node */
    topview_edge *e;
    topview_edge *se;		/*selected edge , closest one */
    GLfloat closest_dif = 100000000;
    GLfloat closest_dif2 = 100000000;
    float a, b, c;
    double d;
    int ind;
    int selnodes=atoi(agget(view->g[view->activeGraph],"nodesselectable"));
    int seledges=atoi(agget(view->g[view->activeGraph],"edgesselectable"));
    sn = (topview_node *) 0;
    se = (topview_edge *) 0;

    



    for (ind = 0;( (selnodes)&&(ind < view->Topview->Nodecount)); ind++) {
	n = &view->Topview->Nodes[ind];
	if (!select_node)
	    break;


	a = ABS(n->distorted_x - view->mouse.GLpos.x);
	b = ABS(n->distorted_y - view->mouse.GLpos.y);
	c = ABS(n->distorted_z - view->mouse.GLpos.z);
	a = (float) pow((a * a + b * b + c * c), (float) 0.5);
	if (a < closest_dif) {
	    sn = n;
	    closest_dif = a;
	}
    }

    for (ind = 0; ((seledges)&&(ind < view->Topview->Edgecount)); ind++) {
	point3f p1, p2, p3;
	e = &view->Topview->Edges[ind];
	if (!e->data.Visible)
	    continue;

	p1.x = e->Node1->distorted_x;
	p1.y = e->Node1->distorted_y;
	p1.z = e->Node1->distorted_z;

	p2.x = e->Node2->distorted_x;
	p2.y = e->Node2->distorted_y;
	p2.z = e->Node2->distorted_z;

	p3.x = view->mouse.GLpos.x;
	p3.y = view->mouse.GLpos.y;
	p3.z = view->mouse.GLpos.z;
	d = point_to_lineseg_dist(p3, p1, p2);

	if (d < closest_dif2) {
	    se = e;
	    closest_dif2 = d;
	}
    }

    if ((closest_dif < closest_dif2 * 3) ) 
    {
	if (sn) {
	    if (!is_node_picked(sn))
		add_to_pick_list(sn);
	    else
		remove_from_pick_list(sn);
	}
    } else {
	if (se) {
	    if (!is_edge_picked(se))
		add_edge_to_pick_list(se);
	    else
		remove_edge_from_pick_list(se);
	}
    }

}







/*int pick_node(topview_node * n)
{
    static int closest_dif = 3;
	static char buf[512];
	float a, b;
	a = ABS(n->distorted_x - view->mouse.GLpos.x);
    b = ABS(n->distorted_y - view->mouse.GLpos.y);
    a = (float) pow((a * a + b * b), (float) 0.5);
    if (a < closest_dif) {
	if (!is_node_picked(n)) {
	    if (add_to_pick_list(n)) {
		sprintf(buf,"Clicked node name:%s\n",agnameof(n->Node));
		write_to_console(buf);
		return 1;
	    }
	    return 0;
	} else {
	    if (remove_from_pick_list(n)) {
		return 1;
	    }
	    return 0;
	}
    }
    return 0;

}*/

int is_node_picked(topview_node * n)
{
    int ind = 0;
    int found = 0;
    for (; ind < view->Topview->picked_node_count; ind++) {
	if ((view->Topview->picked_nodes[ind] == n) && (!found))
	    return 1;
    }
    return 0;
}

int remove_from_pick_list(topview_node * n)
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
	deselect_node(n);
	return 1;
    }
    return 0;
}

/*
You can then use sf as you would FILE* for writing, but using sfio functions. For example,

  sfprintf(sf, "<unknown (%d)>", op);

You can write to your heart's content; sfio will take care of memory. 
When you are ready to use the
string, call

  char* ptr = sfstruse(sf);

This adds a '\0'-byte at the end and returns a pointer to the beginning of the buffer. This also resets the write pointer, so you can use sf over again. Note, therefore, that this means you don't want to call sfstruse(sf) twice, as the second time will put a '\0' at the beginning of the buffer.

Once you have ptr set, you can use it to insert the character string into the console window.

To clean up, call

  sfclose (sf);

This codes requires the vmalloc, ast, and sfio libraries, but these are already being loaded because gvpr needs them.
*/

static void print_object(void* obj)
{
    static Sfio_t *sf;
    int ind=0;
    char* val;
    char* str;
    attr_list* l=view->Topview->attributes;
    if (!sf)
		sf= sfstropen();
    if(AGTYPE(obj)==AGNODE)
	sfprintf(sf,"Node Summary\n");
    if(AGTYPE(obj)==AGEDGE)
	sfprintf(sf,"Edge Summary\n");
    sfprintf(sf,"-------------\n");
    sfprintf(sf,"name: %s\n",agnameof(obj));
    for (;ind < l->attr_count;ind ++)
    {
	if(l->attributes[ind]->propagate)
	{
	    val=agget(obj,l->attributes[ind]->name);
	    if (val)
	    {
		sfprintf(sf,"%s:%s\n",l->attributes[ind]->name,val);

	    }
	}
    }
    sfprintf(sf,"-------------\n");
    str=sfstruse(sf);
    append_textview((GtkTextView *)glade_xml_get_widget(xml, "mainconsole"), str, strlen(str));


}

int add_to_pick_list(topview_node * n)
{
    view->Topview->picked_node_count++;
    view->Topview->picked_nodes =
	realloc(view->Topview->picked_nodes,
		sizeof(topview_node *) * view->Topview->picked_node_count);
    view->Topview->picked_nodes[view->Topview->picked_node_count - 1] = n;
    select_node(n);
    print_object((void*) n->Node);	
    return 1;
}







int draw_node_hint_boxes(void)
{
    int ind;
    float del = 0.01;
    float fs = GetOGLDistance(12);
    char *lbl;
    topview_node *n;
    topview_edge *e;
    double dx, dy, dz;
    char buf[512];		/* FIX!!! static buffer */

//    view->widgets->fontset->fonts[view->widgets->fontset->activefont]->fontheight=fs;


    for (ind = 0; ind < view->Topview->picked_node_count; ind++) {
	n = view->Topview->picked_nodes[ind];
	lbl = agget(n->Node, "hint");
	if ((!lbl) || (strlen(lbl) == 0))
	    lbl = agnameof(n->Node);
	dx = n->distorted_x;
	dy = n->distorted_y;
	dz = n->distorted_z + 0.001;


	/*blue font color */
	glColor4f(0, 0, 1, 1);
	glprintfglut(GLUT_BITMAP_HELVETICA_12, dx, dy, dz + del, "[");
	glprintfglut(GLUT_BITMAP_HELVETICA_12, dx,
		     (dy + fs + fs / (GLfloat) 5.0), dz + del, lbl);
//              glprintfglut (GLUT_BITMAP_HELVETICA_12, dx,(dy+fs+fs/(GLfloat)5.0),dz,"aaaaaaaa");
//              ffs=(dy+fs+fs/(GLfloat)5.0)-GetOGLDistance(14)/view->zoom*-1;
//              glprintfglut (GLUT_BITMAP_HELVETICA_12, dx,ffs,dz,"bbbbbbbbbb");

#if UNUSED
	char *buf;
	char *nc;
	buf = malloc(sizeof(char) * 512);
	if (!bf)
	    return;
	if (strlen(bf) > 512)
	    return;
	strcpy(buf, bf);
	nc = buf;

	for (nc; *nc != NULL; nc++) {
	    if (*nc == '\n') {
		int a = glutBitmapWidth(font, buf);
		*nc = NULL;
		glRasterPos3f(xpos, ypos, zpos + 0.001);
		print_bitmap_string(font, buf);
		nc++;
		buf = nc;
		ypos = ypos - 14.00;
	    }
	}
	glRasterPos3f(xpos, ypos, zpos + 0.001);
	print_bitmap_string(font, buf);
#endif

    }
    glColor4f(0, 1, 0, 0.5);
    glLineWidth(2);
/*    glBegin(GL_LINES);
    for (ind = 0; ind < view->Topview->picked_edge_count; ind++) {
	float x1, x2, y1, y2, z1, z2;
	e = view->Topview->picked_edges[ind];
	x1 = e->Node1->distorted_x;
	x2 = e->Node2->distorted_x;
	y1 = e->Node1->distorted_y;
	y2 = e->Node2->distorted_y;
	z1 = e->Node1->distorted_z;
	z2 = e->Node2->distorted_z;

	dx = (x1 + x2) / 2.0;
	dy = (y1 + y2) / 2.0;
	dz = (z1 + z2) / 2.0;


	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);


    }
    glEnd();*/
    glLineWidth(1);
    for (ind = 0; ind < view->Topview->picked_edge_count; ind++) {
	float x1, x2, y1, y2, z1, z2;
	char *s;
	glColor4f(0, 1, 0, 0.5);
	e = view->Topview->picked_edges[ind];
	x1 = e->Node1->distorted_x;
	x2 = e->Node2->distorted_x;
	y1 = e->Node1->distorted_y;
	y2 = e->Node2->distorted_y;
	z1 = e->Node1->distorted_z;
	z2 = e->Node2->distorted_z;
	if ((s = agget(e->Edge, "hint")) && s[0])
	    strcpy(buf, s);
	else {
	    strcpy(buf, agnameof(e->Node1->Node));
	    strcat(buf, " - ");
	    strcat(buf, agnameof(e->Node2->Node));
	}
	dx = (x1 + x2) / 2.0;
	dy = (y1 + y2) / 2.0;
	dz = (z1 + z2) / 2.0;



	glColor4f(0, 0, 1, 1);

	/*blue font color */
	glprintfglut(GLUT_BITMAP_HELVETICA_12, dx, dy, dz + del, "|");
	glprintfglut(GLUT_BITMAP_HELVETICA_12, dx,
		     (dy + fs + fs / (GLfloat) 5.0), dz + del, buf);
    }


    return 1;
}
