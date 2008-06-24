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
#include "assert.h"
#include "hier.h"

static double dist(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
static double dist3d(double x1, double y1, double z1, double x2, double y2,
		     double z2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) +
		(z1 - z2) * (z1 - z2));
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
void fisheye_spherical(double x_focus, double y_focus, double z_focus,
		       topview * t)
{
    int i;
    double distance, distorted_distance, ratio, range;

    range = 0;
    for (i = 1; i < t->Nodecount; i++) {
	if (point_within_sphere_with_coords
	    ((float) x_focus, (float) y_focus, (float) z_focus,
	     (float) view->fmg.R, t->Nodes[i].x, t->Nodes[i].y,
	     t->Nodes[i].z)) {


	    range =
		MAX(range,
		    dist3d(t->Nodes[i].x, t->Nodes[i].y, t->Nodes[i].z,
			   x_focus, y_focus, z_focus));
	}
    }

    for (i = 1; i < t->Nodecount; i++) {


	if (point_within_sphere_with_coords
	    ((float) x_focus, (float) y_focus, (float) z_focus,
	     (float) view->fmg.R, t->Nodes[i].x, t->Nodes[i].y,
	     t->Nodes[i].z)) {
	    distance =
		dist3d(t->Nodes[i].x, t->Nodes[i].y, t->Nodes[i].z,
		       x_focus, y_focus, z_focus);
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

static v_data *makeGraph(topview * tv, int *nedges)
{
    int i;
    int ne = tv->Edgecount;	/* upper bound */
    int nv = tv->Nodecount;
    v_data *graph = N_NEW(nv, v_data);
    int *edges = N_NEW(2 * ne + nv, int);  /* reserve space for self loops */
    float *ewgts = N_NEW(2 * ne + nv, float);
    Agnode_t *np;
    Agedge_t *ep;
    Agraph_t *g = NULL;
    int i_nedges;

    ne = 0;
    for (i = 0; i < nv; i++) {
	graph[i].edges = edges++;	/* reserve space for the self loop */
	graph[i].ewgts = ewgts++;
#ifdef STYLES
	graph[i].styles = NULL;
#endif
	i_nedges = 1;		/* one for the self */

	np = tv->Nodes[i].Node;
	if (!g)
	    g = agraphof(np);
	for (ep = agfstedge(g, np); ep; ep = agnxtedge(g, ep, np)) {
	    Agnode_t *vp;
	    Agnode_t *tp = agtail(ep);
	    Agnode_t *hp = aghead(ep);
	    assert(hp != tp);
	    /* FIX: handle multiedges */
	    vp = (tp == np ? hp : tp);
	    ne++;
	    i_nedges++;
	    *edges++ = OD_TVRef(vp);
	    *ewgts++ = 1;
	}

	graph[i].nedges = i_nedges;
	graph[i].edges[0] = i;
	graph[i].ewgts[0] = 1 - i_nedges;
    }
    ne /= 2;			/* each edge counted twice */
    *nedges = ne;
    return graph;
}


/* To use:
 * double* x_coords; // initial x coordinates
 * double* y_coords; // initial y coordinates
 * focus_t* fs;
 * int ne;
 * v_data* graph = makeGraph (topview*, &ne);
 * hierarchy = makeHier(topview->NodeCount, ne, graph, x_coords, y_coords);
 * freeGraph (graph);
 * fs = initFocus (topview->Nodecount); // create focus set
 */
void prepare_topological_fisheye(topview* t)
{
    double *x_coords = N_NEW(t->Nodecount, double);	// initial x coordinates
    double *y_coords = N_NEW(t->Nodecount, double);	// initial y coordinates
    focus_t *fs;
    int ne;
    int i;
    int closest_fine_node;
    int cur_level = 0;
    Hierarchy *hp;
    ex_vtx_data *gg;
    topview_node *np;

    v_data *graph = makeGraph(t, &ne);

//	t->animate=1;	//turn the animation on
	for (i = 0, np = t->Nodes; i < t->Nodecount; i++, np++) {
	x_coords[i] = np->x;
	y_coords[i] = np->y;
    }
    hp = t->h = 
	makeHier(t->Nodecount, ne, graph, x_coords, y_coords, &(t->parms.hier));
    freeGraph(graph);
    free(x_coords);
    free(y_coords);

	fs = t->fs = initFocus(t->Nodecount);	// create focus set
    gg = hp->geom_graphs[0];

    closest_fine_node = 0; /* first node */
    fs->num_foci = 1;
    fs->foci_nodes[0] = closest_fine_node;
    fs->x_foci[0] = hp->geom_graphs[cur_level][closest_fine_node].x_coord;
    fs->y_foci[0] = hp->geom_graphs[cur_level][closest_fine_node].y_coord;

    view->Topview->parms.repos.width =(int) (view->bdxRight-view->bdxLeft);
    view->Topview->parms.repos.height =(int) (view->bdyTop-view->bdyBottom);
	set_active_levels(hp, fs->foci_nodes, fs->num_foci, &(t->parms.level));
    positionAllItems(hp, fs, &(t->parms.repos));
	refresh_old_values(t);

/* fprintf (stderr, "No. of active nodes = %d\n", count_active_nodes(hp)); */

}

/*
	draws all level 0 nodes and edges, during animation
*/

void drawtopologicalfisheye2(topview * t)
{
    int level, v, i, n;
    Hierarchy *hp = t->h;
	float R,G,B;

	static int debug_mode=1;
	view->Topview->animate=1;

	//draw only nodes and super nodes
	glPointSize(4);
    glBegin(GL_POINTS);

    for (level = 0; level < hp->nlevels; level++)
	{
		for (v = 0; v < hp->nvtxs[level]; v++)
		{
			{
				/* ex_vtx_data *gg = hp->geom_graphs[level]; */
					double x0,y0;
					if(get_temp_coords(t,level,v,&x0,&y0,&R,&G,&B))
					{
						glColor3f((GLfloat)R,(GLfloat)G,(GLfloat)B);
						glVertex3f((GLfloat) x0, (GLfloat) y0, (GLfloat) 0);
					}
			}
		}
	}
	glEnd();




	//draw edges
	glBegin(GL_LINES);
    for (level = 0; level < hp->nlevels; level++)
	{
		for (v = 0; v < hp->nvtxs[level]; v++) 
		{
			ex_vtx_data *gg = hp->geom_graphs[level];
			v_data *g = hp->graphs[level];
				double x0,y0;
				if(get_temp_coords(t,level,v,&x0,&y0,&R,&G,&B)&& ((gg[v].active_level ==level ) || (gg[v].old_active_level ==level)) )
				{
					for (i = 1; i < g[v].nedges; i++) 
					{
						double x, y;
						n = g[v].edges[i];
						if(get_temp_coords(t,level,n,&x,&y,&R,&G,&B))
						{
								if (((x0==0)||(x==0) || (y0==0) || (y==0)) &&(debug_mode))
								{
									/*printf ("(%f,%f)->(%f,%f)\n",x0,y0,x,y);*/
								}
								else
								{
									glColor3f((GLfloat)R,(GLfloat)G,(GLfloat)B);
//									glColor3f((GLfloat) (hp->nlevels - level) /	(GLfloat) hp->nlevels,(GLfloat) level / (GLfloat) hp->nlevels, 0);
									glVertex3f((GLfloat) x0, (GLfloat) y0,(GLfloat) 0);
									glVertex3f((GLfloat) x, (GLfloat) y,(GLfloat) 0);
								}
						}
						else
						{
							//get_temp_coords(t,level,n,&x,&y);
						}
					}
				}
			
		}
    }
    glEnd();

}
int get_temp_coords(topview* t,int level,int v,double* coord_x,double* coord_y,float *R,float *G,float *B)
{
    Hierarchy *hp = t->h;
	ex_vtx_data *gg = hp->geom_graphs[level];
	/* v_data *g = hp->graphs[level]; */
	int OAL,AL;
	OAL=gg[v].old_active_level;
	AL=gg[v].active_level;
	/*if ((OAL > level) && (AL>level))
		return 0;*/
	if ((OAL < level) && (AL<level))
		return 0;

	if (!t->animate)
	{
		if (AL == level)
		{
			*coord_x=(double)gg[v].physical_x_coord;
			*coord_y=(double)gg[v].physical_y_coord;
			return 1;
		}
		else
			return 0;
	}
	else
	{

		double x0,y0,x1,y1;	
		x0=0;
		y0=0;
		x1=0;
		y1=0;

		get_active_frame(t);
		if ((OAL == level) && (AL==level))
		{
			x0=(double)gg[v].old_physical_x_coord;
			y0=(double)gg[v].old_physical_y_coord;
			x1=(double)gg[v].physical_x_coord;
			y1=(double)gg[v].physical_y_coord;
			*G=0;
			*R=1;

		}
		if ((OAL == level) && (AL>level))
		{
			x0=(double)gg[v].old_physical_x_coord;
			y0=(double)gg[v].old_physical_y_coord;
			find_physical_coords(t->h,level,v,&x1,&y1);
			*G=view->active_frame/view->total_frames;
				*R=0;

		}
		if ((OAL > level) && (AL==level))
		{
			find_old_physical_coords(t->h,level,v,&x0,&y0);
			x1=(double)gg[v].physical_x_coord;
			y1=(double)gg[v].physical_y_coord;
			*R=view->active_frame/view->total_frames;
			*G=1/(view->active_frame/view->total_frames+0.0000001);

		}
		if ((OAL > level) && (AL>level))
		{
			find_old_physical_coords(t->h,level,v,&x0,&y0);
			find_physical_coords(t->h,level,v,&x1,&y1);
			*G=1;
			*R=0;

		}

		get_interpolated_coords(x0,y0,x1,y1,view->active_frame,view->total_frames,coord_x,coord_y);
		
		return 1;
	}
	return 0;
}




void drawtopologicalfisheye(topview * t)
{
    int level, v, i, n;
	float R,G,B;    
	Hierarchy *hp = t->h;

	//draw only nodes and super nodes
	glPointSize(5);
    glBegin(GL_POINTS);
    for (level = 0; level < hp->nlevels; level++)
	{
		for (v = 0; v < hp->nvtxs[level]; v++)
		{
		    ex_vtx_data *gg = hp->geom_graphs[level];
		    if (gg[v].active_level == level)	
			{
				double x0,y0;
				t->animate=0;
				get_temp_coords(t,level,v,&x0,&y0,&R,&G,&B);
				glColor3f((GLfloat) (hp->nlevels - level) /
					(GLfloat) hp->nlevels,
					(GLfloat) level / (GLfloat) hp->nlevels, 0);
				glVertex3f((GLfloat) x0, (GLfloat) y0, (GLfloat) 0);
				t->animate=1;
			}
		}
	}
    glEnd();

	//draw edges
	glBegin(GL_LINES);
    for (level = 0; level < hp->nlevels; level++)
	{
		for (v = 0; v < hp->nvtxs[level]; v++) 
		{
			ex_vtx_data *gg = hp->geom_graphs[level];
			v_data *g = hp->graphs[level];
			if (gg[v].active_level == level) 
			{
				double x0,y0;
				get_temp_coords(t,level,v,&x0,&y0,&R,&G,&B);
				for (i = 1; i < g[v].nedges; i++) 
				{
					double x, y;
					n = g[v].edges[i];
					glColor3f((GLfloat) (hp->nlevels - level) /
						(GLfloat) hp->nlevels,
						(GLfloat) level / (GLfloat) hp->nlevels, 0);
				    if (gg[n].active_level == level)
					{
						if (v < n)
						{
							get_temp_coords(t,level,n,&x,&y,&R,&G,&B);
							glVertex3f((GLfloat) x0, (GLfloat) y0,(GLfloat) 0);
							glVertex3f((GLfloat) x, (GLfloat) y,(GLfloat) 0);
						}
					}
					else if (gg[n].active_level > level) 
					{
						find_physical_coords(hp, level, n, &x, &y);
						glVertex3f((GLfloat) x0, (GLfloat) y0,(GLfloat) 0);
						glVertex3f((GLfloat) x, (GLfloat) y, (GLfloat) 0);
					}
				}
			}
		}
    }
    glEnd();
}
/*  In loop,
 *  update fs.
 *    For example, if user clicks mouse at (p.x,p.y) to pick a single new focus,
 *      int closest_fine_node;
 *      find_closest_active_node(hierarchy, p.x, p.y, &closest_fine_node);
 *      fs->num_foci = 1;
 *      fs->foci_nodes[0] = closest_fine_node;
 *      fs->x_foci[0] = hierarchy->geom_graphs[cur_level][closest_fine_node].x_coord;
 *      fs->y_foci[0] = hierarchy->geom_graphs[cur_level][closest_fine_node].y_coord;
 *  set_active_levels(hierarchy, fs->foci_nodes, fs->num_foci);
 *  positionAllItems(hierarchy, fs, parms)
 */
void changetopfishfocus(topview * t, float *x, float *y,
				   float *z, int num_foci)
{
	focus_t *fs = t->fs;
    int i;
    int closest_fine_node;
    int cur_level = 0;

	Hierarchy *hp = t->h;
	refresh_old_values(t);
	fs->num_foci = num_foci;
    for (i = 0; i < num_foci; i++) {
	find_closest_active_node(hp, x[i], y[i], &closest_fine_node);
	fs->foci_nodes[i] = closest_fine_node;
	fs->x_foci[i] =
	    hp->geom_graphs[cur_level][closest_fine_node].x_coord;
	fs->y_foci[i] =
	    hp->geom_graphs[cur_level][closest_fine_node].y_coord;
    }

    set_active_levels(hp, fs->foci_nodes, fs->num_foci, &(t->parms.level));
    view->Topview->parms.repos.width =(int) (view->bdxRight-view->bdxLeft);
    view->Topview->parms.repos.height =(int) (view->bdyTop-view->bdyBottom);
	positionAllItems(hp, fs, &(t->parms.repos));
	t->animate=1;

	if(t->animate)
	{
		view->active_frame=0;
		g_timer_start(view->timer); 
	}

}
void refresh_old_values(topview* t)
{
    int level, v;
    Hierarchy *hp = t->h;
	t->animate=0;
    for (level = 0; level < hp->nlevels; level++)
	{
		for (v = 0; v < hp->nvtxs[level]; v++)
		{
		    ex_vtx_data *gg = hp->geom_graphs[level];
		    /* v_data *g = hp->graphs[level]; */
			/* double x0,y0; */
			gg[v].old_physical_x_coord=gg[v].physical_x_coord;
			gg[v].old_physical_y_coord=gg[v].physical_y_coord;
			gg[v].old_active_level=gg[v].active_level;
		}
	}

}
void get_interpolated_coords(double x0,double y0,double x1,double y1,int fr,int total_fr, double* x,double* y)
{
	*x=x0+(x1-x0)/(double)total_fr*(double)(fr+1);
	*y=	y0+(y1-y0)/(double)total_fr*(double)(fr+1);
}
int get_active_frame(topview* t)
{
	gulong microseconds;
	gdouble seconds;
	int fr;
	seconds=g_timer_elapsed(view->timer,&microseconds);
	fr=(int)(seconds/((double)view->frame_length/(double)1000));
	if (fr<view->total_frames)
	{

		if (fr==view->active_frame)
			return 0;
		else
		{
			view->active_frame=fr;
			return 1;
		}
	}
	else
	{
		g_timer_stop(view->timer); 
//		view->Topview->animate=0;
		return 0;
	}

}



