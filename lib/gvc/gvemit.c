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
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "const.h"
#include "types.h"
#include "macros.h"

#include "gvc.h"

extern void emit_graph(GVC_t * gvc, graph_t * g, int flags);

//#if !defined(X_DISPLAY_MISSING) && !defined(DISABLE_GVRENDER) && defined(HAVE_CAIRO)
#if 0

#include <cairo.h>

#ifdef CAIRO_HAS_XLIB_SURFACE
#include <cairo-xlib.h>

#define PANFACTOR 10
#define ZOOMFACTOR 1.1
#define EPSILON .0001

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

typedef int (*key_callback_t) (gvrender_job_t * job);

typedef struct key_binding {
    char *key;
    int is_alias;
    KeyCode keycode;
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
    /* Keysym, Alias, Keycode, callback */
    {"Q", 0, 0, quit_cb},
    {"Left", 0, 0, left_cb},
    {"KP_Left", 0, 0, left_cb},
    {"Right", 0, 0, right_cb},
    {"KP_Right", 0, 0, right_cb},
    {"Up", 0, 0, up_cb},
    {"KP_Up", 0, 0, up_cb},
    {"Down", 0, 0, down_cb},
    {"KP_Down", 0, 0, down_cb},
    {"plus", 0, 0, zoom_in_cb},
    {"KP_Add", 0, 0, zoom_in_cb},
    {"minus", 0, 0, zoom_out_cb},
    {"KP_Subtract", 0, 0, zoom_out_cb},
    {"F", 0, 0, toggle_fit_cb},
};

static Visual *find_argb_visual(Display * dpy, int scr)
{
    XVisualInfo *xvi;
    XVisualInfo template;
    int nvi;
    int i;
    XRenderPictFormat *format;
    Visual *visual;

    template.screen = scr;
    template.depth = 32;
    template.class = TrueColor;
    xvi = XGetVisualInfo(dpy,
			 VisualScreenMask |
			 VisualDepthMask |
			 VisualClassMask, &template, &nvi);
    if (!xvi)
	return 0;
    visual = 0;
    for (i = 0; i < nvi; i++) {
	format = XRenderFindVisualFormat(dpy, xvi[i].visual);
	if (format->type == PictTypeDirect && format->direct.alphaMask) {
	    visual = xvi[i].visual;
	    break;
	}
    }

    XFree(xvi);
    return visual;
}

static void win_init(gvrender_job_t * job, int argb, const char *geometry,
	 const char *base)
{
    unsigned int i;
    XGCValues gcv;
    XSetWindowAttributes attributes;
    XWMHints *wmhints;
    XSizeHints *normalhints;
    XClassHint *classhint;
    unsigned long attributemask = 0;
    cairo_surface_t *surface;
    char *name;
    Display *dpy;
    int scr;

    job->surface = cairo_create();
    job->external_surface = TRUE;

    dpy = job->dpy;
    job->scr = scr = DefaultScreen(dpy);

    job->fit_mode = 0;
    job->needs_refresh = 1;

    if (argb && (job->visual = find_argb_visual(dpy, scr))) {
	job->cmap = XCreateColormap(dpy, RootWindow(dpy, scr),
				    job->visual, AllocNone);
	attributes.override_redirect = False;
	attributes.background_pixel = 0;
	attributes.border_pixel = 0;
	attributes.colormap = job->cmap;
	attributemask = (CWBackPixel |
			 CWBorderPixel | CWOverrideRedirect | CWColormap);
	job->depth = 32;
    } else {
	job->cmap = DefaultColormap(dpy, scr);
	job->visual = DefaultVisual(dpy, scr);
	attributes.background_pixel = WhitePixel(dpy, scr);
	attributes.border_pixel = BlackPixel(dpy, scr);
	attributemask = (CWBackPixel | CWBorderPixel);
	job->depth = DefaultDepth(dpy, scr);
    }

    if (geometry) {
	int x, y;
	XParseGeometry(geometry, &x, &y, &job->width, &job->height);
    }

    job->win = XCreateWindow(dpy, RootWindow(dpy, scr),
			     0, 0, job->width, job->height, 0, job->depth,
			     InputOutput, job->visual,
			     attributemask, &attributes);

    name = malloc(strlen("graphviz: ") + strlen(base) + 1);
    strcpy(name, "graphviz: ");
    strcat(name, base);

    normalhints = XAllocSizeHints();
    normalhints->flags = 0;
    normalhints->x = 0;
    normalhints->y = 0;
    normalhints->width = job->width;
    normalhints->height = job->height;

    classhint = XAllocClassHint();
    classhint->res_name = "graphviz";
    classhint->res_class = "Graphviz";

    wmhints = XAllocWMHints();
    wmhints->flags = InputHint;
    wmhints->input = True;

    Xutf8SetWMProperties(dpy, job->win, name, base, 0, 0,
			 normalhints, wmhints, classhint);
    XFree(wmhints);
    XFree(classhint);
    XFree(normalhints);
    free(name);

    job->pix =
	XCreatePixmap(dpy, job->win, job->width, job->height, job->depth);
    if (argb)
	gcv.foreground = 0;
    else
	gcv.foreground = WhitePixel(dpy, scr);
    job->gc = XCreateGC(dpy, job->pix, GCForeground, &gcv);
    XFillRectangle(dpy, job->pix, job->gc, 0, 0, job->width, job->height);

    for (i = 0; i < ARRAY_SIZE(key_binding); i++) {
	KeySym keysym;
	keysym = XStringToKeysym(key_binding[i].key);
	if (keysym == NoSymbol)
	    fprintf(stderr, "ERROR: No keysym for \"%s\"\n",
		    key_binding[i].key);
	else
	    key_binding[i].keycode = XKeysymToKeycode(dpy, keysym);
    }
    surface = cairo_xlib_surface_create(dpy, job->pix, job->visual,
					CAIRO_FORMAT_ARGB32, job->cmap);
    cairo_set_target_surface(job->surface, surface);
    cairo_surface_destroy(surface);

    /* XXX: This probably doesn't need to be here (eventually) */
    cairo_set_rgb_color(job->surface, 1, 1, 1);
    job->event_mask = (
          ButtonPressMask
        | ButtonReleaseMask
        | PointerMotionMask
        | KeyPressMask
	| StructureNotifyMask
	| ExposureMask);
    XSelectInput(dpy, job->win, job->event_mask);
    job->wm_delete_window_atom =
	XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, job->win, &job->wm_delete_window_atom, 1);
    XMapWindow(dpy, job->win);

    job->click = 0;
    job->active = 0;
}

static void win_deinit(gvrender_job_t * job)
{
    XFreeGC(job->dpy, job->gc);
    XDestroyWindow(job->dpy, job->win);
    cairo_destroy(job->surface);
    job->external_surface = FALSE;
}

static void win_refresh(gvrender_job_t * job)
{
    XFillRectangle(job->dpy, job->pix, job->gc, 0, 0,
		job->width, job->height);

    emit_graph(job->gvc, job->g, job->flags);

    XCopyArea(job->dpy, job->pix, job->win, job->gc,
	      0, 0, job->width, job->height, 0, 0);
}

static void win_grow_pixmap(gvrender_job_t * job)
{
    Pixmap new;
    cairo_surface_t *surface;

    new = XCreatePixmap(job->dpy, job->win, job->width, job->height,
			job->depth);
    XFillRectangle(job->dpy, new, job->gc, 0, 0,
		job->width, job->height);
    XCopyArea(job->dpy, job->pix, new, job->gc, 0, 0,
		job->width, job->height, 0, 0);
    XFreePixmap(job->dpy, job->pix);
    job->pix = new;
    surface = cairo_xlib_surface_create(job->dpy, job->pix, job->visual,
					CAIRO_FORMAT_ARGB32, job->cmap);
    cairo_set_target_surface(job->surface, surface);
    cairo_surface_destroy(surface);
}

static void win_handle_button_press(gvrender_job_t * job, XButtonEvent *bev)
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

static void win_handle_motion(gvrender_job_t * job, XMotionEvent *mev)
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

static void win_handle_button_release(gvrender_job_t *job, XButtonEvent *bev)
{
    job->click = 0;
    job->active = 0;
}

static int win_handle_key_press(gvrender_job_t * job, XKeyEvent * kev)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(key_binding); i++)
	if (key_binding[i].keycode == kev->keycode)
	    return (key_binding[i].callback) (job);
    return 0;
}

static void win_reconfigure_normal(gvrender_job_t * job, unsigned int width,
		       unsigned int height)
{
    int has_grown = 0;

    if (width > job->width || height > job->height)
	has_grown = 1;
    job->width = width;
    job->height = height;
    if (has_grown)
	win_grow_pixmap(job);
    job->needs_refresh = 1;
}

static void
win_reconfigure_fit_mode(gvrender_job_t * job, unsigned int width,
			 unsigned int height)
{
    int dflt_width, dflt_height;

    dflt_width = job->width;
    dflt_height = job->height;
    job->zoom =
	MIN((double) width / (double) dflt_width,
	    (double) height / (double) dflt_height);

    win_reconfigure_normal(job, width, height);
}

static void win_handle_configure(gvrender_job_t * job, XConfigureEvent * cev)
{
    if (job->fit_mode)
	win_reconfigure_fit_mode(job, cev->width, cev->height);
    else
	win_reconfigure_normal(job, cev->width, cev->height);
}

static void win_handle_expose(gvrender_job_t * job, XExposeEvent * eev)
{
    XCopyArea(job->dpy, job->pix, job->win, job->gc,
	      eev->x, eev->y, eev->width, eev->height, eev->x, eev->y);
}

static void win_handle_client_message(gvrender_job_t * job, XClientMessageEvent * cmev)
{
    if (cmev->format == 32
	&& (Atom) cmev->data.l[0] == job->wm_delete_window_atom)
	exit(0);
}

static void win_handle_events(gvrender_job_t *job)
{
    int done;
    XEvent xev;

    while (1) {
	if (!XPending(job->dpy) && job->needs_refresh) {
	    win_refresh(job);
	    job->needs_refresh = 0;
	}

	XNextEvent(job->dpy, &xev);

	switch (xev.xany.type) {
	case ButtonPress:
            win_handle_button_press(job, &xev.xbutton);
            break;
        case MotionNotify:
            win_handle_motion(job, &xev.xmotion);
            break;
        case ButtonRelease:
            win_handle_button_release(job, &xev.xbutton);
            break;
	case KeyPress:
	    done = win_handle_key_press(job, &xev.xkey);
	    if (done)
		return;
	    break;
	case ConfigureNotify:
	    win_handle_configure(job, &xev.xconfigure);
	    break;
	case Expose:
	    win_handle_expose(job, &xev.xexpose);
	    break;
	case ClientMessage:
	    win_handle_client_message(job, &xev.xclient);
	    break;
	}
    }
}

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
#endif /* CAIRO_HAS_XLIB_SURFACE */
#endif /* X_DISPLAY_MISSING */

void gvemit_graph(GVC_t * gvc, graph_t * g, int flags)
{
    gvrender_job_t *job = gvc->job;

    job->gvc = gvc;
    job->g = g;
    job->flags = flags;

   if (flags & GVRENDER_X11_EVENTS) {
//#ifdef CAIRO_HAS_XLIB_SURFACE
#if 0

	const char *display=NULL;
	int argb=0;
	const char *geometry=NULL;

	job->dpy = XOpenDisplay(display);
	if (job->dpy == NULL) {
	    fprintf(stderr, "Failed to open display: %s\n",
		    XDisplayName(display));
	    return;
	}

	win_init(job, argb, geometry, gvc->layout_type);

	win_handle_events(job);

	win_deinit(job);

	XCloseDisplay(job->dpy);
#else
	fprintf(stderr,"No X11 support available\n");
#endif
   }
   else {
	emit_graph(job->gvc, job->g, job->flags);
   }
}
