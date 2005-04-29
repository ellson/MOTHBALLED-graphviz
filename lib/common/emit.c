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

#include	<ctype.h>

#include	"render.h"
#include	"agxbuf.h"
#include	"gvc.h"

static char *defaultlinestyle[3] = { "solid\0", "setlinewidth\0001\0", 0 };
int    emitState;

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

/* chkOrder:
 * Determine order of output.
 * Output usually in breadth first graph walk order
 */
static int chkOrder(graph_t * g)
{
    char *p = agget(g, "outputorder");
    if (p) {
        char c = *p;
        if ((c == 'n') && !strcmp(p + 1, "odesfirst"))
            return EMIT_SORTED;
        if ((c == 'e') && !strcmp(p + 1, "dgesfirst"))
            return EMIT_EDGE_SORTED;
    }
    return 0;
}

static void init_job_flags(GVJ_t * job, graph_t * g)
{
    switch (job->output_lang) {
    case GVRENDER_PLUGIN:
        job->flags = chkOrder(g) | job->render_features->flags;
        break;
    case POSTSCRIPT:
        job->flags = chkOrder(g) | GVRENDER_DOES_MULTIGRAPH_OUTPUT_FILES;
        break;
    case ISMAP: case IMAP: case CMAP: case CMAPX:
        /* output in breadth first graph walk order, but
         * with nodes edges and nested clusters before
         * clusters */
        job->flags = EMIT_CLUSTERS_LAST;
        break;
    case FIG:
        /* output color definition objects first */
        job->flags = EMIT_COLORS;
        break;
    case VTX:
        /* output sorted, i.e. all nodes then all edges */
        job->flags = EMIT_SORTED;
        break;
    case DIA:
        /* output in preorder traversal of the graph */
        job->flags = EMIT_PREORDER;
        break;
    case EXTENDED_DOT: case ATTRIBUTED_DOT: case CANONICAL_DOT:
    case PLAIN: case PLAIN_EXT:
        job->flags = 0;
        break;
    default:
        job->flags = chkOrder(g);
        break;
    }
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
	gvc->numLayers = parse_layers(gvc, g, str);
    } else {
	gvc->layerIDs = NULL;
	gvc->numLayers = 1;
    }
}

static void firstlayer(GVJ_t *job)
{
    job->numLayers = job->gvc->numLayers;
    if ((job->numLayers > 1)
		&& (! (gvrender_features(job) & GVRENDER_DOES_LAYERS))) {
	agerr(AGWARN, "layers not supported in %s output\n",
		job->output_langname);
	job->numLayers = 1;
    }

    job->layerNum = 1;
}

static boolean validlayer(GVJ_t *job)
{
    return (job->layerNum <= job->numLayers);
}

static void nextlayer(GVJ_t *job)
{
    job->layerNum++;
}

static point pagecode(GVJ_t *job, char c)
{
    point rv;
    rv.x = rv.y = 0;
    switch (c) {
    case 'T':
	job->pagesArrayFirst.y = job->pagesArraySize.y - 1;
	rv.y = -1;
	break;
    case 'B':
	rv.y = 1;
	break;
    case 'L':
	rv.x = 1;
	break;
    case 'R':
	job->pagesArrayFirst.x = job->pagesArraySize.x - 1;
	rv.x = -1;
	break;
    }
    return rv;
}

static void set_pagedir(GVC_t *gvc, graph_t * g)
{
    GVJ_t *job = gvc->job;
    char *str;

    job->pagesArrayMajor.x = job->pagesArrayMajor.y 
		= job->pagesArrayMinor.x = job->pagesArrayMinor.y = 0;
    job->pagesArrayFirst.x = job->pagesArrayFirst.y = 0;
    str = agget(g, "pagedir");
    if (str && str[0]) {
	job->pagesArrayMajor = pagecode(job, str[0]);
	job->pagesArrayMinor = pagecode(job, str[1]);
    }
    if ((abs(job->pagesArrayMajor.x + job->pagesArrayMinor.x) != 1)
     || (abs(job->pagesArrayMajor.y + job->pagesArrayMinor.y) != 1)) {
	job->pagesArrayMajor = pagecode(job, 'B');
	job->pagesArrayMinor = pagecode(job, 'L');
	if (str)
	    agerr(AGWARN, "pagedir=%s ignored\n", str);
    }
}

static void init_job_pagination(GVJ_t * job, graph_t * g)
{
    GVC_t *gvc = job->gvc;
    pointf pageSizeCenteredLessMargins;	 /* page for centering less margins - graph units*/
    pointf deviceSize;			/* device size for a page of the graph - graph units */
    pointf extra, size;

    /* determine desired device size in graph units */
    deviceSize.x = job->width * POINTS_PER_INCH / job->dpi;
    deviceSize.y = job->height * POINTS_PER_INCH / job->dpi;
    if (GD_drawing(g)->landscape)
	deviceSize = exch_xyf(deviceSize);

    /* determine pagination */
    if (gvc->graph_sets_pageSize) {
	/* page was set by user */
#if 0
fprintf(stderr,"graph_sets_pageSize\n");
#endif
	pageSizeCenteredLessMargins.x = gvc->pageSize.x - 2 * job->margin.x;
	pageSizeCenteredLessMargins.y = gvc->pageSize.y - 2 * job->margin.y;
	job->pageSize.x = pageSizeCenteredLessMargins.x;
	job->pageSize.y = pageSizeCenteredLessMargins.y;
	if (GD_drawing(g)->landscape)
	    job->pageSize = exch_xyf(job->pageSize);
	job->pageSize.x /= job->zoom;
	job->pageSize.y /= job->zoom;
	/* we don't want graph page to exceed its bounding box */
	job->pageSize.x = MIN(job->pageSize.x, gvc->bb.UR.x);
	job->pageSize.y = MIN(job->pageSize.y, gvc->bb.UR.y);
	job->pagesArraySize.x =
	    (job->pageSize.x > 0) ? ceil((gvc->bb.UR.x) / job->pageSize.x) : 1;
	job->pagesArraySize.y =
	    (job->pageSize.y > 0) ? ceil((gvc->bb.UR.y) / job->pageSize.y) : 1;
	job->numPages = job->pagesArraySize.x * job->pagesArraySize.y;

	/* find the drawable size in graph coords */
	deviceSize.x = MIN(deviceSize.x, pageSizeCenteredLessMargins.x);
	deviceSize.y = MIN(deviceSize.y, pageSizeCenteredLessMargins.y);
    } else {
	/* page not set by user, assume default when centering,
	   but allow infinite page for any other interpretation */
	job->pageSize.x = gvc->bb.UR.x;
	job->pageSize.y = gvc->bb.UR.y;
	pageSizeCenteredLessMargins.x = DEFAULT_PAGEWD - 2 * job->margin.x;
	pageSizeCenteredLessMargins.y = DEFAULT_PAGEHT - 2 * job->margin.y;
	job->pagesArraySize.x = job->pagesArraySize.y = job->numPages = 1;
    }

    set_pagedir(gvc, g);

    /* determine page box including centering */
    extra.x = extra.y = 0.;
    if (GD_drawing(g)->centered) {
	if ((extra.x = pageSizeCenteredLessMargins.x - deviceSize.x) < 0)
	    extra.x = 0.;
	if ((extra.y = pageSizeCenteredLessMargins.y - deviceSize.y) < 0)
	    extra.y = 0.;
	extra.x /= 2.;
	extra.y /= 2.;
    }

    job->boundingBox.LL.x = ROUND((job->margin.x + extra.x) * job->dpi / POINTS_PER_INCH);
    job->boundingBox.LL.y = ROUND((job->margin.y + extra.y) * job->dpi / POINTS_PER_INCH);
    size.x = job->pageSize.x * job->zoom * job->dpi / POINTS_PER_INCH;
    size.y = job->pageSize.y * job->zoom * job->dpi / POINTS_PER_INCH;
    if (GD_drawing(g)->landscape)
	size = exch_xyf(size);
    job->boundingBox.UR.x = job->boundingBox.LL.x + ROUND(size.x + 1);
    job->boundingBox.UR.y = job->boundingBox.LL.y + ROUND(size.y + 1);

#if 0
fprintf(stderr,"bb = %g,%g %g,%g (graph units)\n",
	gvc->bb.LL.x,
	gvc->bb.LL.y,
	gvc->bb.UR.x,
	gvc->bb.UR.y);
fprintf(stderr,"margin = %g,%g deviceSize = %g,%g (graph units)\n",
	job->margin.x, job->margin.y,
	deviceSize.x, deviceSize.y);
fprintf(stderr,"pageSizeCenteredLessMargins = %g,%g (graph units)\n",
	pageSizeCenteredLessMargins.x, pageSizeCenteredLessMargins.y);
fprintf(stderr,"dpi = %d zoom = %g rotation = %d\n",
        job->dpi, job->zoom, job->rotation);
fprintf(stderr,"boundingBox = %d,%d %d,%d (device units)\n",
        job->boundingBox.LL.x,
        job->boundingBox.LL.y,
        job->boundingBox.UR.x,
        job->boundingBox.UR.y);
fprintf(stderr,"width,height = %d,%d (device units)\n",
        job->width,
        job->height);
#endif
}

static void firstpage(GVJ_t *job)
{
    job->pagesArrayElem = job->pagesArrayFirst;
}

static boolean validpage(GVJ_t *job)
{
    return ((job->pagesArrayElem.x >= 0)
	 && (job->pagesArrayElem.x < job->pagesArraySize.x)
	 && (job->pagesArrayElem.y >= 0)
	 && (job->pagesArrayElem.y < job->pagesArraySize.y));
}

static void nextpage(GVJ_t *job)
{
    job->pagesArrayElem = add_points(job->pagesArrayElem, job->pagesArrayMinor);
    if (validpage(job) == FALSE) {
	if (job->pagesArrayMajor.y)
	    job->pagesArrayElem.x = job->pagesArrayFirst.x;
	else
	    job->pagesArrayElem.y = job->pagesArrayFirst.y;
	job->pagesArrayElem = add_points(job->pagesArrayElem, job->pagesArrayMajor);
    }
}

static boolean write_edge_test(Agraph_t * g, Agedge_t * e)
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

static boolean write_node_test(Agraph_t * g, Agnode_t * n)
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

void emit_background(GVJ_t * job, graph_t *g)
{
    char *str;
    pointf AF[4];
    point A[4];
    int i;

    if (((str = agget(g, "bgcolor")) != 0)
		&& str[0]
		&& strcmp(str, "white") != 0
		&& strcmp(str, "transparent") != 0) {
	AF[0].x = AF[1].x = job->pageBox.LL.x;
	AF[2].x = AF[3].x = job->pageBox.UR.x;
	AF[1].y = AF[2].y = job->pageBox.UR.y;
	AF[3].y = AF[0].y = job->pageBox.LL.y;
	for (i = 0; i < 4; i++) {
	    PF2P(AF[i],A[i]);
	}
	gvrender_set_fillcolor(job, str);
	gvrender_set_pencolor(job, str);
	gvrender_polygon(job, A, 4, TRUE);	/* filled */
    }
}

static void emit_defaults(GVJ_t * job)
{
    GVC_t * gvc = job->gvc;

    gvrender_set_pencolor(job, DEFAULT_COLOR);
    gvrender_set_font(job, gvc->defaultfontname, gvc->defaultfontsize);
}


/* even if this makes you cringe, at least it's short */
static void setup_page(GVJ_t * job, graph_t * g)
{
    /* establish current box in graph coordinates */
    job->pageBox.LL.x = job->pagesArrayElem.x * job->pageSize.x;
    job->pageBox.LL.y = job->pagesArrayElem.y * job->pageSize.y;
    job->pageBox.UR.x = job->pageBox.LL.x + job->pageSize.x;
    job->pageBox.UR.y = job->pageBox.LL.y + job->pageSize.y;

    /* increase pageBox to include margins so that overlapping nodes and edges
	are drawn in the margins of each page */
    job->pageBox.LL = sub_pointfs(job->pageBox.LL,job->margin);
    job->pageBox.UR = add_pointfs(job->pageBox.UR,job->margin);

    /* establish pageOffset to be applied, in graph coordinates */
    if (job->rotation == 0) {
	job->pageOffset.x =  -(job->pagesArrayElem.x)    * job->pageSize.x;
	job->pageOffset.y =  -(job->pagesArrayElem.y)    * job->pageSize.y;
    }
    else {
	job->pageOffset.x =  (job->pagesArrayElem.y + 1) * job->pageSize.y;
	job->pageOffset.y = -(job->pagesArrayElem.x)     * job->pageSize.x;
    }

#if 0
fprintf(stderr,"pagesArrayElem = %d,%d pageSize = %g,%g pageOffset = %g,%g\n",
	job->pagesArrayElem.x, job->pagesArrayElem.y,
	job->pageSize.x, job->pageSize.y,
	job->pageOffset.x, job->pageOffset.y);
#endif

    gvrender_begin_page(job);
    emit_background(job, g);
    emit_defaults(job);
}

static boolean node_in_pageBox(GVC_t *gvc, node_t * n)
{
    GVJ_t *job = gvc->job;
    boxf nb;

#if 0
fprintf(stderr,"pageBox = %g,%g,%g,%g\n",
	job->pageBox.LL.x, job->pageBox.LL.y,
	job->pageBox.UR.x, job->pageBox.UR.y);
#endif

    if (job->numPages == 1)
	return TRUE;
    nb.LL.x = ND_coord_i(n).x - ND_lw_i(n);
    nb.LL.y = ND_coord_i(n).y - ND_ht_i(n) / 2.;
    nb.UR.x = ND_coord_i(n).x + ND_rw_i(n);
    nb.UR.y = ND_coord_i(n).y + ND_ht_i(n) / 2.;
    return boxf_overlap(job->pageBox, nb);
}

static boolean is_natural_number(char *sstr)
{
    unsigned char *str = (unsigned char *) sstr;

    while (*str)
	if (NOT(isdigit(*str++)))
	    return FALSE;
    return TRUE;
}

static int layer_index(GVC_t *gvc, char *str, int all)
{
    GVJ_t *job = gvc->job;
    int i;

    if (streq(str, "all"))
	return all;
    if (is_natural_number(str))
	return atoi(str);
    if (gvc->layerIDs)
	for (i = 1; i <= job->numLayers; i++)
	    if (streq(str, gvc->layerIDs[i]))
		return i;
    return -1;
}

static boolean selectedlayer(GVC_t *gvc, char *spec)
{
    GVJ_t *job = gvc->job;
    int n0, n1;
    unsigned char buf[SMALLBUF];
    char *w0, *w1;
    agxbuf xb;
    boolean rval = FALSE;

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
	n0 = layer_index(gvc, w0, job->layerNum);
	rval = (n0 == job->layerNum);
	break;
    case 2:
	n0 = layer_index(gvc, w0, 0);
	n1 = layer_index(gvc, w1, job->numLayers);
	if ((n0 < 0) || (n1 < 0))
	    rval = TRUE;
	else if (n0 > n1) {
	    int t = n0;
	    n0 = n1;
	    n1 = t;
	}
	rval = BETWEEN(n0, job->layerNum, n1);
	break;
    }
    agxbfree(&xb);
    return rval;
}

static boolean node_in_layer(GVC_t *gvc, graph_t * g, node_t * n)
{
    GVJ_t *job = gvc->job;
    char *pn, *pe;
    edge_t *e;

    if (job->numLayers <= 1)
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

static boolean edge_in_layer(GVC_t *gvc, graph_t * g, edge_t * e)
{
    GVJ_t *job = gvc->job;
    char *pe, *pn;
    int cnt;

    if (job->numLayers <= 1)
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

static boolean clust_in_layer(GVC_t *gvc, graph_t * sg)
{
    GVJ_t *job = gvc->job;
    char *pg;
    node_t *n;

    if (job->numLayers <= 1)
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

static void emit_node(GVJ_t * job, node_t * n)
{
    GVC_t *gvc = job->gvc;
    char *s, *url = NULL, *tooltip = NULL, *target = NULL;

    if (ND_shape(n) == NULL)
	return;

    if (node_in_layer(gvc, n->graph, n)
	    && node_in_pageBox(gvc, n)
	    && (ND_state(n) != gvc->pageNum)) {

        gvrender_comment(job, n->name);

	s = late_string(n, N_comment, "");
	if (s[0])
	    gvrender_comment(job, s);
        
	gvrender_begin_node(job, n);
	if (((s = agget(n, "href")) && s[0])
	    || ((s = agget(n, "URL")) && s[0])) {
	    url = strdup_and_subst_node(s, n);
	    if ((s = agget(n, "tooltip")) && s[0])
		tooltip = strdup_and_subst_node(s, n);
	    else
		tooltip = strdup_and_subst_node(ND_label(n)->text, n);
	    if ((s = agget(n, "target")) && s[0])
		target = strdup_and_subst_node(s, n);
	    gvrender_begin_anchor(job, url, tooltip, target);
	}
	gvrender_begin_context(job);
	ND_shape(n)->fns->codefn(job, n);
	ND_state(n) = gvc->pageNum;
	gvrender_end_context(job);
	if (url) {
	    gvrender_end_anchor(job);
	    free(url);
	    if (tooltip)
		free(tooltip);
	    if (target)
		free(target);
	}
	gvrender_end_node(job);
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

static void emit_attachment(GVJ_t * job, textlabel_t * lp, splines * spl)
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
    gvrender_set_style(job, job->gvc->defaultlinestyle);
    /* Use font color to draw attachment
       - need something unambiguous in case of multicolored parallel edges
       - defaults to black for html-like labels
     */
    gvrender_set_pencolor(job, lp->fontcolor);
    gvrender_polyline(job, A, 3);
}

static boolean edge_in_pageBox(GVC_t *gvc, edge_t * e)
{
    GVJ_t *job = gvc->job;
    int i, j, np;
    bezier bz;
    point *p;
    pointf pp, pn;
    double sx, sy;
    boxf b;
    textlabel_t *lp;

    if (job->numPages == 1)
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
	    if (boxf_overlap(job->pageBox, mkboxf(pp, pn)))
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
    return boxf_overlap(job->pageBox, b);
}

void emit_edge_graphics(GVJ_t * job, edge_t * e)
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
    extern int xdemitState;

#define SEP 2.0

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
		gvrender_end_edge(job);
		return;
	    }
	}
    }
    xdemitState = EMIT_DRAW;
    if (ED_spl(e)) {
	scale = late_double(e, E_arrowsz, 1.0, 0.0);
	color = late_string(e, E_color, "");

	if (color[0] || styles) {
	    gvrender_begin_context(job);
	    if (styles)
		gvrender_set_style(job, styles);
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
		    gvrender_set_pencolor(job, color);
		    gvrender_set_fillcolor(job, color);
		} else {
		    gvrender_set_fillcolor(job, DEFAULT_COLOR);
		}
		for (i = 0; i < tmpspl.size; i++) {
		    tmplist = tmpspl.list[i].list;
		    offlist = offspl.list[i].list;
		    for (j = 0; j < tmpspl.list[i].size; j++) {
			tmplist[j].x += offlist[j].x;
			tmplist[j].y += offlist[j].y;
		    }
		    gvrender_beziercurve(job, tmplist, tmpspl.list[i].size,
					 FALSE, FALSE, FALSE);
		}
	    }
	    xdemitState = EMIT_TDRAW;
	    if (bz.sflag)
		arrow_gen(job, bz.sp, bz.list[0], scale, bz.sflag);
	    xdemitState = EMIT_HDRAW;
	    if (bz.eflag)
		arrow_gen(job, bz.ep, bz.list[bz.size - 1], scale,
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
		gvrender_set_pencolor(job, color);
		gvrender_set_fillcolor(job, color);
	    } else {
		gvrender_set_fillcolor(job, DEFAULT_COLOR);
	    }
	    for (i = 0; i < ED_spl(e)->size; i++) {
		bz = ED_spl(e)->list[i];
		/* convert points to pointf for gvrender api */
		bzf.size = bz.size;
		bzf.list = malloc(sizeof(pointf) * bzf.size);
		for (j = 0; j < bz.size; j++)
		    P2PF(bz.list[j], bzf.list[j]);
		if (gvrender_features(job) & GVRENDER_DOES_ARROWS) {
		    gvrender_beziercurve(job, bzf.list, bz.size, bz.sflag,
					 bz.eflag, FALSE);
		} else {
		    gvrender_beziercurve(job, bzf.list, bz.size, FALSE,
					 FALSE, FALSE);
		    xdemitState = EMIT_TDRAW;
		    if (bz.sflag)
			arrow_gen(job, bz.sp, bz.list[0], scale, bz.sflag);
		    xdemitState = EMIT_HDRAW;
		    if (bz.eflag)
			arrow_gen(job, bz.ep, bz.list[bz.size - 1], scale,
				  bz.eflag);
		}
		free(bzf.list);
	    }
	}
    }
    xdemitState = EMIT_LABEL;
    if (ED_label(e)) {
	emit_label(job, ED_label(e), (void *) e);
	if (mapbool(late_string(e, E_decorate, "false")) && ED_spl(e))
	    emit_attachment(job, ED_label(e), ED_spl(e));
    }
    xdemitState = EMIT_HLABEL;
    if (ED_head_label(e))
	emit_label(job, ED_head_label(e), (void *) e);	/* vladimir */
    xdemitState = EMIT_TLABEL;
    if (ED_tail_label(e))
	emit_label(job, ED_tail_label(e), (void *) e);	/* vladimir */

    if (saved)
	gvrender_end_context(job);
}

static void emit_edge(GVJ_t * job, edge_t * e)
{
    GVC_t *gvc = job->gvc;
    char *s, *url = NULL, *label = NULL, *tooltip = NULL, *target = NULL;
    textlabel_t *lab = NULL;

    if (! edge_in_pageBox(gvc, e) || ! edge_in_layer(gvc, e->head->graph, e))
	return;

    s = malloc(strlen(e->tail->name) + 2 + strlen(e->head->name) + 1);
    strcpy(s,e->tail->name);
    if (AG_IS_DIRECTED(e->tail->graph))
        strcat(s,"->");
    else
        strcat(s,"--");
    strcat(s,e->head->name);
    gvrender_comment(job, s);
    free(s);

    s = late_string(e, E_comment, "");
    if (s[0])
        gvrender_comment(job, s);

    gvrender_begin_edge(job, e);
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
	gvrender_begin_anchor(job, url, tooltip, target);
    }
    emit_edge_graphics (job, e);
    if (url) {
	gvrender_end_anchor(job);
	free(url);
	if (tooltip)
	    free(tooltip);
	if (target)
	    free(target);
    }
    gvrender_end_edge(job);
}

static void init_gvc_from_graph(GVC_t * gvc, graph_t * g)
{
    double xf, yf;
    char *p;
    int i;

    /* margins */
    gvc->graph_sets_margin = FALSE;
    if ((p = agget(g, "margin"))) {
        i = sscanf(p, "%lf,%lf", &xf, &yf);
        if (i > 0) {
            gvc->margin.x = gvc->margin.y = xf * POINTS_PER_INCH;
            if (i > 1)
                gvc->margin.y = yf * POINTS_PER_INCH;
            gvc->graph_sets_margin = TRUE;
        }
    }

    /* pagesize */
    gvc->graph_sets_pageSize = FALSE;
    P2PF(GD_drawing(g)->page, gvc->pageSize);
    if ((GD_drawing(g)->page.x > 0) && (GD_drawing(g)->page.y > 0)) {
        gvc->graph_sets_pageSize = TRUE;
    }

    /* bounding box */
    B2BF(GD_bb(g),gvc->bb);

    /* rotation */
    gvc->rotation = GD_drawing(g)->landscape ? 90 : 0;

    /* clusters have peripheries */
    G_peripheries = agfindattr(g, "peripheries");

    /* default font */
    gvc->defaultfontname = late_nnstring(g->proto->n,
		N_fontname, DEFAULT_FONTNAME);
    gvc->defaultfontsize = late_double(g->proto->n,
		N_fontsize, DEFAULT_FONTSIZE, MIN_FONTSIZE);

    /* default line style */
    gvc->defaultlinestyle = defaultlinestyle;

    gvc->graphname = g->name;
    gvc->lib = Lib;
}


static void init_job_margin(GVJ_t *job)
{
    GVC_t *gvc = job->gvc;
    
    if (gvc->graph_sets_margin) {
	job->margin = gvc->margin;
    }
    else {
        /* set default margins depending on format */
        switch (job->output_lang) {
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

static void init_job_dpi(GVJ_t *job, graph_t *g)
{
    job->dpi = GD_drawing(g)->dpi;
    if (job->dpi == 0) {
        /* set default margins depending on format */
        switch (job->output_lang) {
        case GVRENDER_PLUGIN:
            job->dpi = job->render_features->default_dpi;
            break;
        case POSTSCRIPT:
	    job->dpi = POINTS_PER_INCH;
            break;
        default:
            job->dpi = DEFAULT_DPI;
            break;
        }
    }
}

static void init_job_viewport(GVJ_t * job, graph_t * g)
{
    pointf UR, size;
    char *str;
    double X, Y, Z, x, y;
    int rv;

    assert((GD_bb(g).LL.x == 0) && (GD_bb(g).LL.y == 0));

    P2PF(GD_bb(g).UR, UR);

    /* determine final drawing size and scale to apply. */
    /* N.B. size given by user is not rotated by landscape mode */
    /* start with "natural" size of layout */

    Z = 1.0;
    if (GD_drawing(g)->size.x > 0) {	/* was given by user... */
	P2PF(GD_drawing(g)->size, size);
	if ((size.x < UR.x) || (size.y < UR.y) /* drawing is too big... */
	    || ((GD_drawing(g)->filled) /* or ratio=filled requested and ... */
		&& (size.x > UR.x) && (size.y > UR.y))) /* drawing is too small... */
	    Z = MIN(size.x/UR.x, size.y/UR.y);
    }
    
    /* default focus, in graph units = center of bb */
    x = UR.x / 2.;
    y = UR.y / 2.;

    /* rotate and scale bb to give default device width and height */
    if (GD_drawing(g)->landscape)
	UR = exch_xyf(UR);
    X = Z * (UR.x + 2 * job->margin.x) * job->dpi / POINTS_PER_INCH;
    Y = Z * (UR.y + 2 * job->margin.y) * job->dpi / POINTS_PER_INCH;

    /* user can override */
    if ((str = agget(g, "viewport")))
	rv = sscanf(str, "%lf,%lf,%lf,%lf,%lf", &X, &Y, &Z, &x, &y);
    /* rv is ignored since args retain previous values if not scanned */
    job->width = ROUND(X + 1); 
    job->height = ROUND(Y + 1);
    job->zoom = Z;              /* scaling factor */
    job->focus.x = x;           /* graph coord of focus - points */
    job->focus.y = y;
    job->rotation = job->gvc->rotation;
}

void emit_graph(GVJ_t * job, graph_t * g)
{
    GVC_t * gvc = job->gvc;
    graph_t *sg;
    node_t *n;
    edge_t *e;
    int c;
    char *str, *colors;
    char *s, *url = NULL, *tooltip = NULL, *target = NULL;
    int flags = job->flags;

    s = late_string(g, agfindattr(g, "comment"), "");
    gvrender_comment(job, s);

    gvrender_begin_graph(job, g);
    if (flags & EMIT_COLORS) {
	gvrender_set_fillcolor(job, DEFAULT_FILL);
	if (((str = agget(g, "bgcolor")) != 0) && str[0])
	    gvrender_set_fillcolor(job, str);
	if (((str = agget(g, "fontcolor")) != 0) && str[0])
	    gvrender_set_pencolor(job, str);
	for (c = 1; c <= GD_n_cluster(g); c++) {
	    sg = GD_clust(g)[c];
	    if (((str = agget(sg, "color")) != 0) && str[0])
		gvrender_set_pencolor(job, str);
	    if (((str = agget(sg, "fillcolor")) != 0) && str[0])
		gvrender_set_fillcolor(job, str);
	    if (((str = agget(sg, "fontcolor")) != 0) && str[0])
		gvrender_set_pencolor(job, str);
	}
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    if (((str = agget(n, "color")) != 0) && str[0])
		gvrender_set_pencolor(job, str);
	    if (((str = agget(n, "fillcolor")) != 0) && str[0])
		gvrender_set_fillcolor(job, str);
	    if (((str = agget(n, "fontcolor")) != 0) && str[0])
		gvrender_set_pencolor(job, str);
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		if (((str = agget(e, "color")) != 0) && str[0]) {
		    if (strchr(str, ':')) {
			colors = strdup(str);
			for (str = strtok(colors, ":"); str;
			     str = strtok(0, ":")) {
			    if (str[0])
				gvrender_set_pencolor(job, str);
			}
			free(colors);
		    } else
			gvrender_set_pencolor(job, str);
		}
		if (((str = agget(e, "fontcolor")) != 0) && str[0])
		    gvrender_set_pencolor(job, str);
	    }
	}
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	ND_state(n) = 0;
    /* iterate layers */
    for (firstlayer(job); validlayer(job); nextlayer(job)) {
	if (job->numLayers > 1)
	    gvrender_begin_layer(job);

	/* iterate pages */
	for (firstpage(job); validpage(job); nextpage(job)) {
	    gvc->pageNum++;
#if 0
fprintf(stderr,"pageNum = %d pagesArrayElem = %d,%d\n",
	gvc->pageNum, job->pagesArrayElem.x, job->pagesArrayElem.y);
#endif
    	    setup_page(job, g);
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
		gvrender_begin_anchor(job, url, tooltip, target);
	    }
	    if (GD_label(g))
		emit_label(job, GD_label(g), (void *) g);
	    Obj = CLST;
	    /* when drawing, lay clusters down before nodes and edges */
	    if (!(flags & EMIT_CLUSTERS_LAST)) {
		emit_clusters(job, g, flags);
	    }
	    if (flags & EMIT_SORTED) {
		/* output all nodes, then all edges */
		Obj = NODE;
		gvrender_begin_nodes(job);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    emit_node(job, n);
		}
		gvrender_end_nodes(job);
		Obj = EDGE;
		gvrender_begin_edges(job);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			emit_edge(job, e);
		    }
		}
		gvrender_end_edges(job);
	    } else if (flags & EMIT_EDGE_SORTED) {
		/* output all edges, then all nodes */
		Obj = EDGE;
		gvrender_begin_edges(job);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			emit_edge(job, e);
		    }
		}
		gvrender_end_edges(job);
		Obj = NODE;
		gvrender_begin_nodes(job);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    emit_node(job, n);
		}
		gvrender_end_nodes(job);
	    } else if (flags & EMIT_PREORDER) {
		Obj = NODE;
		gvrender_begin_nodes(job);
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    if (write_node_test(g, n))
			emit_node(job, n);
		}
		gvrender_end_nodes(job);
		Obj = EDGE;
		gvrender_begin_edges(job);

		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			if (write_edge_test(g, e))
			    emit_edge(job, e);
		    }
		}
		gvrender_end_edges(job);
	    } else {
		/* output in breadth first graph walk order */
		for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		    Obj = NODE;
		    emit_node(job, n);
		    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			Obj = NODE;
			emit_node(job, e->head);
			Obj = EDGE;
			emit_edge(job, e);
		    }
		}
	    }
	    /* when mapping, detect events on clusters after nodes and edges */
	    if (flags & EMIT_CLUSTERS_LAST) {
		emit_clusters(job, g, flags);
	    }
	    Obj = NONE;
	    if (url) {
		gvrender_end_anchor(job);
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
	    gvrender_end_page(job);
	} /* pages */
	if (job->numLayers > 1)
	    gvrender_end_layer(job);
    } /* layers */
    gvrender_end_graph(job);
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

static void emit_once_reset(void)
{
    if (strings) {
	dtclose(strings);
	strings = 0;
    }
}

void emit_jobs_eof(GVC_t * gvc)
{
    GVJ_t *job;

    for (job = gvrender_first_job(gvc); job; job = gvrender_next_job(gvc)) {
        if (job->output_file) {
	    if (gvc->pageNum > 0) {
		gvrender_end_job(job);
		emit_once_reset();
		gvc->pageNum = 0;
	    }
            fclose(job->output_file);
            job->output_file = NULL;
        }
    }
}

void emit_clusters(GVJ_t * job, Agraph_t * g, int flags)
{
    int i, c, filled;
    graph_t *sg;
    point A[4];
    char *str, **style;
    node_t *n;
    edge_t *e;
    char *s, *url = NULL, *tooltip = NULL, *target = NULL;
    extern int xdemitState;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	sg = GD_clust(g)[c];
	if (clust_in_layer(job->gvc, sg) == FALSE)
	    continue;
	/* when mapping, detect events on clusters after sub_clusters */
	if (flags & EMIT_CLUSTERS_LAST) {
	    emit_clusters(job, sg, flags);
	}
	Obj = CLST;
	gvrender_begin_cluster(job, sg);
	if (((s = agget(sg, "href")) && s[0])
	    || ((s = agget(sg, "URL")) && s[0])) {
	    url = strdup_and_subst_graph(s, sg);
	    if ((s = agget(sg, "target")) && s[0])
		target = strdup_and_subst_graph(s, sg);
	    if ((s = agget(sg, "tooltip")) && s[0])
		tooltip = strdup_and_subst_graph(s, sg);
	    else
		tooltip = strdup_and_subst_graph(GD_label(sg)->text, sg);
	    gvrender_begin_anchor(job, url, tooltip, target);
	}
	gvrender_begin_context(job);
	filled = FALSE;
	xdemitState = EMIT_DRAW;
	if (((str = agget(sg, "style")) != 0) && str[0]) {
	    gvrender_set_style(job, (style = parse_style(str)));
	    for (i = 0; style[i]; i++)
		if (strcmp(style[i], "filled") == 0) {
		    filled = TRUE;
		    break;
		}
	}
	if (((str = agget(sg, "pencolor")) != 0) && str[0])
	    gvrender_set_pencolor(job, str);
	else if (((str = agget(sg, "color")) != 0) && str[0])
	    gvrender_set_pencolor(job, str);
	/* bgcolor is supported for backward compatability */
	else if (((str = agget(sg, "bgcolor")) != 0) && str[0])
	    gvrender_set_pencolor(job, str);

	str = 0;
	if (((str = agget(sg, "fillcolor")) != 0) && str[0])
	    gvrender_set_fillcolor(job, str);
	else if (((str = agget(sg, "color")) != 0) && str[0])
	    gvrender_set_fillcolor(job, str);
	/* bgcolor is supported for backward compatability */
	else if (((str = agget(sg, "bgcolor")) != 0) && str[0]) {
	    filled = TRUE;
	    gvrender_set_fillcolor(job, str);
	}
	A[0] = GD_bb(sg).LL;
	A[2] = GD_bb(sg).UR;
	A[1].x = A[2].x;
	A[1].y = A[0].y;
	A[3].x = A[0].x;
	A[3].y = A[2].y;
	if (late_int(sg, G_peripheries, 1, 0)) {
	    gvrender_polygon(job, A, 4, filled);
	} else if (filled) {
	    gvrender_set_pencolor(job, str);
	    gvrender_polygon(job, A, 4, filled);
	}
	xdemitState = EMIT_DRAW;
	if (GD_label(sg))
	    emit_label(job, GD_label(sg), (void *) sg);

	if (flags & EMIT_PREORDER) {
	    for (n = agfstnode(sg); n; n = agnxtnode(sg, n)) {
		Obj = NODE;
		emit_node(job, n);
		for (e = agfstout(sg, n); e; e = agnxtout(sg, e)) {
		    Obj = EDGE;
		    emit_edge(job, e);
		}
	    }
	    Obj = NONE;
	}

	gvrender_end_context(job);
	if (url) {
	    gvrender_end_anchor(job);
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
	gvrender_end_cluster(job);
	/* when drawing, lay down clusters before sub_clusters */
	if (!(flags & EMIT_CLUSTERS_LAST)) {
	    emit_clusters(job, sg, flags);
	}
    }
}

static boolean is_style_delim(int c)
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
	while (!is_style_delim(c = *p)) {
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

static void cleanup(void)
{
    agxbfree(&ps_xb);
}

/* parse_style:
 * This is one of the worse internal designs in graphviz.
 * The use of '\0' characters within strings seems cute but it
 * makes all of the standard functions useless if not dangerous.
 * Plus the function uses static memory for both the array and
 * the character buffer. One hopes all of the values are used
 * before the function is called again.
 */
char **parse_style(char *s)
{
    static char *parse[FUNLIMIT];
    static boolean is_first = TRUE;
    int fun = 0;
    boolean in_parens = FALSE;
    unsigned char buf[SMALLBUF];
    char *p;
    int c;
    agxbuf xb;

    if (is_first) {
	agxbinit(&ps_xb, SMALLBUF, outbuf);
	atexit(cleanup);
	is_first = FALSE;
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

void use_library(char *name)
{
    static int cnt = 0;
    if (name) {
	Lib = ALLOC(cnt + 2, Lib, char *);
	Lib[cnt++] = name;
	Lib[cnt] = NULL;
    }
}

static void emit_job(GVJ_t * job, graph_t * g)
{
    if (!GD_drawing(g)) {
	fprintf (stderr,"layout was not done\n");
	return;
    }

#ifndef DISABLE_CODEGENS
    Output_file = job->output_file;
    Output_lang = job->output_lang;
#endif

    init_job_flags(job, g);
    init_job_margin(job);
    init_job_dpi(job, g);
    init_job_viewport(job, g);
    init_job_pagination(job, g);

    gvrender_begin_job(job);

    switch (job->output_lang) {
    case EXTENDED_DOT:
        write_extended_dot(job, g, job->output_file);
        break;
    case ATTRIBUTED_DOT:
        write_attributed_dot(g, job->output_file);
        break;
    case CANONICAL_DOT:
        write_canonical_dot(g, job->output_file);
        break;
    case PLAIN:
        write_plain(job, g, job->output_file);
        break;
    case PLAIN_EXT:
        write_plain_ext(job, g, job->output_file);
        break;
    default:
	if (! (job->flags & GVRENDER_X11_EVENTS))
            emit_graph(job, g);
        break;
    }

    /* Flush is necessary because we may be writing to a pipe. */
    if (! job->external_surface && job->output_lang != TK)
        fflush(job->output_file);
}

static FILE *file_select(char *str)
{
    FILE *rv;
    rv = fopen(str, "wb");
    if (rv == NULL) {
        perror(str);
        exit(1);
    }
    return rv;
}

void emit_jobs (GVC_t * gvc, graph_t * g)
{
    GVJ_t *job;
    char *prev_langname = "";

    init_gvc_from_graph(gvc, g);
    init_layering(gvc, g);

    gvc->active_jobs = NULL;
    for (job = gvrender_first_job(gvc); job; job = gvrender_next_job(gvc)) {
	job->g = g;

        if (!job->output_file) {        /* if not yet opened */
            if (job->output_filename == NULL) {
                job->output_file = stdout;
            } else {
                job->output_file = file_select(job->output_filename);
            }
        }
        job->output_lang = gvrender_select(job, job->output_langname);
	if (job->output_lang == NO_SUPPORT) {
	    fprintf(stderr,"renderer for %s is unavailable\n", job->output_langname);
	    return;
	}

	/* insert job in active list */
	job->next_active = gvc->active_jobs;
	gvc->active_jobs = job;

	if (strcmp(job->output_langname,prev_langname) != 0) {
	    prev_langname = job->output_langname;
	    gvrender_initialize(gvc);
	}

        emit_job(job, g);

	if (!job->next || strcmp(job->next->output_langname,prev_langname) != 0) {
	    gvrender_finalize(gvc);
	    /* clear active list */
	    gvc->active_jobs = NULL;
        }
    }
}
