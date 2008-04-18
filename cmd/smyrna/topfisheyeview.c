#include "topfisheyeview.h"

#include "glTexFont.h"
#include "glTexFontTGA.h"
#include "glTexFontDefs.h"
#include "glTexFontInclude.h"
#include "math.h"
#include "memory.h"
#include "viewport.h"
#include "viewportcamera.h"
#include "draw.h"
#include "selection.h"
#include "hier.h"

static double dist(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
static double dist3d(double x1, double y1,double z1, double x2, double y2,double z2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)+(z1 - z2) * (z1 - z2));
}


static double G(double x)
{
    // distortion function for fisheye display
    return (view->fmg.fisheye_distortion_fac +
	    1) * x / (view->fmg.fisheye_distortion_fac * x + 1);
}

void fisheye_polar(double x_focus, double y_focus, topview * t)
{
    int i;
    double distance, distorted_distance, ratio, range;

    range = 0;
    for (i = 1; i < t->Nodecount; i++) {
	if (point_within_ellips_with_coords
	    ((float) x_focus, (float) y_focus, (float) view->fmg.R,
	     (float) view->fmg.R, t->Nodes[i].x, t->Nodes[i].y)) {
	    range =
		MAX(range,
		    dist(t->Nodes[i].x, t->Nodes[i].y, x_focus, y_focus));
	}
    }

    for (i = 1; i < t->Nodecount; i++) {

	if (point_within_ellips_with_coords
	    ((float) x_focus, (float) y_focus, (float) view->fmg.R,
	     (float) view->fmg.R, t->Nodes[i].x, t->Nodes[i].y)) {
	    distance =
		dist(t->Nodes[i].x, t->Nodes[i].y, x_focus, y_focus);
	    distorted_distance = G(distance / range) * range;
	    if (distance != 0) {
		ratio = distorted_distance / distance;
	    } else {
		ratio = 0;
	    }
	    t->Nodes[i].distorted_x =
		(float) x_focus + (t->Nodes[i].x -
				   (float) x_focus) * (float) ratio;
	    t->Nodes[i].distorted_y =
		(float) y_focus + (t->Nodes[i].y -
				   (float) y_focus) * (float) ratio;
	    t->Nodes[i].zoom_factor =
		(float) 1 *(float) distorted_distance / (float) distance;
	} else {
	    t->Nodes[i].distorted_x = t->Nodes[i].x;
	    t->Nodes[i].distorted_y = t->Nodes[i].y;
	    t->Nodes[i].zoom_factor = 1;
	}
    }
}
void fisheye_spherical(double x_focus, double y_focus,double z_focus, topview * t)
{
    int i;
    double distance, distorted_distance, ratio, range;

    range = 0;
    for (i = 1; i < t->Nodecount; i++) {
	if (	point_within_sphere_with_coords((float) x_focus, (float) y_focus, (float)z_focus, (float) view->fmg.R
								,t->Nodes[i].x, t->Nodes[i].y,t->Nodes[i].z))
	{
		
		
	    range =
		MAX(range,
		    dist3d(t->Nodes[i].x, t->Nodes[i].y,t->Nodes[i].z, x_focus, y_focus,z_focus));
	}
    }

    for (i = 1; i < t->Nodecount; i++) {


	if (
			point_within_sphere_with_coords((float) x_focus, (float) y_focus, (float)z_focus, (float) view->fmg.R
								,t->Nodes[i].x, t->Nodes[i].y,t->Nodes[i].z))
	{
	    distance =
		dist3d(t->Nodes[i].x, t->Nodes[i].y,t->Nodes[i].z, x_focus, y_focus,z_focus);
	    distorted_distance = G(distance / range) * range;
	    if (distance != 0) {
		ratio = distorted_distance / distance;
	    } else {
		ratio = 0;
	    }
	    t->Nodes[i].distorted_x =
		(float) x_focus + (t->Nodes[i].x -
				   (float) x_focus) * (float) ratio;
	    t->Nodes[i].distorted_y =
		(float) y_focus + (t->Nodes[i].y -
				   (float) y_focus) * (float) ratio;
	    t->Nodes[i].distorted_z =
		(float) z_focus + (t->Nodes[i].z -
				   (float) z_focus) * (float) ratio;
	    t->Nodes[i].zoom_factor =
		(float) 1 *(float) distorted_distance / (float) distance;
	} else {
	    t->Nodes[i].distorted_x = t->Nodes[i].x;
	    t->Nodes[i].distorted_y = t->Nodes[i].y;
	    t->Nodes[i].distorted_z = t->Nodes[i].z;
	    t->Nodes[i].zoom_factor = 1;
	}
    }
}




void prepare_topological_fisheye(topview * t)
{
/* To use:
  double* x_coords; // initial x coordinates
  double* y_coords; // initial y coordinates
  focus_t* fs;
  int ne;
  vtx_data* graph = makeGraph (topview*, &ne);
  hierarchy = makeHier(topview->NodeCount, ne, graph, x_coords, y_coords);
  freeGraph (graph);
  fs = initFocus (topview->Nodecount); // create focus set

  In loop, 
    update fs.
      For example, if user clicks mouse at (p.x,p.y) to pick a single new focus,
        int closest_fine_node;
        find_closest_active_node(hierarchy, p.x, p.y, &closest_fine_node);
        fs->num_foci = 1;
        fs->foci_nodes[0] = closest_fine_node;
        fs->x_foci[0] = hierarchy->geom_graphs[cur_level][closest_fine_node].x_coord; 
        fs->y_foci[0] = hierarchy->geom_graphs[cur_level][closest_fine_node].y_coord;
      
    set_active_levels(hierarchy, fs->foci_nodes, fs->num_foci);
    positionAllItems(hierarchy, fs, parms)

  When done:
    release (hierarchy);
*/
    double *x_coords = N_NEW(t->Nodecount,double);  // initial x coordinates
    double *y_coords = N_NEW(t->Nodecount,double);  // initial y coordinates
    focus_t *fs;
    int ne;
    int i;
    int closest_fine_node;
    int   cur_level = 0;
    hierparms_t parms;
    Hierarchy* hp;
    ex_vtx_data* gg;

    topview_node* np;
    vtx_data *graph = makeGraph(t, &ne);

    for (i = 0, np = t->Nodes; i < t->Nodecount; i++, np++) {
	x_coords[i] = np->x; 
	y_coords[i] = np->y; 
	}
    hp = t->h = makeHier(t->Nodecount, ne, graph, x_coords, y_coords);
    freeGraph(graph);
    free (x_coords);
    free (y_coords);
    fs = initFocus(t->Nodecount);	// create focus set
    gg = hp->geom_graphs[0];

/*
    find_closest_active_node(hp, 465.966318,438.649967, &closest_fine_node);
*/
    closest_fine_node = 0;
/* fprintf (stderr, "Closest node [%d] %s\n", closest_fine_node, agnameof(t->Nodes[closest_fine_node].Node)); */
    fs->num_foci = 1;
    fs->foci_nodes[0] = closest_fine_node;
    fs->x_foci[0] =
	hp->geom_graphs[cur_level][closest_fine_node].x_coord;
    fs->y_foci[0] =
	hp->geom_graphs[cur_level][closest_fine_node].y_coord;

    set_active_levels(hp, fs->foci_nodes, fs->num_foci);

    parms.rescale = NoRescale;
/*	parms.width=view->bdxRight-view->bdxLeft;
	parms.height=view->bdyTop-view->bdyBottom;
	parms.margin=0;
	parms.graphSize=100;*/
    positionAllItems(hp, fs, &parms);
/* fprintf (stderr, "No. of active nodes = %d\n", count_active_nodes(hp)); */
}

void drawtopologicalfisheye(topview* t)
{
    int level, v, i, n;
    Hierarchy* hp = t->h;

	glPointSize(3);
	glBegin(GL_POINTS);
    for (level=0;level < hp->nlevels;level++) {
	for (v=0;v < hp->nvtxs[level]; v++) {
	    ex_vtx_data* gg = hp->geom_graphs[level];
	    vtx_data* g = hp->graphs[level];
	    if(gg[v].active_level==level) {
		double x0 = gg[v].physical_x_coord;
		double y0 = gg[v].physical_y_coord;
		glColor3f((GLfloat)(hp->nlevels-level)/(GLfloat)hp->nlevels,(GLfloat)level/(GLfloat)hp->nlevels,0);
		glVertex3f((GLfloat)x0,(GLfloat)y0,(GLfloat)0);
	    }
	}
    }
    glEnd();

	
	
	
	glBegin(GL_LINES);
    for (level=0;level < hp->nlevels;level++) {
	for (v=0;v < hp->nvtxs[level]; v++) {
	    ex_vtx_data* gg = hp->geom_graphs[level];
	    vtx_data* g = hp->graphs[level];
	    if(gg[v].active_level==level) {
		double x0 = gg[v].physical_x_coord;
		double y0 = gg[v].physical_y_coord;

		for (i=1;i < g[v].nedges;i++) {
		    double x,y;
			n = g[v].edges[i];
			glColor3f((GLfloat)(hp->nlevels-level)/(GLfloat)hp->nlevels,(GLfloat)level/(GLfloat)hp->nlevels,0);
			if (gg[n].active_level == level) {
			if (v < n) {
			    x = gg[n].physical_x_coord;
			    y = gg[n].physical_y_coord;
			    glVertex3f((GLfloat)x0,(GLfloat)y0,(GLfloat)0);
			    glVertex3f((GLfloat)x,(GLfloat)y,(GLfloat)0);
			}
			}
		    else if (gg[n].active_level > level) {
			find_physical_coords(hp, level, n, &x, &y);
			glVertex3f((GLfloat)x0,(GLfloat)y0,(GLfloat)0);
			glVertex3f((GLfloat)x,(GLfloat)y,(GLfloat)0);
		    }
		}
	    }
	}
    }
    glEnd();
}
void changetopologicalfisheyefocus(topview* t,float* x,float* y,float* z,int num_foci)
{
/*  In loop,
    update fs.
      For example, if user clicks mouse at (p.x,p.y) to pick a single new focus,
        int closest_fine_node;
        find_closest_active_node(hierarchy, p.x, p.y, &closest_fine_node);
        fs->num_foci = 1;
        fs->foci_nodes[0] = closest_fine_node;
        fs->x_foci[0] = 
hierarchy->geom_graphs[cur_level][closest_fine_node].x_coord;
        fs->y_foci[0] = 
hierarchy->geom_graphs[cur_level][closest_fine_node].y_coord;

    set_active_levels(hierarchy, fs->foci_nodes, fs->num_foci);
    positionAllItems(hierarchy, fs, parms)*/

	focus_t *fs;
    int ne;
    int i;
    int closest_fine_node;
    int   cur_level = 0;
    hierparms_t parms;
    Hierarchy* hp;

//    ex_vtx_data* gg;

    topview_node* np;
	printf ("c(%f,%f)  z:%f\n",x[0],y[0],view->zoom);

	hp = t->h;
    fs = initFocus(t->Nodecount);	// create focus set
//    gg = hp->geom_graphs[0];


	fs->num_foci = num_foci;
	for (i=0;i < num_foci;i++)
	{
	    find_closest_active_node(hp, x[i],y[i], &closest_fine_node);
		fs->foci_nodes[i] = closest_fine_node;
		fs->x_foci[i] =
		hp->geom_graphs[cur_level][closest_fine_node].x_coord;
		fs->y_foci[i] =
		hp->geom_graphs[cur_level][closest_fine_node].y_coord;
	}


	
	set_active_levels(hp, fs->foci_nodes, fs->num_foci);
    parms.rescale = NoRescale;
    positionAllItems(hp, fs, &parms);
}
