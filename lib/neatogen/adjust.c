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
#include "utils.h"
#include "voronoi.h"
#include "info.h"
#include "edges.h"
#include "site.h"
#include "heap.h"
#include "hedges.h"

static double margin = 0.05;	/* Create initial bounding box by adding
				 * margin * dimension around box enclosing
				 * nodes.
				 */
static double incr = 0.05;	/* Increase bounding box by adding
				 * incr * dimension around box.
				 */
static double pmargin = 5.0 / POINTS_PER_INCH;	/* Margin around polygons, in inches */
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
static void freeNodes()
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

    nsites = agnnodes(graph);
    geominit();

    nodeInfo = N_GNEW(nsites, Info_t);

    node = agfstnode(graph);
    ip = nodeInfo;

    pmargin = expFactor (graph);
    for (i = 0; i < nsites; i++) {
	ip->site.coord.x = ND_pos(node)[0];
	ip->site.coord.y = ND_pos(node)[1];

	makePoly(&ip->poly, node, pmargin);

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
static void sortSites()
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

static
Site *nextOne()
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
static void rmEquality()
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

static void increaseBoundBox()
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
static void addCorners()
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
static void newPos()
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
static void cleanup()
{
    PQcleanup();
    ELcleanup();
    siteinit();			/* free memory */
    edgeinit();			/* free memory */
}

static int vAdjust()
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

static int sAdjust()
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

typedef enum {
    AM_NONE, AM_VOR, AM_SCALE, AM_NSCALE, AM_SCALEXY, AM_PUSH, AM_PUSHPULL,
    AM_ORTHOXY, AM_ORTHOYX, AM_COMPRESS
} adjust_mode;
typedef struct {
    adjust_mode mode;
    char *attrib;
    char *print;
} adjust_data;
static adjust_data adjustMode[] = {
    {AM_NONE, "", ""},
    {AM_VOR, "", "Voronoi"},
    {AM_SCALE, "oscale", "old scaling"},
    {AM_NSCALE, "scale", "scaling"},
    {AM_SCALEXY, "scalexy", "x and y scaling"},
    /* {AM_PUSH, "push", "push scan adjust"}, */
    /* {AM_PUSHPULL, "pushpull", "push-pull scan adjust"}, */
    {AM_ORTHOXY, "orthoxy", "xy orthogonal constraints"},
    {AM_ORTHOYX, "orthoyx", "yx orthogonal constraints"},
    {AM_COMPRESS, "compress", "compress"},
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

/* removeOverlapAs:
 * Use flag value to determine if and how to remove
 * node overlaps.
 */
void 
removeOverlapAs(graph_t * G, char* flag)
{
    /* int          userWindow = 0; */
    int ret;
    extern void cAdjust(graph_t *, int);
    extern void scAdjust(graph_t *, int);
    /* extern void  scanAdjust(graph_t*, int); */

    adjust_data *am;

    if (agnnodes(G) < 2)
	return;
    if (flag == NULL)
	return;

    am = getAdjustMode(flag);
    if (am->mode == AM_NONE)
	return;

    if (Verbose)
	fprintf(stderr, "Adjusting %s using %s\n", G->name, am->print);

    if (am->mode > AM_SCALE) {
/* start_timer(); */
	switch (am->mode) {
	case AM_NSCALE:
	    scAdjust(G, 1);
	    break;
	case AM_SCALEXY:
	    scAdjust(G, 0);
	    break;
	case AM_PUSH:
	    /* scanAdjust (G, 1); */
	    break;
	case AM_PUSHPULL:
	    /* scanAdjust (G, 0); */
	    break;
	case AM_ORTHOXY:
	    cAdjust(G, 1);
	    break;
	case AM_ORTHOYX:
	    cAdjust(G, 0);
	    break;
	case AM_COMPRESS:
	    scAdjust(G, -1);
	    break;
	default:		/* to silence warnings */
	    break;
	}
/* fprintf (stderr, "%s %.4f sec\n", am->attrib, elapsed_sec()); */
	return;
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
/* fprintf (stderr, "old scale %.4f sec\n", elapsed_sec()); */

}

/* removeOverlap:
 */
void 
removeOverlap(graph_t * G)
{
    removeOverlapAs(G, agget(G, "overlap"));
}

void adjustNodes(graph_t * G)
{
    if (agnnodes(G) < 2)
	return;
    normalize(G);
    removeOverlap (G);
}

/* expFactor:
 * Return factor by which to scale up nodes.
 */
double 
expFactor(graph_t* g)
{
    double pmargin;
    char*  marg;

    if ((marg = agget(g, "sep"))) {
	pmargin = 1.0 + atof(marg);
    } else
	pmargin = 1.01;
    return pmargin;
}

