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

#include <stdlib.h>
#include <string.h>
#include "gvplugin_render.h"

#include <ming.h>

#define SWFVERSION 6
#define SWFCOMPRESSION 9
#define SWFFRAMERATE .5

typedef enum { FORMAT_SWF } format_type; 

static void ming_begin_job(GVJ_t * job)
{
    SWFMovie movie;

    Ming_init();
    Ming_useSWFVersion(SWFVERSION);
    Ming_setSWFCompression(SWFCOMPRESSION);
    movie = newSWFMovie();
    SWFMovie_setRate(movie, SWFFRAMERATE);
    SWFMovie_setDimension(movie, job->width, job->height);

    job->surface = (void*) movie;
}

static void ming_end_job(GVJ_t * job)
{
    SWFMovie movie = (SWFMovie)(job->surface);

    SWFMovie_output_to_stream(movie, job->output_file);
    destroySWFMovie(movie);
    job->surface = NULL;
}

static void ming_begin_page(GVJ_t * job)
{
//    SWFMovie movie = (SWFMovie)(job->surface);

//    SWFMovie_setNumberOfFrames(movie, job->common->viewNum + 1);

#if 0
    cairo_scale(cr, job->scale.x, job->scale.y);
    cairo_rotate(cr, -job->rotation * M_PI / 180.);
    cairo_translate(cr, job->translation.x, -job->translation.y);
#endif
}

static void ming_end_page(GVJ_t * job)
{
    SWFMovie movie = (SWFMovie)(job->surface);

    SWFMovie_nextFrame(movie);
}

extern char* gvconfig_libdir(void);
#define FONT "Bitstream_Vera_Serif.fdb"

static void ming_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    SWFMovie movie = (SWFMovie)(job->surface);
    SWFTextField textfield;
    SWFDisplayItem item;
    obj_state_t *obj = job->obj;
    gvcolor_t pencolor = obj->pencolor;
    pointf offset;
    char *font_file_name;
    FILE *font_file;
    char *libdir;
    static SWFFont font;

/* FIXME - hardcoded to a Times-like font */
    if (font == NULL) {
    	libdir=gvconfig_libdir();
	font_file_name = malloc(strlen(libdir)+strlen(FONT)+2);
	strcpy(font_file_name, libdir);
	strcat(font_file_name, "/");
	strcat(font_file_name, FONT);
	font_file = fopen(font_file_name, "r");
	if (font_file == NULL ) {
	    perror(font_file_name);
	    free(font_file_name);
	    exit(1);
	}
	font = loadSWFFontFromFile(font_file);
	fclose(font_file);
	free(font_file_name);
    }

    textfield = newSWFTextField();
    SWFTextField_setFont(textfield, (SWFBlock)font);
    SWFTextField_addChars(textfield, para->str);
    SWFTextField_addUTF8String(textfield, para->str);
    SWFTextField_setColor(textfield,
	 pencolor.u.rgba[0],
	 pencolor.u.rgba[1],
	 pencolor.u.rgba[2],
	 pencolor.u.rgba[3]);
    SWFTextField_setHeight(textfield, para->fontsize);

    switch (para->just) {
    case 'r':
	offset.x = 0.;
	break;
    case 'l':
	offset.x = -para->width;
	break;
    case 'n':
    default:
	offset.x = -para->width/2.;
	break;
    }
    /* offset to baseline */
    offset.y = -para->height + para->fontsize*.4;  /* empirically determined */

    item = SWFMovie_add(movie, (SWFBlock)textfield);
    SWFDisplayItem_moveTo(item, p.x + offset.x, p.y + offset.y);
}

static void ming_ellipse(GVJ_t * job, pointf * A, int filled)
{
    SWFMovie movie = (SWFMovie)(job->surface);
    SWFShape shape;
    SWFFill fill;
    SWFDisplayItem item;
    obj_state_t *obj = job->obj;
    gvcolor_t pencolor = obj->pencolor;
    gvcolor_t fillcolor = obj->fillcolor;
    double rx, ry;

    shape = newSWFShape();
    SWFShape_setLineStyle(shape, obj->penwidth * 20,
	 pencolor.u.rgba[0],
	 pencolor.u.rgba[1],
	 pencolor.u.rgba[2],
	 pencolor.u.rgba[3]);
    if (filled) {
	fill = SWFShape_addSolidFill(shape,
	    fillcolor.u.rgba[0],
	    fillcolor.u.rgba[1],
	    fillcolor.u.rgba[2],
	    fillcolor.u.rgba[3]);
	SWFShape_setRightFill(shape, fill);
    }
    SWFShape_movePenTo(shape, 0, 0);
    rx = A[1].x - A[0].x;
    ry = A[1].y - A[0].y;
    SWFShape_drawCircle(shape, rx);
    item = SWFMovie_add(movie, (SWFBlock)shape);
    SWFDisplayItem_scale(item, 1., ry/rx);
    SWFDisplayItem_moveTo(item, A[0].x, A[0].y);
}

static void
ming_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    SWFMovie movie = (SWFMovie)(job->surface);
    SWFShape shape;
    SWFFill fill;
    obj_state_t *obj = job->obj;
    gvcolor_t pencolor = obj->pencolor;
    gvcolor_t fillcolor = obj->fillcolor;
    int i;

    shape = newSWFShape();
    SWFShape_setLineStyle(shape, obj->penwidth * 20,
	 pencolor.u.rgba[0],
	 pencolor.u.rgba[1],
	 pencolor.u.rgba[2],
	 pencolor.u.rgba[3]);
    if (filled) {
	fill = SWFShape_addSolidFill(shape,
	    fillcolor.u.rgba[0],
	    fillcolor.u.rgba[1],
	    fillcolor.u.rgba[2],
	    fillcolor.u.rgba[3]);
	SWFShape_setRightFill(shape, fill);
    }
    SWFShape_movePenTo(shape, A[0].x, A[0].y);
    for (i = 1; i < n; i++)
	SWFShape_drawLineTo(shape, A[i].x, A[i].y);
    SWFShape_drawLineTo(shape, A[0].x, A[0].y);
    SWFMovie_add(movie, (SWFBlock)shape);
}

static void
ming_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
		int arrow_at_end, int filled)
{
    SWFMovie movie = (SWFMovie)(job->surface);
    SWFShape shape;
    obj_state_t *obj = job->obj;
    gvcolor_t pencolor = obj->pencolor;
    int i;

    shape = newSWFShape();
    SWFShape_setLineStyle(shape, obj->penwidth * 20,
	 pencolor.u.rgba[0],
	 pencolor.u.rgba[1],
	 pencolor.u.rgba[2],
	 pencolor.u.rgba[3]);
    SWFShape_movePenTo(shape, A[0].x, A[0].y);
    for (i = 1; i < n; i+=3)
	SWFShape_drawCubicTo(shape,
		A[i].x, A[i].y, A[i+1].x, A[i+1].y, A[i+2].x, A[i+2].y);
    SWFMovie_add(movie, (SWFBlock)shape);
}

static void
ming_polyline(GVJ_t * job, pointf * A, int n)
{
    SWFMovie movie = (SWFMovie)(job->surface);
    SWFShape shape;
    obj_state_t *obj = job->obj;
    gvcolor_t pencolor = obj->pencolor;
    int i;

    shape = newSWFShape();
    SWFShape_setLineStyle(shape, obj->penwidth * 20,
	 pencolor.u.rgba[0],
	 pencolor.u.rgba[1],
	 pencolor.u.rgba[2],
	 pencolor.u.rgba[3]);
    SWFShape_movePenTo(shape, A[0].x, A[0].y);
    for (i = 1; i < n; i++)
	SWFShape_drawLineTo(shape, A[i].x, A[i].y);
    SWFMovie_add(movie, (SWFBlock)shape);
}

static gvrender_engine_t ming_engine = {
    ming_begin_job,
    ming_end_job,
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
    0,				/* ming_library_shape */
};

static gvrender_features_t ming_features = {
    (	GVRENDER_DOES_MULTIGRAPHS
	| GVRENDER_DOES_TRUECOLOR
	| GVRENDER_Y_GOES_DOWN
//	| GVRENDER_DOES_TRANSFORM
    ),				/* feature flags */
    0,				/* default margin - points */
    4.,                         /* default pad - graph units */
    {0.,0.},                    /* default page width, height - points */
    {96.,96.},			/* default dpi */
    0,				/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_BYTE,			/* color_type */
    NULL,			/* imageloader for usershapes */
};

gvplugin_installed_t gvrender_ming_types[] = {
    {FORMAT_SWF, "ming", 10, &ming_engine, NULL},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_ming_types[] = {
    {FORMAT_SWF, "swf:ming", 10, NULL, &ming_features},
    {0, NULL, 0, NULL, NULL}
};
