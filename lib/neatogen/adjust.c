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

/* adjust.c
 * Routines for repositioning nodes after initial layout in
 * order to reduce/remove node overlaps.
 */

#include "neato.h"
#include "agxbuf.h"
#include "utils.h"
#include "ctype.h"
#include "voronoi.h"
#include "info.h"
#include "edges.h"
#include "site.h"
#include "heap.h"
#include "hedges.h"
#include "digcola.h"
#if ((defined(HAVE_GTS) || defined(HAVE_TRIANGLE)) && defined(SFDP))
#include "sfdp.h"
#endif
#ifdef IPSEPCOLA
#include "csolve_VPSC.h"
#include "quad_prog_vpsc.h"
#endif

#define SEPFACT         0.8  /* default esep/sep */

static double margin = 0.05;	/* Create initial bounding box by adding
				 * margin * dimension around box enclosing
				 * nodes.
				 */
static double incr = 0.05;	/* Increase bounding box by adding
				 * incr * dimension around box.
				 */
static int iterations = -1;	/* Number of iterations */
static int useIter = 0;		/* Use specified number of iterations */

static int doAll = 0;		/* Move all nodes, regardless of overlap */
static Site **sites;		/* Array of pointers to sites; used in qsort */
static Site **endSite;		/* Sentinel on sites array */
static Point nw, ne, sw, se;	/* Corners of clipping window */

static Site **nextSite;

static void setBoundBox(Point * ll, Point * ur)
{
    pxmin = ll->x;
    pxmax = ur->x;
    pymin = ll->y;
    pymax = ur->y;
    nw.x = sw.x = pxmin;
    ne.x = se.x = pxmax;
    nw.y = ne.y = pymax;
    sw.y = se.y = pymin;
}

 /* freeNodes:
  * Free node resources.
  */
static void freeNodes(void)
{
    int i;
    Info_t *ip = nodeInfo;

    for (i = 0; i < nsites; i++) {
	breakPoly(&ip->poly);
	ip++;
    }
    polyFree();
    infoinit();			/* Free vertices */
    free(nodeInfo);
}

/* chkBoundBox:
 *   Compute extremes of graph, then set up bounding box.
 *   If user supplied a bounding box, use that;
 *   else if "window" is a graph attribute, use that; 
 *   otherwise, define bounding box as a percentage expansion of
 *   graph extremes.
 *   In the first two cases, check that graph fits in bounding box.
 */
static void chkBoundBox(Agraph_t * graph)
{
    char *marg;
    Point ll, ur;
    int i;
    double x, y;
    double xmin, xmax, ymin, ymax;
    double xmn, xmx, ymn, ymx;
    double ydelta, xdelta;
    Info_t *ip;
    Poly *pp;
    /* int          cnt; */

    ip = nodeInfo;
    pp = &ip->poly;
    x = ip->site.coord.x;
    y = ip->site.coord.y;
    xmin = pp->origin.x + x;
    ymin = pp->origin.y + y;
    xmax = pp->corner.x + x;
    ymax = pp->corner.y + y;
    for (i = 1; i < nsites; i++) {
	ip++;
	pp = &ip->poly;
	x = ip->site.coord.x;
	y = ip->site.coord.y;
	xmn = pp->origin.x + x;
	ymn = pp->origin.y + y;
	xmx = pp->corner.x + x;
	ymx = pp->corner.y + y;
	if (xmn < xmin)
	    xmin = xmn;
	if (ymn < ymin)
	    ymin = ymn;
	if (xmx > xmax)
	    xmax = xmx;
	if (ymx > ymax)
	    ymax = ymx;
    }

    marg = agget(graph, "voro_margin");
    if (marg && (*marg != '\0')) {
	margin = atof(marg);
    }
    ydelta = margin * (ymax - ymin);
    xdelta = margin * (xmax - xmin);
    ll.x = xmin - xdelta;
    ll.y = ymin - ydelta;
    ur.x = xmax + xdelta;
    ur.y = ymax + ydelta;

    setBoundBox(&ll, &ur);
}

 /* makeInfo:
  * For each node in the graph, create a Info data structure 
  */
static void makeInfo(Agraph_t * graph)
{
    Agnode_t *node;
    int i;
    Info_t *ip;
    expand_t pmargin;
    void (*polyf)(Poly *, Agnode_t *, float, float);

    nsites = agnnodes(graph);
    geominit();

    nodeInfo = N_GNEW(nsites, Info_t);

    node = agfstnode(graph);
    ip = nodeInfo;

    pmargin = sepFactor (graph);

    if (pmargin.doAdd) {
	polyf = makeAddPoly;
	/* we need inches for makeAddPoly */
	pmargin.x = PS2INCH(pmargin.x);
	pmargin.y = PS2INCH(pmargin.y);
    }
	
    else polyf = makePoly;
    for (i = 0; i < nsites; i++) {
	ip->site.coord.x = ND_pos(node)[0];
	ip->site.coord.y = ND_pos(node)[1];

	polyf(&ip->poly, node, pmargin.x, pmargin.y);

	ip->site.sitenbr = i;
	ip->site.refcnt = 1;
	ip->node = node;
	ip->verts = NULL;
	node = agnxtnode(graph, node);
	ip++;
    }
}

/* sort sites on y, then x, coord */
static int scomp(const void *S1, const void *S2)
{
    Site *s1, *s2;

    s1 = *(Site **) S1;
    s2 = *(Site **) S2;
    if (s1->coord.y < s2->coord.y)
	return (-1);
    if (s1->coord.y > s2->coord.y)
	return (1);
    if (s1->coord.x < s2->coord.x)
	return (-1);
    if (s1->coord.x > s2->coord.x)
	return (1);
    return (0);
}

 /* sortSites:
  * Fill array of pointer to sites and sort the sites using scomp
  */
static void sortSites(void)
{
    int i;
    Site **sp;
    Info_t *ip;

    if (sites == 0) {
	sites = N_GNEW(nsites, Site *);
	endSite = sites + nsites;
    }

    sp = sites;
    ip = nodeInfo;
    infoinit();
    for (i = 0; i < nsites; i++) {
	*sp++ = &(ip->site);
	ip->verts = NULL;
	ip->site.refcnt = 1;
	ip++;
    }

    qsort(sites, nsites, sizeof(Site *), scomp);

    /* Reset site index for nextOne */
    nextSite = sites;

}

static void geomUpdate(int doSort)
{
    int i;

    if (doSort)
	sortSites();

    /* compute ranges */
    xmin = sites[0]->coord.x;
    xmax = sites[0]->coord.x;
    for (i = 1; i < nsites; i++) {
	if (sites[i]->coord.x < xmin)
	    xmin = sites[i]->coord.x;
	if (sites[i]->coord.x > xmax)
	    xmax = sites[i]->coord.x;
    }
    ymin = sites[0]->coord.y;
    ymax = sites[nsites - 1]->coord.y;

    deltay = ymax - ymin;
    deltax = xmax - xmin;
}

static Site *nextOne(void)
{
    Site *s;

    if (nextSite < endSite) {
	s = *nextSite++;
	return (s);
    } else
	return ((Site *) NULL);
}

/* rmEquality:
 * Check for nodes with identical positions and tweak
 * the positions.
 */
static void rmEquality(void)
{
    int i, cnt;
    Site **ip;
    Site **jp;
    Site **kp;
    double xdel;

    sortSites();
    ip = sites;

    while (ip < endSite) {
	jp = ip + 1;
	if ((jp >= endSite) ||
	    ((*jp)->coord.x != (*ip)->coord.x) ||
	    ((*jp)->coord.y != (*ip)->coord.y)) {
	    ip = jp;
	    continue;
	}

	/* Find first node kp with position different from ip */
	cnt = 2;
	kp = jp + 1;
	while ((kp < endSite) &&
	       ((*kp)->coord.x == (*ip)->coord.x) &&
	       ((*kp)->coord.y == (*ip)->coord.y)) {
	    cnt++;
	    jp = kp;
	    kp = jp + 1;
	}

	/* If next node exists and is on the same line */
	if ((kp < endSite) && ((*kp)->coord.y == (*ip)->coord.y)) {
	    xdel = ((*kp)->coord.x - (*ip)->coord.x) / cnt;
	    i = 1;
	    for (jp = ip + 1; jp < kp; jp++) {
		(*jp)->coord.x += i * xdel;
		i++;
	    }
	} else {		/* nothing is to the right */
	    Info_t *info;
	    for (jp = ip + 1; jp < kp; ip++, jp++) {
		info = nodeInfo + (*ip)->sitenbr;
		xdel = info->poly.corner.x - info->poly.origin.x;
		info = nodeInfo + (*jp)->sitenbr;
		xdel += info->poly.corner.x - info->poly.origin.x;
		(*jp)->coord.x = (*ip)->coord.x + xdel / 2;
	    }
	}
	ip = kp;
    }
}

/* countOverlap:
 * Count number of node-node overlaps at iteration iter.
 */
static int countOverlap(int iter)
{
    int count = 0;
    int i, j;
    Info_t *ip = nodeInfo;
    Info_t *jp;

    for (i = 0; i < nsites; i++)
	nodeInfo[i].overlaps = 0;

    for (i = 0; i < nsites - 1; i++) {
	jp = ip + 1;
	for (j = i + 1; j < nsites; j++) {
	    if (polyOverlap
		(ip->site.coord, &ip->poly, jp->site.coord, &jp->poly)) {
		count++;
		ip->overlaps = 1;
		jp->overlaps = 1;
	    }
	    jp++;
	}
	ip++;
    }

    if (Verbose > 1)
	fprintf(stderr, "overlap [%d] : %d\n", iter, count);
    return count;
}

static void increaseBoundBox(void)
{
    double ydelta, xdelta;
    Point ll, ur;

    ur.x = pxmax;
    ur.y = pymax;
    ll.x = pxmin;
    ll.y = pymin;

    ydelta = incr * (ur.y - ll.y);
    xdelta = incr * (ur.x - ll.x);

    ur.x += xdelta;
    ur.y += ydelta;
    ll.x -= xdelta;
    ll.y -= ydelta;

    setBoundBox(&ll, &ur);
}

 /* areaOf:
  * Area of triangle whose vertices are a,b,c
  */
static double areaOf(Point a, Point b, Point c)
{
    double area;

    area =
	(double) (fabs
		  (a.x * (b.y - c.y) + b.x * (c.y - a.y) +
		   c.x * (a.y - b.y)) / 2);
    return area;
}

 /* centroidOf:
  * Compute centroid of triangle with vertices a, b, c.
  * Return coordinates in x and y.
  */
static void centroidOf(Point a, Point b, Point c, double *x, double *y)
{
    *x = (a.x + b.x + c.x) / 3;
    *y = (a.y + b.y + c.y) / 3;
}

 /* newpos;
  * The new position is the centroid of the
  * voronoi polygon. This is the weighted sum of the
  * centroids of a triangulation, normalized to the
  * total area.
  */
static void newpos(Info_t * ip)
{
    PtItem *anchor = ip->verts;
    PtItem *p, *q;
    double totalArea = 0.0;
    double cx = 0.0;
    double cy = 0.0;
    double x;
    double y;
    double area;

    p = anchor->next;
    q = p->next;
    while (q != NULL) {
	area = areaOf(anchor->p, p->p, q->p);
	centroidOf(anchor->p, p->p, q->p, &x, &y);
	cx = cx + area * x;
	cy = cy + area * y;
	totalArea = totalArea + area;
	p = q;
	q = q->next;
    }

    ip->site.coord.x = cx / totalArea;
    ip->site.coord.y = cy / totalArea;
}

 /* addCorners:
  * Add corners of clipping window to appropriate sites.
  * A site gets a corner if it is the closest site to that corner.
  */
static void addCorners(void)
{
    Info_t *ip = nodeInfo;
    Info_t *sws = ip;
    Info_t *nws = ip;
    Info_t *ses = ip;
    Info_t *nes = ip;
    double swd = dist_2(&ip->site.coord, &sw);
    double nwd = dist_2(&ip->site.coord, &nw);
    double sed = dist_2(&ip->site.coord, &se);
    double ned = dist_2(&ip->site.coord, &ne);
    double d;
    int i;

    ip++;
    for (i = 1; i < nsites; i++) {
	d = dist_2(&ip->site.coord, &sw);
	if (d < swd) {
	    swd = d;
	    sws = ip;
	}
	d = dist_2(&ip->site.coord, &se);
	if (d < sed) {
	    sed = d;
	    ses = ip;
	}
	d = dist_2(&ip->site.coord, &nw);
	if (d < nwd) {
	    nwd = d;
	    nws = ip;
	}
	d = dist_2(&ip->site.coord, &ne);
	if (d < ned) {
	    ned = d;
	    nes = ip;
	}
	ip++;
    }

    addVertex(&sws->site, sw.x, sw.y);
    addVertex(&ses->site, se.x, se.y);
    addVertex(&nws->site, nw.x, nw.y);
    addVertex(&nes->site, ne.x, ne.y);
}

 /* newPos:
  * Calculate the new position of a site as the centroid
  * of its voronoi polygon, if it overlaps other nodes.
  * The polygons are finite by being clipped to the clipping
  * window.
  * We first add the corner of the clipping windows to the
  * vertex lists of the appropriate sites.
  */
static void newPos(void)
{
    int i;
    Info_t *ip = nodeInfo;

    addCorners();
    for (i = 0; i < nsites; i++) {
	if (doAll || ip->overlaps)
	    newpos(ip);
	ip++;
    }
}

/* cleanup:
 * Cleanup voronoi memory.
 * Note that PQcleanup and ELcleanup rely on the number
 * of sites, so should at least be reset everytime we use
 * vAdjust.
 * This could be optimized, over multiple components or
 * even multiple graphs, but probably not worth it.
 */
static void cleanup(void)
{
    PQcleanup();
    ELcleanup();
    siteinit();			/* free memory */
    edgeinit();			/* free memory */
}

static int vAdjust(void)
{
    int iterCnt = 0;
    int overlapCnt = 0;
    int badLevel = 0;
    int increaseCnt = 0;
    int cnt;

    if (!useIter || (iterations > 0))
	overlapCnt = countOverlap(iterCnt);

    if ((overlapCnt == 0) || (iterations == 0))
	return 0;

    rmEquality();
    geomUpdate(0);
    voronoi(0, nextOne);
    while (1) {
	newPos();
	iterCnt++;

	if (useIter && (iterCnt == iterations))
	    break;
	cnt = countOverlap(iterCnt);
	if (cnt == 0)
	    break;
	if (cnt >= overlapCnt)
	    badLevel++;
	else
	    badLevel = 0;
	overlapCnt = cnt;

	switch (badLevel) {
	case 0:
	    doAll = 1;
	    break;
/*
      case 1:
        doAll = 1;
        break;
*/
	default:
	    doAll = 1;
	    increaseCnt++;
	    increaseBoundBox();
	    break;
	}

	geomUpdate(1);
	voronoi(0, nextOne);
    }

    if (Verbose) {
	fprintf(stderr, "Number of iterations = %d\n", iterCnt);
	fprintf(stderr, "Number of increases = %d\n", increaseCnt);
    }

    cleanup();
    return 1;
}

static double rePos(Point c)
{
    int i;
    Info_t *ip = nodeInfo;
    double f = 1.0 + incr;

    for (i = 0; i < nsites; i++) {
	/* ip->site.coord.x = f*(ip->site.coord.x - c.x) + c.x; */
	/* ip->site.coord.y = f*(ip->site.coord.y - c.y) + c.y; */
	ip->site.coord.x = f * ip->site.coord.x;
	ip->site.coord.y = f * ip->site.coord.y;
	ip++;
    }
    return f;
}

static int sAdjust(void)
{
    int iterCnt = 0;
    int overlapCnt = 0;
    int cnt;
    Point center;
    /* double sc; */

    if (!useIter || (iterations > 0))
	overlapCnt = countOverlap(iterCnt);

    if ((overlapCnt == 0) || (iterations == 0))
	return 0;

    rmEquality();
    center.x = (pxmin + pxmax) / 2.0;
    center.y = (pymin + pymax) / 2.0;
    while (1) {
	/* sc = */ rePos(center);
	iterCnt++;

	if (useIter && (iterCnt == iterations))
	    break;
	cnt = countOverlap(iterCnt);
	if (cnt == 0)
	    break;
    }

    if (Verbose) {
	fprintf(stderr, "Number of iterations = %d\n", iterCnt);
    }

    return 1;
}

 /* updateGraph:
  * Enter new node positions into the graph
  */
static void updateGraph(Agraph_t * graph)
{
    /* Agnode_t*    node; */
    int i;
    Info_t *ip;
    /* char         pos[100]; */

    ip = nodeInfo;
    for (i = 0; i < nsites; i++) {
	ND_pos(ip->node)[0] = ip->site.coord.x;
	ND_pos(ip->node)[1] = ip->site.coord.y;
	ip++;
    }
}

#ifdef IPSEPCOLA
static int
vpscAdjust(graph_t* G)
{
    int dim = 2;
    int nnodes = agnnodes(G);
    ipsep_options opt;
    pointf* nsize = N_GNEW(nnodes, pointf);
    float** coords = N_GNEW(dim, float*);
    float* f_storage = N_GNEW(dim * nnodes, float);
    int i, j;
    Agnode_t* v;
    expand_t margin;

    for (i = 0; i < dim; i++) {
	coords[i] = f_storage + i * nnodes;
    }

    j = 0;
    for (v = agfstnode(G); v; v = agnxtnode(G, v)) {
	for (i = 0; i < dim; i++) {
	    coords[i][j] =  (float) (ND_pos(v)[i]);
	}
	nsize[j].x = ND_width(v);
	nsize[j].y = ND_height(v);
	j++;
    }

    opt.diredges = 0;
    opt.edge_gap = 0;
    opt.noverlap = 2;
    opt.clusters = NEW(cluster_data);
    margin = sepFactor (G);
 	/* Multiply by 2 since opt.gap is the gap size, not the margin */
    if (margin.doAdd) {
	opt.gap.x = 2.0*PS2INCH(margin.x);
	opt.gap.y = 2.0*PS2INCH(margin.y);
    }
    else {
	opt.gap.x = opt.gap.y = 2.0*PS2INCH(DFLT_MARGIN);
    }
    opt.nsize = nsize;

    removeoverlaps(nnodes, coords, &opt);

    j = 0;
    for (v = agfstnode(G); v; v = agnxtnode(G, v)) {
	for (i = 0; i < dim; i++) {
	    ND_pos(v)[i] = coords[i][j];
	}
	j++;
    }

    free (opt.clusters);
    free (f_storage);
    free (coords);
    free (nsize);
    return 0;
}
#endif

/* normalize:
 * If normalize is set, move first node to origin, then
 * rotate graph so that first edge is horizontal.
 * FIX: Generalize to allow rotation determined by graph shape.
 */
void normalize(graph_t * g)
{
    node_t *v;
    edge_t *e;

    double theta;
    pointf p;

    if (!mapbool(agget(g, "normalize")))
	return;

    v = agfstnode(g);
    p.x = ND_pos(v)[0];
    p.y = ND_pos(v)[1];
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	ND_pos(v)[0] -= p.x;
	ND_pos(v)[1] -= p.y;
    }

    e = NULL;
    for (v = agfstnode(g); v; v = agnxtnode(g, v))
	if ((e = agfstout(g, v)))
	    break;
    if (e == NULL)
	return;

    theta = -atan2(ND_pos(e->head)[1] - ND_pos(e->tail)[1],
		   ND_pos(e->head)[0] - ND_pos(e->tail)[0]);

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	p.x = ND_pos(v)[0];
	p.y = ND_pos(v)[1];
	ND_pos(v)[0] = p.x * cos(theta) - p.y * sin(theta);
	ND_pos(v)[1] = p.x * sin(theta) + p.y * cos(theta);
    }
}

static adjust_data adjustMode[] = {
    {AM_NONE, "", ""},
    {AM_VOR, "", "Voronoi"},
    {AM_SCALE, "oscale", "old scaling"},
    {AM_NSCALE, "scale", "scaling"},
    {AM_SCALEXY, "scalexy", "x and y scaling"},
    /* {AM_PUSH, "push", "push scan adjust"}, */
    /* {AM_PUSHPULL, "pushpull", "push-pull scan adjust"}, */
    {AM_ORTHO, "ortho", "orthogonal constraints"},
    {AM_ORTHO_YX, "ortho_yx", "orthogonal constraints"},
    {AM_ORTHOXY, "orthoxy", "xy orthogonal constraints"},
    {AM_ORTHOYX, "orthoyx", "yx orthogonal constraints"},
    {AM_PORTHO, "portho", "pseudo-orthogonal constraints"},
    {AM_PORTHO_YX, "portho_yx", "pseudo-orthogonal constraints"},
    {AM_PORTHOXY, "porthoxy", "xy pseudo-orthogonal constraints"},
    {AM_PORTHOYX, "porthoyx", "yx pseudo-orthogonal constraints"},
    {AM_COMPRESS, "compress", "compress"},
    {AM_VPSC, "vpsc", "vpsc"},
    {AM_IPSEP, "ipsep", "ipsep"},
    {AM_FDP, "fdp", "fdp"},
    {AM_NONE, 0, 0}
};

/* getAdjustMode:
 * Convert string value to internal value of adjustment mode.
 * Assume s != NULL.
 */
static adjust_data *getAdjustMode(char *s)
{
    adjust_data *ap = adjustMode + 2;
    if (*s == '\0') return adjustMode;
    while (ap->attrib) {
	if (!strcasecmp(s, ap->attrib))
	    return ap;
	ap++;
    }
    if (mapbool(s))
	return adjustMode;
    else
	return adjustMode + 1;
}

adjust_data *graphAdjustMode(graph_t *G)
{
    char* am = agget(G, "overlap");
    return (getAdjustMode (am ? am : ""));
}

/* removeOverlapAs:
 * Use flag value to determine if and how to remove
 * node overlaps.
 */
int 
removeOverlapAs(graph_t * G, char* flag)
{
    /* int          userWindow = 0; */
    int ret = 0;
    /* extern void  scanAdjust(graph_t*, int); */

    adjust_data *am;

    if (agnnodes(G) < 2)
	return 0;
    if (flag == NULL)
	return 0;

    am = getAdjustMode(flag);
    if (am->mode == AM_NONE)
	return 0;

    if (Verbose)
	fprintf(stderr, "Adjusting %s using %s\n", G->name, am->print);

    if (am->mode > AM_SCALE) {
/* start_timer(); */
	switch (am->mode) {
	case AM_NSCALE:
	    ret = scAdjust(G, 1);
	    break;
	case AM_SCALEXY:
	    ret = scAdjust(G, 0);
	    break;
	case AM_PUSH:
	    /* scanAdjust (G, 1); */
	    break;
	case AM_PUSHPULL:
	    /* scanAdjust (G, 0); */
	    break;
	case AM_PORTHO_YX:
	case AM_PORTHO:
	case AM_PORTHOXY:
	case AM_PORTHOYX:
	case AM_ORTHO_YX:
	case AM_ORTHO:
	case AM_ORTHOXY:
	case AM_ORTHOYX:
	    cAdjust(G, am->mode);
	    break;
	case AM_COMPRESS:
	    ret = scAdjust(G, -1);
	    break;
#if ((HAVE_GTS || HAVE_TRIANGLE) && SFDP)
	case AM_FDP:
	    ret = fdpAdjust(G);
	    break;
#endif
#ifdef IPSEPCOLA
	case AM_IPSEP:
	    return 0;   /* handled during layout */
	    break;
	case AM_VPSC:
	    ret = vpscAdjust(G);
	    break;
#endif
	default:		/* to silence warnings */
	    if ((am->mode != AM_VOR) && (am->mode != AM_SCALE))
		agerr(AGWARN, "Unhandled adjust option %s\n", am->print);
	    break;
	}
/* fprintf (stderr, "%s %.4f sec\n", am->print, elapsed_sec()); */
	return ret;
    }

    /* create main array */
/* start_timer(); */
    makeInfo(G);

    /* establish and verify bounding box */
    chkBoundBox(G);

    if (am->mode == AM_SCALE)
	ret = sAdjust();
    else
	ret = vAdjust();

    if (ret)
	updateGraph(G);

    freeNodes();
    free(sites);
    sites = 0;
/* fprintf (stderr, "%s %.4f sec\n", am->print, elapsed_sec()); */

    return ret;
}

/* removeOverlap:
 */
int 
removeOverlap(graph_t * G)
{
    return (removeOverlapAs(G, agget(G, "overlap")));
}

/* adjustNodes:
 * Remove node overlap relying on graph's overlap attribute.
 * Return non-zero if graph has changed.
 */
int adjustNodes(graph_t * G)
{
    if (agnnodes(G) < 2)
	return 0;
    normalize(G);
    return removeOverlap (G);
}

/* parseFactor:
 * Convert "sep" attribute into expand_t.
 * Input "+x,y" becomes {x,y,true}
 * Input "x,y" becomes {1 + x/sepfact,1 + y/sepfact,false}
 * Return 1 on success, 0 on failure
 */
static int
parseFactor (char* s, expand_t* pp, float sepfact)
{
    int i;
    float x, y;

    while (isspace(*s)) s++;
    if (*s == '+') {
	s++;
	pp->doAdd = 1;
    }
    else pp->doAdd = 0;

    if ((i = sscanf(s, "%f,%f", &x, &y))) {
	if (i == 1) y = x;
	if (pp->doAdd) {
	    pp->x = x/sepfact;
	    pp->y = y/sepfact;
	}
	else {
	    pp->x = 1.0 + x/sepfact;
	    pp->y = 1.0 + y/sepfact;
	}
	return 1;
    }
    else return 0;
}

/* sepFactor:
 */
expand_t
sepFactor(graph_t* g)
{
    expand_t pmargin;
    char*  marg;

    if ((marg = agget(g, "sep")) && parseFactor(marg, &pmargin, 1.0)) {
    }
    else if ((marg = agget(g, "esep")) && parseFactor(marg, &pmargin, SEPFACT)) {
    }
    else { /* default */
	pmargin.x = pmargin.y = DFLT_MARGIN;
	pmargin.doAdd = 1;
    }
    if (Verbose)
	fprintf (stderr, "Node separation: add=%d (%f,%f)\n",
	    pmargin.doAdd, pmargin.x, pmargin.y);
    return pmargin;
}

/* esepFactor:
 * This value should be smaller than the sep value used to expand
 * nodes during adjustment. If not, when the adjustment pass produces
 * a fairly tight layout, the spline code will find that some nodes
 * still overlap.
 */
expand_t
esepFactor(graph_t* g)
{
    expand_t pmargin;
    char*  marg;

    if ((marg = agget(g, "esep")) && parseFactor(marg, &pmargin, 1.0)) {
    }
    else if ((marg = agget(g, "sep")) && parseFactor(marg, &pmargin, 1.0/SEPFACT)) {
    }
    else {
	pmargin.x = pmargin.y = SEPFACT*DFLT_MARGIN;
	pmargin.doAdd = 1;
    }
    if (Verbose)
	fprintf (stderr, "Edge separation: add=%d (%f,%f)\n",
	    pmargin.doAdd, pmargin.x, pmargin.y);
    return pmargin;
}
