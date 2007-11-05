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
#include <fcntl.h>

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

#if 0
# include <fenv.h>
#elif HAVE_FPU_CONTROL_H
# include <fpu_control.h>
#elif HAVE_SYS_FPU_H
# include <sys/fpu.h>
#endif
#endif

#include "gvplugin_render.h"
#include "gvplugin_device.h"

#ifdef HAVE_PANGOCAIRO
#include <pango/pangocairo.h>

typedef enum {
		FORMAT_GLITZ,
		FORMAT_CAIRO,
		FORMAT_PNG,
		FORMAT_PS,
		FORMAT_PDF,
		FORMAT_QUARTZ,
		FORMAT_SVG,
    } format_type;

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

/* FIXME - FONT_DPI also defined in gvtextlayout_pango.c - need shared header */
#define FONT_DPI 96.

static double dashed[] = {6.};
static int dashed_len = ARRAY_SIZE(dashed);

static double dotted[] = {2., 6.};
static int dotted_len = ARRAY_SIZE(dotted);

#ifdef CAIRO_HAS_PS_SURFACE
#include <cairo-ps.h>
#endif

#ifdef CAIRO_HAS_PDF_SURFACE
#include <cairo-pdf.h>
#endif

#ifdef CAIRO_HAS_SVG_SURFACE
#include <cairo-svg.h>
#endif

#ifdef CAIRO_HAS_QUARTZ_SURFACE
#include <cairo-quartz.h>
#endif

#ifdef CAIRO_HAS_GLITZ
#include <cairo-glitz.h>
#endif

#if 0
#if defined(HAVE_FENV_H) && defined(HAVE_FESETENV) && defined(HAVE_FEGETENV) && defined(HAVE_FEENABLEEXCEPT)
/* place to save fp environment temporarily */
static fenv_t fenv; /* FIXME - not thread safe */
#endif
#endif

static void cairogen_set_color(cairo_t * cr, gvcolor_t * color)
{
    cairo_set_source_rgba(cr, color->u.RGBA[0], color->u.RGBA[1],
                        color->u.RGBA[2], color->u.RGBA[3]);
}

extern size_t gvdevice_write(GVJ_t * job, const unsigned char *s, unsigned int len);

static cairo_status_t
writer (void *closure, const unsigned char *data, unsigned int length)
{
    if (length == gvdevice_write((GVJ_t *)closure, data, length))
	return CAIRO_STATUS_SUCCESS;
    return CAIRO_STATUS_WRITE_ERROR;
}

static void cairogen_begin_page(GVJ_t * job)
{
    cairo_t *cr = NULL;
    cairo_surface_t *surface;

#if 0
#if defined(HAVE_FENV_H) && defined(HAVE_FESETENV) && defined(HAVE_FEGETENV) && defined(HAVE_FEDISABLEEXCEPT)
    /* cairo generates FE_INVALID and other exceptions we 
     * want to ignore for now.  Save the current fenv and
     * set one just for cairo.
     * The fenv is restored in cairogen_end_graph  */
    fegetenv(&fenv);
    fedisableexcept(FE_ALL_EXCEPT);
#endif
#endif

    if (job->context) 
	cr = (cairo_t *) job->context;
    if (job->external_context && cr)
        cairo_save(cr);
    else {
	if (cr)
	    cairo_destroy(cr);
        switch (job->render.id) {
        case FORMAT_PS:
	    surface = cairo_ps_surface_create_for_stream (writer,
			job, job->width, job->height);
	    break;
        case FORMAT_PDF:
	    surface = cairo_pdf_surface_create_for_stream (writer,
			job, job->width, job->height);
	    break;
        case FORMAT_SVG:
	    surface = cairo_svg_surface_create_for_stream (writer,
			job, job->width, job->height);
	    break;
        case FORMAT_CAIRO:
        case FORMAT_PNG:
        default:
	    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
			job->width, job->height);
	    break;
        }
        cr = cairo_create(surface);
        cairo_surface_destroy (surface);
        job->context = (void *) cr;
    }

    cairo_scale(cr, job->scale.x, job->scale.y);
    cairo_rotate(cr, -job->rotation * M_PI / 180.);
    cairo_translate(cr, job->translation.x, -job->translation.y);

    cairo_rectangle(cr,
	    job->clip.LL.x,
	    - job->clip.LL.y,
	    job->clip.UR.x - job->clip.LL.x,
	    - (job->clip.UR.y - job->clip.LL.y));
    cairo_clip(cr);
}

static void cairogen_end_page(GVJ_t * job)
{
    cairo_t *cr = (cairo_t *) job->context;
    cairo_surface_t *surface;

    switch (job->render.id) {

#ifdef CAIRO_HAS_PNG_FUNCTIONS
    case FORMAT_PNG:
        surface = cairo_get_target(cr);
	cairo_surface_write_to_png_stream(surface, writer, job);
	break;
#endif

    case FORMAT_PS:
    case FORMAT_PDF:
    case FORMAT_SVG:
	cairo_show_page(cr);
	cairo_destroy(cr);
	job->context = NULL;
	break;

    case FORMAT_CAIRO:
    default:
        surface = cairo_get_target(cr);
	job->imagedata = cairo_image_surface_get_data(surface);	
	break;
       	/* formatting will be done by gvdevice_format() */
    }

    if (job->external_context)
	cairo_restore(cr);

#if 0
#if defined HAVE_FENV_H && defined HAVE_FESETENV && defined HAVE_FEGETENV && defined(HAVE_FEENABLEEXCEPT)
    /* Restore FP environment */
    fesetenv(&fenv);
#endif
#endif
}

static void cairogen_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->context;

    cairo_set_dash (cr, dashed, 0, 0.0);  /* clear any dashing */
    cairogen_set_color(cr, &(obj->pencolor));

    switch (para->just) {
    case 'r':
	p.x -= para->width;
	break;
    case 'l':
	p.x -= 0.0;
	break;
    case 'n':
    default:
	p.x -= para->width / 2.0;
	break;
    }
    p.y += para->yoffset_centerline + para->yoffset_layout;

    cairo_move_to (cr, p.x, -p.y);
    cairo_save(cr);
    cairo_scale(cr, POINTS_PER_INCH / FONT_DPI, POINTS_PER_INCH / FONT_DPI);
    pango_cairo_show_layout(cr, (PangoLayout*)(para->layout));
    cairo_restore(cr);
}

static void cairogen_set_penstyle(GVJ_t *job, cairo_t *cr)
{
    obj_state_t *obj = job->obj;

    if (obj->pen == PEN_DASHED) {
	cairo_set_dash (cr, dashed, dashed_len, 0.0);
    } else if (obj->pen == PEN_DOTTED) {
	cairo_set_dash (cr, dotted, dotted_len, 0.0);
    } else {
	cairo_set_dash (cr, dashed, 0, 0.0);
    }
    cairo_set_line_width (cr, obj->penwidth);

}

static void cairogen_ellipse(GVJ_t * job, pointf * A, int filled)
{
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->context;
    cairo_matrix_t matrix;
    double rx, ry;

    cairogen_set_penstyle(job, cr);

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
	cairogen_set_color(cr, &(obj->fillcolor));
	cairo_fill_preserve(cr);
    }
    cairogen_set_color(cr, &(obj->pencolor));
    cairo_stroke(cr);
}

static void
cairogen_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->context;
    int i;

    cairogen_set_penstyle(job, cr);

    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i++)
	cairo_line_to(cr, A[i].x, -A[i].y);
    cairo_close_path(cr);
    if (filled) {
	cairogen_set_color(cr, &(obj->fillcolor));
	cairo_fill_preserve(cr);
    }
    cairogen_set_color(cr, &(obj->pencolor));
    cairo_stroke(cr);
}

static void
cairogen_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
		int arrow_at_end, int filled)
{
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->context;
    int i;

    cairogen_set_penstyle(job, cr);

    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i += 3)
	cairo_curve_to(cr, A[i].x, -A[i].y, A[i + 1].x, -A[i + 1].y,
		       A[i + 2].x, -A[i + 2].y);
    cairogen_set_color(cr, &(obj->pencolor));
    cairo_stroke(cr);
}

static void
cairogen_polyline(GVJ_t * job, pointf * A, int n)
{
    obj_state_t *obj = job->obj;
    cairo_t *cr = (cairo_t *) job->context;
    int i;

    cairogen_set_penstyle(job, cr);

    cairo_set_line_width (cr, obj->penwidth * job->scale.x);
    cairo_move_to(cr, A[0].x, -A[0].y);
    for (i = 1; i < n; i++)
	cairo_line_to(cr, A[i].x, -A[i].y);
    cairogen_set_color(cr, &(obj->pencolor));
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
    0,				/* cairogen_library_shape */
};

static gvrender_features_t render_features_cairo = {
    GVRENDER_Y_GOES_DOWN
	| GVRENDER_DOES_TRANSFORM, /* flags */
    4.,                         /* default pad - graph units */
    0,				/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_DOUBLE,		/* color_type */
};

static gvdevice_features_t device_features_png = {
    GVDEVICE_BINARY_FORMAT
      | GVDEVICE_DOES_TRUECOLOR,/* flags */
    {0.,0.},			/* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {96.,96.},			/* typical monitor dpi */
};

static gvdevice_features_t device_features_ps = {
    GVDEVICE_DOES_TRUECOLOR,    /* flags */
    {36.,36.},			/* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {72.,72.},			/* postscript 72 dpi */
};

static gvdevice_features_t device_features_svg = {
    GVDEVICE_DOES_TRUECOLOR,    /* flags */
    {0.,0.},			/* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {72.,72.},			/* svg 72 dpi */
};
#endif

gvplugin_installed_t gvrender_pango_types[] = {
#ifdef HAVE_PANGOCAIRO
    {FORMAT_CAIRO, "cairo", 10, &cairogen_engine, &render_features_cairo},
#endif
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_pango_types[] = {
#ifdef HAVE_PANGOCAIRO
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    {FORMAT_PNG, "png:cairo", 10, NULL, &device_features_png},
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    {FORMAT_PS, "ps:cairo", -10, NULL, &device_features_ps},
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    {FORMAT_PDF, "pdf:cairo", 10, NULL, &device_features_ps},
#endif
#ifdef CAIRO_HAS_SVG_SURFACE
    {FORMAT_SVG, "svg:cairo", -10, NULL, &device_features_svg},
#endif
//#ifdef CAIRO_HAS_XCB_SURFACE
//    {FORMAT_XCB, "xcb:cairo", 0, NULL, &device_features_xcb},
//#endif
//#ifdef CAIRO_HAS_SDL_SURFACE
//    {FORMAT_SDL, "sdl:cairo", 0, NULL, &device_features_sdl},
//#endif
//#ifdef CAIRO_HAS_GLITZ_SURFACE
//    {FORMAT_GLITZ, "glitz:cairo", 0, NULL, &device_features_glitz},
//#endif
//#ifdef CAIRO_HAS_QUARTZ_SURFACE
//    {FORMAT_QUARTZ, "quartz:cairo", 0, NULL, &device_features_quartz},
//#endif
//#ifdef CAIRO_HAS_WIN32_SURFACE
//    {FORMAT_WIN32, "win32:cairo", 0, NULL, &device_features_win32},
//#endif
#endif
    {0, NULL, 0, NULL, NULL}
};
