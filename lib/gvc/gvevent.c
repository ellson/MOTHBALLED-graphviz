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

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

typedef int (*key_callback_t) (gvrender_job_t * job);

typedef struct key_binding {
    char *key;
    key_callback_t callback;
} key_binding_t;

/* callbacks */
static int quit_cb(gvrender_job_t * job);
static int left_cb(gvrender_job_t * job);
static int right_cb(gvrender_job_t * job);
static int up_cb(gvrender_job_t * job);
static int down_cb(gvrender_job_t * job);
static int zoom_in_cb(gvrender_job_t * job);
static int zoom_out_cb(gvrender_job_t * job);
static int toggle_fit_cb(gvrender_job_t * job);

static key_binding_t key_binding[] = {
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

void gvevent_refresh(gvrender_job_t * job)
{
    emit_graph(job->gvc, job->g);
}

#if 0

void gvevent_grow_pixmap(gvrender_job_t * job)
{

    Pixmap new;
#if 0
    cairo_surface_t *surface;
#endif

    new = XCreatePixmap(job->dpy, job->win, job->width, job->height,
			job->depth);
    XFillRectangle(job->dpy, new, job->gc, 0, 0,
		job->width, job->height);
    XCopyArea(job->dpy, job->pix, new, job->gc, 0, 0,
		job->width, job->height, 0, 0);
    XFreePixmap(job->dpy, job->pix);
    job->pix = new;
#if 0
    surface = cairo_xlib_surface_create(job->dpy, job->pix, job->visual,
					CAIRO_FORMAT_ARGB32, job->cmap);
    cairo_set_target_surface(job->surface, surface);
    cairo_surface_destroy(surface);
#endif
}

void gvevent_button_press(gvrender_job_t * job, XButtonEvent *bev)
{
    switch (bev->button) {
    case 1: /* select / create in edit mode */
    case 2: /* pan */
    case 3: /*        / delete in edit mode */
        job->click = 1;
	job->active = bev->button;
	job->needs_refresh = 1;
	break;
    case 4:
	/* scrollwheel zoom in at current mouse x,y */
	job->fit_mode = 0;
	job->focus.x +=  (bev->x - job->width / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->focus.y += -(bev->y - job->height / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->zoom *= ZOOMFACTOR;
	job->needs_refresh = 1;
	break;
    case 5: /* scrollwheel zoom out at current mouse x,y */
	job->fit_mode = 0;
	job->zoom /= ZOOMFACTOR;
	job->focus.x -=  (bev->x - job->width / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->focus.y -= -(bev->y - job->height / 2.)
		* (ZOOMFACTOR - 1.) / job->zoom;
	job->needs_refresh = 1;
	break;
    }
    job->oldx = bev->x;
    job->oldy = bev->y;
}

void gvevent_motion(gvrender_job_t * job, XMotionEvent *mev)
{
    double dx = mev->x - job->oldx;
    double dy = mev->y - job->oldy;

    if (abs(dx) < EPSILON && abs(dy) < EPSILON)  /* ignore motion events with no motion */
	return;

    switch (job->active) {
    case 0: /* drag with no button - */
	return;
	break;
    case 1: /* drag with button 1 - drag object */
	/* FIXME - to be implemented */
	break;
    case 2: /* drag with button 2 - pan graph */
	job->focus.x -=  dx / job->zoom;
	job->focus.y -= -dy / job->zoom;
	job->needs_refresh = 1;
	break;
    case 3: /* drag with button 3 - unused */
	break;
    }
    job->oldx = mev->x;
    job->oldy = mev->y;
}

void gvevent_button_release(gvrender_job_t *job, XButtonEvent *bev)
{
    job->click = 0;
    job->active = 0;
}

int gvevent_key_press(gvrender_job_t * job, XKeyEvent * kev)
{
#if 0
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(key_binding); i++)
	if (key_binding[i].keycode == kev->keycode)
	    return (key_binding[i].callback) (job);
#endif
    return 0;
}

static void gvevent_reconfigure_normal(gvrender_job_t * job, unsigned int width,
		       unsigned int height)
{
    int has_grown = 0;

    if (width > job->width || height > job->height)
	has_grown = 1;
    job->width = width;
    job->height = height;
    if (has_grown)
	gvevent_grow_pixmap(job);
    job->needs_refresh = 1;
}

static void gvevent_reconfigure_fit_mode(gvrender_job_t * job, unsigned int width,
			 unsigned int height)
{
    int dflt_width, dflt_height;

    dflt_width = job->width;
    dflt_height = job->height;
    job->zoom =
	MIN((double) width / (double) dflt_width,
	    (double) height / (double) dflt_height);

    gvevent_reconfigure_normal(job, width, height);
}

void gvevent_configure(gvrender_job_t * job, XConfigureEvent * cev)
{
    if (job->fit_mode)
	gvevent_reconfigure_fit_mode(job, cev->width, cev->height);
    else
	gvevent_reconfigure_normal(job, cev->width, cev->height);
}

void gvevent_expose(gvrender_job_t * job, XExposeEvent * eev)
{
    XCopyArea(job->dpy, job->pix, job->win, job->gc,
	      eev->x, eev->y, eev->width, eev->height, eev->x, eev->y);
}

void gvevent_client_message(gvrender_job_t * job, XClientMessageEvent * cmev)
{
    if (cmev->format == 32
	&& (Atom) cmev->data.l[0] == job->wm_delete_window_atom)
	exit(0);
}
#endif

static int quit_cb(gvrender_job_t * job)
{
    return 1;
}

static int left_cb(gvrender_job_t * job)
{
    job->fit_mode = 0;
    job->focus.x += PANFACTOR / job->zoom;
    job->needs_refresh = 1;
    return 0;
}

static int right_cb(gvrender_job_t * job)
{
    job->fit_mode = 0;
    job->focus.x -= PANFACTOR / job->zoom;
    job->needs_refresh = 1;
    return 0;
}

static int up_cb(gvrender_job_t * job)
{
    job->fit_mode = 0;
    job->focus.y += -(PANFACTOR / job->zoom);
    job->needs_refresh = 1;
    return 0;
}

static int down_cb(gvrender_job_t * job)
{
    job->fit_mode = 0;
    job->focus.y -= -(PANFACTOR / job->zoom);
    job->needs_refresh = 1;
    return 0;
}

static int zoom_in_cb(gvrender_job_t * job)
{
    job->fit_mode = 0;
    job->zoom *= ZOOMFACTOR;
    job->needs_refresh = 1;
    return 0;
}

static int zoom_out_cb(gvrender_job_t * job)
{
    job->fit_mode = 0;
    job->zoom /= ZOOMFACTOR;
    job->needs_refresh = 1;
    return 0;
}

static int toggle_fit_cb(gvrender_job_t * job)
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
