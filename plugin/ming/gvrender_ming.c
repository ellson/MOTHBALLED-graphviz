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

#include "gvplugin_render.h"

#ifdef HAVE_LIBMING
#include <ming.h>

typedef enum { FORMAT_SWF } format_type; 

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

static void ming_begin_page(GVJ_t * job)
{
#if 0
    cairo_t *cr = NULL;
    cairo_surface_t *surface;

    if (job->surface)
        cr = (cairo_t *) job->surface;

    switch (job->render.id) {
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    case FORMAT_PNG:
	if (!cr) {
	    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
			job->width, job->height);
	    cr = cairo_create(surface);
	    cairo_surface_destroy (surface);
	}
	break;
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    case FORMAT_PS:
	if (!cr) {
	    surface = cairo_ps_surface_create_for_stream (writer,
			job->output_file, job->width, job->height);
	    cr = cairo_create(surface);
	    cairo_surface_destroy (surface);
	}
	break;
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    case FORMAT_PDF:
	if (!cr) {
	    surface = cairo_pdf_surface_create_for_stream (writer,
			job->output_file, job->width, job->height);
	    cr = cairo_create(surface);
	    cairo_surface_destroy (surface);
	}
	break;
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
    case FORMAT_SVG:
	if (!cr) {
	    surface = cairo_svg_surface_create_for_stream (writer,
			job->output_file, job->width, job->height);
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
#ifdef CAIRO_HAS_QUARTZ_SURFACE
    case FORMAT_QUARTZ:
	break;
#endif
    default:
	break;
    }
    job->surface = (void *) cr;

    cairo_save(cr);
    cairo_scale(cr, job->scale.x, job->scale.y);
    cairo_rotate(cr, -job->rotation * M_PI / 180.);
    cairo_translate(cr, job->translation.x, -job->translation.y);
#endif
}

static void ming_end_page(GVJ_t * job)
{
#if 0
    cairo_t *cr = (cairo_t *) job->surface;
    cairo_surface_t *surface;

    switch (job->render.id) {
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    case FORMAT_PNG:
        surface = cairo_get_target(cr);
	cairo_surface_write_to_png_stream(surface, writer, job->output_file);
	break;
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
    if (job->external_surface)
	cairo_restore(cr);
    else {
	cairo_destroy(cr);
	job->surface = NULL;
    }

#if defined HAVE_FENV_H && defined HAVE_FESETENV && defined HAVE_FEGETENV && defined(HAVE_FEENABLEEXCEPT)
    /* Restore FP environment */
    fesetenv(&fenv);
#endif
#endif
}

static void ming_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
#if 0
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->surface;
    pointf offset;
    PangoLayout *layout = (PangoLayout*)(para->layout);
    PangoLayoutIter* iter;

    cairo_set_dash (cr, dashed, 0, 0.0);  /* clear any dashing */
    pango_set_color(cr, &(obj->pencolor));

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
#endif
}

static void ming_ellipse(GVJ_t * job, pointf * A, int filled)
{
#if 0
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->surface;
    cairo_matrix_t matrix;
    double rx, ry;

    pango_set_penstyle(job, cr);

    cairo_get_matrix(cr, &matrix);
    cairo_translate(cr, A[0].x, -A[0].y);

    rx = A[1].x - A[0].x;
    ry = A[1].y - A[0].y;
    cairo_scale(cr, 1, ry / rx);
    cairo_move_to(cr, rx, 0);
    cairo_arc(cr, 0, 0, rx, 0, 2 * M_PI);
    cairo_close_path(cr);

    cairo_set_matrix(cr, &matrix);

    if (filled) {
	pango_set_color(cr, &(obj->fillcolor));
	cairo_fill_preserve(cr);
    }
    pango_set_color(cr, &(obj->pencolor));
    cairo_stroke(cr);
#endif
}

static void
ming_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
#if 0
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->surface;
    int i;

    pango_set_penstyle(job, cr);

    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i++)
	cairo_line_to(cr, A[i].x, -A[i].y);
    cairo_close_path(cr);
    if (filled) {
	pango_set_color(cr, &(obj->fillcolor));
	cairo_fill_preserve(cr);
    }
    pango_set_color(cr, &(obj->pencolor));
    cairo_stroke(cr);
#endif
}

static void
ming_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
		int arrow_at_end, int filled)
{
#if 0
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->surface;
    int i;

    pango_set_penstyle(job, cr);

    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i += 3)
	cairo_curve_to(cr, A[i].x, -A[i].y, A[i + 1].x, -A[i + 1].y,
		       A[i + 2].x, -A[i + 2].y);
    pango_set_color(cr, &(obj->pencolor));
    cairo_stroke(cr);
#endif
}

static void
ming_polyline(GVJ_t * job, pointf * A, int n)
{
#if 0
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->surface;
    int i;

    pango_set_penstyle(job, cr);

    cairo_set_line_width (cr, obj->penwidth * job->scale.x);
    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i++)
	cairo_line_to(cr, A[i].x, -A[i].y);
    pango_set_color(cr, &(obj->pencolor));
    cairo_stroke(cr);
#endif
}

static gvrender_engine_t ming_engine = {
    0,				/* ming_begin_job */
    0,				/* ming_end_job */
    0,				/* ming_begin_graph */
    0,				/* ming_end_graph */
    0,				/* ming_begin_layer */
    0,				/* ming_end_layer */
    ming_begin_page,
    ming_end_page,
    0,				/* ming_begin_cluster */
    0,				/* ming_end_cluster */
    0,				/* ming_begin_nodes */
    0,				/* ming_end_nodes */
    0,				/* ming_begin_edges */
    0,				/* ming_end_edges */
    0,				/* ming_begin_node */
    0,				/* ming_end_node */
    0,				/* ming_begin_edge */
    0,				/* ming_end_edge */
    0,				/* ming_begin_anchor */
    0,				/* ming_end_anchor */
    ming_textpara,
    0,				/* ming_resolve_color */
    ming_ellipse,
    ming_polygon,
    ming_bezier,
    ming_polyline,
    0,				/* ming_comment */
};

static gvrender_features_t ming_features = {
    GVRENDER_DOES_TRUECOLOR
	| GVRENDER_Y_GOES_DOWN
	| GVRENDER_DOES_TRANSFORM, /* flags */
    0,				/* default margin - points */
    4.,                         /* default pad - graph units */
    {0.,0.},                    /* default page width, height - points */
    {72.,72.},			/* default dpi */
    0,				/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_DOUBLE,		/* color_type */
    NULL,			/* device */
    NULL,			/* gvloadimage target for usershapes */
};
#endif

gvplugin_installed_t gvrender_ming_types[] = {
#ifdef HAVE_LIBMING
    {FORMAT_SWF, "swf", 10, &ming_engine, &ming_features},
#endif
    {0, NULL, 0, NULL, NULL}
};
