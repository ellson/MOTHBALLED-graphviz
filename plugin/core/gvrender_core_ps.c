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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_LIBZ
#include <zlib.h>
#ifdef MSWIN32
#include <io.h>
#endif
#endif

#undef HAVE_LIBGD

#include "gvplugin_render.h"
#include "graph.h"
#include "agxbuf.h"
#include "utils.h"
#ifdef HAVE_LIBGD
#include "gd.h"
#endif

/* for CHAR_LATIN1  */
#include "const.h"

/*
 *     J$: added `pdfmark' URL embedding.  PostScript rendered from
 *         dot files with URL attributes will get active PDF links
 *         from Adobe's Distiller.
 */
#define PDFMAX  14400           /*  Maximum size of PDF page  */

extern void epsf_define(FILE * of);
extern char *ps_string(char *ins, int latin);

typedef enum { FORMAT_PS, FORMAT_PS2, } format_type;

static int isLatin1;
static char setupLatin1;

static char *last_fontname;
static double last_fontsize;
static gvcolor_t last_color;

static void psgen_begin_job(GVJ_t * job)
{
    last_fontname = NULL;
    last_fontsize = 0.0;
    last_color.u.HSV[0] = 0;
    last_color.u.HSV[1] = 0;
    last_color.u.HSV[2] = 0;

    fprintf(job->output_file, "%%!PS-Adobe-2.0\n");
    fprintf(job->output_file, "%%%%Creator: %s version %s (%s)\n",
	    job->common->info[0], job->common->info[1], job->common->info[2]);
    fprintf(job->output_file, "%%%%For: %s\n", job->common->user);
}

static void psgen_end_job(GVJ_t * job)
{
    fprintf(job->output_file, "%%%%Trailer\n");
    fprintf(job->output_file, "%%%%Pages: %d\n", job->common->viewNum);
    if (job->common->show_boxes == NULL)
	fprintf(job->output_file, "%%%%BoundingBox: %d %d %d %d\n",
	    job->boundingBox.LL.x, job->boundingBox.LL.y,
	    job->boundingBox.UR.x, job->boundingBox.UR.y);
    fprintf(job->output_file, "end\nrestore\n");
    fprintf(job->output_file, "%%%%EOF\n");
}

static void psgen_begin_graph(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    setupLatin1 = FALSE;

    if (job->common->viewNum == 0) {
        fprintf(job->output_file, "%%%%Title: %s\n", obj->g->name);
        fprintf(job->output_file, "%%%%Pages: (atend)\n");
        if (job->common->show_boxes == NULL)
            fprintf(job->output_file, "%%%%BoundingBox: (atend)\n");
        fprintf(job->output_file, "%%%%EndComments\nsave\n");
        cat_preamble(job, job->common->lib);
        epsf_define(job->output_file);
    }
    isLatin1 = (GD_charset(obj->g) == CHAR_LATIN1);
    if (isLatin1 && !setupLatin1) {
	fprintf(job->output_file, "setupLatin1\n");	/* as defined in ps header */
	setupLatin1 = TRUE;
    }
    /*  Set base URL for relative links (for Distiller >= 3.0)  */
    if (obj->url)
	fprintf(job->output_file, "[ {Catalog} << /URI << /Base (%s) >> >>\n"
		"/PUT pdfmark\n", obj->url);
}

static void psgen_end_graph(GVJ_t * job)
{
#if 0
    if (EPSF_contents) {
	dtclose(EPSF_contents);
	EPSF_contents = 0;
	N_EPSF_files = 0;
    }
    onetime = FALSE;
#endif
}

static void psgen_begin_layer(GVJ_t * job, char *layername, int layerNum, int numLayers)
{
    fprintf(job->output_file, "%d %d setlayer\n", layerNum, numLayers);
}

static void psgen_begin_page(GVJ_t * job)
{
    box pbr = job->pageBoundingBox;

    fprintf(job->output_file, "%%%%Page: %d %d\n",
	    job->common->viewNum + 1, job->common->viewNum + 1);
    if (job->common->show_boxes == NULL)
        fprintf(job->output_file, "%%%%PageBoundingBox: %d %d %d %d\n",
	    pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    fprintf(job->output_file, "%%%%PageOrientation: %s\n",
	    (job->rotation ? "Landscape" : "Portrait"));
    if (job->common->show_boxes == NULL)
        fprintf(job->output_file, "gsave\n%d %d %d %d boxprim clip newpath\n",
	    pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
	fprintf(job->output_file, "gsave %g %g set_scale %d rotate %g %g translate\n",
		job->scale.x, job->scale.y,
		job->rotation,
		job->translation.x, job->translation.y);

    /*  Define the size of the PS canvas  */
    if (job->render.id == FORMAT_PS2) {
	if (pbr.UR.x >= PDFMAX || pbr.UR.y >= PDFMAX)
	    job->common->errorfn("canvas size (%d,%d) exceeds PDF limit (%d)\n"
		  "\t(suggest setting a bounding box size, see dot(1))\n",
		  pbr.UR.x, pbr.UR.y, PDFMAX);
	fprintf(job->output_file, "[ /CropBox [%d %d %d %d] /PAGES pdfmark\n",
		pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    }
}

static void psgen_end_page(GVJ_t * job)
{
    if (job->common->show_boxes)
	cat_libfile(job->output_file, NULL, job->common->show_boxes + 1);
    /* the showpage is really a no-op, but at least one PS processor
     * out there needs to see this literal token.  endpage does the real work.
     */
    fprintf(job->output_file, "endpage\nshowpage\ngrestore\n");
    fprintf(job->output_file, "%%%%PageTrailer\n");
    fprintf(job->output_file, "%%%%EndPage: %d\n", job->common->viewNum);
}

static void psgen_begin_cluster(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    fprintf(job->output_file, "%% %s\n", obj->sg->name);

    if (obj->url &&  obj->url_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
        fprintf(job->output_file, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->url, isLatin1));
    }
}

static void psgen_begin_node(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    if (obj->url && obj->url_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
        fprintf(job->output_file, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->url, isLatin1));
    }
}

static void
psgen_begin_edge(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    if (obj->url && obj->url_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
        fprintf(job->output_file, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->url, isLatin1));
    }
    if (obj->tailurl && obj->tailurl_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->tailurl_map_p[0].x, obj->tailurl_map_p[0].y,
		obj->tailurl_map_p[1].x, obj->tailurl_map_p[1].y);
        fprintf(job->output_file, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->tailurl, isLatin1));
    }
    if (obj->headurl && obj->headurl_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->headurl_map_p[0].x, obj->headurl_map_p[0].y,
		obj->headurl_map_p[1].x, obj->headurl_map_p[1].y);
        fprintf(job->output_file, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->headurl, isLatin1));
    }
    if (obj->tailurl && obj->tailendurl_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->tailendurl_map_p[0].x, obj->tailendurl_map_p[0].y,
		obj->tailendurl_map_p[1].x, obj->tailendurl_map_p[1].y);
        fprintf(job->output_file, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->tailurl, isLatin1));
    }
    if (obj->headurl && obj->headendurl_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->headendurl_map_p[0].x, obj->headendurl_map_p[0].y,
		obj->headendurl_map_p[1].x, obj->headendurl_map_p[1].y);
        fprintf(job->output_file, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->headurl, isLatin1));
    }
}

static void
ps_set_pen_style(GVJ_t *job)
{
#if 0
    double penwidth = job->style->penwidth * job->zoom;
    char *p, *line, **s = job->rawstyle;

    fprintf(stderr,"%g setlinewidth\n", penwidth);

    while (s && (p = line = *s++)) {
	if (streq(line, "setlinewidth"))
	    continue;
	while (*p)
	    p++;
	p++;
	while (*p) {
            fprintf(stderr,"%s ", p);
	    while (*p)
		p++;
	    p++;
	}
//	if (streq(line, "invis"))
//	    job->style->width = 0;
	fprintf(stderr, "%s\n", line);
    }
#endif
}

static void ps_set_color(GVJ_t *job, gvcolor_t *color)
{
    char *objtype;

    if (color) {
	if (job->obj->g)
	   objtype = "graph";
	else if (job->obj->sg)
	   objtype = "cluster";
	else if (job->obj->n)
	   objtype = "node";
	else if (job->obj->e)
	   objtype = "edge";
	else
	   objtype = "sethsb";
	if ( last_color.u.HSV[0] != color->u.HSV[0]
	  || last_color.u.HSV[1] != color->u.HSV[1]
	  || last_color.u.HSV[2] != color->u.HSV[2]) {
	    fprintf(job->output_file, "%.3f %.3f %.3f %scolor\n",
		color->u.HSV[0], color->u.HSV[1], color->u.HSV[2], objtype);
	    last_color.u.HSV[0] = color->u.HSV[0];
	    last_color.u.HSV[1] = color->u.HSV[1];
	    last_color.u.HSV[2] = color->u.HSV[2];
	}
    }
}

static void psgen_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    double adj, sz;
    char *str;

    ps_set_color(job, &(job->style->pencolor));
    if (para->fontname) {
	sz = para->fontsize;
        if (sz != last_fontsize
          || last_fontname == NULL
	  || strcmp(para->fontname, last_fontname) != 0) {
	    fprintf(job->output_file, "%.2f /%s set_font\n", sz, para->fontname);
	    last_fontsize = sz;
	    last_fontname = para->fontname;
	}
    }

    str = ps_string(para->str,isLatin1);
    if (para->xshow) {
        switch (para->just) {
        case 'l':
            break;
        case 'r':
            p.x -= para->width;
            break;
        default:
        case 'n':
            p.x -= para->width / 2;
            break;
        }
        fprintf(job->output_file, "%g %g moveto\n%s\n[%s]\nxshow\n",
                p.x, p.y, str, para->xshow);
    } else {
        switch (para->just) {
        case 'l':
            adj = 0.0;
            break;
        case 'r':
            adj = -1.0;
            break;
        default:
        case 'n':
            adj = -0.5;
            break;
        }
        fprintf(job->output_file, "%g %g moveto %g %g %s alignedtext\n",
                p.x, p.y, para->width, adj, str);
    }
}

static void psgen_ellipse(GVJ_t * job, pointf * A, int filled)
{
    /* A[] contains 2 points: the center and corner. */

    if (filled) {
	ps_set_color(job, &(job->style->fillcolor));
	fprintf(job->output_file, "%g %g %g %g ellipse_path fill\n",
	    A[0].x, A[0].y, fabs(A[1].x - A[0].x), fabs(A[1].y - A[0].y));
    }
    ps_set_pen_style(job);
    ps_set_color(job, &(job->style->pencolor));
    fprintf(job->output_file, "%g %g %g %g ellipse_path stroke\n",
	A[0].x, A[0].y, fabs(A[1].x - A[0].x), fabs(A[1].y - A[0].y));
}

static void
psgen_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
	     int arrow_at_end, int filled)
{
    int j;

    if (filled) {
	ps_set_color(job, &(job->style->fillcolor));
	fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
	for (j = 1; j < n; j += 3)
	    fprintf(job->output_file, "%g %g %g %g %g %g curveto\n",
		A[j].x, A[j].y, A[j + 1].x, A[j + 1].y, A[j + 2].x,
		A[j + 2].y);
	fprintf(job->output_file, "closepath fill\n");
    }
    ps_set_pen_style(job);
    ps_set_color(job, &(job->style->pencolor));
    fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
    for (j = 1; j < n; j += 3)
	fprintf(job->output_file, "%g %g %g %g %g %g curveto\n",
		A[j].x, A[j].y, A[j + 1].x, A[j + 1].y, A[j + 2].x,
		A[j + 2].y);
    fprintf(job->output_file, "stroke\n");
}

static void psgen_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    int j;

    if (filled) {
	ps_set_color(job, &(job->style->fillcolor));
	fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
	for (j = 1; j < n; j++)
	    fprintf(job->output_file, "%g %g lineto\n", A[j].x, A[j].y);
	fprintf(job->output_file, "closepath fill\n");
    }
    ps_set_pen_style(job);
    ps_set_color(job, &(job->style->pencolor));
    fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
    for (j = 1; j < n; j++)
	fprintf(job->output_file, "%g %g lineto\n", A[j].x, A[j].y);
    fprintf(job->output_file, "closepath stroke\n");
}

static void psgen_polyline(GVJ_t * job, pointf * A, int n)
{
    int j;

    ps_set_pen_style(job);
    ps_set_color(job, &(job->style->pencolor));
    fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
    for (j = 1; j < n; j++)
	fprintf(job->output_file, "%g %g lineto\n", A[j].x, A[j].y);
    fprintf(job->output_file, "stroke\n");
}

static void psgen_comment(GVJ_t * job, char *str)
{
    fprintf(job->output_file, "%% %s\n", str);
}

static gvrender_engine_t psgen_engine = {
    psgen_begin_job,
    psgen_end_job,
    psgen_begin_graph,
    psgen_end_graph,
    psgen_begin_layer,
    0,				/* psgen_end_layer */
    psgen_begin_page,
    psgen_end_page,
    psgen_begin_cluster,
    0,				/* psgen_end_cluster */
    0,				/* psgen_begin_nodes */
    0,				/* psgen_end_nodes */
    0,				/* psgen_begin_edges */
    0,				/* psgen_end_edges */
    psgen_begin_node,
    0,				/* psgen_end_node */
    psgen_begin_edge,
    0,				/* psgen_end_edge */
    0,				/* psgen_begin_anchor */
    0,				/* psgen_end_anchor */
    psgen_textpara,
    0,				/* psgen_resolve_color */
    psgen_ellipse,
    psgen_polygon,
    psgen_bezier,
    psgen_polyline,
    psgen_comment,
};

static gvrender_features_t psgen_features = {
    GVRENDER_DOES_MULTIGRAPH_OUTPUT_FILES
	| GVRENDER_DOES_TRANSFORM
	| GVRENDER_DOES_MAPS
	| GVRENDER_DOES_MAP_RECT,
    36,				/* default margin - points */
    {72.,72.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    HSV_DOUBLE,			/* color_type */
    NULL,                       /* device */
    "ps",                       /* gvloadimage target for usershapes */
};

gvplugin_installed_t gvrender_core_ps_types[] = {
    {FORMAT_PS, "ps", -2, &psgen_engine, &psgen_features},
    {FORMAT_PS2, "ps2", -2, &psgen_engine, &psgen_features},
    {0, NULL, 0, NULL, NULL}
};
