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

#include <string.h>
#include "memory.h"
#include "const.h"
#include "macros.h"
#include "gvplugin_render.h"
#include "globals.h"
#include "graph.h"
#include "gvcint.h"
#include "colorprocs.h"
#include "gvcproc.h"
#include "htmltable.h"

#include "render.h"

#define FUZZ 3

#define P2RECT(p, pr, sx, sy) (pr[0].x = p.x - sx, pr[0].y = p.y - sy, pr[1].x = p.x + sx, pr[1].y = p.y + sy)

extern int emit_once(char *str);

/* storage for temporary hacks until client API is FP */
static pointf *AF;
static int sizeAF;
#ifdef WITH_CODEGENS
static point *A;
static int sizeA;
#endif
/* end hack */

int gvrender_select(GVJ_t * job, char *str)
{
    GVC_t *gvc = job->gvc;
    gvplugin_available_t *plugin;
    gvplugin_installed_t *typeptr;
    char *device;
#ifdef WITH_CODEGENS
    codegen_info_t *cg_info;
#endif

    plugin = gvplugin_load(gvc, API_render, str);
    if (plugin) {
#ifdef WITH_CODEGENS
	if (strcmp(plugin->packagename, "cg") == 0) {
	    cg_info = (codegen_info_t *) (plugin->typeptr);
	    job->codegen = cg_info->cg;
	    job->render.engine = NULL;
	    job->device.engine = NULL;
	    return cg_info->id;
	} else {
#endif
	    typeptr = plugin->typeptr;
	    job->render.engine = (gvrender_engine_t *) (typeptr->engine);
	    job->render.features =
		(gvrender_features_t *) (typeptr->features);
	    job->render.id = typeptr->id;
	    device = job->render.features->device;
	    if (device) {
		plugin = gvplugin_load(gvc, API_device, device);
		if (! plugin)
		    return NO_SUPPORT;  /* FIXME - should differentiate no device from no renderer */
	        typeptr = plugin->typeptr;
		job->device.engine = (gvdevice_engine_t *) (typeptr->engine);
	        job->device.features =
		    (gvdevice_features_t *) (typeptr->features);
	        job->device.id = typeptr->id;
	    }
	    return GVRENDER_PLUGIN;
#ifdef WITH_CODEGENS
	}
#endif
    }
    return NO_SUPPORT;
}

int gvrender_features(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;
    int features = 0;

    if (gvre) {
	features = job->render.features->flags;
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg) {
	    if (cg->bezier_has_arrows)
		features |= GVRENDER_DOES_ARROWS;
	    if (cg->begin_layer)
		features |= GVRENDER_DOES_LAYERS;
	}
    }
#endif
    return features;
}

static obj_state_t* push_obj_state(GVJ_t *job)
{
    obj_state_t *obj;

    if (! (obj = zmalloc(sizeof(obj_state_t))))
	agerr(AGERR, "no memory from zmalloc()\n");

    obj->parent = job->obj;
    job->obj = obj;

    return obj;
}

static void pop_obj_state(GVJ_t *job)
{
    obj_state_t *obj = job->obj;

    assert(obj);

    if (obj->url) free(obj->url);
    if (obj->tailurl) free(obj->tailurl);
    if (obj->headurl) free(obj->headurl);
    if (obj->tooltip) free(obj->tooltip);
    if (obj->tailtooltip) free(obj->tailtooltip);
    if (obj->headtooltip) free(obj->headtooltip);
    if (obj->target) free(obj->target);
    if (obj->tailtarget) free(obj->tailtarget);
    if (obj->headtarget) free(obj->headtarget);
    if (obj->url_map_p) free(obj->url_map_p);
    if (obj->url_bsplinemap_p) free(obj->url_bsplinemap_p);
    if (obj->url_bsplinemap_n) free(obj->url_bsplinemap_n);
    if (obj->tailurl_map_p) free(obj->tailurl_map_p);
    if (obj->headurl_map_p) free(obj->headurl_map_p);

    job->obj = obj->parent;
    free(obj);
}

static void doHTMLdata(htmldata_t * dp, point p, void *obj)
{
    char *url = NULL, *target = NULL, *title = NULL;
    pointf p1, p2;
    int havetitle = 0;

    if ((url = dp->href) && url[0]) {
        switch (agobjkind(obj)) {
        case AGGRAPH:
            url = strdup_and_subst_graph(url, (graph_t *) obj);
            break;
        case AGNODE:
            url = strdup_and_subst_node(url, (node_t *) obj);
            break;
        case AGEDGE:
            url = strdup_and_subst_edge(url, (edge_t *) obj);
            break;
        }
    }
    target = dp->target;
    if ((title = dp->title) && title[0]) {
        havetitle++;
        switch (agobjkind(obj)) {
        case AGGRAPH:
            title = strdup_and_subst_graph(title, (graph_t *) obj);
            break;
        case AGNODE:
            title = strdup_and_subst_node(title, (node_t *) obj);
            break;
        case AGEDGE:
            title = strdup_and_subst_edge(title, (edge_t *) obj);
            break;
        }
    }
    if (url || title) {
        p1.x = p.x + dp->box.LL.x;
        p1.y = p.y + dp->box.LL.y;
        p2.x = p.x + dp->box.UR.x;
        p2.y = p.y + dp->box.UR.y;
//  FIXME
//        map_output_rect(p1, p2, url, target, "", title);
    }
    free(url);
    free(title);
}

/* forward declaration */
static void doHTMLcell(htmlcell_t * cp, point p, void *obj);

static void doHTMLtbl(htmltbl_t * tbl, point p, void *obj)
{
    htmlcell_t **cells = tbl->u.n.cells;
    htmlcell_t *cp;

    while ((cp = *cells++))
        doHTMLcell(cp, p, obj);
    if (tbl->data.href)
        doHTMLdata(&tbl->data, p, obj);
}

static void doHTMLcell(htmlcell_t * cp, point p, void *obj)
{
    if (cp->child.kind == HTML_TBL)
        doHTMLtbl(cp->child.u.tbl, p, obj);
    if (cp->data.href)
        doHTMLdata(&cp->data, p, obj);
}

static void doHTMLlabel(htmllabel_t * lbl, point p, void *obj)
{
    if (lbl->kind == HTML_TBL) {
        doHTMLtbl(lbl->u.tbl, p, obj);
    }
}

/* isRect:
 *  * isRect function returns true when polygon has
 *   * regular rectangular shape. Rectangle is regular when
 *    * it is not skewed and distorted and orientation is almost zero
 *     */
void gvrender_begin_job(GVJ_t * job)
{
    GVC_t *gvc = job->gvc;
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
        if (gvre->begin_job)
	    gvre->begin_job(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_job)
	    cg->begin_job(job->output_file, gvc->g,
		    gvc->common.lib,
		    gvc->common.user,
		    gvc->common.info,
		    job->pagesArraySize);
    }
#endif
}

void gvrender_end_job(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
       	if (gvre->end_job)
	    gvre->end_job(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_job)
	    cg->end_job();
    }
#endif
    job->gvc->common.lib = NULL;    /* FIXME - minimally this doesn't belong here */
}

/* font modifiers */
#define REGULAR 0
#define BOLD    1
#define ITALIC  2

static pointf gvrender_ptf(GVJ_t *job, pointf p)
{
    pointf rv;

    if (job->rotation) {
	rv.x =  -(p.y + job->comptrans.y) * job->compscale.x;
	rv.y =  (p.x + job->comptrans.x) * job->compscale.y;
    } else {
	rv.x =  (p.x + job->comptrans.x) * job->compscale.x;
	rv.y =  (p.y + job->comptrans.y) * job->compscale.y;
    }
    return rv;
}

/* transform an array of n points */
/*  *AF and *af must be preallocated */
/*  *AF can be the same as *af for inplace transforms */
static pointf* gvrender_ptf_A(GVJ_t *job, pointf *af, pointf *AF, int n)
{
    int i;
    pointf trans = job->comptrans, scale = job->compscale;

    if (job->rotation) {
        for (i = 0; i < n; i++) {
	    AF[i].x = -(af[i].y + trans.y) * scale.x;
	    AF[i].y =  (af[i].x + trans.x) * scale.y;
	}
    }
    else {
        for (i = 0; i < n; i++) {
	    AF[i].x =  (af[i].x + trans.x) * scale.x;
	    AF[i].y =  (af[i].y + trans.y) * scale.y;
	}
    }
    return AF;
}

static int gvrender_comparestr(const void *s1, const void *s2)
{
    return strcmp(*(char **) s1, *(char **) s2);
}

static void rect2poly(pointf *p)
{
    p[3].x = p[2].x = p[1].x;
    p[2].y = p[1].y;
    p[3].y = p[0].y;
    p[1].x = p[0].x;
}

static void gvrender_resolve_color(gvrender_features_t * features,
				   char *name, gvcolor_t * color)
{
    char *tok;
    int rc;

    color->u.string = name;
    color->type = COLOR_STRING;
    tok = canontoken(name);
    if (!features->knowncolors || (bsearch(&tok, features->knowncolors, features->sz_knowncolors,
		 sizeof(char *), gvrender_comparestr)) == NULL) {
	/* if tok was not found in known_colors */
	rc = colorxlate(name, color, features->color_type);
	if (rc != COLOR_OK) {
	    if (rc == COLOR_UNKNOWN) {
		char *missedcolor = gmalloc(strlen(name) + 16);
		sprintf(missedcolor, "color %s", name);
		if (emit_once(missedcolor))
		    agerr(AGWARN, "%s is not a known color.\n", name);
		free(missedcolor);
	    }
	    else {
		agerr(AGERR, "error in colxlate()\n");
	    }
	}
    }
}

#define EPSILON .0001

void gvrender_begin_graph(GVJ_t * job, graph_t * g)
{
    GVC_t *gvc = job->gvc;
    obj_state_t *obj;
    gvrender_engine_t *gvre = job->render.engine;
    textlabel_t *lab;
    char *s;
    double sx, sy;

    sx = job->width / (job->zoom * 2.);
    sy = job->height / (job->zoom * 2.);

    obj = push_obj_state(job);
    obj->g = g;
    if ((job->flags & GVRENDER_DOES_LABELS) && ((lab = GD_label(g)))) {
        if (lab->html)
            doHTMLlabel(lab->u.html, lab->p, (void *) g);
	obj->label = lab->text;
    }
    if ((job->flags & GVRENDER_DOES_MAPS)
        && (((s = agget(g, "href")) && s[0]) || ((s = agget(g, "URL")) && s[0]))) {
        obj->url = strdup_and_subst_graph(s, g);
    } 

    job->compscale.x = job->scale.x = job->zoom * job->dpi.x / POINTS_PER_INCH;
    job->compscale.y = job->scale.y = job->zoom * job->dpi.y / POINTS_PER_INCH;
    job->compscale.y *= (job->flags & GVRENDER_Y_GOES_DOWN) ? -1. : 1.;
    if (job->rotation) {
        job->clip.UR.x = job->focus.x + sy + EPSILON;
        job->clip.UR.y = job->focus.y + sx + EPSILON;
        job->clip.LL.x = job->focus.x - sy - EPSILON;
        job->clip.LL.y = job->focus.y - sx - EPSILON;
	job->offset.x = -job->focus.y * job->compscale.x + job->width * 3 / 2;
	job->offset.y = -job->focus.x * job->compscale.y + job->height / 2.;
    } else {
        job->clip.UR.x = job->focus.x + sx + EPSILON;
        job->clip.UR.y = job->focus.y + sy + EPSILON;
        job->clip.LL.x = job->focus.x - sx - EPSILON;
        job->clip.LL.y = job->focus.y - sy - EPSILON;
	job->offset.x = -job->focus.x * job->compscale.x + job->width / 2.;
	job->offset.y = -job->focus.y * job->compscale.y + job->height / 2.;
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

    if (gvre) {
	/* render specific init */
	if (gvre->begin_graph)
	    gvre->begin_graph(job);

	/* background color */
	if (((s = agget(g, "bgcolor")) != 0) && s[0]) {
	    gvrender_resolve_color(job->render.features, s, &(gvc->bgcolor));
	    if (gvre->resolve_color)
		gvre->resolve_color(job, &(gvc->bgcolor));
	}

    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;
	
	if (cg && cg->begin_graph)
	    cg->begin_graph(gvc, g, job->canvasBox, gvc->pb);
    }
#endif
}

void gvrender_end_graph(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
       	if (gvre->end_graph)
	    gvre->end_graph(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_graph)
	    cg->end_graph();
    }
#endif
    pop_obj_state(job);
}

void gvrender_begin_page(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;
    obj_state_t *obj = job->obj;
    int nump = 0, flags = job->flags;
    pointf *p = NULL;

    if (flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS)) {
        if (flags & (GVRENDER_DOES_MAP_RECTANGLE | GVRENDER_DOES_MAP_POLYGON)) {
	    if (flags & GVRENDER_DOES_MAP_RECTANGLE) {
	        obj->url_map_shape = MAP_RECTANGLE;
	        nump = 2;
	    }
	    else {
	        obj->url_map_shape = MAP_POLYGON;
	        nump = 4;
	    }

	    p = N_NEW(nump, pointf);
	    p[0] = job->pageBox.LL;
	    p[1] = job->pageBox.UR;

	    if (! (flags & (GVRENDER_DOES_MAP_RECTANGLE)))
		rect2poly(p);
	}
	if (! (flags & GVRENDER_DOES_TRANSFORM))
	    gvrender_ptf_A(job, p, p, nump);
	obj->url_map_p = p;
	obj->url_map_n = nump;
    }

    if (gvre) {
	if (gvre->begin_anchor && (obj->url || obj->explicit_tooltip))
	    gvre->begin_anchor(job, obj->url, obj->tooltip, obj->target);
        if (gvre->begin_page)
	    gvre->begin_page(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;
        point offset;

        PF2P(job->pageOffset, offset);
	if (cg && cg->begin_anchor && (obj->url || obj->explicit_tooltip))
	    cg->begin_anchor(obj->url, obj->tooltip, obj->target);
        if (cg && cg->begin_page)
            cg->begin_page(job->gvc->g, job->pagesArrayElem,
               job->zoom, job->rotation, offset);
    }
#endif
}

void gvrender_end_page(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;
    obj_state_t *obj = job->obj;

    if (gvre) {
       	if (gvre->end_page)
	    gvre->end_page(job);
	if (gvre->end_anchor && (obj->url || obj->explicit_tooltip))
	    gvre->end_anchor(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_page)
	    cg->end_page();
	if (cg && cg->end_anchor && (obj->url || obj->explicit_tooltip))
	    cg->end_anchor();
    }
#endif
}

void gvrender_begin_layer(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->begin_layer)
	    gvre->begin_layer(job, job->gvc->layerIDs[job->layerNum], job->layerNum, job->numLayers);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_layer)
	    cg->begin_layer(job->gvc->layerIDs[job->layerNum], job->layerNum, job->numLayers);
    }
#endif
}

void gvrender_end_layer(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
        if (gvre->end_layer)
	    gvre->end_layer(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_layer)
	    cg->end_layer();
    }
#endif
}

void gvrender_begin_cluster(GVJ_t * job, graph_t * sg)
{
    gvrender_engine_t *gvre = job->render.engine;
    obj_state_t *obj;
    textlabel_t *lab;
    char *s;
    int flags, nump = 0;
    pointf *p = NULL;

    flags = job->flags;
    obj = push_obj_state(job);
    obj->sg = sg;
    if ((flags & GVRENDER_DOES_LABELS) && ((lab = GD_label(sg)))) {
        if (lab->html)
            doHTMLlabel(lab->u.html, lab->p, (void *) sg);
	obj->label = lab->text;
    }
    if ((flags & GVRENDER_DOES_MAPS)
        && (((s = agget(sg, "href")) && s[0]) || ((s = agget(sg, "URL")) && s[0])))
	obj->url = strdup_and_subst_graph(s, sg);

    if ((flags & GVRENDER_DOES_TARGETS) && ((s = agget(sg, "target")) && s[0]))
	obj->target = strdup_and_subst_graph(s, sg);

    if ((flags & GVRENDER_DOES_TOOLTIPS) && ((s = agget(sg, "tooltip")) && s[0]))
        obj->tooltip = strdup_and_subst_graph(s, sg);

    if (flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS)) {
        if (flags & (GVRENDER_DOES_MAP_RECTANGLE | GVRENDER_DOES_MAP_POLYGON)) {
	    if (flags & GVRENDER_DOES_MAP_RECTANGLE) {
	        obj->url_map_shape = MAP_RECTANGLE;
	        nump = 2;
	    }
	    else {
	        obj->url_map_shape = MAP_POLYGON;
	        nump = 4;
	    }

	    p = N_NEW(nump, pointf);
	    P2PF(GD_bb(sg).LL, p[0]);
	    P2PF(GD_bb(sg).UR, p[1]);

	    if (! (flags & (GVRENDER_DOES_MAP_RECTANGLE)))
		rect2poly(p);
	}
	obj->url_map_p = p;
	obj->url_map_n = nump;
    }

    if (gvre) {
	if (gvre->begin_anchor && (obj->url || obj->explicit_tooltip))
	    gvre->begin_anchor(job, obj->url, obj->tooltip, obj->target);
	if (gvre->begin_cluster)
	    gvre->begin_cluster(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

        Obj = CLST;
	if (cg && cg->begin_anchor && (obj->url || obj->explicit_tooltip))
	    cg->begin_anchor(obj->url, obj->tooltip, obj->target);
	if (cg && cg->begin_cluster)
	    cg->begin_cluster(sg);
    }
#endif
}

void gvrender_end_cluster(GVJ_t * job, graph_t *g)
{
    gvrender_engine_t *gvre = job->render.engine;
    obj_state_t *obj = job->obj;

    if (gvre) {
       	if (gvre->end_cluster)
	    gvre->end_cluster(job);
	if (gvre->end_anchor && (obj->url || obj->explicit_tooltip))
	    gvre->end_anchor(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_cluster)
	    cg->end_cluster();
	if (cg && cg->end_anchor && (obj->url || obj->explicit_tooltip))
	    cg->end_anchor();
    }
    Obj = NONE;
#endif
    pop_obj_state(job);
}

void gvrender_begin_nodes(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->begin_nodes)
	    gvre->begin_nodes(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_nodes)
	    cg->begin_nodes();
    }
#endif
}

void gvrender_end_nodes(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->end_nodes)
	    gvre->end_nodes(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_nodes)
	    cg->end_nodes();
    }
#endif
}

void gvrender_begin_edges(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->begin_edges)
	    gvre->begin_edges(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_edges)
	    cg->begin_edges();
    }
#endif
}

void gvrender_end_edges(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->end_edges)
	    gvre->end_edges(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_edges)
	    cg->end_edges();
    }
#endif
}

static bool isRect(polygon_t * p)
{
    return (p->sides == 4 && (ROUND(p->orientation) % 90) == 0
            && p->distortion == 0.0 && p->skew == 0.0);
}

/*
 * isFilled function returns 1 if filled style has been set for node 'n'
 * otherwise returns 0. it accepts pointer to node_t as an argument
 */
static int ifFilled(node_t * n)
{
    char *style, *p, **pp;
    int r = 0;
    style = late_nnstring(n, N_style, "");
    if (style[0]) {
        pp = parse_style(style);
        while ((p = *pp)) {
            if (strcmp(p, "filled") == 0)
                r = 1;
            pp++;
        }
    }
    return r;
}

/* pEllipse:
 * pEllipse function returns 'np' points from the circumference
 * of ellipse described by radii 'a' and 'b'.
 * Assumes 'np' is greater than zero.
 * 'np' should be at least 4 to sample polygon from ellipse
 */
static pointf *pEllipse(double a, double b, int np)
{
    double theta = 0.0;
    double deltheta = 2 * M_PI / np;
    int i;
    pointf *ps;

    ps = N_NEW(np, pointf);
    for (i = 0; i < np; i++) {
        ps[i].x = a * cos(theta);
        ps[i].y = b * sin(theta);
        theta += deltheta;
    }
    return ps;
}


void gvrender_begin_node(GVJ_t * job, node_t * n)
{
    gvrender_engine_t *gvre = job->render.engine;
    obj_state_t *obj;
    textlabel_t *lab;
    int flags, sides, peripheries, i, j, filled = 0, rect = 0, shape, nump = 0;
    polygon_t *poly = NULL;
    pointf *vertices, ldimen, *p =  NULL;
    point coord;
    char *s;

    flags = job->flags;
    obj = push_obj_state(job);
    obj->n = n;

    if (flags & GVRENDER_DOES_Z) {
        obj->z = late_double(n, N_z, 0.0, -MAXFLOAT);
    }
    if ((flags & GVRENDER_DOES_LABELS) && ((lab = ND_label(n)))) {
        if (lab->html)
            doHTMLlabel(lab->u.html, lab->p, (void *) n);
	obj->label = lab->text;
    }
    if ((flags & GVRENDER_DOES_MAPS)
        && (((s = agget(n, "href")) && s[0]) || ((s = agget(n, "URL")) && s[0]))) {
        obj->url = strdup_and_subst_node(s, n);
    }
    if (flags & GVRENDER_DOES_TOOLTIPS) {
        if ((s = agget(n, "tooltip")) && s[0])
            obj->tooltip = strdup_and_subst_node(s, n);
	else
	    obj->tooltip = strdup(ND_label(n)->text);
    } 
    if ((flags & GVRENDER_DOES_TARGETS) && ((s = agget(n, "target")) && s[0])) {
        obj->target = strdup_and_subst_node(s, n);
    }
    if (flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS)) {

        /* checking shape of node */
        shape = shapeOf(n);
        /* node coordinate */
        coord = ND_coord_i(n);
        /* checking if filled style has been set for node */
        filled = ifFilled(n);

        if (shape == SH_POLY || shape == SH_POINT) {
            poly = (polygon_t *) ND_shape_info(n);

            /* checking if polygon is regular rectangle */
            if (isRect(poly) && (poly->peripheries || filled))
                rect = 1;
        }

        /* When node has polygon shape and requested output supports polygons
         * we use a polygon to map the clickable region that is a:
         * circle, ellipse, polygon with n side, or point.
         * For regular rectangular shape we have use node's bounding box to map clickable region
         */
        if (poly && !rect && (flags & GVRENDER_DOES_MAP_POLYGON)) {

            if (poly->sides < 3)
                sides = 1;
            else
                sides = poly->sides;

            if (poly->peripheries < 2)
                peripheries = 1;
            else
                peripheries = poly->peripheries;

            vertices = poly->vertices;

	    if ((s = agget(n, "samplepoints")))
		nump = atoi(s);
	    /* We want at least 4 points. For server-side maps, at most 100
	     * points are allowed. To simplify things to fit with the 120 points
	     * used for skewed ellipses, we set the bound at 60.
	     */
	    if ((nump < 4) || (nump > 60))
		nump = DFLT_SAMPLE;
            /* use bounding box of text label for mapping
	     * when polygon has no peripheries and node is not filled
	     */
            if (poly->peripheries == 0 && !filled) {
                obj->url_map_shape = MAP_RECTANGLE;
                nump = 2;
                p = N_NEW(nump, pointf);
                ldimen = ND_label(n)->dimen;
		P2RECT(coord, p, ldimen.x / 2.0, ldimen.y / 2.0);
            }
            /* circle or ellipse */
            else if (poly->sides < 3 && poly->skew == 0.0 && poly->distortion == 0.0) {
                if (poly->regular) {
                    obj->url_map_shape = MAP_CIRCLE;
                    nump = 2;              /* center of circle and top right corner of bb */
                    p = N_NEW(nump, pointf);
                    p[0].x = coord.x;
                    p[0].y = coord.y;
                    p[1].x = coord.x + vertices[peripheries - 1].x;
                    p[1].y = coord.y + vertices[peripheries - 1].y;
                }
                else { /* ellipse is treated as polygon */
                    obj->url_map_shape= MAP_POLYGON;
                    p = pEllipse((double)(vertices[peripheries - 1].x),
				 (double)(vertices[peripheries - 1].y), nump);
                    for (i = 0; i < nump; i++) {
                        p[i].x += coord.x;
                        p[i].y += coord.y;
                    }
                }
            }
            /* all other polygonal shape */
	    else {
                int offset = (peripheries - 1)*(poly->sides);
                obj->url_map_shape = MAP_POLYGON;
                /* distorted or skewed ellipses and circles are polygons with 120
		 * sides. For mapping we convert them into polygon with sample sides
		 */
                if (poly->sides >= nump) {
                    int delta = poly->sides / nump;
                    p = N_NEW(nump, pointf);
                    for (i = 0, j = 0; j < nump; i += delta, j++) {
                        p[j].x = coord.x + vertices[i + offset].x;
                        p[j].y = coord.y + vertices[i + offset].y;
                    }
                } else {
                    nump = sides;
                    p = N_NEW(nump, pointf);
                    for (i = 0; i < nump; i++) {
                        p[i].x = coord.x + vertices[i + offset].x;
                        p[i].y = coord.y + vertices[i + offset].y;
                    }
                }
            }
	}
	else {
            /* we have to use the node's bounding box to map clickable region
	     * when requested output format is not capable of polygons.
	     */
            obj->url_map_shape = MAP_RECTANGLE;
            nump = 2;
            p = N_NEW(nump, pointf);
            p[0].x = coord.x - ND_lw_i(n);
            p[0].y = coord.y - (ND_ht_i(n) / 2);
            p[1].x = coord.x + ND_rw_i(n);
            p[1].y = coord.y + (ND_ht_i(n) / 2);
        }
	if (! (flags & GVRENDER_DOES_TRANSFORM))
	    gvrender_ptf_A(job, p, p, nump);
	obj->url_map_p = p;
	obj->url_map_n = nump;
    }

    if (gvre) {
	if (gvre->begin_anchor && (obj->url || obj->explicit_tooltip))
	    gvre->begin_anchor(job, obj->url, obj->tooltip, obj->target);
	if (gvre->begin_node)
	    gvre->begin_node(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

        Obj = NODE;
	if (cg && cg->begin_anchor && (obj->url || obj->explicit_tooltip))
	    cg->begin_anchor(obj->url, obj->tooltip, obj->target);
	if (cg && cg->begin_node)
	    cg->begin_node(n);
    }
#endif
}

void gvrender_end_node(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;
    obj_state_t *obj = job->obj;

    if (gvre) {
	if (gvre->end_node)
	    gvre->end_node(job);
	if (gvre->end_anchor && (obj->url || obj->explicit_tooltip))
	    gvre->end_anchor(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_node)
	    cg->end_node();
	if (cg && cg->end_anchor && (obj->url || obj->explicit_tooltip))
	    cg->end_anchor();
    }
    Obj = NONE;
#endif
    pop_obj_state(job);
}

#define HW 2.0   /* maximum distance away from line, in points */

/* check_control_points:
 * check_control_points function checks the size of quadrilateral
 * formed by four control points
 * returns 1 if four points are in line (or close to line)
 * else return 0
 */
static int check_control_points(pointf *cp)
{
    double dis1 = ptToLine2 (cp[0], cp[3], cp[1]);
    double dis2 = ptToLine2 (cp[0], cp[3], cp[2]);
    if (dis1 < HW*HW && dis2 < HW*HW)
	return 1;
    else 
	return 0;
}

#ifdef DEBUG
static void psmapOutput (point* ps, int n)
{
   int i;
   fprintf (stdout, "newpath %d %d moveto\n", ps[0].x, ps[0].y);
   for (i=1; i < n; i++)
	fprintf (stdout, "%d %d lineto\n", ps[i].x, ps[i].y);
   fprintf (stdout, "closepath stroke\n");
}
#endif

typedef struct segitem_s {
    pointf p;
    struct segitem_s* next;
} segitem_t;

#define MARK_FIRST_SEG(L) ((L)->next = (segitem_t*)1)
#define FIRST_SEG(L) ((L)->next == (segitem_t*)1)
#define INIT_SEG(P,L) {(L)->next = 0; (L)->p = P;} 

static segitem_t* appendSeg (pointf p, segitem_t* lp)
{
    segitem_t* s = GNEW(segitem_t);
    INIT_SEG (p, s);
    lp->next = s;
    return s;
}

/* map_bspline_poly:
 * Output the polygon determined by the n points in p1, followed
 * by the n points in p2 in reverse order. Assumes n <= 50.
 */
static void map_bspline_poly(pointf **pbs_p, int **pbs_n, int *pbs_poly_n, int n, pointf* p1, pointf* p2) 
{
    int i = 0, nump = 0, last = 2*n-1;

    for ( ; i < *pbs_poly_n; i++)
	nump += (*pbs_n)[i];

    (*pbs_poly_n)++;
    *pbs_n = grealloc(*pbs_n, (*pbs_poly_n) * sizeof(int));
    (*pbs_n)[i] = 2*n;
    *pbs_p = grealloc(*pbs_p, (nump + 2*n) * sizeof(pointf));

    for (i = 0; i < n; i++) {
	(*pbs_p)[nump+i] = p1[i];
	(*pbs_p)[nump+last-i] = p2[i];
    }
#ifdef DEBUG
    psmapOutput (*pbs_p + nump, last+1);
#endif
}

/* approx_bezier:
 * Approximate Bezier by line segments. If the four points are
 * almost colinear, as determined by check_control_points, we store
 * the segment cp[0]-cp[3]. Otherwise we split the Bezier into 2
 * and recurse. 
 * Since 2 contiguous segments share an endpoint, we actually store
 * the segments as a list of points.
 * New points are appended to the list given by lp. The tail of the
 * list is returned.
 */ 
static segitem_t* approx_bezier (pointf *cp, segitem_t* lp)
{
    pointf sub_curves[8];

    if (check_control_points(cp)) {
	if (FIRST_SEG (lp)) INIT_SEG (cp[0], lp);
	lp = appendSeg (cp[3], lp);
    }
    else {
	Bezier (cp, 3, 0.5, sub_curves, sub_curves+4);
	lp = approx_bezier (sub_curves, lp);
	lp = approx_bezier (sub_curves+4, lp);
    }
    return lp;
}

/* bisect:
 * Return the angle of the bisector between the two rays
 * pp-cp and cp-np. The bisector returned is always to the
 * left of pp-cp-np.
 */
static double bisect (pointf pp, pointf cp, pointf np)
{
  double ang, theta, phi;
  theta = atan2(np.y - cp.y,np.x - cp.x);
  phi = atan2(pp.y - cp.y,pp.x - cp.x);
  ang = theta - phi;
  if (ang > 0) ang -= 2*M_PI;

  return (phi + ang/2.0);
}

/* mkSegPts:
 * Determine polygon points related to 2 segments prv-cur and cur-nxt.
 * The points lie on the bisector of the 2 segments, passing through cur,
 * and distance HW from cur. The points are stored in p1 and p2.
 * If p1 is NULL, we use the normal to cur-nxt.
 * If p2 is NULL, we use the normal to prv-cur.
 * Assume at least one of prv or nxt is non-NULL.
 */
static void mkSegPts (segitem_t* prv, segitem_t* cur, segitem_t* nxt, 
	pointf* p1, pointf* p2)
{
    pointf cp, pp, np;
    double theta, delx, dely;
    pointf p;

    cp = cur->p;
    /* if prv or nxt are NULL, use the one given to create a collinear
     * prv or nxt. This could be more efficiently done with special case code, 
     * but this way is more uniform.
     */
    if (prv) {
	pp = prv->p;
	if (nxt)
	    np = nxt->p;
	else {
	    np.x = 2*cp.x - pp.x;
	    np.y = 2*cp.y - pp.y;
	}
    }
    else {
	np = nxt->p;
	pp.x = 2*cp.x - np.x;
	pp.y = 2*cp.y - np.y;
    }
    theta = bisect(pp,cp,np);
    delx = HW*cos(theta);
    dely = HW*sin(theta);
    p.x = cp.x + delx;
    p.y = cp.y + dely;
    *p1 = p;
    p.x = cp.x - delx;
    p.y = cp.y - dely;
    *p2 = p;
}

/* map_output_bspline:
 * Construct and output a closed polygon approximating the input
 * B-spline bp. We do this by first approximating bp by a sequence
 * of line segments. We then use the sequence of segments to determine
 * the polygon.
 * In cmapx, polygons are limited to 100 points, so we output polygons
 * in chunks of 100.
 */
static void map_output_bspline (pointf **pbs, int **pbs_n, int *pbs_poly_n, bezier* bp)
{
    segitem_t* segl = GNEW(segitem_t);
    segitem_t* segp = segl;
    segitem_t* segprev;
    segitem_t* segnext;
    int nc, j, k, cnt;
    pointf pts[4];
    pointf pt1[50], pt2[50];

    MARK_FIRST_SEG(segl);
    nc = (bp->size - 1)/3; /* nc is number of bezier curves */
    for (j = 0; j < nc; j++) { 
	for (k = 0; k < 4; k++) {
	    pts[k].x = (double)bp->list[3*j + k].x;
	    pts[k].y = (double)bp->list[3*j + k].y;
	}
	segp = approx_bezier (pts, segp);
    }

    segp = segl;
    segprev = 0;
    cnt = 0;
    while (segp) {
	segnext = segp->next;
	mkSegPts (segprev, segp, segnext, pt1+cnt, pt2+cnt);
	cnt++;
	if ((segnext == NULL) || (cnt == 50)) {
	    map_bspline_poly (pbs, pbs_n, pbs_poly_n, cnt, pt1, pt2);
	    pt1[0] = pt1[cnt-1];
	    pt2[0] = pt2[cnt-1];
	    cnt = 1;
	}
	segprev = segp;
	segp = segnext;
    }

    /* free segl */
    while (segl) {
	segp = segl->next;
	free (segl);
	segl = segp;
    }
}

void gvrender_begin_edge(GVJ_t * job, edge_t * e)
{
    gvrender_engine_t *gvre = job->render.engine;
    obj_state_t *obj;
    char *s;
    textlabel_t *lab = NULL, *tlab = NULL, *hlab = NULL;
    pointf *p = NULL, *pt = NULL, *ph = NULL, *pte = NULL, *phe = NULL, *pbs = NULL;
    int flags, i, nump, *pbs_n = NULL, pbs_poly_n = 0;
    bezier bz;

    flags = job->flags;
    obj = push_obj_state(job);
    obj->e = e;

    if (flags & GVRENDER_DOES_Z) {
        obj->tail_z= late_double(e->tail, N_z, 0.0, -1000.0);
        obj->head_z= late_double(e->head, N_z, 0.0, -MAXFLOAT);
    }

    if (flags & GVRENDER_DOES_LABELS) {
	if ((lab = ED_label(e))) {
	    if (lab->html)
		doHTMLlabel(lab->u.html, lab->p, (void *) e);
	    obj->label = lab->text;
	}
	obj->taillabel = obj->headlabel = obj->label;
	if ((tlab = ED_tail_label(e))) {
	    if (tlab->html)
		doHTMLlabel(tlab->u.html, tlab->p, (void *) e);
	    obj->taillabel = tlab->text;
	}
	if ((hlab = ED_head_label(e))) {
	    if (hlab->html)
		doHTMLlabel(hlab->u.html, hlab->p, (void *) e);
	    obj->headlabel = hlab->text;
	}
    }

    if (flags & GVRENDER_DOES_MAPS) {
        if (((s = agget(e, "href")) && s[0]) || ((s = agget(e, "URL")) && s[0]))
            obj->url = strdup_and_subst_edge(s, e);
	if (((s = agget(e, "tailhref")) && s[0]) || ((s = agget(e, "tailURL")) && s[0]))
            obj->tailurl = strdup_and_subst_edge(s, e);
	else if (obj->url)
	    obj->tailurl = strdup(obj->url);
	if (((s = agget(e, "headhref")) && s[0]) || ((s = agget(e, "headURL")) && s[0]))
            obj->headurl = strdup_and_subst_edge(s, e);
	else if (obj->url)
	    obj->headurl = strdup(obj->url);
    } 

    if (flags & GVRENDER_DOES_TARGETS) {
        if ((s = agget(e, "target")) && s[0])
            obj->target = strdup_and_subst_edge(s, e);
        if ((s = agget(e, "tailtarget")) && s[0])
            obj->tailtarget = strdup_and_subst_edge(s, e);
	else if (obj->target)
	    obj->tailtarget = strdup(obj->target);
        if ((s = agget(e, "headtarget")) && s[0])
            obj->headtarget = strdup_and_subst_edge(s, e);
	else if (obj->target)
	    obj->headtarget = strdup(obj->target);
    } 

    if (flags & GVRENDER_DOES_TOOLTIPS) {
        if ((s = agget(e, "tooltip")) && s[0]) {
            obj->tooltip = strdup_and_subst_edge(s, e);
	    obj->explicit_tooltip = true;
	}
	else if (obj->label)
	    obj->tooltip = strdup(obj->label);
        if ((s = agget(e, "tailtooltip")) && s[0]) {
            obj->tailtooltip = strdup_and_subst_edge(s, e);
	    obj->explicit_tailtooltip = true;
	}
	else if (obj->taillabel)
	    obj->tailtooltip = strdup(obj->taillabel);
        if ((s = agget(e, "headtooltip")) && s[0]) {
            obj->headtooltip = strdup_and_subst_edge(s, e);
	    obj->explicit_headtooltip = true;
	}
	else if (obj->headlabel)
	    obj->headtooltip = strdup(obj->headlabel);
    } 

    if (flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS)) {
        if (flags & (GVRENDER_DOES_MAP_RECTANGLE | GVRENDER_DOES_MAP_POLYGON)) {
            if (flags & GVRENDER_DOES_MAP_RECTANGLE) {
	        obj->url_map_shape = MAP_RECTANGLE;
	        nump = 2;
	    }
	    else { /* GVRENDER_DOES_MAP_POLYGON */
	        obj->url_map_shape = MAP_POLYGON;
	        nump = 4;
	    }

	    if (lab && (obj->url || obj->tooltip)) {
		obj->url_map_n = nump;
	        p = N_NEW(nump, pointf);
		P2RECT(lab->p, p, lab->dimen.x / 2., lab->dimen.y / 2.);
	    }

	    if (tlab && (obj->tailurl || obj->tailtooltip)) {
		obj->tailurl_map_n = nump;
	        pt = N_NEW(nump, pointf);
		P2RECT(tlab->p, pt, tlab->dimen.x / 2., tlab->dimen.y / 2.);
	    }

	    if (hlab && (obj->headurl || obj->headtooltip)) {
		obj->headurl_map_n = nump;
	        ph = N_NEW(nump, pointf);
		P2RECT(hlab->p, ph, hlab->dimen.x / 2., hlab->dimen.y / 2.);
	    }

           /* process intersecion with tail node */
            if (ED_spl(e) && (obj->tailurl || obj->tailtooltip)) {
		obj->tailendurl_map_n = nump;
	        pte = N_NEW(nump, pointf);
                bz = ED_spl(e)->list[0];
                if (bz.sflag) {
                    /* Arrow at start of splines */
		    P2RECT(bz.sp, pte, FUZZ, FUZZ);
                } else {
                    /* No arrow at start of splines */
		    P2RECT(bz.list[0], pte, FUZZ, FUZZ);
                }
            }
        
            /* process intersection with head node */
            if (ED_spl(e) && (obj->headurl || obj->headtooltip)) {
		obj->headendurl_map_n = nump;
	        phe = N_NEW(nump, pointf);
                bz = ED_spl(e)->list[ED_spl(e)->size - 1];
                if (bz.eflag) {
                    /* Arrow at end of splines */
		    P2RECT(bz.ep, phe, FUZZ, FUZZ);
                } else {
                    /* No arrow at end of splines */
		    P2RECT(bz.list[bz.size - 1], phe, FUZZ, FUZZ);
                }
            }

	    if (ED_spl(e) && (obj->url || obj->tooltip) && (flags & GVRENDER_DOES_MAP_POLYGON)) {
		int ns;
		splines *spl;

		spl = ED_spl(e);
		ns = spl->size; /* number of splines */
		for (i = 0; i < ns; i++)
		    map_output_bspline (&pbs, &pbs_n, &pbs_poly_n, spl->list+i);
		obj->url_bsplinemap_poly_n = pbs_poly_n;
		obj->url_bsplinemap_n = pbs_n;
	    }
	    
	    if (! (flags & GVRENDER_DOES_TRANSFORM)) {
		if (p) gvrender_ptf_A(job, p, p, 2);
		if (pt) gvrender_ptf_A(job, pt, pt, 2);
		if (ph) gvrender_ptf_A(job, ph, ph, 2);
		if (pte) gvrender_ptf_A(job, pte, pte, 2);
		if (phe) gvrender_ptf_A(job, phe, phe, 2);
		if (pbs) {
    		    for ( nump = 0, i = 0; i < pbs_poly_n; i++)
        		nump += pbs_n[i];
		    gvrender_ptf_A(job, pbs, pbs, nump);		
		}
	    }
	    if (! (flags & GVRENDER_DOES_MAP_RECTANGLE)) {
		if (p) rect2poly(p);
		if (pt) rect2poly(pt);
		if (ph) rect2poly(ph);
		if (pte) rect2poly(pte);
		if (phe) rect2poly(phe);
	    }

	}
	obj->url_map_p = p;
	obj->tailurl_map_p = pt;
	obj->headurl_map_p = ph;
	obj->tailendurl_map_p = pte;
	obj->headendurl_map_p = phe;
	obj->url_bsplinemap_p = pbs;
    }

    if (gvre) {
	if (gvre->begin_anchor && (obj->url || obj->explicit_tooltip))
	    gvre->begin_anchor(job, obj->url, obj->tooltip, obj->target);
	if (gvre->begin_edge)
	    gvre->begin_edge(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

        Obj = EDGE;
	if (cg && cg->begin_anchor && (obj->url || obj->explicit_tooltip))
	    cg->begin_anchor(obj->url, obj->tooltip, obj->target);
	if (cg && cg->begin_edge)
	    cg->begin_edge(e);
    }
#endif
}

void gvrender_end_edge(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;
    obj_state_t *obj = job->obj;

    if (gvre) {
	if (gvre->end_edge)
	    gvre->end_edge(job);
	if (gvre->end_anchor && (obj->url || obj->explicit_tooltip))
	    gvre->end_anchor(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_edge)
	    cg->end_edge();
	if (cg && cg->end_anchor && (obj->url || obj->explicit_tooltip))
	    cg->end_anchor();
    }
    Obj = NONE;
#endif
    pop_obj_state(job);
}

void gvrender_begin_context(GVJ_t * job)
{
    GVC_t *gvc = job->gvc;
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	(gvc->SP)++;
	assert((gvc->SP) < MAXNEST);
	gvc->styles[gvc->SP] = gvc->styles[(gvc->SP) - 1];
	job->style = &(gvc->styles[gvc->SP]);
    }
#ifdef WITH_CODEGENS
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
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	gvc->SP--;
	assert(gvc->SP >= 0);
	job->style = &(gvc->styles[gvc->SP]);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_context)
	    cg->end_context();
    }
#endif
}

void gvrender_begin_anchor(GVJ_t * job, char *href, char *tooltip, char *target)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->begin_anchor)
	    gvre->begin_anchor(job, href, tooltip, target);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->begin_anchor)
	    cg->begin_anchor(href, tooltip, target);
    }
#endif
}

void gvrender_end_anchor(GVJ_t * job)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->end_anchor)
	    gvre->end_anchor(job);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->end_anchor)
	    cg->end_anchor();
    }
#endif
}

void gvrender_set_font(GVJ_t * job, char *fontname, double fontsize)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	job->style->fontfam = fontname;
	job->style->fontsz = fontsize;
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->set_font)
	    cg->set_font(fontname, fontsize);
    }
#endif
}

void gvrender_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    gvrender_engine_t *gvre = job->render.engine;
    pointf PF;

    if (para->str && para->str[0]
	    && ( ! job->style  /* because of xdgen non-conformity */
		|| job->style->pen != PEN_NONE)) {
	if (job->flags & GVRENDER_DOES_TRANSFORM)
	    PF = p;
	else
	    PF = gvrender_ptf(job, p);
	if (gvre) {
	    if (gvre->textpara)
		gvre->textpara(job, PF, para);
	}
#ifdef WITH_CODEGENS
	else {
	    codegen_t *cg = job->codegen;
	    point P;

	    PF2P(p, P);
	    if (cg && cg->textpara)
		cg->textpara(P, para);
	}
#endif
    }
}

void gvrender_set_pencolor(GVJ_t * job, char *name)
{
    gvrender_engine_t *gvre = job->render.engine;
    gvcolor_t *color = &(job->style->pencolor);

    if (gvre) {
	gvrender_resolve_color(job->render.features, name, color);
	if (gvre->resolve_color)
	    gvre->resolve_color(job, color);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->set_pencolor)
	    cg->set_pencolor(name);
    }
#endif
}

void gvrender_set_fillcolor(GVJ_t * job, char *name)
{
    gvrender_engine_t *gvre = job->render.engine;
    gvcolor_t *color = &(job->style->fillcolor);

    if (gvre) {
	gvrender_resolve_color(job->render.features, name, color);
	if (gvre->resolve_color)
	    gvre->resolve_color(job, color);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->set_fillcolor)
	    cg->set_fillcolor(name);
    }
#endif
}

void gvrender_set_style(GVJ_t * job, char **s)
{
    gvrender_engine_t *gvre = job->render.engine;
    char *line, *p;
    gvstyle_t *style = job->style;

    job->rawstyle = s;
    if (gvre) {
	while ((p = line = *s++)) {
	    if (streq(line, "solid"))
		style->pen = PEN_SOLID;
	    else if (streq(line, "dashed"))
		style->pen = PEN_DASHED;
	    else if (streq(line, "dotted"))
		style->pen = PEN_DOTTED;
	    else if (streq(line, "invis") || streq(line, "invisible"))
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
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->set_style)
	    cg->set_style(s);
    }
#endif
}

void gvrender_ellipse(GVJ_t * job, pointf pf, double rx, double ry, bool filled)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->ellipse && job->style->pen != PEN_NONE) {
	    pointf af[2];

	    /* center */
	    af[0] = pf;
	    /* corner */
	    af[1].x = pf.x + rx;
	    af[1].y = pf.y + ry;

	    if (! (job->flags & GVRENDER_DOES_TRANSFORM))
	        gvrender_ptf_A(job, af, af, 2);
	    gvre->ellipse(job, af, filled);
	}
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->ellipse) {
	    point p;

	    PF2P(pf, p);
	    cg->ellipse(p, ROUND(rx), ROUND(ry), filled);
	}
    }
#endif
}

void gvrender_polygon(GVJ_t * job, pointf * af, int n, bool filled)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->polygon && job->style->pen != PEN_NONE) {
	    if (job->flags & GVRENDER_DOES_TRANSFORM)
		gvre->polygon(job, af, n, filled);
	    else {
	        if (sizeAF < n) {
		    sizeAF = n+10;
		    AF = grealloc(AF, sizeAF * sizeof(pointf));
	        }
	        gvrender_ptf_A(job, af, AF, n);
	        gvre->polygon(job, AF, n, filled);
	    }
	}
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;
        int i;

	if (sizeA < n) {
	    sizeA = n+10;
	    A = grealloc(A, sizeA * sizeof(point));
	}
	for (i = 0; i < n; i++)
	    PF2P(af[i], A[i]);
	if (cg && cg->polygon)
	    cg->polygon(A, n, filled);
    }
#endif
}

void gvrender_box(GVJ_t * job, boxf B, bool filled)
{
    pointf A[4];

    A[0] = B.LL;
    A[2] = B.UR;
    A[1].x = A[0].x;
    A[1].y = A[2].y;
    A[3].x = A[2].x;
    A[3].y = A[0].y;
    
    gvrender_polygon(job, A, 4, filled);
}

void gvrender_beziercurve(GVJ_t * job, pointf * af, int n,
			  int arrow_at_start, int arrow_at_end, bool filled)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->beziercurve && job->style->pen != PEN_NONE) {
	    if (job->flags & GVRENDER_DOES_TRANSFORM)
		gvre->beziercurve(job, af, n, arrow_at_start, arrow_at_end,filled);
	    else {
	        if (sizeAF < n) {
		    sizeAF = n+10;
		    AF = grealloc(AF, sizeAF * sizeof(pointf));
	        }
	        gvrender_ptf_A(job, af, AF, n);
	        gvre->beziercurve(job, AF, n, arrow_at_start, arrow_at_end,filled);
	    }
	}
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;
	int i;
	if (sizeA < n) {
	    sizeA = n+10;
	    A = grealloc(A, sizeA * sizeof(point));
	}
	for (i = 0; i < n; i++) {
	    PF2P(af[i], A[i]);
	}
	if (cg && cg->beziercurve)
	    cg->beziercurve(A, n, arrow_at_start, arrow_at_end, filled);
    }
#endif
}

void gvrender_polyline(GVJ_t * job, pointf * af, int n)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (gvre) {
	if (gvre->polyline && job->style->pen != PEN_NONE) {
	    if (job->flags & GVRENDER_DOES_TRANSFORM)
	        gvre->polyline(job, af, n);
            else {
	        if (sizeAF < n) {
		    sizeAF = n+10;
		    AF = grealloc(AF, sizeAF * sizeof(pointf));
	        }
	        gvrender_ptf_A(job, af, AF, n);
	        gvre->polyline(job, AF, n);
	    }
	}
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;
        int i;

	if (sizeA < n) {
	    sizeA = n+10;
	    A = grealloc(A, sizeA * sizeof(point));
	}
	for (i = 0; i < n; i++)
	    PF2P(af[i], A[i]);
	if (cg && cg->polyline)
	    cg->polyline(A, n);
    }
#endif
}

void gvrender_comment(GVJ_t * job, char *str)
{
    gvrender_engine_t *gvre = job->render.engine;

    if (!str || !str[0])
	return;

    if (gvre) {
	if (gvre->comment)
	    gvre->comment(job, str);
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->comment)
	    cg->comment(str);
    }
#endif
}

void gvrender_usershape(GVJ_t * job, char *name, pointf * a, int n, bool filled)
{
    gvrender_engine_t *gvre = job->render.engine;
    usershape_t *us;
    double iw, ih, pw, ph, tw, th;  
    double scalex, scaley;  /* scale factors */
    boxf b;		    /* target box */
    int i;
    pointf *af;

    if (! (us = gvusershape_find(name)))
        return;

    if (job->flags & GVRENDER_DOES_TRANSFORM)
	af = a;
    else {
        if (sizeAF < n) {
	    sizeAF = n+10;
	    AF = grealloc(AF, sizeAF * sizeof(pointf));
        }
	gvrender_ptf_A(job, a, AF, n);
	af = AF;
    }

    /* compute bb of polygon */
    b.LL = b.UR = af[0];
    for (i = 1; i < n; i++) {
	EXPANDBP(b, af[i]);
    }

    ih = (double)us->h;
    iw = (double)us->w;
    pw = b.UR.x - b.LL.x;
    ph = b.UR.y - b.LL.y;
    scalex = pw / iw;
    scaley = ph / ih;

    /* keep aspect ratio fixed by just using the smaller scale */
    if (scalex < scaley) {
	tw = iw * scalex;
	th = ih * scalex;
    } else {
	tw = iw * scaley;
	th = ih * scaley;
    }
    /* if image is smaller than target area then center it */
    if (tw < pw) {
	b.LL.x += (pw - tw) / 2.0;
	b.UR.x -= (pw - tw) / 2.0;
    }
    if (th < ph) {
	b.LL.y += (ph - th) / 2.0;
	b.UR.y -= (ph - th) / 2.0;
    }

    if (gvre) {
	if (job->render.features->loadimage_target)
	    gvloadimage(job, us, b, filled, job->render.features->loadimage_target);
    }
#ifdef WITH_CODEGENS
    else {
        codegen_t *cg = job->codegen;

	if (sizeA < n) {
	    sizeA = n+10;
	    A = grealloc(A, sizeA * sizeof(point));
	}
	for (i = 0; i < n; i++)
	    PF2P(a[i], A[i]);
        if (cg && cg->usershape)
            cg->usershape(us, b, A, n, filled);
    }
#endif
}
