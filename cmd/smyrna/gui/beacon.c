#include "beacon.h"
#include "viewport.h"
#include "selection.h"
#include "gltemplate.h"
#include "toolboxcallbacks.h"
typedef struct tagXYZ
{
    float X, Y, Z;
}
XYZ;
void pick_node_from_coords(float x,float y,float z)
{
	topview_node* n;
	topview_node* sn;	/*selected node , closest node*/
	topview_edge* e;
	topview_edge* se;/*selected edge , closest one*/
	GLfloat closest_dif = 1000;
	GLfloat closest_dif2 = 1000;
	static char buf[512];
	float a, b,c;
	int ind;
	sn=(topview_node*)0;
	se=(topview_edge*)0;

	for (ind = 0; ind < view->Topview->Nodecount; ind++) 
	{
		n = &view->Topview->Nodes[ind];
		a = ABS(n->distorted_x - view->mouse.GLX);
		b = ABS(n->distorted_y - view->mouse.GLY);
		c = ABS(n->distorted_z - view->mouse.GLZ);
		a = (float) pow((a * a + b * b+ c*c), (float) 0.5);
		if (a < closest_dif)
		{
			sn=n;
			closest_dif=a;
		}
	}

	for (ind = 0; ind < view->Topview->Edgecount; ind++) 
	{
		XYZ p1,p2,p3;
		e = &view->Topview->Edges[ind];

		p1.X=e->Node1->distorted_x;
		p1.Y=e->Node1->distorted_y;
		p1.Z=e->Node1->distorted_z;

		p2.X=e->Node2->distorted_x;
		p2.Y=e->Node2->distorted_y;
		p2.Z=e->Node2->distorted_z;

		p3.X=view->mouse.GLX;
		p3.Y=view->mouse.GLY;
		p3.Z=view->mouse.GLZ;
		if(DistancePointLine( &p3, &p1, &p2, &a))
		{
			if (a < closest_dif2 )
			{
				se=e;
				closest_dif2=a;
			}
		}
	}

	if (closest_dif < closest_dif2 * 10)
	{
		if (sn)
		{
			if (!is_node_picked(sn)) 
				add_to_pick_list(sn);
			else
				remove_from_pick_list(sn);
		}
	}
	else
	{
		if (se)
		{
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
	a = ABS(n->distorted_x - view->mouse.GLX);
    b = ABS(n->distorted_y - view->mouse.GLY);
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
	return 1;
    }
    return 0;
}

int add_to_pick_list(topview_node * n)
{
    view->Topview->picked_node_count++;
    view->Topview->picked_nodes =realloc(view->Topview->picked_nodes,sizeof(topview_node *) * view->Topview->picked_node_count);
    view->Topview->picked_nodes[view->Topview->picked_node_count - 1] = n;
    return 1;
}

int is_edge_picked(topview_edge * e)
{
    int ind = 0;
    int found = 0;
    for (; ind < view->Topview->picked_edge_count; ind++) {
	if ((view->Topview->picked_edges[ind] == e) && (!found))
	    return 1;
    }
    return 0;
}

int remove_edge_from_pick_list(topview_edge * e)
{
    int ind = 0;
    int found = 0;
	for (; ind < view->Topview->picked_edge_count; ind++) 
	{
		if ((view->Topview->picked_edges[ind] == e) && (!found))
			found = 1;
		if ((found) && (ind < (view->Topview->picked_edge_count - 1))) 
		{
			view->Topview->picked_edges[ind] = view->Topview->picked_edges[ind + 1];
		}
    }
    if (found) 
	{
		view->Topview->picked_edge_count--;
		view->Topview->picked_edges =realloc(view->Topview->picked_edges,sizeof(topview_edge *) *view->Topview->picked_edge_count);
		return 1;
    }
    return 0;
}

int add_edge_to_pick_list(topview_edge * e)
{
    view->Topview->picked_edge_count++;
    view->Topview->picked_edges =realloc(view->Topview->picked_edges,sizeof(topview_edge *) * view->Topview->picked_edge_count);
    view->Topview->picked_edges[view->Topview->picked_edge_count - 1] = e;
    return 1;
}







int draw_node_hint_boxes(void)
{
    int ind;
    float fs = GetOGLDistance(12);
    char* lbl;
    topview_node* n;
    topview_edge* e;
    double dx, dy, dz;
	char buf[512];

    view->widgets->fontset->fonts[view->widgets->fontset->activefont]->fontheight=fs;


	for (ind = 0; ind < view->Topview->picked_node_count; ind++) 
	{
		n = view->Topview->picked_nodes[ind];
		lbl = agnameof(n->Node);
		dx = n->distorted_x;
		dy = n->distorted_y;
		dz = n->distorted_z;


		/*blue font color*/
		glColor4f(0, 0, 1, 1);
		glprintfglut (GLUT_BITMAP_HELVETICA_12, dx,dy,dz,"[");
		glprintfglut (GLUT_BITMAP_HELVETICA_12, dx,(dy+fs+fs/(GLfloat)5.0),dz,lbl);
    }
	glLineWidth(5);
	glColor4f(0, 1, 0, 0.5);
	glBegin(GL_LINES);
	for (ind = 0; ind < view->Topview->picked_edge_count; ind++) 
	{
		float x1,x2,x3,y1,y2,y3,z1,z2,z3l;
		e = view->Topview->picked_edges[ind];
			x1=e->Node1->distorted_x;
			x2=e->Node2->distorted_x;
			y1=e->Node1->distorted_y;
			y2=e->Node2->distorted_y;
			z1=e->Node1->distorted_z;
			z2=e->Node2->distorted_z;

		dx = (x1+x2)/2.0;
		dy = (y1+y2)/2.0;
		dz = (z1+z2)/2.0;


		glVertex3f(x1,y1,z1);
		glVertex3f(x2,y2,z2);


    }
	glEnd();
	glLineWidth(0);

	for (ind = 0; ind < view->Topview->picked_edge_count; ind++) 
	{
		float x1,x2,x3,y1,y2,y3,z1,z2,z3l;
		buf[0]=(char*)0;
		glColor4f(0, 1, 0, 0.5);
		e = view->Topview->picked_edges[ind];
			x1=e->Node1->distorted_x;
			x2=e->Node2->distorted_x;
			y1=e->Node1->distorted_y;
			y2=e->Node2->distorted_y;
			z1=e->Node1->distorted_z;
			z2=e->Node2->distorted_z;

			strcat(buf,agnameof(e->Node1->Node));
		strcat(buf," - ");
		strcat(buf,agnameof(e->Node2->Node));

		dx = (x1+x2)/2.0;
		dy = (y1+y2)/2.0;
		dz = (z1+z2)/2.0;



		glColor4f(0, 0, 1, 1);

		/*blue font color*/
		glprintfglut (GLUT_BITMAP_HELVETICA_12, dx,dy,dz,"|");
		glprintfglut (GLUT_BITMAP_HELVETICA_12, dx,(dy+fs+fs/(GLfloat)5.0),dz,buf);
    }


    return 1;
}

