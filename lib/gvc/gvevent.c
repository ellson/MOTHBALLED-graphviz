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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "const.h"
#include "types.h"
#include "macros.h"

#include "render.h"

#include "gvc.h"

#define PANFACTOR 10
#define ZOOMFACTOR 1.1
#define EPSILON .0001

void gvevent_refresh(GVJ_t * job)
{
    emit_graph(job, job->g);
}

static boolean inside_node_bb(node_t *n, pointf P)
{
    boxf bb;

    bb.UR.x = ND_coord_i(n).x + ND_rw_i(n);
    bb.UR.y = ND_coord_i(n).y + ND_ht_i(n) / 2.;
    bb.LL.x = ND_coord_i(n).x - ND_lw_i(n);
    bb.LL.y = ND_coord_i(n).y - ND_ht_i(n) / 2.;
    return (INSIDE(P, bb));
}

static boolean inside_node(node_t *n, pointf p)
{
    inside_t ictxt;

    ictxt.s.n = n;
    ictxt.s.bp = NULL;

//    return ND_shape(n)->fns->insidefn(&ictxt, p);
    return TRUE;
}

static boolean inside_edge_bb(edge_t *e, pointf P)
{
    int i, j, k;
    bezier bz;
    boxf BB;
    box bb;
    
    for (i = 0; i < ED_spl(e)->size; i++) {
	bz = ED_spl(e)->list[i];
	for (j = 0; j < bz.size -1; j += 3) {
	    /* compute a bb for the bezier segment */
	    bb.LL = bb.UR = bz.list[j];
	    for (k = j+1; k < j+4; k++) {
	        bb.LL.x = MIN(bb.LL.x,bz.list[k].x);
	        bb.LL.y = MIN(bb.LL.y,bz.list[k].y);
	        bb.UR.x = MAX(bb.UR.x,bz.list[k].x);
	        bb.UR.y = MAX(bb.UR.y,bz.list[k].y);
	    }
	    B2BF(bb,BB);
	    if (INSIDE(P,BB))
		return TRUE;
	}
    }
    return FALSE;
}

static boolean inside_edge(edge_t *n, pointf p)
{
    return TRUE;
}

static graph_t *gvevent_find_cluster(graph_t *g, pointf P)
{
    int i;
    graph_t *sg;
    boxf BB;

    for (i = 1; i <= GD_n_cluster(g); i++) {
	sg = gvevent_find_cluster(GD_clust(g)[i], P);
	if (sg)
	    return(sg);
    }
    B2BF(GD_bb(g), BB);
    if (INSIDE(P, BB))
	return g;
    return NULL;
}

static void * gvevent_find_obj(GVJ_t * job, pointf p)
{
    graph_t *sg, *g = job->g;
    node_t *n;
    edge_t *e;
    pointf P; /* point in graph coordinates */

    /* convert point to graph coordinates */
    if (job->rotation) {
	P.x = job->focus.y - (p.y - job->height / 2.) / job->compscale.x;
	P.y = (p.x - job->width / 2.) / job->compscale.y + job->focus.x;
    }
    else {
	P.x = (p.x - job->width / 2.) / job->compscale.x + job->focus.x;
	P.y = (p.y - job->height / 2.) / job->compscale.y + job->focus.y;
    }

    /* search graph backwards to get topmost node, in case of overlap */
    for (n = aglstnode(g); n; n = agprvnode(g, n)) {
	if (inside_node_bb(n, P) && inside_node(n, p))
	    return (void *)n;
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (inside_edge_bb(e, P) && inside_edge(e, p))
	        return (void *)e;
	}
    }
    /* no node or edge found, so
    	search for innermost cluster */
    sg = gvevent_find_cluster(g, P);
    if (sg)
	return (void *)sg;

    /* otherwise - we're always in the graph */
    return (void *)g;
}

static void gvevent_find_current_obj(GVJ_t * job, double x, double y)
{
    void *obj;
    pointf p;

    p.x = x;
    p.y = y;
    obj = gvevent_find_obj(job, p);
    if (obj != job->current_obj) {
	job->current_obj = obj;
fprintf(stderr,"obj=%x kind=%d\n",obj,agobjkind(obj));
    }
}

void gvevent_button_press(GVJ_t * job, int button, double x, double y)
{
    switch (button) {
    case 1: /* select / create in edit mode */
    case 3: /* insert node or edge */
	gvevent_find_current_obj(job, x, y);
        /* fall through */
    case 2: /* pan */
        job->click = 1;
	job->active = button;
	job->needs_refresh = 1;
	break;
    case 4:
	/* scrollwheel zoom in at current mouse x,y */
	job->fit_mode = 0;
	job->focus.x +=  (x - job->width / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->focus.y += -(y - job->height / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->zoom *= ZOOMFACTOR;
	job->needs_refresh = 1;
	break;
    case 5: /* scrollwheel zoom out at current mouse x,y */
	job->fit_mode = 0;
	job->zoom /= ZOOMFACTOR;
	job->focus.x -=  (x - job->width / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->focus.y -= -(y - job->height / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->needs_refresh = 1;
	break;
    }
    job->oldx = x;
    job->oldy = y;
}

void gvevent_motion(GVJ_t * job, double x, double y)
{
    double dx = x - job->oldx;
    double dy = y - job->oldy;

    if (abs(dx) < EPSILON && abs(dy) < EPSILON)  /* ignore motion events with no motion */
	return;

    switch (job->active) {
    case 0: /* drag with no button - */
	gvevent_find_current_obj(job, x, y);
	break;
    case 1: /* drag with button 1 - drag object */
	/* FIXME - to be implemented */
	break;
    case 2: /* drag with button 2 - pan graph */
	job->focus.x -=  dx / job->zoom;
	job->focus.y -= -dy / job->zoom;
	job->needs_refresh = 1;
	break;
    case 3: /* drag with button 3 - drag inserted node or uncompleted edge */
	break;
    }
    job->oldx = x;
    job->oldy = y;
}

void gvevent_button_release(GVJ_t *job, int button, double x, double y)
{
    job->click = 0;
    job->active = 0;
}

static int quit_cb(GVJ_t * job)
{
    return 1;
}

static int left_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->focus.x += PANFACTOR / job->zoom;
    job->needs_refresh = 1;
    return 0;
}

static int right_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->focus.x -= PANFACTOR / job->zoom;
    job->needs_refresh = 1;
    return 0;
}

static int up_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->focus.y += -(PANFACTOR / job->zoom);
    job->needs_refresh = 1;
    return 0;
}

static int down_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->focus.y -= -(PANFACTOR / job->zoom);
    job->needs_refresh = 1;
    return 0;
}

static int zoom_in_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->zoom *= ZOOMFACTOR;
    job->needs_refresh = 1;
    return 0;
}

static int zoom_out_cb(GVJ_t * job)
{
    job->fit_mode = 0;
    job->zoom /= ZOOMFACTOR;
    job->needs_refresh = 1;
    return 0;
}

static int toggle_fit_cb(GVJ_t * job)
{
    job->fit_mode = !job->fit_mode;
    if (job->fit_mode) {
	int dflt_width, dflt_height;
	dflt_width = job->width;
	dflt_height = job->height;
	job->zoom =
	    MIN((double) job->width / (double) dflt_width,
		(double) job->height / (double) dflt_height);
	job->focus.x = 0.0;
	job->focus.y = 0.0;
	job->needs_refresh = 1;
    }
    return 0;
}

gvevent_key_binding_t gvevent_key_binding[] = {
    {"Q", quit_cb},
    {"Left", left_cb},
    {"KP_Left", left_cb},
    {"Right", right_cb},
    {"KP_Right", right_cb},
    {"Up", up_cb},
    {"KP_Up", up_cb},
    {"Down", down_cb},
    {"KP_Down", down_cb},
    {"plus", zoom_in_cb},
    {"KP_Add", zoom_in_cb},
    {"minus", zoom_out_cb},
    {"KP_Subtract", zoom_out_cb},
    {"F", toggle_fit_cb},
};

int gvevent_key_binding_size = ARRAY_SIZE(gvevent_key_binding);
