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

/*
 *  graphics code generator wrapper
 *
 *  This library forms the socket for run-time loadable render plugins.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "const.h"
#include "types.h"
#include "macros.h"
#include "globals.h"
#include "graph.h"
#include "cdt.h"

#include "gvplugin_render.h"
#include "gvc.h"

/* FIXME - need these but without rest of crap in common/ */
extern void colorxlate(char *str, color_t * color,
		       color_type_t target_type);
extern char *canontoken(char *str);

int gvrender_select(GVJ_t * job, char *str)
{
    GVC_t *gvc = job->gvc;
    gv_plugin_t *plugin;
    gvplugin_type_t *typeptr;
    char *device;
#ifndef DISABLE_CODEGENS
    codegen_info_t *cg_info;
#endif

    plugin = gvplugin_load(gvc, API_render, str);
    if (plugin) {
#ifndef DISABLE_CODEGENS
	if (strcmp(plugin->packagename, "cg") == 0) {
	    cg_info = (codegen_info_t *) (plugin->typeptr);
	    job->codegen = cg_info->cg;
	    return cg_info->id;
	} else {
#endif
	    typeptr = plugin->typeptr;
	    job->render_engine = (gvrender_engine_t *) (typeptr->engine);
	    job->render_features =
		(gvrender_features_t *) (typeptr->features);
	    job->render_id = typeptr->id;
	    device = job->render_features->device;
	    if (device) {
		plugin = gvplugin_load(gvc, API_device, device);
		if (! plugin)
		    return NO_SUPPORT;
	        typeptr = plugin->typeptr;
		job->device_engine = (gvdevice_engine_t *) (typeptr->engine);
	        job->device_features =
		    (gvdevice_features_t *) (typeptr->features);
	        job->device_id = typeptr->id;
	    }
	    return GVRENDER_PLUGIN;
#ifndef DISABLE_CODEGENS
	}
#endif
    }
    return NO_SUPPORT;
}

int gvrender_features(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;
    int features = 0;

    if (gvre) {
	features = job->render_features->flags;
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg) {
	    if (cg->bezier_has_arrows)
		features |= GVRENDER_DOES_ARROWS;
	    if (cg->begin_layer)
		features |= GVRENDER_DOES_LAYERS;
	    /* WARNING - nasty hack to avoid modifying old codegens */
	    if (cg == &PS_CodeGen)
		features |= GVRENDER_DOES_MULTIGRAPH_OUTPUT_FILES;
	}
    }
#endif
    return features;
}

void gvrender_begin_job(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre) {
        if (gvre->begin_job)
	    gvre->begin_job(job);
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_job)
	    cg->begin_job(job->output_file, job->g, job->gvc->lib, job->gvc->user,
			  job->gvc->info, job->pagesArraySize);
    }
#endif
}

void gvrender_end_job(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_job)
	gvre->end_job(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_job)
	    cg->end_job();
    }
#endif
    job->gvc->lib = NULL;
}

/* font modifiers */
#define REGULAR 0
#define BOLD    1
#define ITALIC  2

static pointf gvrender_ptf(GVJ_t *job, pointf p)
{
    pointf rv;

    if (job->render_features->flags & GVRENDER_DOES_TRANSFORM)
	return p;

    if (job->rotation) {
	rv.x = -p.y * job->compscale.x + job->offset.x;
	rv.y =  p.x * job->compscale.y + job->offset.y;
    } else {
	rv.x =  p.x * job->compscale.x + job->offset.x;
	rv.y =  p.y * job->compscale.y + job->offset.y;
    }
    return rv;
}

static pointf gvrender_pt(GVJ_t *job, point p)
{
    pointf pf;

    P2PF(p, pf);
    return gvrender_ptf(job, pf);
}

static int gvrender_comparestr(const void *s1, const void *s2)
{
    return strcmp(*(char **) s1, *(char **) s2);
}

static void gvrender_resolve_color(gvrender_features_t * features,
				   char *name, color_t * color)
{
    char *tok;

    color->u.string = name;
    color->type = COLOR_STRING;
    tok = canontoken(name);
    if (!features->knowncolors || (bsearch(&tok, features->knowncolors, features->sz_knowncolors,
		 sizeof(char *), gvrender_comparestr)) == NULL) {
	/* if tok was not found in known_colors */
	colorxlate(name, color, features->color_type);
    }
}

#define EPSILON .0001

void gvrender_begin_graph(GVJ_t * job, graph_t * g)
{
    GVC_t *gvc = job->gvc;
    gvrender_engine_t *gvre = job->render_engine;
    char *str;
    double sx, sy;

    sx = job->width / (job->zoom * 2.);
    sy = job->height / (job->zoom * 2.);

    job->sg = g;  /* current subgraph/cluster */
    job->compscale.y = job->compscale.x = job->zoom * job->dpi / POINTS_PER_INCH;
    job->compscale.y *= (job->flags & GVRENDER_Y_GOES_DOWN) ? -1. : 1.;
    if (job->rotation) {
        job->clip.UR.x = job->focus.x + sy + EPSILON;
        job->clip.UR.y = job->focus.y + sx + EPSILON;
        job->clip.LL.x = job->focus.x - sy - EPSILON;
        job->clip.LL.y = job->focus.y - sx - EPSILON;
	job->offset.x = -job->focus.y * job->compscale.x + job->width / 2.;
	job->offset.y = -job->focus.x * job->compscale.y + job->height / 2.;

	job->transform.xx = 0;
	job->transform.yy = 0;
	job->transform.xy = job->compscale.x;
	job->transform.yx = job->compscale.y;
	job->transform.x0 = job->offset.y;
	job->transform.y0 = job->offset.x;
    } else {
        job->clip.UR.x = job->focus.x + sx + EPSILON;
        job->clip.UR.y = job->focus.y + sy + EPSILON;
        job->clip.LL.x = job->focus.x - sx - EPSILON;
        job->clip.LL.y = job->focus.y - sy - EPSILON;
	job->offset.x = -job->focus.x * job->compscale.x + job->width / 2.;
	job->offset.y = -job->focus.y * job->compscale.y + job->height / 2.;

	job->transform.xx = job->compscale.x;
	job->transform.yy = job->compscale.y;
	job->transform.xy = 0;
	job->transform.yx = 0;
	job->transform.x0 = job->offset.x;
	job->transform.y0 = job->offset.y;
    }

    if (gvre) {
	/* render specific init */
	if (gvre->begin_graph)
	    gvre->begin_graph(job, gvc->graphname);

	/* background color */
	if (((str = agget(g, "bgcolor")) != 0) && str[0]) {
	    gvrender_resolve_color(job->render_features, str,
				   &(gvc->bgcolor));
	    if (gvre->resolve_color)
		gvre->resolve_color(job, &(gvc->bgcolor));
	}

	/* init stack */
	gvc->SP = 0;
	job->style = &(gvc->styles[0]);
	gvrender_set_pencolor(job, DEFAULT_COLOR);
	gvrender_set_fillcolor(job, DEFAULT_FILL);
	job->style->fontfam = DEFAULT_FONTNAME;
	job->style->fontsz = DEFAULT_FONTSIZE;
	job->style->fontopt = FONT_REGULAR;
	job->style->pen = PEN_SOLID;
	job->style->fill = FILL_NONE;
	job->style->penwidth = PENWIDTH_NORMAL;
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;
	
#if 0
fprintf(stderr,"boundingBox = %d,%d %d,%d\n",
        job->boundingBox.LL.x,
        job->boundingBox.LL.y,
        job->boundingBox.UR.x,
        job->boundingBox.UR.y);
fprintf(stderr,"pb = %d,%d %d,%d\n",
        gvc->pb.x, gvc->pb.y);
#endif

	if (cg && cg->begin_graph)
	    cg->begin_graph(gvc, g, job->boundingBox, gvc->pb);
    }
#endif
}

void gvrender_end_graph(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_graph)
	gvre->end_graph(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_graph)
	    cg->end_graph();
    }
#endif
    job->sg = NULL;
}

void gvrender_begin_page(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->begin_page)
	gvre->begin_page(job);

#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;
	point offset;

	PF2P(job->pageOffset, offset);
	if (cg && cg->begin_page)
	    cg->begin_page(job->g, job->pagesArrayElem,
		job->zoom, job->rotation, offset);
    }
#endif
}

void gvrender_end_page(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_page)
	gvre->end_page(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_page)
	    cg->end_page();
    }
#endif
}

void gvrender_begin_layer(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->begin_layer)
	gvre->begin_layer(job, job->gvc->layerIDs[job->layerNum], job->layerNum, job->numLayers);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_layer)
	    cg->begin_layer(job->gvc->layerIDs[job->layerNum], job->layerNum, job->numLayers);
    }
#endif
}

void gvrender_end_layer(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_layer)
	gvre->end_layer(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_layer)
	    cg->end_layer();
    }
#endif
}

void gvrender_begin_cluster(GVJ_t * job, graph_t * sg)
{
    gvrender_engine_t *gvre = job->render_engine;

    job->sg = sg;  /* set current cluster graph object */
#ifndef DISABLE_CODEGENS
    Obj = CLST;
#endif
    if (gvre && gvre->begin_cluster)
	gvre->begin_cluster(job, sg->name, sg->meta_node->id);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_cluster)
	    cg->begin_cluster(sg);
    }
#endif
}

void gvrender_end_cluster(GVJ_t * job, graph_t *g)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_cluster)
	gvre->end_cluster(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_cluster)
	    cg->end_cluster();
    }
    Obj = NONE;
#endif
    job->sg = g;  /* reset current cluster to parent graph or cluster */
}

void gvrender_begin_nodes(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->begin_nodes)
	gvre->begin_nodes(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_nodes)
	    cg->begin_nodes();
    }
#endif
}

void gvrender_end_nodes(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_nodes)
	gvre->end_nodes(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_nodes)
	    cg->end_nodes();
    }
#endif
}

void gvrender_begin_edges(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->begin_edges)
	gvre->begin_edges(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_edges)
	    cg->begin_edges();
    }
#endif
}

void gvrender_end_edges(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_edges)
	gvre->end_edges(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_edges)
	    cg->end_edges();
    }
#endif
}

void gvrender_begin_node(GVJ_t * job, node_t * n)
{
    gvrender_engine_t *gvre = job->render_engine;

#ifndef DISABLE_CODEGENS
    Obj = NODE;
#endif
    job->n = n; /* set current node */
    if (gvre && gvre->begin_node)
	gvre->begin_node(job, n->name, n->id);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_node)
	    cg->begin_node(n);
    }
#endif
}

void gvrender_end_node(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_node)
	gvre->end_node(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_node)
	    cg->end_node();
    }
    Obj = NONE;
#endif
    job->n = NULL; /* clear current node */
}

void gvrender_begin_edge(GVJ_t * job, edge_t * e)
{
    gvrender_engine_t *gvre = job->render_engine;

#ifndef DISABLE_CODEGENS
    Obj = EDGE;
#endif
    job->e = e; /* set current edge */
    if (gvre && gvre->begin_edge)
	gvre->begin_edge(job, e->tail->name,
			 e->tail->graph->root->kind & AGFLAG_DIRECTED,
			 e->head->name, e->id);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_edge)
	    cg->begin_edge(e);
    }
#endif
}

void gvrender_end_edge(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_edge)
	gvre->end_edge(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_edge)
	    cg->end_edge();
    }
    Obj = NONE;
#endif
    job->e = NULL; /* clear current edge */
}

void gvrender_begin_context(GVJ_t * job)
{
    GVC_t *gvc = job->gvc;
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre) {
	(gvc->SP)++;
	assert((gvc->SP) < MAXNEST);
	(gvc->styles)[gvc->SP] = (gvc->styles)[(gvc->SP) - 1];
	job->style = &((gvc->styles)[gvc->SP]);
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_context)
	    cg->begin_context();
    }
#endif
}

void gvrender_end_context(GVJ_t * job)
{
    GVC_t *gvc = job->gvc;
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre) {
	gvc->SP--;
	assert(gvc->SP >= 0);
	job->style = &(gvc->styles[gvc->SP]);
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_context)
	    cg->end_context();
    }
#endif
}

void gvrender_begin_anchor(GVJ_t * job, char *href, char *tooltip,
			   char *target)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->begin_anchor)
	gvre->begin_anchor(job, href, tooltip, target);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_anchor)
	    cg->begin_anchor(href, tooltip, target);
    }
#endif
}

void gvrender_end_anchor(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->end_anchor)
	gvre->end_anchor(job);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_anchor)
	    cg->end_anchor();
    }
#endif
}

void gvrender_set_font(GVJ_t * job, char *fontname, double fontsize)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre) {
	job->style->fontfam = fontname;
	job->style->fontsz = fontsize;
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->set_font)
	    cg->set_font(fontname, fontsize);
    }
#endif
}

void gvrender_textline(GVJ_t * job, pointf p, textline_t * line)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (line->str && line->str[0]) {
	if (gvre && gvre->textline) {
	    if (job->style->pen != PEN_NONE) {
		gvre->textline(job, gvrender_ptf(job, p), line);
	    }
	}
#ifndef DISABLE_CODEGENS
	else {
	    codegen_t *cg = job->codegen;
	    point P;

	    PF2P(p, P);
	    if (cg && cg->textline)
		cg->textline(P, line);
	}
#endif
    }
}

void gvrender_set_pencolor(GVJ_t * job, char *name)
{
    gvrender_engine_t *gvre = job->render_engine;
    color_t *color = &(job->style->pencolor);

    if (gvre) {
	gvrender_resolve_color(job->render_features, name, color);
	if (gvre->resolve_color)
	    gvre->resolve_color(job, color);
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->set_pencolor)
	    cg->set_pencolor(name);
    }
#endif
}

void gvrender_set_fillcolor(GVJ_t * job, char *name)
{
    gvrender_engine_t *gvre = job->render_engine;
    color_t *color = &(job->style->fillcolor);

    if (gvre) {
	gvrender_resolve_color(job->render_features, name, color);
	if (gvre->resolve_color)
	    gvre->resolve_color(job, color);
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->set_fillcolor)
	    cg->set_fillcolor(name);
    }
#endif
}

void gvrender_set_style(GVJ_t * job, char **s)
{
    gvrender_engine_t *gvre = job->render_engine;
    char *line, *p;
    gvstyle_t *style = job->style;

    if (gvre) {
	while ((p = line = *s++)) {
	    if (streq(line, "solid"))
		style->pen = PEN_SOLID;
	    else if (streq(line, "dashed"))
		style->pen = PEN_DASHED;
	    else if (streq(line, "dotted"))
		style->pen = PEN_DOTTED;
	    else if (streq(line, "invis"))
		style->pen = PEN_NONE;
	    else if (streq(line, "bold"))
		style->penwidth = PENWIDTH_BOLD;
	    else if (streq(line, "setlinewidth")) {
		while (*p)
		    p++;
		p++;
		style->penwidth = atof(p);
	    } else if (streq(line, "filled"))
		style->fill = FILL_SOLID;
	    else if (streq(line, "unfilled"))
		style->fill = FILL_NONE;
	    else {
		agerr(AGWARN,
		      "gvrender_set_style: unsupported style %s - ignoring\n",
		      line);
	    }
	}
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->set_style)
	    cg->set_style(s);
    }
#endif
}

void gvrender_ellipse(GVJ_t * job, point p, int rx, int ry, int filled)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->ellipse) {
	if (job->style->pen != PEN_NONE) {
/* temporary hack until client API is FP */
	    pointf AF[2];
	    int i;

	    /* center */
	    AF[0].x = (double) p.x;
	    AF[0].y = (double) p.y;
	    /* corner */
	    AF[1].x = (double) (p.x + rx);
	    AF[1].y = (double) (p.y + ry);
/* end hack */
	    for (i = 0; i < 2; i++)
		AF[i] = gvrender_ptf(job, AF[i]);
	    gvre->ellipse(job, AF, filled);
	}
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->ellipse)
	    cg->ellipse(p, rx, ry, filled);
    }
#endif
}

void gvrender_polygon(GVJ_t * job, point * A, int n, int filled)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->polygon) {
	if (job->style->pen != PEN_NONE) {
/* temporary hack until client API is FP */
	    static pointf *AF;
	    static int sizeAF;
	    int i;

	    if (sizeAF < n)
		AF = realloc(AF, n * sizeof(pointf));
/* end hack */
	    for (i = 0; i < n; i++)
		AF[i] = gvrender_pt(job, A[i]);
	    gvre->polygon(job, AF, n, filled);
	}
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->polygon)
	    cg->polygon(A, n, filled);
    }
#endif
}

void gvrender_beziercurve(GVJ_t * job, pointf * AF, int n,
			  int arrow_at_start, int arrow_at_end, int filled)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->beziercurve) {
	if (job->style->pen != PEN_NONE) {
	    static pointf *AF2;
	    static int szAF2;
	    int i;

	    if (szAF2 < n) {
		AF2 = realloc(AF2, n * sizeof(pointf));
		szAF2 = n;
	    }
	    for (i = 0; i < n; i++)
		AF2[i] = gvrender_ptf(job, AF[i]);
	    gvre->beziercurve(job, AF2, n, arrow_at_start, arrow_at_end,filled);
	}
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;
	/* hack for old codegen int API */
	static point *A;
	static int szA;
	int i;

	if (szA < n) {
	    A = realloc(A, n * sizeof(point));
	    szA = n;
	}
	for (i = 0; i < n; i++)
	    PF2P(AF[i], A[i]);
	/* end hack */

	if (cg && cg->beziercurve)
	    cg->beziercurve(A, n, arrow_at_start, arrow_at_end, filled);
    }
#endif
}

void gvrender_polyline(GVJ_t * job, point * A, int n)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (gvre && gvre->polyline) {
	if (job->style->pen != PEN_NONE) {
	    static pointf *AF;
	    static int szAF;
	    int i;

	    if (szAF < n) {
		AF = realloc(AF, n * sizeof(pointf));
		szAF = n;
	    }
	    for (i = 0; i < n; i++)
		AF[i] = gvrender_pt(job, A[i]);
	    gvre->polyline(job, AF, n);
	}
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->polyline)
	    cg->polyline(A, n);
    }
#endif
}

void gvrender_comment(GVJ_t * job, char *str)
{
    gvrender_engine_t *gvre = job->render_engine;

    if (!str || !str[0])
	return;

    if (gvre && gvre->comment) {
	gvre->comment(job, str);
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->comment)
	    cg->comment(str);
    }
#endif
}

void gvrender_user_shape(GVJ_t * job, char *name, point * A, int n,
			 int filled)
{
    gvrender_engine_t *gvre = job->render_engine;

/* temporary hack until client API is FP */
    static pointf *AF;
    static int szAF;
    int i;

    if (szAF < n) {
	AF = realloc(AF, n * sizeof(pointf));
	szAF = n;
    }
    for (i = 0; i < n; i++)
	P2PF(A[i], AF[i]);
/* end hack */

    if (gvre && gvre->user_shape)
	gvre->user_shape(job, name, AF, n, filled);
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->user_shape)
	    cg->user_shape(name, A, n, filled);
    }
#endif
}
