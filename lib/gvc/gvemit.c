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
#include <cairo.h>

#include "const.h"
#include "types.h"
#include "macros.h"

#include "gvrender.h"

extern void emit_graph(GVC_t * gvc, graph_t * g, int flags);

#ifndef X_DISPLAY_MISSING

#define PANFACTOR 10
#define ZOOMFACTOR 1.1

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

typedef struct win {
    Display *dpy;
    int scr;
    Window win;
    unsigned long event_mask;
    Pixmap pix;
    GC gc;
    Visual *visual;
    Colormap cmap;
    int depth;

    unsigned int width, height;

    cairo_t *cr;
    GVC_t *gvc;

    double tx, ty, zoom, oldx, oldy;
    int needs_refresh, fit_mode, click, active;

    Atom wm_delete_window_atom;
} win_t;

typedef int (*key_callback_t) (win_t * win);

typedef struct key_binding {
    char *key;
    int is_alias;
    KeyCode keycode;
    key_callback_t callback;
} key_binding_t;

/* callbacks */
static int quit_cb(win_t * win);
static int left_cb(win_t * win);
static int right_cb(win_t * win);
static int up_cb(win_t * win);
static int down_cb(win_t * win);
static int zoom_in_cb(win_t * win);
static int zoom_out_cb(win_t * win);
static int toggle_fit_cb(win_t * win);

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

static void win_init(win_t * win, int argb, const char *geometry,
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

    dpy = win->dpy;
    win->scr = scr = DefaultScreen(dpy);

    win->fit_mode = 0;

    win->tx = win->ty = 0.0;
    win->zoom = 1.0;

    win->needs_refresh = 1;

    if (argb && (win->visual = find_argb_visual(dpy, scr))) {
	win->cmap = XCreateColormap(dpy, RootWindow(dpy, scr),
				    win->visual, AllocNone);
	attributes.override_redirect = False;
	attributes.background_pixel = 0;
	attributes.border_pixel = 0;
	attributes.colormap = win->cmap;
	attributemask = (CWBackPixel |
			 CWBorderPixel | CWOverrideRedirect | CWColormap);
	win->depth = 32;
    } else {
	win->cmap = DefaultColormap(dpy, scr);
	win->visual = DefaultVisual(dpy, scr);
	attributes.background_pixel = WhitePixel(dpy, scr);
	attributes.border_pixel = BlackPixel(dpy, scr);
	attributemask = (CWBackPixel | CWBorderPixel);
	win->depth = DefaultDepth(dpy, scr);
    }

    if (geometry) {
	int x, y;
	XParseGeometry(geometry, &x, &y, &win->width, &win->height);
    } else {
/* FIXME - gvc->size is 0,0 at this time.  need to set viewport and transform earlier */
//	svg_cairo_get_size(win->svgc, &win->width, &win->height);
        win->width = win->gvc->size.x;
        win->height = win->gvc->size.y;
    }

    win->win = XCreateWindow(dpy, RootWindow(dpy, scr),
			     0, 0, win->width, win->height, 0, win->depth,
			     InputOutput, win->visual,
			     attributemask, &attributes);

    name = malloc(strlen("graphviz: ") + strlen(base) + 1);
    strcpy(name, "graphviz: ");
    strcat(name, base);

    normalhints = XAllocSizeHints();
    normalhints->flags = 0;
    normalhints->x = 0;
    normalhints->y = 0;
    normalhints->width = win->width;
    normalhints->height = win->height;

    classhint = XAllocClassHint();
    classhint->res_name = "graphviz";
    classhint->res_class = "Graphviz";

    wmhints = XAllocWMHints();
    wmhints->flags = InputHint;
    wmhints->input = True;

    Xutf8SetWMProperties(dpy, win->win, name, base, 0, 0,
			 normalhints, wmhints, classhint);
    XFree(wmhints);
    XFree(classhint);
    XFree(normalhints);
    free(name);

    win->pix =
	XCreatePixmap(dpy, win->win, win->width, win->height, win->depth);
    if (argb)
	gcv.foreground = 0;
    else
	gcv.foreground = WhitePixel(dpy, scr);
    win->gc = XCreateGC(dpy, win->pix, GCForeground, &gcv);
    XFillRectangle(dpy, win->pix, win->gc, 0, 0, win->width, win->height);

    for (i = 0; i < ARRAY_SIZE(key_binding); i++) {
	KeySym keysym;
	keysym = XStringToKeysym(key_binding[i].key);
	if (keysym == NoSymbol)
	    fprintf(stderr, "ERROR: No keysym for \"%s\"\n",
		    key_binding[i].key);
	else
	    key_binding[i].keycode = XKeysymToKeycode(dpy, keysym);
    }
    win->cr = cairo_create();
    surface = cairo_xlib_surface_create(dpy, win->pix, win->visual,
					CAIRO_FORMAT_ARGB32, win->cmap);
    cairo_set_target_surface(win->cr, surface);

// FIXME - messy win + gvc structs
    win->gvc->surface = win->cr;
     win->gvc->external_surface = TRUE;    /* FIXME - who clears this? */

    cairo_surface_destroy(surface);
    /* XXX: This probably doesn't need to be here (eventually) */
    cairo_set_rgb_color(win->cr, 1, 1, 1);
//    svg_cairo_set_viewport_dimension(win->svgc, win->width, win->height);
    win->event_mask = (
          ButtonPressMask
        | ButtonReleaseMask
        | PointerMotionMask
        | KeyPressMask
	| StructureNotifyMask
	| ExposureMask);
    XSelectInput(dpy, win->win, win->event_mask);
    win->wm_delete_window_atom =
	XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win->win, &win->wm_delete_window_atom, 1);
    XMapWindow(dpy, win->win);

    win->click = 0;
    win->active = 0;
}

static void win_deinit(win_t * win)
{
    cairo_destroy(win->cr);
    win->cr = NULL;
    XFreeGC(win->dpy, win->gc);
    XDestroyWindow(win->dpy, win->win);
}

static void win_refresh(win_t * win)
{
    XFillRectangle(win->dpy, win->pix, win->gc, 0, 0, win->width,
		   win->height);
    cairo_save(win->cr);
    cairo_translate(win->cr, win->tx, win->ty);
    cairo_scale(win->cr, win->zoom, win->zoom);

//    svg_cairo_render(win->svgc, win->cr);
// FIXME:  flags = chkOrder(g) | gvc->render_features->flags;
    emit_graph(win->gvc, win->gvc->g, win->gvc->render_features->flags);

    cairo_restore(win->cr);
    XCopyArea(win->dpy, win->pix, win->win, win->gc,
	      0, 0, win->width, win->height, 0, 0);
}

static void win_grow_pixmap(win_t * win)
{
    Pixmap new;
    cairo_surface_t *surface;

    new = XCreatePixmap(win->dpy, win->win, win->width, win->height,
			win->depth);
    XFillRectangle(win->dpy, new, win->gc, 0, 0, win->width, win->height);
    XCopyArea(win->dpy, win->pix, new, win->gc, 0, 0, win->width,
	      win->height, 0, 0);
    XFreePixmap(win->dpy, win->pix);
    win->pix = new;
    surface = cairo_xlib_surface_create(win->dpy, win->pix, win->visual,
					CAIRO_FORMAT_ARGB32, win->cmap);
    cairo_set_target_surface(win->cr, surface);
    cairo_surface_destroy(surface);
    win->needs_refresh = 1;
}

static void win_handle_button_press(win_t *win, XButtonEvent *bev)
{
    switch (bev->button) {
    case 1: /* select / create in edit mode */
    case 2: /* pan */
    case 3: /*        / delete in edit mode */
        win->click = 1;
	win->active = bev->button;
	win->needs_refresh = 1;
	break;
    case 4:
	/* scrollwheel zoom in at current mouse x,y */
	win->fit_mode = 0;
	win->tx += bev->x * win->zoom;
	win->ty += bev->y * win->zoom;
	win->zoom *= ZOOMFACTOR;
	win->tx *= ZOOMFACTOR;
	win->ty *= ZOOMFACTOR;
	win->tx -= bev->x * win->zoom;
	win->ty -= bev->y * win->zoom;
	win->needs_refresh = 1;
	break;
    case 5: /* scrollwheel zoom out at current mouse x,y */
	win->fit_mode = 0;
	win->tx += bev->x * win->zoom;
	win->ty += bev->y * win->zoom;
	win->zoom /= ZOOMFACTOR;
	win->tx /= ZOOMFACTOR;
	win->ty /= ZOOMFACTOR;
	win->tx -= bev->x * win->zoom;
	win->ty -= bev->y * win->zoom;
	win->needs_refresh = 1;
	break;
    }
    win->oldx = bev->x;
    win->oldy = bev->y;
}

static void win_handle_motion(win_t *win, XMotionEvent *mev)
{
    switch (win->active) {
    case 0: /* drag object */
	return;
	break;
    case 1:
	break;
    case 2: /* pan */
	win->tx += (mev->x - win->oldx) * win->zoom;
	win->ty += (mev->y - win->oldy) * win->zoom;
	win->needs_refresh = 1;
	break;
    case 3: /* unused */
	break;
    }
    win->oldx = mev->x;
    win->oldy = mev->y;
}

static void win_handle_button_release(win_t *win, XButtonEvent *bev)
{
    win->click = 0;
    win->active = 0;
}

static int win_handle_key_press(win_t * win, XKeyEvent * kev)
{
    unsigned int i;

// fprintf(stderr,"keycode = %d\n", kev->keycode);

    for (i = 0; i < ARRAY_SIZE(key_binding); i++)
	if (key_binding[i].keycode == kev->keycode)
	    return (key_binding[i].callback) (win);
    return 0;
}

static void win_reconfigure_normal(win_t * win, unsigned int width,
		       unsigned int height)
{
    int has_grown = 0;

    if (width > win->width || height > win->height)
	has_grown = 1;
    win->width = width;
    win->height = height;
//    svg_cairo_set_viewport_dimension(win->svgc, win->width, win->height);
    if (has_grown)
	win_grow_pixmap(win);
}

static void
win_reconfigure_fit_mode(win_t * win, unsigned int width,
			 unsigned int height)
{
    int dflt_width, dflt_height;
    int has_grown = 0;

    if (width > win->width || height > win->height)
	has_grown = 1;
//    svg_cairo_get_size(win->svgc, &dflt_width, &dflt_height);
    dflt_width = win->gvc->size.x;
    dflt_height = win->gvc->size.y;
    win->zoom =
	MIN((double) width / (double) dflt_width,
	    (double) height / (double) dflt_height);

    win->width = width;
    win->height = height;
//    svg_cairo_set_viewport_dimension(win->svgc, win->width, win->height);
    win->needs_refresh = 1;
    if (has_grown)
	win_grow_pixmap(win);
}

static void win_handle_configure(win_t * win, XConfigureEvent * cev)
{
    if (win->fit_mode)
	win_reconfigure_fit_mode(win, cev->width, cev->height);
    else
	win_reconfigure_normal(win, cev->width, cev->height);
}

static void win_handle_expose(win_t * win, XExposeEvent * eev)
{
    XCopyArea(win->dpy, win->pix, win->win, win->gc,
	      eev->x, eev->y, eev->width, eev->height, eev->x, eev->y);
}

static void win_handle_client_message(win_t * win, XClientMessageEvent * cmev)
{
    if (cmev->format == 32
	&& (Atom) cmev->data.l[0] == win->wm_delete_window_atom)
	exit(0);
}

static void win_handle_events(win_t * win)
{
    int done;
    XEvent xev;

    while (1) {
	if (!XPending(win->dpy) && win->needs_refresh) {
	    win_refresh(win);
	    win->needs_refresh = 0;
	}

	XNextEvent(win->dpy, &xev);

	switch (xev.xany.type) {
	case ButtonPress:
            win_handle_button_press(win, &xev.xbutton);
            break;
        case MotionNotify:
            win_handle_motion(win, &xev.xmotion);
            break;
        case ButtonRelease:
            win_handle_button_release(win, &xev.xbutton);
            break;
	case KeyPress:
	    done = win_handle_key_press(win, &xev.xkey);
	    if (done)
		return;
	    break;
	case ConfigureNotify:
	    win_handle_configure(win, &xev.xconfigure);
	    break;
	case Expose:
	    win_handle_expose(win, &xev.xexpose);
	    break;
	case ClientMessage:
	    win_handle_client_message(win, &xev.xclient);
	    break;
	}
    }
}

static int quit_cb(win_t * win)
{
    return 1;
}

static int left_cb(win_t * win)
{
    win->fit_mode = 0;
    win->tx -= PANFACTOR * win->zoom;
    win->needs_refresh = 1;
    return 0;
}

static int right_cb(win_t * win)
{
    win->fit_mode = 0;
    win->tx += PANFACTOR * win->zoom;
    win->needs_refresh = 1;
    return 0;
}

static int up_cb(win_t * win)
{
    win->fit_mode = 0;
    win->ty -= PANFACTOR * win->zoom;
    win->needs_refresh = 1;
    return 0;
}

static int down_cb(win_t * win)
{
    win->fit_mode = 0;
    win->ty += PANFACTOR * win->zoom;
    win->needs_refresh = 1;
    return 0;
}

static int zoom_in_cb(win_t * win)
{
    win->fit_mode = 0;
    win->tx += win->width * win->zoom;
    win->ty += win->height * win->zoom;
    win->zoom *= ZOOMFACTOR;
    win->tx *= ZOOMFACTOR;
    win->ty *= ZOOMFACTOR;
    win->tx -= win->width * win->zoom;
    win->ty -= win->height * win->zoom;
    win->needs_refresh = 1;
    return 0;
}

static int zoom_out_cb(win_t * win)
{
    win->fit_mode = 0;
    win->tx += win->width * win->zoom;
    win->ty += win->height * win->zoom;
    win->zoom /= ZOOMFACTOR;
    win->tx /= ZOOMFACTOR;
    win->ty /= ZOOMFACTOR;
    win->tx -= win->width * win->zoom;
    win->ty -= win->height * win->zoom;
    win->needs_refresh = 1;
    return 0;
}

static int toggle_fit_cb(win_t * win)
{
    win->fit_mode = !win->fit_mode;
    if (win->fit_mode) {
	int dflt_width, dflt_height;
//	svg_cairo_get_size(win->svgc, &dflt_width, &dflt_height);
	dflt_width = win->gvc->size.x;
	dflt_height = win->gvc->size.y;
	win->zoom =
	    MIN((double) win->width / (double) dflt_width,
		(double) win->height / (double) dflt_height);
	win->tx = 0.0;
	win->ty = 0.0;
	win->needs_refresh = 1;
    }
    return 0;
}
#endif /* X_DISPLAY_MISSING */


void gvemit_graph(GVC_t * gvc, graph_t * g, int flags)
{
   if (flags & GVRENDER_X11_EVENTS) {
#ifndef X_DISPLAY_MISSING
	win_t win;
	const char *display=NULL;
	int argb=0;
	const char *geometry="200x200";

	win.gvc = gvc;

	win.dpy = XOpenDisplay(display);
	if (win.dpy == NULL) {
	    fprintf(stderr, "Failed to open display: %s\n",
		    XDisplayName(display));
	    return;
	}

	win_init(&win, argb, geometry, gvc->layout_type);

	win_handle_events(&win);

	win_deinit(&win);

	XCloseDisplay(win.dpy);
#else
	fprintf(stderr,"No X11 support available\n");
#endif
   }
   else {
	emit_graph(gvc, g, flags);
   }
}
