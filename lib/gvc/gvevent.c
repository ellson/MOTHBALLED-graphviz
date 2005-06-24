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

/*
#include "const.h"
#include "types.h"
#include "macros.h"
*/

#include "render.h"

#include "gvc.h"

#define PANFACTOR 10
#define ZOOMFACTOR 1.1
#define EPSILON .0001

void gvevent_refresh(GVJ_t * job)
{
    emit_graph(job, job->g);
}

/* recursively find innermost cluster containing the point */
static graph_t *gvevent_find_cluster(graph_t *g, boxf b)
{
    int i;
    graph_t *sg;
    boxf bb;

    for (i = 1; i <= GD_n_cluster(g); i++) {
	sg = gvevent_find_cluster(GD_clust(g)[i], b);
	if (sg)
	    return(sg);
    }
    B2BF(GD_bb(g), bb);
    if (OVERLAP(b, bb))
	return g;
    return NULL;
}

static void * gvevent_find_obj(graph_t *g, boxf b)
{
    graph_t *sg;
    node_t *n;
    edge_t *e;

    /* edges might overlap nodes, so search them first */
    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    if (overlap_edge(e, b))
	        return (void *)e;
    /* search graph backwards to get topmost node, in case of overlap */
    for (n = aglstnode(g); n; n = agprvnode(g, n))
	if (overlap_node(n, b))
	    return (void *)n;
    /* search for innermost cluster */
    sg = gvevent_find_cluster(g, b);
    if (sg)
	return (void *)sg;

    /* otherwise - we're always in the graph */
    return (void *)g;
}

static void gvevent_leave_obj(GVJ_t * job)
{
    void *obj = job->current_obj;

    if (obj) {
        switch (agobjkind(obj)) {
        case AGGRAPH:
	    GD_active((graph_t*)obj) = FALSE;
	    break;
        case AGNODE:
	    ND_active((node_t*)obj) = FALSE;
	    break;
        case AGEDGE:
	    ED_active((edge_t*)obj) = FALSE;
	    break;
        }
    }
}

static void gvevent_enter_obj(GVJ_t * job)
{
    void *obj = job->current_obj;

    if (obj) {
        switch (agobjkind(obj)) {
        case AGGRAPH:
	    GD_active((graph_t*)obj) = TRUE;
	    break;
        case AGNODE:
	    ND_active((node_t*)obj) = TRUE;
	    break;
        case AGEDGE:
	    ED_active((edge_t*)obj) = TRUE;
	    break;
        }
    }
}

/* CLOSEENOUGH is in window units - probably should be a feature... */
#define CLOSEENOUGH 1

static void gvevent_find_current_obj(GVJ_t * job, pointf pointer)
{
    void *obj;
    pointf p;
    boxf b;
    double closeenough;

    /* convert window point to graph coordinates */
    if (job->rotation) {
	p.x = job->focus.y - (pointer.y - job->height / 2.) / job->compscale.x;
	p.y = job->focus.x + (pointer.x - job->width / 2.) / job->compscale.y;
    }
    else {
	p.x = job->focus.x + (pointer.x - job->width / 2.) / job->compscale.x;
	p.y = job->focus.y + (pointer.y - job->height / 2.) / job->compscale.y;
    }
    closeenough = CLOSEENOUGH / job->compscale.x;

    b.UR.x = p.x + closeenough;
    b.UR.y = p.y + closeenough;
    b.LL.x = p.x - closeenough;
    b.LL.y = p.y - closeenough;

    obj = gvevent_find_obj(job->g, b);
    if (obj != job->current_obj) {
	gvevent_leave_obj(job);
	job->current_obj = obj;
	gvevent_enter_obj(job);
	job->needs_refresh = 1;
    }
}

void gvevent_button_press(GVJ_t * job, int button, pointf pointer)
{
    switch (button) {
    case 1: /* select / create in edit mode */
    case 3: /* insert node or edge */
	gvevent_find_current_obj(job, pointer);
        /* fall through */
    case 2: /* pan */
        job->click = 1;
	job->active = button;
	job->needs_refresh = 1;
	break;
    case 4:
	/* scrollwheel zoom in at current mouse x,y */
	job->fit_mode = 0;
	job->focus.x +=  (pointer.x - job->width / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->focus.y += -(pointer.y - job->height / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->zoom *= ZOOMFACTOR;
	job->needs_refresh = 1;
	break;
    case 5: /* scrollwheel zoom out at current mouse x,y */
	job->fit_mode = 0;
	job->zoom /= ZOOMFACTOR;
	job->focus.x -=  (pointer.x - job->width / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->focus.y -= -(pointer.y - job->height / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->needs_refresh = 1;
	break;
    }
    job->oldpointer = pointer;
}

void gvevent_motion(GVJ_t * job, pointf pointer)
{
    double dx = pointer.x - job->oldpointer.x;
    double dy = pointer.y - job->oldpointer.y;

    if (abs(dx) < EPSILON && abs(dy) < EPSILON)  /* ignore motion events with no motion */
	return;

    switch (job->active) {
    case 0: /* drag with no button - */
	gvevent_find_current_obj(job, pointer);
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
    job->oldpointer = pointer;
}

void gvevent_button_release(GVJ_t *job, int button, pointf pointer)
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
