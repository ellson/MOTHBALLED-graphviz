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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#if defined(HAVE_FENV_H) && defined(HAVE_FESETENV) && defined(HAVE_FEGETENV) && defined(HAVE_FEENABLEEXCEPT)

/* _GNU_SOURCE is needed (supposedly) for the feenableexcept
 * prototype to be defined in fenv.h on GNU systems.
 * Presumably it will do no harm on other systems.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* We are not supposed to need __USE_GNU, but I can't see
 * how to get the prototype for fedisableexcept from
 * /usr/include/fenv.h without it.
 */
#ifndef __USE_GNU
#define __USE_GNU
#endif

# include <fenv.h>
#elif HAVE_FPU_CONTROL_H
# include <fpu_control.h>
#elif HAVE_SYS_FPU_H
# include <sys/fpu.h>
#endif

#include "gvplugin_render.h"

#ifdef HAVE_PANGOCAIRO
#include <pango/pangocairo.h>

typedef enum { FORMAT_PNG,
		FORMAT_PS,
		FORMAT_PDF,
		FORMAT_SVG,
		FORMAT_GTK,
		FORMAT_XLIB,
		FORMAT_XCB,
		FORMAT_SDL,
		FORMAT_GLITZ,
		FORMAT_QUARTZ,
		FORMAT_WIN32,
    } format_type;

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

static double dashed[] = {10.};
static int dashed_len = ARRAY_SIZE(dashed);

static double dotted[] = {2., 10.};
static int dotted_len = ARRAY_SIZE(dotted);

#ifdef CAIRO_HAS_PNG_SURFACE
#include <cairo-png.h>
#endif

#ifdef CAIRO_HAS_PS_SURFACE
#include <cairo-ps.h>
#endif

#ifdef CAIRO_HAS_PDF_SURFACE
#include <cairo-pdf.h>
#endif

#ifdef CAIRO_HAS_SVG_SURFACE
#include <cairo-svg.h>
#endif

#ifdef CAIRO_HAS_XLIB_SURFACE
#include <cairo-xlib.h>
#endif

#if defined(HAVE_FENV_H) && defined(HAVE_FESETENV) && defined(HAVE_FEGETENV) && defined(HAVE_FEENABLEEXCEPT)
/* place to save fp environment temporarily */
static fenv_t fenv; /* FIXME - not thread safe */
#endif

static void cairogen_set_color(cairo_t * cr, gvcolor_t * color)
{
    cairo_set_source_rgba(cr, color->u.RGBA[0], color->u.RGBA[1],
                        color->u.RGBA[2], color->u.RGBA[3]);
}

static cairo_status_t
writer (void *closure, const unsigned char *data, unsigned int length)
{
    if (length == fwrite(data, 1, length, (FILE *)closure))
	return CAIRO_STATUS_SUCCESS;
    return CAIRO_STATUS_WRITE_ERROR;
}

static void cairogen_begin_page(GVJ_t * job)
{
    cairo_t *cr;
    cairo_surface_t *surface;

#if defined(HAVE_FENV_H) && defined(HAVE_FESETENV) && defined(HAVE_FEGETENV) && defined(HAVE_FEDISABLEEXCEPT)
    /* cairo generates FE_INVALID and other exceptions we 
     * want to ignore for now.  Save the current fenv and
     * set one just for cairo.
     * The fenv is restored in cairogen_end_graph  */
    fegetenv(&fenv);
    fedisableexcept(FE_ALL_EXCEPT);
#endif

    cr = (cairo_t *) job->surface; /* might be NULL */

    switch (job->render.id) {
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    case FORMAT_PNG:
	if (!cr) {
	    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
			(double)(job->width), (double)(job->height));
	    cr = cairo_create(surface);
	    cairo_surface_destroy (surface);
	}
	break;
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    case FORMAT_PS:
	if (!cr) {
	    surface = cairo_ps_surface_create_for_stream (writer,
			job->output_file, (double)(job->width), (double)(job->height));
	    cr = cairo_create(surface);
	    cairo_surface_destroy (surface);
	}
	break;
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    case FORMAT_PDF:
	if (!cr) {
	    surface = cairo_pdf_surface_create_for_stream (writer,
			job->output_file, (double)(job->width), (double)(job->height));
	    cr = cairo_create(surface);
	    cairo_surface_destroy (surface);
	}
	break;
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
    case FORMAT_SVG:
	if (!cr) {
	    surface = cairo_svg_surface_create_for_stream (writer,
			job->output_file, (double)(job->width), (double)(job->height));
	    cr = cairo_create(surface);
	    cairo_surface_destroy (surface);
	}
	break;
#endif
#ifdef CAIRO_HAS_XLIB_SURFACE
    case FORMAT_GTK:
	break;
    case FORMAT_XLIB:
	break;
#endif
#ifdef CAIRO_HAS_XCB_SURFACE
    case FORMAT_XCB:
	break;
#endif
#ifdef CAIRO_HAS_SDL_SURFACE
    case FORMAT_SDL:
	break;
#endif
#ifdef CAIRO_HAS_GLITZ_SURFACE
    case FORMAT_GLITZ:
	break;
#endif
    default:
	break;
    }
    job->surface = (void *) cr;

    cairo_scale(cr, job->scale.x, job->scale.y);
    cairo_rotate(cr, job->rotation * M_PI / 180.);
    cairo_translate(cr, job->translation.x, job->translation.y);
}

static void cairogen_end_page(GVJ_t * job)
{
    cairo_t *cr = (cairo_t *) job->surface;
    cairo_surface_t *surface;

    switch (job->render.id) {
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    case FORMAT_PNG:
	surface = cairo_get_target(cr);
	cairo_surface_write_to_png_stream(surface, writer, job->output_file);
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    case FORMAT_PS:
	cairo_show_page(cr);
	break;
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    case FORMAT_PDF:
	cairo_show_page(cr);
	break;
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
    case FORMAT_SVG:
	cairo_show_page(cr);
	break;
#endif
    default:
	break;
    }
    if (!job->external_surface)
	cairo_destroy(cr);

#if defined HAVE_FENV_H && defined HAVE_FESETENV && defined HAVE_FEGETENV && defined(HAVE_FEENABLEEXCEPT)
    /* Restore FP environment */
    fesetenv(&fenv);
#endif
}

static void cairogen_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    gvstyle_t *style = job->style;
    cairo_t *cr = (cairo_t *) job->surface;
    pointf offset;
    PangoLayout *layout = (PangoLayout*)(para->layout);
    PangoLayoutIter* iter;

    cairo_set_dash (cr, dashed, 0, 0.0);  /* clear any dashing */
    cairogen_set_color(cr, &(style->pencolor));

    switch (para->just) {
    case 'r':
	offset.x = para->width;
	break;
    case 'l':
	offset.x = 0.0;
	break;
    case 'n':
    default:
	offset.x = para->width / 2.0;
	break;
    }
    /* offset to baseline */
    iter = pango_layout_get_iter (layout);
    offset.y = pango_layout_iter_get_baseline (iter) / PANGO_SCALE;

    cairo_move_to (cr, p.x-offset.x, -p.y-offset.y);
    pango_cairo_show_layout(cr, layout);
}

static void cairogen_ellipse(GVJ_t * job, pointf * A, int filled)
{
    gvstyle_t *style = job->style;
    cairo_t *cr = (cairo_t *) job->surface;
    cairo_matrix_t matrix;
    double rx, ry;

    if (style->pen == PEN_DASHED) {
	cairo_set_dash (cr, dashed, dashed_len, 0.0);
    } else if (style->pen == PEN_DOTTED) {
	cairo_set_dash (cr, dotted, dotted_len, 0.0);
    } else {
	cairo_set_dash (cr, dashed, 0, 0.0);
    }
    cairo_set_line_width (cr, style->penwidth * job->scale.x);

    cairo_get_matrix(cr, &matrix);
    cairo_translate(cr, A[0].x, -A[0].y);

    rx = fabs(A[1].x - A[0].x);
    ry = fabs(A[1].y - A[0].y);
    cairo_scale(cr, 1, ry / rx);
    cairo_move_to(cr, rx, 0);
    cairo_arc(cr, 0, 0, rx, 0, 2 * M_PI);
    cairo_close_path(cr);

    cairo_set_matrix(cr, &matrix);

    if (filled) {
	cairogen_set_color(cr, &(style->fillcolor));
	cairo_fill_preserve(cr);
    }
    cairogen_set_color(cr, &(style->pencolor));
    cairo_stroke(cr);
}

static void
cairogen_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    gvstyle_t *style = job->style;
    cairo_t *cr = (cairo_t *) job->surface;
    int i;

    if (style->pen == PEN_DASHED) {
	cairo_set_dash (cr, dashed, dashed_len, 0.0);
    } else if (style->pen == PEN_DOTTED) {
	cairo_set_dash (cr, dotted, dotted_len, 0.0);
    } else {
	cairo_set_dash (cr, dashed, 0, 0.0);
    }
    cairo_set_line_width (cr, style->penwidth * job->scale.x);
    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i++)
	cairo_line_to(cr, A[i].x, -A[i].y);
    cairo_close_path(cr);
    if (filled) {
	cairogen_set_color(cr, &(style->fillcolor));
	cairo_fill_preserve(cr);
    }
    cairogen_set_color(cr, &(style->pencolor));
    cairo_stroke(cr);
}

static void
cairogen_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
		int arrow_at_end, int filled)
{
    gvstyle_t *style = job->style;
    cairo_t *cr = (cairo_t *) job->surface;
    int i;

    if (style->pen == PEN_DASHED) {
	cairo_set_dash (cr, dashed, dashed_len, 0.0);
    } else if (style->pen == PEN_DOTTED) {
	cairo_set_dash (cr, dotted, dotted_len, 0.0);
    } else {
	cairo_set_dash (cr, dashed, 0, 0.0);
    }
    cairo_set_line_width (cr, style->penwidth * job->scale.x);
    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i += 3)
	cairo_curve_to(cr, A[i].x, -A[i].y, A[i + 1].x, -A[i + 1].y,
		       A[i + 2].x, -A[i + 2].y);
    cairogen_set_color(cr, &(style->pencolor));
    cairo_stroke(cr);
}

static void
cairogen_polyline(GVJ_t * job, pointf * A, int n)
{
    gvstyle_t *style = job->style;
    cairo_t *cr = (cairo_t *) job->surface;
    int i;

    if (style->pen == PEN_DASHED) {
	cairo_set_dash (cr, dashed, dashed_len, 0.0);
    } else if (style->pen == PEN_DOTTED) {
	cairo_set_dash (cr, dotted, dotted_len, 0.0);
    } else {
	cairo_set_dash (cr, dashed, 0, 0.0);
    }
    cairo_set_line_width (cr, style->penwidth * job->scale.x);
    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i++)
	cairo_line_to(cr, A[i].x, -A[i].y);
    cairogen_set_color(cr, &(style->pencolor));
    cairo_stroke(cr);
}

static gvrender_engine_t cairogen_engine = {
    0,				/* cairogen_begin_job */
    0,				/* cairogen_end_job */
    0,				/* cairogen_begin_graph */
    0,				/* cairogen_end_graph */
    0,				/* cairogen_begin_layer */
    0,				/* cairogen_end_layer */
    cairogen_begin_page,
    cairogen_end_page,
    0,				/* cairogen_begin_cluster */
    0,				/* cairogen_end_cluster */
    0,				/* cairogen_begin_nodes */
    0,				/* cairogen_end_nodes */
    0,				/* cairogen_begin_edges */
    0,				/* cairogen_end_edges */
    0,				/* cairogen_begin_node */
    0,				/* cairogen_end_node */
    0,				/* cairogen_begin_edge */
    0,				/* cairogen_end_edge */
    0,				/* cairogen_begin_anchor */
    0,				/* cairogen_end_anchor */
    cairogen_textpara,
    0,				/* cairogen_resolve_color */
    cairogen_ellipse,
    cairogen_polygon,
    cairogen_bezier,
    cairogen_polyline,
    0,				/* cairogen_comment */
};

static gvrender_features_t cairogen_features = {
    GVRENDER_DOES_TRUECOLOR
	| GVRENDER_Y_GOES_DOWN
	| GVRENDER_DOES_TRANSFORM, /* flags */
    0,				/* default margin - points */
    {96.,96.},			/* default dpi */
    0,				/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_DOUBLE,		/* color_type */
    0,				/* device */
    "cairo",			/* gvloadimage target for usershapes */
};

static gvrender_features_t cairogen_features_ps = {
    GVRENDER_DOES_TRUECOLOR
	| GVRENDER_Y_GOES_DOWN
	| GVRENDER_DOES_TRANSFORM, /* flags */
    36,				/* default margin - points */
    {72.,72.},			/* postscript 72 dpi */
    0,				/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_DOUBLE,		/* color_type */
    0,				/* device */
    "cairo",			/* gvloadimage target for usershapes */
};

#if 0
static gvrender_features_t cairogen_features_x = {
    GVRENDER_DOES_TRUECOLOR
	| GVRENDER_Y_GOES_DOWN
	| GVRENDER_DOES_TRANSFORM
	| GVRENDER_X11_EVENTS,	/* flags */
    0,				/* default margin - points */
    {96.,96.},			/* default dpi */
    0,				/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_DOUBLE,		/* color_type */
    "xlib",			/* device */
    "cairo",			/* gvloadimage target for usershapes */
};

static gvrender_features_t cairogen_features_gtk = {
    GVRENDER_DOES_TRUECOLOR
	| GVRENDER_Y_GOES_DOWN
	| GVRENDER_DOES_TRANSFORM
	| GVRENDER_X11_EVENTS,	/* flags */
    0,				/* default margin - points */
    {96.,96.},			/* default dpi */
    0,				/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_DOUBLE,		/* color_type */
    "gtk",			/* device */
    "cairo",			/* gvloadimage target for usershapes */
};
#endif
#endif

gvplugin_installed_t gvrender_pango_types[] = {
#ifdef HAVE_PANGOCAIRO
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    {FORMAT_PNG, "png", 10, &cairogen_engine, &cairogen_features},
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    {FORMAT_PS, "ps", -10, &cairogen_engine, &cairogen_features_ps},
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    {FORMAT_PDF, "pdf", 1, &cairogen_engine, &cairogen_features_ps},
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
    {FORMAT_SVG, "svg", -10, &cairogen_engine, &cairogen_features_ps},
#endif
#if 0
#ifdef CAIRO_HAS_XCB_SURFACE
    {FORMAT_XCB, "xcb", 0, &cairogen_engine, &cairogen_features_x},
#endif
#ifdef CAIRO_HAS_SDL_SURFACE
    {FORMAT_SDL, "sdl", 0, &cairogen_engine, &cairogen_features_x},
#endif
#ifdef CAIRO_HAS_GLITZ_SURFACE
    {FORMAT_GLITZ, "glitz", 0, &cairogen_engine, &cairogen_features_x},
#endif
#ifdef CAIRO_HAS_QUARTZ_SURFACE
    {FORMAT_QUARTZ, "quartz", 0, &cairogen_engine, &cairogen_features_x},
#endif
#ifdef CAIRO_HAS_WIN32_SURFACE
    {FORMAT_WIN32, "win32", 0, &cairogen_engine, &cairogen_features_x},
#endif
#ifdef CAIRO_HAS_XLIB_SURFACE
    {FORMAT_GTK, "gtk", 0, &cairogen_engine, &cairogen_features_gtk},
    {FORMAT_XLIB, "xlib", 0, &cairogen_engine, &cairogen_features_x},
#endif
#endif
#endif
    {0, NULL, 0, NULL, NULL}
};
