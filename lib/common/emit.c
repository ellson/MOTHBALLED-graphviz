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
 *  graphics code generator
 */

#include	"render.h"
#include	"agxbuf.h"
#include	"utils.h"
#include	"htmltable.h"
#include	"gvc.h"
#include	<ctype.h>

#ifdef QUARTZ_RENDER
#include <QuickTime/QuickTime.h>
#endif

#define MAX_CODEGENS 100

#define PAGINATIONBUG 1

char *BaseLineStyle[3] = { "solid\0", "setlinewidth\0001\0", 0 };
int Obj;
static double Deffontsize;
static char *Deffontname;
static attrsym_t *G_peripheries;

static point exch_xy(point p)
{
    int t;
    t = p.x;
    p.x = p.y;
    p.y = t;
    return p;
}

static pointf exch_xyf(pointf p)
{
    double t;
    t = p.x;
    p.x = p.y;
    p.y = t;
    return p;
}

/* parse_layers:
 * Split input string into tokens, with separators specified by
 * the layersep attribute. Store the values in the gvc->layerIDs array,
 * starting at index 1, and return the count.
 * Free previously stored list. Note that there is no mechanism
 * to free the memory before exit.
 */
static int parse_layers(GVC_t *gvc, graph_t * g, char *p)
{
    int ntok;
    char *tok;
    int sz;

    gvc->layerDelims = agget(g, "layersep");
    if (!gvc->layerDelims)
	gvc->layerDelims = DEFAULT_LAYERSEP;

    ntok = 0;
    sz = 0;
    gvc->layers = strdup(p);

    for (tok = strtok(gvc->layers, gvc->layerDelims); tok;
	 tok = strtok(NULL, gvc->layerDelims)) {
	ntok++;
	if (ntok > sz) {
	    sz += SMALLBUF;
	    gvc->layerIDs = ALLOC(sz, gvc->layerIDs, char *);
	}
	gvc->layerIDs[ntok] = tok;
    }
    if (ntok) {
	gvc->layerIDs = RALLOC(ntok + 2, gvc->layerIDs, char *);	/* shrink to minimum size */
	gvc->layerIDs[0] = NULL;
	gvc->layerIDs[ntok + 1] = NULL;
    }

    return ntok;
}

static void init_layering(GVC_t * gvc, graph_t * g)
{
    char *str;

    /* free layer strings and pointers from previous graph */
    if (gvc->layers)
	free(gvc->layers);
    if (gvc->layerIDs)
	free(gvc->layerIDs);

    if ((str = agget(g, "layers")) != 0) {
	if (gvrender_features(gvc) & GVRENDER_DOES_LAYERS) {
	    gvc->numLayers = parse_layers(gvc, g, str);
	}
	else {
	    agerr(AGWARN, "layers not supported in %s output\n",
		  gvc->job->output_langname);
	    gvc->numLayers = 1;
	}
    } else {
	gvc->layerIDs = NULL;
	gvc->numLayers = 1;
    }
}

static void firstlayer(GVC_t *gvc)
{
    gvc->layerNum = 1;
}

static boolean validlayer(GVC_t *gvc)
{
    return (gvc->layerNum <= gvc->numLayers);
}

static void nextlayer(GVC_t *gvc)
{
    gvc->layerNum++;
}

static point pagecode(GVC_t *gvc, char c)
{
    point rv;
    rv.x = rv.y = 0;
    switch (c) {
    case 'T':
	gvc->pagesArrayFirst.y = gvc->pagesArraySize.y - 1;
	rv.y = -1;
	break;
    case 'B':
	rv.y = 1;
	break;
    case 'L':
	rv.x = 1;
	break;
    case 'R':
	gvc->pagesArrayFirst.x = gvc->pagesArraySize.x - 1;
	rv.x = -1;
	break;
    }
    return rv;
}

static void set_pagedir(GVC_t *gvc, graph_t * g)
{
    char *str;

    gvc->pagesArrayMajor.x = gvc->pagesArrayMajor.y 
		= gvc->pagesArrayMinor.x = gvc->pagesArrayMinor.y = 0;
    gvc->pagesArrayFirst.x = gvc->pagesArrayFirst.y = 0;
    str = agget(g, "pagedir");
    if (str && str[0]) {
	gvc->pagesArrayMajor = pagecode(gvc, str[0]);
	gvc->pagesArrayMinor = pagecode(gvc, str[1]);
    }
    if ((abs(gvc->pagesArrayMajor.x + gvc->pagesArrayMinor.x) != 1)
     || (abs(gvc->pagesArrayMajor.y + gvc->pagesArrayMinor.y) != 1)) {
	gvc->pagesArrayMajor = pagecode(gvc, 'B');
	gvc->pagesArrayMinor = pagecode(gvc, 'L');
	if (str)
	    agerr(AGWARN, "pagedir=%s ignored\n", str);
    }
}

static void init_job_pagination(GVC_t * gvc, graph_t * g)
{
    gvrender_job_t *job = gvc->job;
    pointf PFCLM;		/* page for centering less margins */
    pointf DS;			/* device drawable region for a page of the graph */

    /* determine pagination */
    job->pageBox.LL = job->margin;
    if (gvc->graph_sets_page) {
	/* page was set by user */
	point tp;
	job->pageBoxCentered = gvc->page;
	PFCLM.x = job->pageBoxCentered.x - 2 * job->pageBox.LL.x;
	PFCLM.y = job->pageBoxCentered.y - 2 * job->pageBox.LL.y;
	gvc->pageSize.x = PFCLM.x;
	gvc->pageSize.y = PFCLM.y;		/* convert to double */
	if (GD_drawing(g)->landscape)
	    gvc->pageSize = exch_xyf(gvc->pageSize);
	gvc->pageSize.x /= gvc->job->zoom;
	gvc->pageSize.y /= gvc->job->zoom;
	/* we don't want graph page to exceed its bounding box */
	gvc->pageSize.x = MIN(gvc->pageSize.x, gvc->bb.UR.x);
	gvc->pageSize.y = MIN(gvc->pageSize.y, gvc->bb.UR.y);
	gvc->pagesArraySize.x =
	    (gvc->pageSize.x > 0) ? ceil((gvc->bb.UR.x) / gvc->pageSize.x) : 1;
	gvc->pagesArraySize.y =
	    (gvc->pageSize.y > 0) ? ceil((gvc->bb.UR.y) / gvc->pageSize.y) : 1;
	gvc->numPages = gvc->pagesArraySize.x * gvc->pagesArraySize.y;

	/* find the drawable size in device coords */
#ifdef PAGINATIONBUG
	tp = GD_drawing(g)->size;
#else
	tp.x = gvc->job->width;
	tp.y = gvc->job->height;
#endif
	if (GD_drawing(g)->landscape)
	    tp = exch_xy(tp);
	DS.x = MIN(tp.x, PFCLM.x);
	DS.y = MIN(tp.y, PFCLM.y);
    } else {
	/* page not set by user, assume default when centering,
	   but allow infinite page for any other interpretation */
	gvc->pageSize.x = gvc->bb.UR.x;
	gvc->pageSize.y = gvc->bb.UR.y;
	job->pageBoxCentered.x = DEFAULT_PAGEWD;
	job->pageBoxCentered.y = DEFAULT_PAGEHT;
	PFCLM.x = job->pageBoxCentered.x - 2 * job->pageBox.LL.x;
	PFCLM.y = job->pageBoxCentered.y - 2 * job->pageBox.LL.y;
#ifdef PAGINATIONBUG
	P2PF(GD_drawing(g)->size, DS);
#else
	DS.x = gvc->job->width;
	DS.y = gvc->job->height;
#endif
	if (GD_drawing(g)->landscape)
	    DS = exch_xyf(DS);
	gvc->pagesArraySize.x = gvc->pagesArraySize.y = gvc->numPages = 1;
    }

    set_pagedir(gvc, g);

    /* determine page box including centering */
    if (GD_drawing(g)->centered) {
	point extra;
	if ((extra.x = PFCLM.x - DS.x) < 0)
	    extra.x = 0;
	if ((extra.y = PFCLM.y - DS.y) < 0)
	    extra.y = 0;
	job->pageBox.LL.x += extra.x / 2;
	job->pageBox.LL.y += extra.y / 2;
    }
    job->pageBox.UR = add_pointfs(job->pageBox.LL, DS);
}

static void firstpage(GVC_t *gvc)
{
    gvc->pagesArrayElem = gvc->pagesArrayFirst;
    gvc->pageNum = 1;
}

static int validpage(GVC_t *gvc)
{
    return ((gvc->pagesArrayElem.x >= 0)
	 && (gvc->pagesArrayElem.x < gvc->pagesArraySize.x)
	 && (gvc->pagesArrayElem.y >= 0)
	 && (gvc->pagesArrayElem.y < gvc->pagesArraySize.y));
}

static void nextpage(GVC_t *gvc)
{
    gvc->pagesArrayElem = add_points(gvc->pagesArrayElem, gvc->pagesArrayMinor);
    if (validpage(gvc) == FALSE) {
	if (gvc->pagesArrayMajor.y)
	    gvc->pagesArrayElem.x = gvc->pagesArrayFirst.x;
	else
	    gvc->pagesArrayElem.y = gvc->pagesArrayFirst.y;
	gvc->pagesArrayElem = add_points(gvc->pagesArrayElem, gvc->pagesArrayMajor);
    }
    gvc->pageNum = gvc->pagesArrayElem.x + gvc->pagesArrayElem.y * gvc->pagesArraySize.x + 1;
}

static int write_edge_test(Agraph_t * g, Agedge_t * e)
{
    Agraph_t *sg;
    int c;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	sg = GD_clust(g)[c];
	if (agcontains(sg, e))
	    return FALSE;
    }
    return TRUE;
}

static int write_node_test(Agraph_t * g, Agnode_t * n)
{
    Agraph_t *sg;
    int c;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	sg = GD_clust(g)[c];
	if (agcontains(sg, n))
	    return FALSE;
    }
    return TRUE;
}

void emit_reset(GVC_t * gvc, graph_t * g)
{
    Agnode_t *n;

    /* reset state */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	ND_state(n) = 0;
    }

    /* reset renderer */
    gvrender_reset(gvc);
}

static void emit_background(GVC_t * gvc, boxf pageBox)
{
    char *str;
    point A[4];
    graph_t *g = gvc->g;
    box PB;

    if (((str = agget(g, "bgcolor")) != 0)
	&& str[0]
	&& strcmp(str, "white") != 0 && strcmp(str, "transparent") != 0) {
	/* increment to cover int rounding errors */
	BF2B(pageBox, PB);
	A[0].x = A[1].x = PB.LL.x - GD_drawing(g)->margin.x - 1;
	A[2].x = A[3].x = PB.UR.x + GD_drawing(g)->margin.x + 1;
	A[1].y = A[2].y = PB.UR.y + GD_drawing(g)->margin.y + 1;
	A[3].y = A[0].y = PB.LL.y - GD_drawing(g)->margin.y - 1;
	gvrender_set_fillcolor(gvc, str);
	gvrender_set_pencolor(gvc, str);
	gvrender_polygon(gvc, A, 4, TRUE);	/* filled */
    }
}

static void emit_defaults(GVC_t * gvc)
{
    gvrender_set_pencolor(gvc, DEFAULT_COLOR);
    gvrender_set_font(gvc, Deffontname, Deffontsize);
}


/* even if this makes you cringe, at least it's short */
static void setup_page(GVC_t * gvc)
{
    point offset;
    int rot;
    graph_t *g = gvc->g;

    /* establish current box in graph coordinates */
    gvc->pageBox.LL.x = gvc->pagesArrayElem.x * gvc->pageSize.x;
    gvc->pageBox.LL.y = gvc->pagesArrayElem.y * gvc->pageSize.y;
    gvc->pageBox.UR.x = gvc->pageBox.LL.x + gvc->pageSize.x;
    gvc->pageBox.UR.y = gvc->pageBox.LL.y + gvc->pageSize.y;

    /* establish offset to be applied, in graph coordinates */
    if (GD_drawing(g)->landscape == FALSE)
	offset = pointof(-gvc->pageBox.LL.x, -gvc->pageBox.LL.y);
    else {
	offset.x = (gvc->pagesArrayElem.y + 1) * gvc->pageSize.y;
	offset.y = -(gvc->pagesArrayElem.x) * gvc->pageSize.x;
    }
    rot = GD_drawing(g)->landscape ? 90 : 0;

    gvrender_begin_page(gvc, gvc->job->zoom, rot, offset);
    emit_background(gvc, gvc->pageBox);
    emit_defaults(gvc);
}

static boolean node_in_pageBox(GVC_t *gvc, node_t * n)
{
    boxf nb;

    if (gvc->numPages == 1)
	return TRUE;
    nb.LL.x = ND_coord_i(n).x - ND_lw_i(n);
    nb.LL.y = ND_coord_i(n).y - ND_ht_i(n) / 2.;
    nb.UR.x = ND_coord_i(n).x + ND_rw_i(n);
    nb.UR.y = ND_coord_i(n).y + ND_ht_i(n) / 2.;
    return boxf_overlap(gvc->pageBox, nb);
}

static int is_natural_number(char *sstr)
{
    unsigned char *str = (unsigned char *) sstr;
    while (*str)
	if (NOT(isdigit(*str++)))
	    return FALSE;
    return TRUE;
}

static int layer_index(GVC_t *gvc, char *str, int all)
{
    int i;

    if (streq(str, "all"))
	return all;
    if (is_natural_number(str))
	return atoi(str);
    if (gvc->layerIDs)
	for (i = 1; i <= gvc->numLayers; i++)
	    if (streq(str, gvc->layerIDs[i]))
		return i;
    return -1;
}

static int selectedlayer(GVC_t *gvc, char *spec)
{
    int n0, n1;
    unsigned char buf[SMALLBUF];
    char *w0, *w1;
    agxbuf xb;
    int rval = FALSE;

    agxbinit(&xb, SMALLBUF, buf);
    agxbput(&xb, spec);
    w1 = w0 = strtok(agxbuse(&xb), gvc->layerDelims);
    if (w0)
	w1 = strtok(NULL, gvc->layerDelims);
    switch ((w0 != NULL) + (w1 != NULL)) {
    case 0:
	rval = FALSE;
	break;
    case 1:
	n0 = layer_index(gvc, w0, gvc->layerNum);
	rval = (n0 == gvc->layerNum);
	break;
    case 2:
	n0 = layer_index(gvc, w0, 0);
	n1 = layer_index(gvc, w1, gvc->numLayers);
	if ((n0 < 0) || (n1 < 0))
	    rval = TRUE;
	else if (n0 > n1) {
	    int t = n0;
	    n0 = n1;
	    n1 = t;
	}
	rval = BETWEEN(n0, gvc->layerNum, n1);
	break;
    }
    agxbfree(&xb);
    return rval;
}

static int node_in_layer(GVC_t *gvc, graph_t * g, node_t * n)
{
    char *pn, *pe;
    edge_t *e;

    if (gvc->numLayers <= 1)
	return TRUE;
    pn = late_string(n, N_layer, "");
    if (selectedlayer(gvc, pn))
	return TRUE;
    if (pn[0])
	return FALSE;		/* Only check edges if pn = "" */
    if ((e = agfstedge(g, n)) == NULL)
	return TRUE;
    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	pe = late_string(e, E_layer, "");
	if ((pe[0] == '\0') || selectedlayer(gvc, pe))
	    return TRUE;
    }
    return FALSE;
}

static int edge_in_layer(GVC_t *gvc, graph_t * g, edge_t * e)
{
    char *pe, *pn;
    int cnt;

    if (gvc->numLayers <= 1)
	return TRUE;
    pe = late_string(e, E_layer, "");
    if (selectedlayer(gvc, pe))
	return TRUE;
    if (pe[0])
	return FALSE;
    for (cnt = 0; cnt < 2; cnt++) {
	pn = late_string(cnt < 1 ? e->tail : e->head, N_layer, "");
	if ((pn[0] == '\0') || selectedlayer(gvc, pn))
	    return TRUE;
    }
    return FALSE;
}

static int clust_in_layer(GVC_t *gvc, graph_t * sg)
{
    char *pg;
    node_t *n;

    if (gvc->numLayers <= 1)
	return TRUE;
    pg = late_string(sg, agfindattr(sg, "layer"), "");
    if (selectedlayer(gvc, pg))
	return TRUE;
    if (pg[0])
	return FALSE;
    for (n = agfstnode(sg); n; n = agnxtnode(sg, n))
	if (node_in_layer(gvc, sg, n))
	    return TRUE;
    return FALSE;
}

static void emit_node(GVC_t * gvc, node_t * n)
{
    char *s, *url = NULL, *tooltip = NULL, *target = NULL;

    if (ND_shape(n) == NULL)
	return;
    if (node_in_layer(gvc, n->graph, n) && node_in_pageBox(gvc, n) && (ND_state(n) != gvc->pageNum)) {
	gvrender_begin_node(gvc, n);
	if (((s = agget(n, "href")) && s[0])
	    || ((s = agget(n, "URL")) && s[0])) {
	    url = strdup_and_subst_node(s, n);
	    if ((s = agget(n, "tooltip")) && s[0])
		tooltip = strdup_and_subst_node(s, n);
	    else
		tooltip = strdup_and_subst_node(ND_label(n)->text, n);
	    if ((s = agget(n, "target")) && s[0])
		target = strdup_and_subst_node(s, n);
	    gvrender_begin_anchor(gvc, url, tooltip, target);
	}
	gvrender_begin_context(gvc);
	ND_shape(n)->fns->codefn(gvc, n);
	ND_state(n) = gvc->pageNum;
	gvrender_end_context(gvc);
	if (url) {
	    gvrender_end_anchor(gvc);
	    free(url);
	    if (tooltip)
		free(tooltip);
	    if (target)
		free(target);
	}
	gvrender_end_node(gvc);
    }
}

#define EPSILON .0001

/* calculate an offset vector, length d, perpendicular to line p,q */
static pointf computeoffset_p(pointf p, pointf q, double d)
{
    pointf res;
    double x = p.x - q.x, y = p.y - q.y;

    /* keep d finite as line length approaches 0 */
    d /= sqrt(x * x + y * y + EPSILON);
    res.x = y * d;
    res.y = -x * d;
    return res;
}

/* calculate offset vector, length d, perpendicular to spline p,q,r,s at q&r */
static pointf computeoffset_qr(pointf p, pointf q, pointf r, pointf s,
			       double d)
{
    pointf res;
    double len;
    double x = q.x - r.x, y = q.y - r.y;

    len = sqrt(x * x + y * y);
    if (len < EPSILON) {
	/* control points are on top of each other
	   use slope between endpoints instead */
	x = p.x - s.x, y = p.y - s.y;
	/* keep d finite as line length approaches 0 */
	len = sqrt(x * x + y * y + EPSILON);
    }
    d /= len;
    res.x = y * d;
    res.y = -x * d;
    return res;
}

void emit_attachment(GVC_t * gvc, textlabel_t * lp, splines * spl)
{
    point sz, A[3];
    unsigned char *s;

    for (s = (unsigned char *) (lp->text); *s; s++) {
	if (isspace(*s) == FALSE)
	    break;
    }
    if (*s == 0)
	return;

    PF2P(lp->dimen, sz);
    A[0] = pointof(lp->p.x + sz.x / 2, lp->p.y - sz.y / 2);
    A[1] = pointof(A[0].x - sz.x, A[0].y);
    A[2] = dotneato_closest(spl, lp->p);
    /* Don't use edge style to draw attachment */
    gvrender_set_style(gvc, BaseLineStyle);
    /* Use font color to draw attachment
       - need something unambiguous in case of multicolored parallel edges
       - defaults to black for html-like labels
     */
    gvrender_set_pencolor(gvc, lp->fontcolor);
    gvrender_polyline(gvc, A, 3);
}

static boolean edge_in_pageBox(GVC_t *gvc, edge_t * e)
{
    int i, j, np;
    bezier bz;
    point *p;
    pointf pp, pn;
    double sx, sy;
    boxf b;
    textlabel_t *lp;

    if (gvc->numPages == 1)
	return TRUE;
    if (ED_spl(e) == NULL)
	return FALSE;
    for (i = 0; i < ED_spl(e)->size; i++) {
	bz = ED_spl(e)->list[i];
	np = bz.size;
	p = bz.list;
	P2PF(p[0],pp);
	for (j = 0; j < np; j++) {
	    P2PF(p[j],pn);
	    if (boxf_overlap(gvc->pageBox, mkboxf(pp, pn)))
		return TRUE;
	    pp = pn;
	}
    }
    if ((lp = ED_label(e)) == NULL)
	return FALSE;
    sx = lp->dimen.x / 2.;
    sy = lp->dimen.y / 2.;
    b.LL.x = lp->p.x - sx;
    b.UR.x = lp->p.x + sx;
    b.LL.y = lp->p.y - sy;
    b.UR.y = lp->p.y + sy;
    return boxf_overlap(gvc->pageBox, b);
}

static void emit_edge(GVC_t * gvc, edge_t * e)
{
    int i, j, cnum, numc = 0;
    char *color, *style;
    char *colors = NULL;
    char **styles = 0;
    char **sp;
    bezier bz = { 0, 0, 0, 0 };
    bezierf bzf;
    splinesf offspl, tmpspl;
    pointf pf0, pf1, pf2 = { 0, 0 }, pf3, *offlist, *tmplist;
    boolean saved = FALSE;
    double scale, numc2;
    char *p;
    char *s, *url = NULL, *label = NULL, *tooltip = NULL, *target = NULL;
    textlabel_t *lab = NULL;

#define SEP 2.0

    if ((edge_in_pageBox(gvc, e) == FALSE)
	|| (edge_in_layer(gvc, e->head->graph, e) == FALSE))
	return;

    gvrender_begin_edge(gvc, e);
    if (((s = agget(e, "href")) && s[0])
	|| ((s = agget(e, "URL")) && s[0])) {
	url = strdup_and_subst_edge(s, e);
	if ((lab = ED_label(e))) {
	    label = lab->text;
	}
	if ((s = agget(e, "tooltip")) && s[0])
	    tooltip = strdup_and_subst_edge(s, e);
	else if (label)
	    tooltip = strdup_and_subst_edge(label, e);
	if ((s = agget(e, "target")) && s[0])
	    target = strdup_and_subst_edge(s, e);
	gvrender_begin_anchor(gvc, url, tooltip, target);
    }
    style = late_string(e, E_style, "");
    /* We shortcircuit drawing an invisible edge because the arrowhead
     * code resets the style to solid, and most of the code generators
     * (except PostScript) won't honor a previous style of invis.
     */
    if (style[0]) {
	styles = parse_style(style);
	sp = styles;
	while ((p = *sp++)) {
	    if (streq(p, "invis")) {
		gvrender_end_edge(gvc);
		return;
	    }
	}
    }
    if (ED_spl(e)) {
	scale = late_double(e, E_arrowsz, 1.0, 0.0);
	color = late_string(e, E_color, "");

	if (color[0] || styles) {
	    gvrender_begin_context(gvc);
	    if (styles)
		gvrender_set_style(gvc, styles);
	    saved = TRUE;
	}
	/* need to know how many colors separated by ':' */
	for (p = color; *p; p++)
	    if (*p == ':')
		numc++;

	/* if more than one color - then generate parallel beziers, one per color */
	if (numc) {
	    /* calculate and save offset vector spline and initialize first offset spline */
	    tmpspl.size = offspl.size = ED_spl(e)->size;
	    offspl.list = malloc(sizeof(bezier) * offspl.size);
	    tmpspl.list = malloc(sizeof(bezier) * tmpspl.size);
	    numc2 = (2 + numc) / 2.0;
	    for (i = 0; i < offspl.size; i++) {
		bz = ED_spl(e)->list[i];
		tmpspl.list[i].size = offspl.list[i].size = bz.size;
		offlist = offspl.list[i].list =
		    malloc(sizeof(pointf) * bz.size);
		tmplist = tmpspl.list[i].list =
		    malloc(sizeof(pointf) * bz.size);
		P2PF(bz.list[0], pf3);
		for (j = 0; j < bz.size - 1; j += 3) {
		    pf0 = pf3;
		    P2PF(bz.list[j + 1], pf1);
		    /* calculate perpendicular vectors for each bezier point */
		    if (j == 0)	/* first segment, no previous pf2 */
			offlist[j] = computeoffset_p(pf0, pf1, SEP);
		    else	/* i.e. pf2 is available from previous segment */
			offlist[j] = computeoffset_p(pf2, pf1, SEP);

		    P2PF(bz.list[j + 2], pf2);
		    P2PF(bz.list[j + 3], pf3);
		    offlist[j + 1] = offlist[j + 2] =
			computeoffset_qr(pf0, pf1, pf2, pf3, SEP);
		    /* initialize tmpspl to outermost position */
		    tmplist[j].x = pf0.x - numc2 * offlist[j].x;
		    tmplist[j].y = pf0.y - numc2 * offlist[j].y;
		    tmplist[j + 1].x = pf1.x - numc2 * offlist[j + 1].x;
		    tmplist[j + 1].y = pf1.y - numc2 * offlist[j + 1].y;
		    tmplist[j + 2].x = pf2.x - numc2 * offlist[j + 2].x;
		    tmplist[j + 2].y = pf2.y - numc2 * offlist[j + 2].y;
		}
		/* last segment, no next pf1 */
		offlist[j] = computeoffset_p(pf2, pf3, SEP);
		tmplist[j].x = pf3.x - numc2 * offlist[j].x;
		tmplist[j].y = pf3.y - numc2 * offlist[j].y;
	    }
	    colors = strdup(color);
	    for (cnum = 0, color = strtok(colors, ":"); color;
		 cnum++, color = strtok(0, ":")) {
		if (color[0]) {
		    gvrender_set_pencolor(gvc, color);
		    gvrender_set_fillcolor(gvc, color);
		} else {
		    gvrender_set_fillcolor(gvc, DEFAULT_COLOR);
		}
		for (i = 0; i < tmpspl.size; i++) {
		    tmplist = tmpspl.list[i].list;
		    offlist = offspl.list[i].list;
		    for (j = 0; j < tmpspl.list[i].size; j++) {
			tmplist[j].x += offlist[j].x;
			tmplist[j].y += offlist[j].y;
		    }
		    gvrender_beziercurve(gvc, tmplist, tmpspl.list[i].size,
					 FALSE, FALSE);
		}
	    }
	    if (bz.sflag)
		arrow_gen(gvc, bz.sp, bz.list[0], scale, bz.sflag);
	    if (bz.eflag)
		arrow_gen(gvc, bz.ep, bz.list[bz.size - 1], scale,
			  bz.eflag);
	    free(colors);
	    for (i = 0; i < offspl.size; i++) {
		free(offspl.list[i].list);
		free(tmpspl.list[i].list);
	    }
	    free(offspl.list);
	    free(tmpspl.list);
	} else {
	    if (color[0]) {
		gvrender_set_pencolor(gvc, color);
		gvrender_set_fillcolor(gvc, color);
	    } else {
		gvrender_set_fillcolor(gvc, DEFAULT_COLOR);
	    }
	    for (i = 0; i < ED_spl(e)->size; i++) {
		bz = ED_spl(e)->list[i];
		/* convert points to pointf for gvrender api */
		bzf.size = bz.size;
		bzf.list = malloc(sizeof(pointf) * bzf.size);
		for (j = 0; j < bz.size; j++)
		    P2PF(bz.list[j], bzf.list[j]);
		if (gvrender_features(gvc) & GVRENDER_DOES_ARROWS) {
		    gvrender_beziercurve(gvc, bzf.list, bz.size, bz.sflag,
					 bz.eflag);
		} else {
		    gvrender_beziercurve(gvc, bzf.list, bz.size, FALSE,
					 FALSE);
		    if (bz.sflag)
			arrow_gen(gvc, bz.sp, bz.list[0], scale, bz.sflag);
		    if (bz.eflag)
			arrow_gen(gvc, bz.ep, bz.list[bz.size - 1], scale,
				  bz.eflag);
		}
		free(bzf.list);
	    }
	}
    }
    if (ED_label(e)) {
	emit_label(gvc, ED_label(e), (void *) e);
	if (mapbool(late_string(e, E_decorate, "false")) && ED_spl(e))
	    emit_attachment(gvc, ED_label(e), ED_spl(e));
    }
    if (ED_head_label(e))
	emit_label(gvc, ED_head_label(e), (void *) e);	/* vladimir */
    if (ED_tail_label(e))
	emit_label(gvc, ED_tail_label(e), (void *) e);	/* vladimir */

    if (saved)
	gvrender_end_context(gvc);
    if (url) {
	gvrender_end_anchor(gvc);
	free(url);
	if (tooltip)
	    free(tooltip);
	if (target)
	    free(target);
    }
    gvrender_end_edge(gvc);
}

#ifdef PAGINATIONBUG
static double setScale(graph_t * g)
{
    double xscale, yscale, scale;

    xscale = ((double) GD_drawing(g)->size.x) / GD_bb(g).UR.x;
    yscale = ((double) GD_drawing(g)->size.y) / GD_bb(g).UR.y;
    scale = MIN(xscale, yscale);
    GD_drawing(g)->scale = scale;
    GD_drawing(g)->size.x = scale * GD_bb(g).UR.x;
    GD_drawing(g)->size.y = scale * GD_bb(g).UR.y;
    return scale;
}
#endif


/* emit_init
 *   - called just once per output device
 *     (where emit_graph can be called many times for refresh callbacks)
 */
void emit_init(GVC_t * gvc, graph_t * g)
{
    char *str;
    double X, Y, Z, x, y;
    point size = GD_drawing(g)->size;
    point UR = GD_bb(g).UR;
#ifdef PAGINATIONBUG
    double scale;
#endif

    assert((GD_bb(g).LL.x == 0) && (GD_bb(g).LL.y == 0));

    /* determine final drawing size and scale to apply. */
    /* N.B. size given by user is not rotated by landscape mode */
    /* start with "natural" size of layout */
#ifdef PAGINATIONBUG
    /* FIXME - this version still needed by psgen.c*/
    scale = GD_drawing(g)->scale = 1.0;
    if (GD_drawing(g)->size.x > 0) {    /* was given by user... */
        if ((GD_drawing(g)->size.x < GD_bb(g).UR.x)     /* drawing is too big... */
            ||(GD_drawing(g)->size.y < GD_bb(g).UR.y)) {
            scale = setScale(g);
        }
	else if (GD_drawing(g)->filled) {
            if ((GD_drawing(g)->size.x > GD_bb(g).UR.x) /* drawing is too small... */
                &&(GD_drawing(g)->size.y > GD_bb(g).UR.y)) {
               scale = setScale(g);
            }
        }
	else {
            GD_drawing(g)->size = GD_bb(g).UR;
	}
    }
    else {
        GD_drawing(g)->size = GD_bb(g).UR;
    }
#endif

    Z = 1.0;
    if (size.x > 0) {	/* was given by user... */
	if ((size.x < UR.x) || (size.y < UR.y) /* drawing is too big... */
	    || ((GD_drawing(g)->filled) /* or ratio=filled requested and ... */
		&& (size.x > UR.x) && (size.y > UR.y))) /* drawing is too small... */
	    Z = MIN(((double)size.x)/UR.x, ((double)size.y)/UR.y);
    }
    X = Z * (double)(GD_bb(g).UR.x + 2 * GD_drawing(g)->margin.x + 2);
    Y = Z * (double)(GD_bb(g).UR.y + 2 * GD_drawing(g)->margin.y + 2);
    x = (double)(GD_bb(g).UR.x) / 2.;
    y = (double)(GD_bb(g).UR.y) / 2.;

    if ((str = agget(g, "viewport")))
	sscanf(str, "%lf,%lf,%lf,%lf,%lf", &X, &Y, &Z, &x, &y);

    G_peripheries = agfindattr(g, "peripheries");

    Deffontname = late_nnstring(g->proto->n, N_fontname, DEFAULT_FONTNAME);
    Deffontsize =
	late_double(g->proto->n, N_fontsize, DEFAULT_FONTSIZE,
		    MIN_FONTSIZE);

    init_layering(gvc, g);

    init_job_pagination(gvc, g);

    gvrender_begin_job(gvc, Lib, X, Y, Z, x, y, GD_drawing(g)->dpi);
}

void emit_deinit(GVC_t * gvc)
{
    gvrender_end_job(gvc);
}

static void init_job_margin(GVC_t *gvc)
{
    gvrender_job_t *job = gvc->job;
    
    if (gvc->graph_sets_margin) {
	job->margin = gvc->margin;
    }
    else {
        /* set default margins depending on format */
        switch (gvc->job->output_lang) {
        case GVRENDER_PLUGIN:
            job->margin.x = job->margin.y = job->render_features->default_margin;
            break;
        case POSTSCRIPT: case PDF: case HPGL: case PCL: case MIF:
        case METAPOST: case FIG: case VTX: case ATTRIBUTED_DOT:
        case PLAIN: case PLAIN_EXT: case QPDF:
            job->margin.x = job->margin.y = DEFAULT_MARGIN;
            break;
        case CANONICAL_DOT:
            job->margin.x = job->margin.y = 0;
            break;
        default:
            job->margin.x = job->margin.y = DEFAULT_EMBED_MARGIN;
            break;
        }
    }
}

void emit_graph(GVC_t * gvc, graph_t * g, int flags)
{
    graph_t *sg;
    node_t *n;
    edge_t *e;
    int c;
    char *str, *colors;
    char *s, *url = NULL, *tooltip = NULL, *target = NULL;

    init_job_margin(gvc);

    init_job_pagination(gvc, g);

    gvrender_begin_graph(gvc, g);
    if (flags & EMIT_COLORS) {
	gvrender_set_fillcolor(gvc, DEFAULT_FILL);
	if (((str = agget(g, "bgcolor")) != 0) && str[0])
	    gvrender_set_fillcolor(gvc, str);
	if (((str = agget(g, "fontcolor")) != 0) && str[0])
	    gvrender_set_pencolor(gvc, str);
	for (c = 1; c <= GD_n_cluster(g); c++) {
	    sg = GD_clust(g)[c];
	    if (((str = agget(sg, "color")) != 0) && str[0])
		gvrender_set_pencolor(gvc, str);
	    if (((str = agget(sg, "fillcolor")) != 0) && str[0])
		gvrender_set_fillcolor(gvc, str);
	    if (((str = agget(sg, "fontcolor")) != 0) && str[0])
		gvrender_set_pencolor(gvc, str);
	}
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    if (((str = agget(n, "color")) != 0) && str[0])
		gvrender_set_pencolor(gvc, str);
	    if (((str = agget(n, "fillcolor")) != 0) && str[0])
		gvrender_set_fillcolor(gvc, str);
	    if (((str = agget(n, "fontcolor")) != 0) && str[0])
		gvrender_set_pencolor(gvc, str);
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		if (((str = agget(e, "color")) != 0) && str[0]) {
		    if (strchr(str, ':')) {
			colors = strdup(str);
			for (str = strtok(colors, ":"); str;
			     str = strtok(0, ":")) {
			    if (str[0])
				gvrender_set_pencolor(gvc, str);
			}
			free(colors);
		    } else
			gvrender_set_pencolor(gvc, str);
		}
		if (((str = agget(e, "fontcolor")) != 0) && str[0])
		    gvrender_set_pencolor(gvc, str);
	    }
	}
    }

    for (firstlayer(gvc); validlayer(gvc); nextlayer(gvc)) {
	if (gvc->numLayers > 1)
	    gvrender_begin_layer(gvc);
	for (firstpage(gvc); validpage(gvc); nextpage(gvc)) {
    	    setup_page(gvc);
	    Obj = NONE;
	    if (((s = agget(g, "href")) && s[0])
		|| ((s = agget(g, "URL")) && s[0])) {
		url = strdup_and_subst_graph(s, g);
		if ((s = agget(g, "target")) && s[0])
		    target = strdup_and_subst_graph(s, g);
		if ((s = agget(g, "tooltip")) && s[0])
		    tooltip = strdup_and_subst_graph(s, g);
		else if (GD_label(g))
		    tooltip = strdup_and_subst_graph(GD_label(g)->text, g);
		gvrender_begin_anchor(gvc, url, tooltip, target);
	    }
	    if (GD_label(g))
		emit_label(gvc, GD_label(g), (void *) g);
	    Obj = CLST;
	    /* when drawing, lay clusters down before nodes and edges */
	    if (!(flags & EMIT_CLUSTERS_LAST)) {
		emit_clusters(gvc, g, flags);
	    }
	    if (flags & EMIT_SORTED) {
		/* output all nodes, then all edges */
		Obj = NODE;
		gvrender_begin_nodes(gvc);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    emit_node(gvc, n);
		}
		gvrender_end_nodes(gvc);
		Obj = EDGE;
		gvrender_begin_edges(gvc);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			emit_edge(gvc, e);
		    }
		}
		gvrender_end_edges(gvc);
	    } else if (flags & EMIT_EDGE_SORTED) {
		/* output all edges, then all nodes */
		Obj = EDGE;
		gvrender_begin_edges(gvc);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			emit_edge(gvc, e);
		    }
		}
		gvrender_end_edges(gvc);
		Obj = NODE;
		gvrender_begin_nodes(gvc);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    emit_node(gvc, n);
		}
		gvrender_end_nodes(gvc);
	    } else if (flags & EMIT_PREORDER) {
		Obj = NODE;
		gvrender_begin_nodes(gvc);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    if (write_node_test(g, n))
			emit_node(gvc, n);
		}
		gvrender_end_nodes(gvc);
		Obj = EDGE;
		gvrender_begin_edges(gvc);

		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			if (write_edge_test(g, e))
			    emit_edge(gvc, e);
		    }
		}
		gvrender_end_edges(gvc);
	    } else {
		/* output in breadth first graph walk order */
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    Obj = NODE;
		    emit_node(gvc, n);
		    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			Obj = NODE;
			emit_node(gvc, e->head);
			Obj = EDGE;
			emit_edge(gvc, e);
		    }
		}
	    }
	    /* when mapping, detect events on clusters after nodes and edges */
	    if (flags & EMIT_CLUSTERS_LAST) {
		emit_clusters(gvc, g, flags);
	    }
	    Obj = NONE;
	    if (url) {
		gvrender_end_anchor(gvc);
		free(url);
		url = NULL;
		if (tooltip) {
		    free(tooltip);
		    tooltip = NULL;
		}
		if (target) {
		    free(target);
		    target = NULL;
		}
	    }
	    gvrender_end_page(gvc);
	}
	if (gvc->numLayers > 1)
	    gvrender_end_layer(gvc);
    }
    gvrender_end_graph(gvc);
}

void emit_eof(GVC_t * gvc)
{
    if (gvc->pageNum > 0) {
        emit_deinit(gvc);
	emit_once_reset();
    }
}

void emit_clusters(GVC_t * gvc, Agraph_t * g, int flags)
{
    int i, c, filled;
    graph_t *sg;
    point A[4];
    char *str, **style;
    node_t *n;
    edge_t *e;
    char *s, *url = NULL, *tooltip = NULL, *target = NULL;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	sg = GD_clust(g)[c];
	if (clust_in_layer(gvc, sg) == FALSE)
	    continue;
	/* when mapping, detect events on clusters after sub_clusters */
	if (flags & EMIT_CLUSTERS_LAST) {
	    emit_clusters(gvc, sg, flags);
	}
	Obj = CLST;
	gvrender_begin_cluster(gvc, sg);
	if (((s = agget(sg, "href")) && s[0])
	    || ((s = agget(sg, "URL")) && s[0])) {
	    url = strdup_and_subst_graph(s, sg);
	    if ((s = agget(sg, "target")) && s[0])
		target = strdup_and_subst_graph(s, sg);
	    if ((s = agget(sg, "tooltip")) && s[0])
		tooltip = strdup_and_subst_graph(s, sg);
	    else
		tooltip = strdup_and_subst_graph(GD_label(sg)->text, sg);
	    gvrender_begin_anchor(gvc, url, tooltip, target);
	}
	gvrender_begin_context(gvc);
	filled = FALSE;
	if (((str = agget(sg, "style")) != 0) && str[0]) {
	    gvrender_set_style(gvc, (style = parse_style(str)));
	    for (i = 0; style[i]; i++)
		if (strcmp(style[i], "filled") == 0) {
		    filled = TRUE;
		    break;
		}
	}
	if (((str = agget(sg, "pencolor")) != 0) && str[0])
	    gvrender_set_pencolor(gvc, str);
	else if (((str = agget(sg, "color")) != 0) && str[0])
	    gvrender_set_pencolor(gvc, str);
	/* bgcolor is supported for backward compatability */
	else if (((str = agget(sg, "bgcolor")) != 0) && str[0])
	    gvrender_set_pencolor(gvc, str);

	str = 0;
	if (((str = agget(sg, "fillcolor")) != 0) && str[0])
	    gvrender_set_fillcolor(gvc, str);
	else if (((str = agget(sg, "color")) != 0) && str[0])
	    gvrender_set_fillcolor(gvc, str);
	/* bgcolor is supported for backward compatability */
	else if (((str = agget(sg, "bgcolor")) != 0) && str[0]) {
	    filled = TRUE;
	    gvrender_set_fillcolor(gvc, str);
	}
	A[0] = GD_bb(sg).LL;
	A[2] = GD_bb(sg).UR;
	A[1].x = A[2].x;
	A[1].y = A[0].y;
	A[3].x = A[0].x;
	A[3].y = A[2].y;
	if (late_int(sg, G_peripheries, 1, 0)) {
	    gvrender_polygon(gvc, A, 4, filled);
	} else if (filled) {
	    gvrender_set_pencolor(gvc, str);
	    gvrender_polygon(gvc, A, 4, filled);
	}
	if (GD_label(sg))
	    emit_label(gvc, GD_label(sg), (void *) sg);

	if (flags & EMIT_PREORDER) {
	    for (n = agfstnode(sg); n; n = agnxtnode(sg, n)) {
		Obj = NODE;
		emit_node(gvc, n);
		for (e = agfstout(sg, n); e; e = agnxtout(sg, e)) {
		    Obj = EDGE;
		    emit_edge(gvc, e);
		}
	    }
	    Obj = NONE;
	}

	gvrender_end_context(gvc);
	if (url) {
	    gvrender_end_anchor(gvc);
	    free(url);
	    url = NULL;
	    if (tooltip) {
		free(tooltip);
		tooltip = NULL;
	    }
	    if (target) {
		free(target);
		target = NULL;
	    }
	}
	gvrender_end_cluster(gvc);
	/* when drawing, lay down clusters before sub_clusters */
	if (!(flags & EMIT_CLUSTERS_LAST)) {
	    emit_clusters(gvc, sg, flags);
	}
    }
}

static int style_delim(int c)
{
    switch (c) {
    case '(':
    case ')':
    case ',':
    case '\0':
	return TRUE;
    default:
	return FALSE;
    }
}

#define SID 1

static int style_token(char **s, agxbuf * xb)
{
    char *p = *s;
    int token;
    char c;

    while (*p && (isspace(*p) || (*p == ',')))
	p++;
    switch (*p) {
    case '\0':
	token = 0;
	break;
    case '(':
    case ')':
	token = *p++;
	break;
    default:
	token = SID;
	while (!style_delim(c = *p)) {
	    agxbputc(xb, c);
	    p++;
	}
    }
    *s = p;
    return token;
}

#define FUNLIMIT 64
static unsigned char outbuf[SMALLBUF];
static agxbuf ps_xb;

static void cleanup()
{
    agxbfree(&ps_xb);
}

char **parse_style(char *s)
{
    static char *parse[FUNLIMIT];
    static int first = 1;
    int fun = 0;
    boolean in_parens = FALSE;
    unsigned char buf[SMALLBUF];
    char *p;
    int c;
    agxbuf xb;

    if (first) {
	agxbinit(&ps_xb, SMALLBUF, outbuf);
	atexit(cleanup);
	first = 0;
    }

    agxbinit(&xb, SMALLBUF, buf);
    p = s;
    while ((c = style_token(&p, &xb)) != 0) {
	switch (c) {
	case '(':
	    if (in_parens) {
		agerr(AGERR, "nesting not allowed in style: %s\n", s);
		parse[0] = (char *) 0;
		agxbfree(&xb);
		return parse;
	    }
	    in_parens = TRUE;
	    break;

	case ')':
	    if (in_parens == FALSE) {
		agerr(AGERR, "unmatched ')' in style: %s\n", s);
		parse[0] = (char *) 0;
		agxbfree(&xb);
		return parse;
	    }
	    in_parens = FALSE;
	    break;

	default:
	    if (in_parens == FALSE) {
		if (fun == FUNLIMIT - 1) {
		    agerr(AGWARN, "truncating style '%s'\n", s);
		    parse[fun] = (char *) 0;
		    agxbfree(&xb);
		    return parse;
		}
		agxbputc(&ps_xb, '\0');	/* terminate previous */
		parse[fun++] = agxbnext(&ps_xb);
	    }
	    agxbput(&ps_xb, agxbuse(&xb));
	    agxbputc(&ps_xb, '\0');
	}
    }

    if (in_parens) {
	agerr(AGERR, "unmatched '(' in style: %s\n", s);
	parse[0] = (char *) 0;
	agxbfree(&xb);
	return parse;
    }
    parse[fun] = (char *) 0;
    agxbfree(&xb);
    agxbuse(&ps_xb);		/* adds final '\0' to buffer */
    return parse;
}

#ifndef DISABLE_CODEGENS
static codegen_info_t cg[MAX_CODEGENS] = {
    {&PS_CodeGen, "ps", POSTSCRIPT},
    {&PS_CodeGen, "ps2", PDF},
    {&HPGL_CodeGen, "hpgl", HPGL},
    {&HPGL_CodeGen, "pcl", PCL},
    {&MIF_CodeGen, "mif", MIF},
    {&PIC_CodeGen, "pic", PIC_format},

    {&GD_CodeGen, "gd", GD},
#ifdef HAVE_LIBZ
    {&GD_CodeGen, "gd2", GD2},
#endif
#ifdef HAVE_GD_GIF
    {&GD_CodeGen, "gif", GIF},
#endif
#ifdef HAVE_GD_JPEG
    {&GD_CodeGen, "jpg", JPEG},
    {&GD_CodeGen, "jpeg", JPEG},
#endif
#ifdef HAVE_GD_PNG
    {&GD_CodeGen, "png", PNG},
    {&VRML_CodeGen, "vrml", VRML},
#endif
    {&GD_CodeGen, "wbmp", WBMP},
#ifdef HAVE_GD_XPM
    {&GD_CodeGen, "xbm", XBM},
    {&GD_CodeGen, "xpm", XBM},
#endif

#ifdef QUARTZ_RENDER
    {&QPDF_CodeGen, "pdf", QPDF},
    {&QEPDF_CodeGen, "epdf", QEPDF},
#endif				/* QUARTZ_RENDER */

    {&MAP_CodeGen, "ismap", ISMAP},
    {&MAP_CodeGen, "imap", IMAP},
    {&MAP_CodeGen, "cmap", CMAP},
    {&MAP_CodeGen, "cmapx", CMAPX},
    {&VTX_CodeGen, "vtx", VTX},
    {&MP_CodeGen, "mp", METAPOST},
    {&FIG_CodeGen, "fig", FIG},
    {&SVG_CodeGen, "svg", SVG},
#ifdef HAVE_LIBZ
    {&SVG_CodeGen, "svgz", SVGZ},
    {&DIA_CodeGen, "dia", DIA},
#endif
#define DUMMY_Codegen XDot_CodeGen
    {&DUMMY_Codegen, "dot", ATTRIBUTED_DOT},
    {&DUMMY_Codegen, "canon", CANONICAL_DOT},
    {&DUMMY_Codegen, "plain", PLAIN},
    {&DUMMY_Codegen, "plain-ext", PLAIN_EXT},
    {&XDot_CodeGen, "xdot", EXTENDED_DOT},
    {NULL, NULL, 0}
};

codegen_info_t *first_codegen(void)
{
    return cg;
}

codegen_info_t *next_codegen(codegen_info_t * p)
{
    ++p;

#ifdef QUARTZ_RENDER
    static int unscanned = 1;
    if (!p->name && unscanned) {
	/* reached end of codegens but haven't yet scanned for Quicktime codegens... */

	unscanned = 0;		/* don't scan again */

	ComponentDescription criteria;
	criteria.componentType = GraphicsExporterComponentType;
	criteria.componentSubType = 0;
	criteria.componentManufacturer = 0;
	criteria.componentFlags = 0;
	criteria.componentFlagsMask = graphicsExporterIsBaseExporter;

	codegen_info_t *next_cg;
	int next_id;
	Component next_component;

	/* make each discovered Quicktime format into a codegen */
	for (next_cg = p, next_id = QBM_FIRST, next_component =
	     FindNextComponent(0, &criteria);
	     next_cg < cg + MAX_CODEGENS - 1 && next_id <= QBM_LAST
	     && next_component;
	     ++next_cg, ++next_id, next_component =
	     FindNextComponent(next_component, &criteria)) {
	    next_cg->cg = &QBM_CodeGen;
	    next_cg->id = next_id;
	    next_cg->info = next_component;

	    /* get four chars of extension, trim and convert to lower case */
	    char extension[5];
	    GraphicsExportGetDefaultFileNameExtension((GraphicsExportComponent) next_component, (OSType *) & extension);
	    extension[4] = '\0';

	    char *extension_ptr;
	    for (extension_ptr = extension; *extension_ptr;
		 ++extension_ptr)
		*extension_ptr =
		    *extension_ptr == ' ' ? '\0' : tolower(*extension_ptr);
	    next_cg->name = strdup(extension);
	}

	/* add new sentinel at end of dynamic codegens */
	next_cg->cg = (codegen_t *) 0;
	next_cg->id = 0;
	next_cg->info = (void *) 0;
	next_cg->name = (char *) 0;
    }
#endif
    return p;
}
#endif

void config_codegen_builtins(GVC_t * gvc)
{
#ifndef DISABLE_CODEGENS
    codegen_info_t *p;

    for (p = cg; p->name; ++p)
	gvplugin_install(gvc, API_render, p->name, 0, "cg",
			 (gvplugin_type_t *) p);
#endif
}

void use_library(char *name)
{
    static int cnt = 0;
    if (name) {
	Lib = ALLOC(cnt + 2, Lib, char *);
	Lib[cnt++] = name;
	Lib[cnt] = NULL;
    }
}

/* support for stderr_once */
/* #include "libgraph.h"		to get agstrdup, agstrfree */
extern char *agstrdup(char *);
extern void agstrfree(char *);

static void free_string_entry(Dict_t * dict, char *key, Dtdisc_t * disc)
{
    agstrfree(key);
}

static Dict_t *strings;
static Dtdisc_t stringdict = {
    0,				/* key  - the object itself */
    0,				/* size - null-terminated string */
    -1,				/* link - allocate separate holder objects  */
    NIL(Dtmake_f),
    (Dtfree_f) free_string_entry,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

int emit_once(char *str)
{
    if (strings == 0)
	strings = dtopen(&stringdict, Dtoset);
    if (!dtsearch(strings, str)) {
	dtinsert(strings, agstrdup(str));
	return TRUE;
    }
    return FALSE;
}

void emit_once_reset()
{
    if (strings) {
	dtclose(strings);
	strings = 0;
    }
}
