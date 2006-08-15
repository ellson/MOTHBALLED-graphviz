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
#endif

#ifdef WIN32
#include <io.h>
#include "compat.h"
#endif

#include "gvplugin_render.h"
#include "graph.h"
#include "agxbuf.h"
#include "utils.h"

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
extern void core_fputs(GVJ_t * job, char *s);
extern void core_printf(GVJ_t * job, const char *format, ...);

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
    last_color.u.HSVA[0] = 0.0;
    last_color.u.HSVA[1] = 0.0;
    last_color.u.HSVA[2] = 0.0;
    last_color.u.HSVA[3] = 1.0;  /* opaque */

    core_fputs(job, "%%!PS-Adobe-2.0\n");
    core_printf(job, "%%%%Creator: %s version %s (%s)\n",
	    job->common->info[0], job->common->info[1], job->common->info[2]);
    core_printf(job, "%%%%For: %s\n", job->common->user);
}

static void psgen_end_job(GVJ_t * job)
{
    core_fputs(job, "%%Trailer\n");
    core_printf(job, "%%%%Pages: %d\n", job->common->viewNum);
    if (job->common->show_boxes == NULL)
	core_printf(job, "%%%%BoundingBox: %d %d %d %d\n",
	    job->boundingBox.LL.x, job->boundingBox.LL.y,
	    job->boundingBox.UR.x, job->boundingBox.UR.y);
    core_fputs(job, "end\nrestore\n");
    core_fputs(job, "%%EOF\n");
}

static void psgen_begin_graph(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    setupLatin1 = FALSE;

    if (job->common->viewNum == 0) {
        core_printf(job, "%%%%Title: %s\n", obj->u.g->name);
        core_fputs(job, "%%%%Pages: (atend)\n");
        if (job->common->show_boxes == NULL)
            core_fputs(job, "%%%%BoundingBox: (atend)\n");
        core_fputs(job, "%%%%EndComments\nsave\n");
        cat_preamble(job, job->common->lib);
        epsf_define(job->output_file);
    }
    isLatin1 = (GD_charset(obj->u.g) == CHAR_LATIN1);
    if (isLatin1 && !setupLatin1) {
	core_fputs(job, "setupLatin1\n");	/* as defined in ps header */
	setupLatin1 = TRUE;
    }
    /*  Set base URL for relative links (for Distiller >= 3.0)  */
    if (obj->url)
	core_printf(job, "[ {Catalog} << /URI << /Base (%s) >> >>\n"
		"/PUT pdfmark\n", obj->url);
}

static void psgen_begin_layer(GVJ_t * job, char *layername, int layerNum, int numLayers)
{
    core_printf(job, "%d %d setlayer\n", layerNum, numLayers);
}

static void psgen_begin_page(GVJ_t * job)
{
    box pbr = job->pageBoundingBox;

// FIXME
    point page = {0,0};
    int N_pages = 0;

    core_printf(job, "%%%%Page: %d %d\n",
	    job->common->viewNum + 1, job->common->viewNum + 1);
    if (job->common->show_boxes == NULL)
        core_printf(job, "%%%%PageBoundingBox: %d %d %d %d\n",
	    pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    core_printf(job, "%%%%PageOrientation: %s\n",
	    (job->rotation ? "Landscape" : "Portrait"));
    if (job->render.id == FORMAT_PS2)
        core_printf(job, "<< /PageSize [%d %d] >> setpagedevice\n",
            pbr.UR.x, pbr.UR.y);
    if (job->common->show_boxes == NULL)
        core_printf(job, "gsave\n%d %d %d %d boxprim clip newpath\n",
	    pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    core_printf(job, "%d %d %d beginpage\n", page.x, page.y, N_pages);
    core_printf(job, "%g %g set_scale %d rotate %g %g translate\n",
	    job->scale.x, job->scale.y,
	    job->rotation,
	    job->translation.x, job->translation.y);

    /*  Define the size of the PS canvas  */
    if (job->render.id == FORMAT_PS2) {
	if (pbr.UR.x >= PDFMAX || pbr.UR.y >= PDFMAX)
	    job->common->errorfn("canvas size (%d,%d) exceeds PDF limit (%d)\n"
		  "\t(suggest setting a bounding box size, see dot(1))\n",
		  pbr.UR.x, pbr.UR.y, PDFMAX);
	core_printf(job, "[ /CropBox [%d %d %d %d] /PAGES pdfmark\n",
		pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    }
}

static void psgen_end_page(GVJ_t * job)
{
//    if (job->common->show_boxes)
//	cat_libfile(job->output_file, NULL, job->common->show_boxes + 1);
    /* the showpage is really a no-op, but at least one PS processor
     * out there needs to see this literal token.  endpage does the real work.
     */
    core_fputs(job, "endpage\nshowpage\ngrestore\n");
    core_fputs(job, "%%%%PageTrailer\n");
    core_printf(job, "%%%%EndPage: %d\n", job->common->viewNum);
}

static void psgen_begin_cluster(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    core_printf(job, "%% %s\n", obj->u.sg->name);

    if (obj->url &&  obj->url_map_p) {
        core_printf(job, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
        core_printf(job, "  /Border [ 0 0 0 ]\n"
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
        core_printf(job, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
        core_printf(job, "  /Border [ 0 0 0 ]\n"
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
        core_printf(job, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
        core_printf(job, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->url, isLatin1));
    }
    if (obj->tailurl && obj->tailurl_map_p) {
        core_printf(job, "[ /Rect [ %g %g %g %g ]\n",
		obj->tailurl_map_p[0].x, obj->tailurl_map_p[0].y,
		obj->tailurl_map_p[1].x, obj->tailurl_map_p[1].y);
        core_printf(job, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->tailurl, isLatin1));
    }
    if (obj->headurl && obj->headurl_map_p) {
        core_printf(job, "[ /Rect [ %g %g %g %g ]\n",
		obj->headurl_map_p[0].x, obj->headurl_map_p[0].y,
		obj->headurl_map_p[1].x, obj->headurl_map_p[1].y);
        core_printf(job, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->headurl, isLatin1));
    }
    if (obj->tailurl && obj->tailendurl_map_p) {
        core_printf(job, "[ /Rect [ %g %g %g %g ]\n",
		obj->tailendurl_map_p[0].x, obj->tailendurl_map_p[0].y,
		obj->tailendurl_map_p[1].x, obj->tailendurl_map_p[1].y);
        core_printf(job, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->tailurl, isLatin1));
    }
    if (obj->headurl && obj->headendurl_map_p) {
        core_printf(job, "[ /Rect [ %g %g %g %g ]\n",
		obj->headendurl_map_p[0].x, obj->headendurl_map_p[0].y,
		obj->headendurl_map_p[1].x, obj->headendurl_map_p[1].y);
        core_printf(job, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(obj->headurl, isLatin1));
    }
}

static void
ps_set_pen_style(GVJ_t *job)
{
    double penwidth = job->obj->penwidth * job->zoom;
    char *p, *line, **s = job->obj->rawstyle;
    FILE *out = job->output_file;

    fprintf(out,"%g setlinewidth\n", penwidth);

    while (s && (p = line = *s++)) {
	if (strcmp(line, "setlinewidth") == 0)
	    continue;
	while (*p)
	    p++;
	p++;
	while (*p) {
            fprintf(out,"%s ", p);
	    while (*p)
		p++;
	    p++;
	}
	if (strcmp(line, "invis") == 0)
	    job->obj->penwidth = 0;
	fprintf(out, "%s\n", line);
    }
}

static void ps_set_color(GVJ_t *job, gvcolor_t *color)
{
    char *objtype;

    if (color) {
	switch (job->obj->type) {
	    case ROOTGRAPH_OBJTYPE:
	    case CLUSTER_OBJTYPE:
		objtype = "graph";
		break;
	    case NODE_OBJTYPE:
		objtype = "node";
		break;
	    case EDGE_OBJTYPE:
		objtype = "edge";
		break;
	    default:
		objtype = "sethsb";
		break;
	}
	if ( last_color.u.HSVA[0] != color->u.HSVA[0]
	  || last_color.u.HSVA[1] != color->u.HSVA[1]
	  || last_color.u.HSVA[2] != color->u.HSVA[2]
	  || last_color.u.HSVA[3] != color->u.HSVA[3]
	  || (job->obj->type == ROOTGRAPH_OBJTYPE)) {
	    core_printf(job, "%.3f %.3f %.3f %scolor\n",
		color->u.HSVA[0], color->u.HSVA[1], color->u.HSVA[2], objtype);
	    last_color.u.HSVA[0] = color->u.HSVA[0];
	    last_color.u.HSVA[1] = color->u.HSVA[1];
	    last_color.u.HSVA[2] = color->u.HSVA[2];
	    last_color.u.HSVA[3] = color->u.HSVA[3];
	}
    }
}

static void psgen_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    double adj, sz;
    char *str;

    if (job->obj->pencolor.u.HSVA[3] < .5)
	return;  /* skip transparent text */

    ps_set_color(job, &(job->obj->pencolor));
    if (para->fontname) {
	sz = para->fontsize;
        if (sz != last_fontsize
          || last_fontname == NULL
	  || strcmp(para->fontname, last_fontname) != 0) {
	    core_printf(job, "%.2f /%s set_font\n", sz, para->fontname);
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
        core_printf(job, "%g %g moveto\n%s\n[%s]\nxshow\n",
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
        core_printf(job, "%g %g moveto %g %g %s alignedtext\n",
                p.x, p.y, para->width, adj, str);
    }
}

static void psgen_ellipse(GVJ_t * job, pointf * A, int filled)
{
    /* A[] contains 2 points: the center and corner. */

    if (filled && job->obj->fillcolor.u.HSVA[3] > .5) {
	ps_set_color(job, &(job->obj->fillcolor));
	core_printf(job, "%g %g %g %g ellipse_path fill\n",
	    A[0].x, A[0].y, fabs(A[1].x - A[0].x), fabs(A[1].y - A[0].y));
    }
    if (job->obj->pencolor.u.HSVA[3] > .5) {
        ps_set_pen_style(job);
        ps_set_color(job, &(job->obj->pencolor));
        core_printf(job, "%g %g %g %g ellipse_path stroke\n",
	    A[0].x, A[0].y, fabs(A[1].x - A[0].x), fabs(A[1].y - A[0].y));
    }
}

static void
psgen_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
	     int arrow_at_end, int filled)
{
    int j;

    if (filled && job->obj->fillcolor.u.HSVA[3] > .5) {
	ps_set_color(job, &(job->obj->fillcolor));
	core_printf(job, "newpath %g %g moveto\n", A[0].x, A[0].y);
	for (j = 1; j < n; j += 3)
	    core_printf(job, "%g %g %g %g %g %g curveto\n",
		A[j].x, A[j].y, A[j + 1].x, A[j + 1].y, A[j + 2].x,
		A[j + 2].y);
	core_fputs(job, "closepath fill\n");
    }
    if (job->obj->pencolor.u.HSVA[3] > .5) {
        ps_set_pen_style(job);
        ps_set_color(job, &(job->obj->pencolor));
        core_printf(job, "newpath %g %g moveto\n", A[0].x, A[0].y);
        for (j = 1; j < n; j += 3)
	    core_printf(job, "%g %g %g %g %g %g curveto\n",
		    A[j].x, A[j].y, A[j + 1].x, A[j + 1].y, A[j + 2].x,
		    A[j + 2].y);
        core_fputs(job, "stroke\n");
    }
}

static void psgen_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    int j;

    if (filled && job->obj->fillcolor.u.HSVA[3] > .5) {
	ps_set_color(job, &(job->obj->fillcolor));
	core_printf(job, "newpath %g %g moveto\n", A[0].x, A[0].y);
	for (j = 1; j < n; j++)
	    core_printf(job, "%g %g lineto\n", A[j].x, A[j].y);
	core_printf(job, "closepath fill\n");
    }
    if (job->obj->pencolor.u.HSVA[3] > .5) {
        ps_set_pen_style(job);
        ps_set_color(job, &(job->obj->pencolor));
        core_printf(job, "newpath %g %g moveto\n", A[0].x, A[0].y);
        for (j = 1; j < n; j++)
	    core_printf(job, "%g %g lineto\n", A[j].x, A[j].y);
        core_printf(job, "closepath stroke\n");
    }
}

static void psgen_polyline(GVJ_t * job, pointf * A, int n)
{
    int j;

    if (job->obj->pencolor.u.HSVA[3] > .5) {
        ps_set_pen_style(job);
        ps_set_color(job, &(job->obj->pencolor));
        core_printf(job, "newpath %g %g moveto\n", A[0].x, A[0].y);
        for (j = 1; j < n; j++)
	    core_printf(job, "%g %g lineto\n", A[j].x, A[j].y);
        core_fputs(job, "stroke\n");
    }
}

static void psgen_comment(GVJ_t * job, char *str)
{
    core_fputs(job, "%% ");
    core_fputs(job, str);
    core_fputs(job, "\n");
}

static gvrender_engine_t psgen_engine = {
    psgen_begin_job,
    psgen_end_job,
    psgen_begin_graph,
    0,				/* psgen_end_graph */
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
	| GVRENDER_DOES_MAP_RECTANGLE,
    36,				/* default margin - points */
    4.,                         /* default pad - graph units */
    {612.,792.},                /* default page width, height - points */
    {72.,72.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    HSVA_DOUBLE,		/* color_type */
    NULL,                       /* device */
    "ps",                       /* gvloadimage target for usershapes */
};

gvplugin_installed_t gvrender_core_ps_types[] = {
    {FORMAT_PS, "ps", 1, &psgen_engine, &psgen_features},
    {FORMAT_PS2, "ps2", 1, &psgen_engine, &psgen_features},
    {0, NULL, 0, NULL, NULL}
};
