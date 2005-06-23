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


/* #include	"dot.h" */
#include	"render.h"
#include	"pathplan.h"

#ifdef UNUSED
static box *bs = NULL;
static int bn;
static int maxbn = 0;
#define BINC 300
#endif

#define PINC 300

#ifdef NOTNOW
static edge_t *origedge;
#endif

static int nedges, nboxes; /* total no. of edges and boxes used in routing */

static int routeinit;
/* static data used across multiple edges */
static point *ps;             /* final spline points */
static int maxpn;             /* size of ps[] */
static Ppoint_t *polypoints;  /* vertices of polygon defined by boxes */
static int polypointn;        /* size of polypoints[] */
static Pedge_t *edges;        /* polygon edges passed to Proutespline */
static int edgen;             /* size of edges[] */

static void checkpath(int, box*, path*);
static void mkspacep(int size);
static void printpath(path * pp);
#ifdef OBSOLETE
static int append(path * path, int bi, point p0, point p1, int);
#endif
#ifdef DEBUG
static void printboxes(int boxn, box* boxes)
{
    point ll, ur;
    int bi;
    char buf[BUFSIZ];
    int newcnt = Show_cnt + boxn;

    Show_boxes = ALLOC(newcnt+2,Show_boxes,char*);
    for (bi = 0; bi < boxn; bi++) {
	ll = boxes[bi].LL, ur = boxes[bi].UR;
	sprintf(buf, "%d %d %d %d pathbox", ll.x, ll.y, ur.x, ur.y);
	Show_boxes[bi+1+Show_cnt] = strdup (buf);
    }
    Show_cnt = newcnt;
    Show_boxes[Show_cnt+1] = NULL;
}

static void psprintpolypts(Ppoint_t * p, int sz)
{
    int i;

    fprintf(stderr, "%%!\n");
    fprintf(stderr, "%% constraint poly\n");
    fprintf(stderr, "newpath\n");
    for (i = 0; i < sz; i++)
	fprintf(stderr, "%f %f %s\n", p[i].x, p[i].y,
		(i == 0 ? "moveto" : "lineto"));
    fprintf(stderr, "closepath stroke\n");
}
static void psprintpoint(point p)
{
    fprintf(stderr, "gsave\n");
    fprintf(stderr,
	    "newpath %d %d moveto %d %d 2 0 360 arc closepath fill stroke\n",
	    p.x, p.y, p.x, p.y);
    fprintf(stderr, "/Times-Roman findfont 4 scalefont setfont\n");
    fprintf(stderr, "%d %d moveto (\\(%d,%d\\)) show\n", p.x + 5, p.y + 5,
	    p.x, p.y);
    fprintf(stderr, "grestore\n");
}

static void psprintspline(Ppolyline_t spl)
{
    char buf[BUFSIZ];
    int newcnt = Show_cnt + spl.pn + 4;
    int li, i;

    Show_boxes = ALLOC(newcnt+2,Show_boxes,char*);
    li = Show_cnt+1;
    Show_boxes[li++] = strdup ("%%!");
    Show_boxes[li++] = strdup ("%% spline");
    Show_boxes[li++] = strdup ("gsave 1 0 0 setrgbcolor newpath");
    for (i = 0; i < spl.pn; i++) {
	sprintf(buf, "%f %f %s", spl.ps[i].x, spl.ps[i].y,
	  (i == 0) ?  "moveto" : ((i % 3 == 0) ? "curveto" : ""));
	Show_boxes[li++] = strdup (buf);
    }
    Show_boxes[li++] = strdup ("stroke grestore");
    Show_cnt = newcnt;
    Show_boxes[Show_cnt+1] = NULL;
}
static void psprintline(Ppolyline_t pl)
{
    char buf[BUFSIZ];
    int newcnt = Show_cnt + pl.pn + 4;
    int i, li;

    Show_boxes = ALLOC(newcnt+2,Show_boxes,char*);
    li = Show_cnt+1;
    Show_boxes[li++] = strdup ("%%!");
    Show_boxes[li++] = strdup ("%% line");
    Show_boxes[li++] = strdup ("gsave 0 0 1 setrgbcolor newpath");
    for (i = 0; i < pl.pn; i++) {
	sprintf(buf, "%f %f %s", pl.ps[i].x, pl.ps[i].y,
		(i == 0 ? "moveto" : "lineto"));
	Show_boxes[li++] = strdup (buf);
    }
    Show_boxes[li++] = strdup ("stroke grestore");
    Show_cnt = newcnt;
    Show_boxes[Show_cnt+1] = NULL;
}
static void psprintpoly(Ppoly_t p)
{
    char buf[BUFSIZ];
    int newcnt = Show_cnt + p.pn + 3;
    point tl, hd;
    int bi, li;
    char*  pfx;

    Show_boxes = ALLOC(newcnt+2,Show_boxes,char*);
    li = Show_cnt+1;
    Show_boxes[li++] = strdup ("%% poly list");
    Show_boxes[li++] = strdup ("gsave 0 1 0 setrgbcolor");
    for (bi = 0; bi < p.pn; bi++) {
	tl.x = (int)p.ps[bi].x;
	tl.y = (int)p.ps[bi].y;
	hd.x = (int)p.ps[(bi+1) % p.pn].x;
	hd.y = (int)p.ps[(bi+1) % p.pn].y;
	if ((tl.x == hd.x) && (tl.y == hd.y)) pfx = "%%";
	else pfx ="";
	sprintf(buf, "%s%d %d %d %d makevec", pfx, tl.x, tl.y, hd.x, hd.y);
	Show_boxes[li++] = strdup (buf);
    }
    Show_boxes[li++] = strdup ("grestore");

    Show_cnt = newcnt;
    Show_boxes[Show_cnt+1] = NULL;
}
static void psprintboxes(int boxn, box* boxes)
{
    char buf[BUFSIZ];
    int newcnt = Show_cnt + 5*boxn + 3;
    point ll, ur;
    int bi, li;

    Show_boxes = ALLOC(newcnt+2,Show_boxes,char*);
    li = Show_cnt+1;
    Show_boxes[li++] = strdup ("%% box list");
    Show_boxes[li++] = strdup ("gsave 0 1 0 setrgbcolor");
    for (bi = 0; bi < boxn; bi++) {
	ll = boxes[bi].LL, ur = boxes[bi].UR;
	sprintf(buf, "newpath\n%d %d moveto", ll.x, ll.y);
	Show_boxes[li++] = strdup (buf);
	sprintf(buf, "%d %d lineto", ll.x, ur.y);
	Show_boxes[li++] = strdup (buf);
	sprintf(buf, "%d %d lineto", ur.x, ur.y);
	Show_boxes[li++] = strdup (buf);
	sprintf(buf, "%d %d lineto", ur.x, ll.y);
	Show_boxes[li++] = strdup (buf);
	Show_boxes[li++] = strdup ("closepath stroke");
    }
    Show_boxes[li++] = strdup ("grestore");

    Show_cnt = newcnt;
    Show_boxes[Show_cnt+1] = NULL;
}
static void psprintinit (int begin)
{
    int newcnt = Show_cnt + 1;

    Show_boxes = ALLOC(newcnt+2,Show_boxes,char*);
    if (begin)
	Show_boxes[1+Show_cnt] = strdup ("dbgstart");
    else
	Show_boxes[1+Show_cnt] = strdup ("grestore");
    Show_cnt = newcnt;
    Show_boxes[Show_cnt+1] = NULL;
}

static int debugleveln(edge_t* realedge, int i)
{
    return (GD_showboxes(realedge->head->graph) == i ||
	    GD_showboxes(realedge->tail->graph) == i ||
	    ED_showboxes(realedge) == i ||
	    ND_showboxes(realedge->head) == i ||
	    ND_showboxes(realedge->tail) == i);
}
#endif

#ifdef OBSOLETE
static point mkpt(int x, int y)
{
    point rv;
    rv.x = x;
    rv.y = y;
    return rv;
}

static int pteq(point p, point q)
{
    return ((p.x == q.x) && (p.y == q.y));
}
#endif

/* routesplinesinit:
 * Data initialized once until matching call to routeplineterm
 * Allows recursive calls to dot
 */
void
routesplinesinit()
{
    if (++routeinit > 1) return;
#ifdef UNUSED
    if (!(bs = N_GNEW(BINC, box))) {
	agerr(AGERR, "cannot allocate bs\n");
	abort();
    }
    maxbn = BINC;
#endif
    if (!(ps = N_GNEW(PINC, point))) {
	agerr(AGERR, "cannot allocate ps\n");
	abort();
    }
    maxpn = PINC;
    if (Show_boxes) {
	int i;
        for (i = 0; Show_boxes[i]; i++)
	    free (Show_boxes[i]);
	free (Show_boxes);
	Show_boxes = NULL;
	Show_cnt = 0;
    }
    nedges = 0;
    nboxes = 0;
    if (Verbose)
	start_timer();
}

void routesplinesterm()
{
    if (--routeinit > 0) return;
    free(ps);
#ifdef UNUSED
    free(bs), bs = NULL /*, maxbn = bn = 0 */ ;
#endif
    if (Verbose)
	fprintf(stderr,
		"routesplines: %d edges, %d boxes %.2f sec\n",
		nedges, nboxes, elapsed_sec());
}

point *routesplines(path * pp, int *npoints)
{
    Ppoly_t poly;
    Ppolyline_t pl, spl;
    int splinepi;
    Ppoint_t eps[2];
    Pvector_t evs[2];
    int edgei, prev, next;
    point sp[4];
    int pi, bi, si;
    double t;
    box *boxes;
    int boxn;
    edge_t* realedge;
    int flip;

    nedges++;
    nboxes += pp->nbox;

    for (realedge = (edge_t *) pp->data;
#ifdef NOTNOW
	 origedge = realedge;
#endif
	 realedge && ED_edge_type(realedge) != NORMAL;
	 realedge = ED_to_orig(realedge));
    if (!realedge) {
	agerr(AGERR, "in routesplines, cannot find NORMAL edge\n");
	abort();
    }

    boxes = pp->boxes;
    boxn = pp->nbox;

    checkpath(boxn, boxes, pp);

#ifdef DEBUG
    if (debugleveln(realedge, 1))
	printboxes(boxn, boxes);
    if (debugleveln(realedge, 3)) {
	psprintinit(1);
	psprintboxes(boxn, boxes);
    }
#endif

    if (boxn * 8 > polypointn) {
	polypoints = ALLOC(boxn * 8, polypoints, Ppoint_t);
	polypointn = boxn * 8;
    }

    if ((boxn > 1) && (boxes[0].LL.y > boxes[1].LL.y)) {
        flip = 1;
	for (bi = 0; bi < boxn; bi++) {
	    int v = boxes[bi].UR.y;
	    boxes[bi].UR.y = -1*boxes[bi].LL.y;
	    boxes[bi].LL.y = -v;
	}
    }
    else flip = 0;

    if (realedge->tail != realedge->head) {
	/* I assume that the path goes either down only or
	   up - right - down */
	for (bi = 0, pi = 0; bi < boxn; bi++) {
	    next = prev = 0;
	    if (bi > 0)
		prev = (boxes[bi].LL.y > boxes[bi - 1].LL.y) ? -1 : 1;
	    if (bi < boxn - 1)
		next = (boxes[bi + 1].LL.y > boxes[bi].LL.y) ? 1 : -1;
	    if (prev != next) {
		if (next == -1 || prev == 1) {
		    polypoints[pi].x = boxes[bi].LL.x;
		    polypoints[pi++].y = boxes[bi].UR.y;
		    polypoints[pi].x = boxes[bi].LL.x;
		    polypoints[pi++].y = boxes[bi].LL.y;
		} else {
		    polypoints[pi].x = boxes[bi].UR.x;
		    polypoints[pi++].y = boxes[bi].LL.y;
		    polypoints[pi].x = boxes[bi].UR.x;
		    polypoints[pi++].y = boxes[bi].UR.y;
		}
	    }
	    else if (prev == 0) { /* single box */
		polypoints[pi].x = boxes[bi].LL.x;
		polypoints[pi++].y = boxes[bi].UR.y;
		polypoints[pi].x = boxes[bi].LL.x;
		polypoints[pi++].y = boxes[bi].LL.y;
	    } 
	    else {
		if (!(prev == -1 && next == -1))
		    abort();
	    }
	}
	for (bi = boxn - 1; bi >= 0; bi--) {
	    next = prev = 0;
	    if (bi < boxn - 1)
		prev = (boxes[bi].LL.y > boxes[bi + 1].LL.y) ? -1 : 1;
	    if (bi > 0)
		next = (boxes[bi - 1].LL.y > boxes[bi].LL.y) ? 1 : -1;
	    if (prev != next) {
		if (next == -1 || prev == 1 ) {
		    polypoints[pi].x = boxes[bi].LL.x;
		    polypoints[pi++].y = boxes[bi].UR.y;
		    polypoints[pi].x = boxes[bi].LL.x;
		    polypoints[pi++].y = boxes[bi].LL.y;
		} else {
		    polypoints[pi].x = boxes[bi].UR.x;
		    polypoints[pi++].y = boxes[bi].LL.y;
		    polypoints[pi].x = boxes[bi].UR.x;
		    polypoints[pi++].y = boxes[bi].UR.y;
		}
	    } 
	    else if (prev == 0) { /* single box */
		polypoints[pi].x = boxes[bi].UR.x;
		polypoints[pi++].y = boxes[bi].LL.y;
		polypoints[pi].x = boxes[bi].UR.x;
		polypoints[pi++].y = boxes[bi].UR.y;
	    }
	    else {
		if (!(prev == -1 && next == -1)) {
		    /* it went badly, e.g. degenerate box in boxlist */
		    *npoints = 0;
		    abort();	/* for correctness sake, it's best to just stop */
		    return ps;	/* could also be reported as a lost edge (no spline) */
		}
		polypoints[pi].x = boxes[bi].UR.x;
		polypoints[pi++].y = boxes[bi].LL.y;
		polypoints[pi].x = boxes[bi].UR.x;
		polypoints[pi++].y = boxes[bi].UR.y;
		polypoints[pi].x = boxes[bi].LL.x;
		polypoints[pi++].y = boxes[bi].UR.y;
		polypoints[pi].x = boxes[bi].LL.x;
		polypoints[pi++].y = boxes[bi].LL.y;
	    }
	}
    } 
    else {
#ifdef OBSOLETE
	/* new, more generalized approach for self-edges.  We do not
	   assume any monotonicity about the box path, only that it
	   is simply connected.  We build up the constraint poly by
	   walking the box path from one end to the other and back
	   in the recursive function append(). A better approach to all
	   of this might be to dispense with the box paths altogether
	   and just compute the constraint poly directly, but this
	   needs to be done as part of a more thorough overhaul. */
	point p0, p1;
	box b0, b1;
	b0 = pp->boxes[0];
	b1 = pp->boxes[1];
	/* determine 'starting' segment (side of b0) for box path search */
	if (b0.UR.x == b1.LL.x) {
	    p0 = b0.LL;
	    p1 = mkpt(b0.LL.x, b0.UR.y);
	} else if (b0.LL.y == b1.UR.y) {
	    p0 = mkpt(b0.LL.x, b0.UR.y);
	    p1 = b0.UR;
	} else if (b0.LL.x == b1.UR.x) {
	    p0 = b0.UR;
	    p1 = mkpt(b0.UR.x, b0.LL.y);
	} else if (b0.UR.y == b1.LL.y) {
	    p0 = mkpt(b0.UR.x, b0.LL.y);
	    p1 = b0.LL;
	} else
	    abort();
	pi = append(pp, 0, p0, p1, 0);
#else
	abort();
#endif
    }


    if (flip) {
	int i;
	for (bi = 0; bi < boxn; bi++) {
	    int v = boxes[bi].UR.y;
	    boxes[bi].UR.y = -1*boxes[bi].LL.y;
	    boxes[bi].LL.y = -v;
	}
	for (i = 0; i < pi; i++)
	    polypoints[i].y *= -1;
    }

    for (bi = 0; bi < boxn; bi++)
	boxes[bi].LL.x = INT_MAX, boxes[bi].UR.x = INT_MIN;
    poly.ps = polypoints, poly.pn = pi;
    eps[0].x = pp->start.p.x, eps[0].y = pp->start.p.y;
    eps[1].x = pp->end.p.x, eps[1].y = pp->end.p.y;
    if (Pshortestpath(&poly, eps, &pl) == -1)
	abort();
#ifdef DEBUG
    if (debugleveln(realedge, 3)) {
	psprintpoly(poly);
	psprintline(pl);
    }
#endif
    if (poly.pn > edgen) {
	edges = ALLOC(poly.pn, edges, Pedge_t);
	edgen = poly.pn;
    }
    for (edgei = 0; edgei < poly.pn; edgei++) {
	edges[edgei].a = polypoints[edgei];
	edges[edgei].b = polypoints[(edgei + 1) % poly.pn];
    }
    if (pp->start.constrained) {
	evs[0].x = cos(pp->start.theta);
	evs[0].y = sin(pp->start.theta);
    } else
	evs[0].x = evs[0].y = 0;
    if (pp->end.constrained) {
	evs[1].x = -cos(pp->end.theta);
	evs[1].y = -sin(pp->end.theta);
    } else
	evs[1].x = evs[1].y = 0;

    if (Proutespline(edges, poly.pn, pl, evs, &spl) == -1)
	abort();
#ifdef DEBUG
    if (debugleveln(realedge, 3)) {
	psprintspline(spl);
	psprintinit(0);
    }
#endif
    mkspacep(spl.pn);
    for (bi = 0; bi <= boxn; bi++) {
	boxes[bi].LL.x = INT_MAX;
	boxes[bi].UR.x = INT_MIN;
    }
    for (splinepi = 0; splinepi < spl.pn; splinepi++) {
	ps[splinepi].x = spl.ps[splinepi].x;
	ps[splinepi].y = spl.ps[splinepi].y;
    }
    for (splinepi = 0; splinepi + 3 < spl.pn; splinepi += 3) {
	for (si = 0; si <= 10 * boxn; si++) {
	    t = si / (10.0 * boxn);
	    sp[0] = ps[splinepi];
	    sp[1] = ps[splinepi + 1];
	    sp[2] = ps[splinepi + 2];
	    sp[3] = ps[splinepi + 3];
	    sp[0].x = sp[0].x + t * (sp[1].x - sp[0].x);
	    sp[0].y = sp[0].y + t * (sp[1].y - sp[0].y);
	    sp[1].x = sp[1].x + t * (sp[2].x - sp[1].x);
	    sp[1].y = sp[1].y + t * (sp[2].y - sp[1].y);
	    sp[2].x = sp[2].x + t * (sp[3].x - sp[2].x);
	    sp[2].y = sp[2].y + t * (sp[3].y - sp[2].y);
	    sp[0].x = sp[0].x + t * (sp[1].x - sp[0].x);
	    sp[0].y = sp[0].y + t * (sp[1].y - sp[0].y);
	    sp[1].x = sp[1].x + t * (sp[2].x - sp[1].x);
	    sp[1].y = sp[1].y + t * (sp[2].y - sp[1].y);
	    sp[0].x = sp[0].x + t * (sp[1].x - sp[0].x);
	    sp[0].y = sp[0].y + t * (sp[1].y - sp[0].y);
	    for (bi = 0; bi < boxn; bi++) {
		if (sp[0].y <= boxes[bi].UR.y && sp[0].y >= boxes[bi].LL.y) {
		    if (boxes[bi].LL.x > sp[0].x)
			boxes[bi].LL.x = sp[0].x;
		    if (boxes[bi].UR.x < sp[0].x)
			boxes[bi].UR.x = sp[0].x;
		}
	    }
	}
    }
    *npoints = spl.pn;

#ifdef DEBUG
    if (GD_showboxes(realedge->head->graph) == 2 ||
	GD_showboxes(realedge->tail->graph) == 2 ||
	ED_showboxes(realedge) == 2 ||
	ND_showboxes(realedge->head) == 2 ||
	ND_showboxes(realedge->tail) == 2)
	printboxes(boxn, boxes);
#endif

    return ps;
}

static int overlap(int i0, int i1, int j0, int j1)
{
    /* i'll bet there's an elegant way to do this */
    if (i1 <= j0)
	return 0;
    if (i0 >= j1)
	return 0;
    if ((j0 <= i0) && (i0 <= j1))
	return (j1 - i0);
    if ((j0 <= i1) && (i1 <= j1))
	return (i1 - j0);
    return MIN(i1 - i0, j1 - j0);
}


/*
 * repairs minor errors in the boxpath, such as boxes not joining
 * or slightly intersecting.  it's sort of the equivalent of the
 * audit process in the 5E control program - if you've given up on
 * fixing all the bugs, at least try to engineer around them!
 * in postmodern CS, we could call this "self-healing code."
 */
static void checkpath(int boxn, box* boxes, path* thepath)
{
    box *ba, *bb;
    int bi, i, errs, l, r, d, u;
    int xoverlap, yoverlap;

#ifndef DONTFIXPATH
    /* remove degenerate boxes. */
    i = 0;
    for (bi = 0; bi < boxn; bi++) {
	if (boxes[bi].LL.y == boxes[bi].UR.y)
	    continue;
	if (boxes[bi].LL.x == boxes[bi].UR.x)
	    continue;
	if (i != bi)
	    boxes[i] = boxes[bi];
	i++;
    }
    boxn = i;
#endif				/* DONTFIXPATH */

    ba = &boxes[0];
    if (ba->LL.x > ba->UR.x || ba->LL.y > ba->UR.y) {
	agerr(AGERR, "in checkpath, box 0 has LL coord > UR coord\n");
	printpath(thepath);
	abort();
    }
    for (bi = 0; bi < boxn - 1; bi++) {
	ba = &boxes[bi], bb = &boxes[bi + 1];
	if (bb->LL.x > bb->UR.x || bb->LL.y > bb->UR.y) {
	    agerr(AGERR, "in checkpath, box %d has LL coord > UR coord\n",
		  bi + 1);
	    printpath(thepath);
	    abort();
	}
	l = (ba->UR.x < bb->LL.x) ? 1 : 0;
	r = (ba->LL.x > bb->UR.x) ? 1 : 0;
	d = (ba->UR.y < bb->LL.y) ? 1 : 0;
	u = (ba->LL.y > bb->UR.y) ? 1 : 0;
	errs = l + r + d + u;
	if (errs > 0 && Verbose) {
	    fprintf(stderr, "in checkpath, boxes %d and %d don't touch\n",
		    bi, bi + 1);
	    printpath(thepath);
	}
#ifndef DONTFIXPATH
	if (errs > 0) {
	    int xy;

	    if (l == 1)
		xy = ba->UR.x, ba->UR.x = bb->LL.x, bb->LL.x = xy, l = 0;
	    else if (r == 1)
		xy = ba->LL.x, ba->LL.x = bb->UR.x, bb->UR.x = xy, r = 0;
	    else if (d == 1)
		xy = ba->UR.y, ba->UR.y = bb->LL.y, bb->LL.y = xy, d = 0;
	    else if (u == 1)
		xy = ba->LL.y, ba->LL.y = bb->UR.y, bb->UR.y = xy, u = 0;
	    for (i = 0; i < errs - 1; i++) {
		if (l == 1)
		    xy = (ba->UR.x + bb->LL.x) / 2.0 + 0.5, ba->UR.x =
			bb->LL.x = xy, l = 0;
		else if (r == 1)
		    xy = (ba->LL.x + bb->UR.x) / 2.0 + 0.5, ba->LL.x =
			bb->UR.x = xy, r = 0;
		else if (d == 1)
		    xy = (ba->UR.y + bb->LL.y) / 2.0 + 0.5, ba->UR.y =
			bb->LL.y = xy, d = 0;
		else if (u == 1)
		    xy = (ba->LL.y + bb->UR.y) / 2.0 + 0.5, ba->LL.y =
			bb->UR.y = xy, u = 0;
	    }
	}
#else
	abort();
#endif
#ifndef DONTFIXPATH
	/* check for overlapping boxes */
	xoverlap = overlap(ba->LL.x, ba->UR.x, bb->LL.x, bb->UR.x);
	yoverlap = overlap(ba->LL.y, ba->UR.y, bb->LL.y, bb->UR.y);
	if (xoverlap && yoverlap) {
	    if (xoverlap < yoverlap) {
		if (ba->UR.x - ba->LL.x > bb->UR.x - bb->LL.x) {
		    /* take space from ba */
		    if (ba->UR.x < bb->UR.x)
			ba->UR.x = bb->LL.x;
		    else
			ba->LL.x = bb->UR.x;
		} else {
		    /* take space from bb */
		    if (ba->UR.x < bb->UR.x)
			bb->LL.x = ba->UR.x;
		    else
			bb->UR.x = ba->LL.x;
		}
	    } else {		/* symmetric for y coords */
		if (ba->UR.y - ba->LL.y > bb->UR.y - bb->LL.y) {
		    /* take space from ba */
		    if (ba->UR.y < bb->UR.y)
			ba->UR.y = bb->LL.y;
		    else
			ba->LL.y = bb->UR.y;
		} else {
		    /* take space from bb */
		    if (ba->UR.y < bb->UR.y)
			bb->LL.y = ba->UR.y;
		    else
			bb->UR.y = ba->LL.y;
		}
	    }
	}
    }
#endif				/* DONTFIXPATH */

    if (thepath->start.p.x < boxes[0].LL.x
	|| thepath->start.p.x > boxes[0].UR.x
	|| thepath->start.p.y < boxes[0].LL.y
	|| thepath->start.p.y > boxes[0].UR.y) {
	if (Verbose) {
	    fprintf(stderr, "in checkpath, start port not in first box\n");
	    printpath(thepath);
	}
#ifndef DONTFIXPATH
	if (thepath->start.p.x < boxes[0].LL.x)
	    thepath->start.p.x = boxes[0].LL.x;
	if (thepath->start.p.x > boxes[0].UR.x)
	    thepath->start.p.x = boxes[0].UR.x;
	if (thepath->start.p.y < boxes[0].LL.y)
	    thepath->start.p.y = boxes[0].LL.y;
	if (thepath->start.p.y > boxes[0].UR.y)
	    thepath->start.p.y = boxes[0].UR.y;
#else
	abort();
#endif
    }
    if (thepath->end.p.x < boxes[boxn - 1].LL.x
	|| thepath->end.p.x > boxes[boxn - 1].UR.x
	|| thepath->end.p.y < boxes[boxn - 1].LL.y
	|| thepath->end.p.y > boxes[boxn - 1].UR.y) {
	if (Verbose) {
	    fprintf(stderr, "in checkpath, end port not in last box\n");
	    printpath(thepath);
	}
#ifndef DONTFIXPATH
	if (thepath->end.p.x < boxes[boxn - 1].LL.x)
	    thepath->end.p.x = boxes[boxn - 1].LL.x;
	if (thepath->end.p.x > boxes[boxn - 1].UR.x)
	    thepath->end.p.x = boxes[boxn - 1].UR.x;
	if (thepath->end.p.y < boxes[boxn - 1].LL.y)
	    thepath->end.p.y = boxes[boxn - 1].LL.y;
	if (thepath->end.p.y > boxes[boxn - 1].UR.y)
	    thepath->end.p.y = boxes[boxn - 1].UR.y;
#else
	abort();
#endif
    }
}

static void mkspacep(int size)
{
    if (size > maxpn) {
	int newmax = maxpn + (size / PINC + 1) * PINC;
	ps = RALLOC(newmax, ps, point);
	maxpn = newmax;
    }
}

#ifdef OBSOLETE
/* new code to create poly from box list
 * given that we entered the box b on segment p0,p1 (p0==p1 allowed) 
 * then add successive points to the constraint poly
 */

#define BOXLEFT 0
#define BOXTOP 1
#define BOXRIGHT 2
#define BOXBOTTOM 3
static box B;

static int sideofB(point p, box B)
{
    if (p.x == B.LL.x)
	return BOXLEFT;
    if (p.y == B.UR.y)
	return BOXTOP;
    if (p.x == B.UR.x)
	return BOXRIGHT;
    if (p.y == B.LL.y)
	return BOXBOTTOM;
    abort();
    return 0;
}

static int appendpt(point p, int polysz)
{
    polypoints[polysz].x = p.x;
    polypoints[polysz].y = p.y;
    return (polysz+1);
}

static int cmpf(const void *pp0, const void *pp1)
{
    point p0, p1;
    int s0, s1;

    p0 = *(point *) pp0;
    p1 = *(point *) pp1;
    s0 = sideofB(p0, B);
    s1 = sideofB(p1, B);

    if (s0 != s1)
	return s1 - s0;
    switch (s0) {
    case BOXLEFT:
	return p1.y - p0.y;
    case BOXTOP:
	return p1.x - p0.x;
    case BOXRIGHT:
	return p0.y - p1.y;
    case BOXBOTTOM:
	return p0.x - p1.x;
    default:
	abort();
    }
    return 0;			/* not reached */
}

/* append:
 */
static int 
append(path * path, int bi, point p0, point p1, int polysz)
{
    point v[8];		/* worst case 4 corners + 2 segs * 2 points each */
    point w[8];
    box b = path->boxes[bi];
    box bb;
    int i, i0, npw, delta;
    point q0 = { 0, 0 }, q1 = { 0, 0}, r;
    int pn;

    /* v = 4 corners of b, p0 and p1 */
    pn = 0;
    v[pn++] = b.LL;
    v[pn++] = mkpt(b.UR.x, b.LL.y);
    v[pn++] = b.UR;
    v[pn++] = mkpt(b.LL.x, b.UR.y);
    v[pn++] = p0;
    v[pn++] = p1;

    if (bi + 1 < path->nbox) {
	bb = path->boxes[bi + 1];
	/* determine points q0,q1 where b and bb touch and append to v */
	if (b.UR.x == bb.LL.x) {
	    q0.x = q1.x = b.UR.x;
	    q0.y = MIN(b.UR.y, bb.UR.y);
	    q1.y = MAX(b.LL.y, bb.LL.y);
	} else if (b.LL.x == bb.UR.x) {
	    q0.x = q1.x = b.LL.x;
	    q0.y = MIN(b.UR.y, bb.UR.y);
	    q1.y = MAX(b.LL.y, bb.LL.y);
	} else if (b.UR.y == bb.LL.y) {
	    q0.y = q1.y = b.UR.y;
	    q0.x = MIN(b.UR.x, bb.UR.x);
	    q1.x = MAX(b.LL.x, bb.LL.x);
	} else if (b.LL.y == bb.UR.y) {
	    q0.y = q1.y = b.LL.y;
	    q0.x = MIN(b.UR.x, bb.UR.x);
	    q1.x = MAX(b.LL.x, bb.LL.x);
	} else
	    abort();
	v[pn++] = q0;
	v[pn++] = q1;
    }

    /* sort v so that the cyclic order is p0, all other points, p1  */
    B = b;
    qsort(v, pn, sizeof(v[0]), cmpf);

    /* eliminate duplicates and record i0 = index of p0 in w */
    w[0] = v[0];
    npw = 1;
    i0 = -1;
    for (i = 0; i < pn; i++) {
	if (pteq(w[npw - 1], p0))
	    i0 = npw - 1;
	if (!pteq(v[i], w[npw - 1]))
	    w[npw++] = v[i];
    }

    i = i0;
    if (bi == 0)
	polysz = appendpt(p0, polysz);
    if (pteq(p1, w[(i0 + 1) % npw]))
	delta = -1;
    else if (pteq(p1, w[(i0 - 1 + npw) % npw]))
	delta = 1;
    else
	abort();
    do {
	i = (i + delta + npw) % npw;	/* go to the next point in order */
	r = w[i];		/* call it r */

	/* append r to current poly, except p0 and p1 are special cases */
	if ((bi == 0) || (!pteq(r, p0) && !pteq(r, p1)))
	    polysz = appendpt(r, polysz);
	if (pteq(r, p1))
	    break;
	if (bi + 1 < path->nbox) {	/* recur when we hit the next box */
	    if (pteq(r, q0)) {
		polysz = append(path, bi + 1, q0, q1, polysz);
		polysz = appendpt(q1, polysz);	/* assumes q1 != p0 and p1 */
		i += delta;	/* skip q1 */
	    } else if (pteq(r, q1)) {
		polysz = append(path, bi + 1, q1, q0, polysz);
		polysz = appendpt(q0, polysz);
		i += delta;
	    }
	}
    } while (i != i0);
    return polysz;
}
#endif

static void printpath(path * pp)
{
    int bi;

#ifdef NOTNOW
    fprintf(stderr, "edge %d from %s to %s\n", nedges,
	    realedge->tail->name, realedge->head->name);
    if (ED_count(origedge) > 1)
	fprintf(stderr, "    (it's part of a concentrator edge)\n");
#endif
    fprintf(stderr, "%d boxes:\n", pp->nbox);
    for (bi = 0; bi < pp->nbox; bi++)
	fprintf(stderr, "%d (%d, %d), (%d, %d)\n", bi, pp->boxes[bi].LL.x,
		pp->boxes[bi].LL.y, pp->boxes[bi].UR.x,
		pp->boxes[bi].UR.y);
    fprintf(stderr, "start port: (%d, %d), tangent angle: %.3f, %s\n",
	    pp->start.p.x, pp->start.p.y, pp->start.theta,
	    pp->start.constrained ? "constrained" : "not constrained");
    fprintf(stderr, "end port: (%d, %d), tangent angle: %.3f, %s\n",
	    pp->end.p.x, pp->end.p.y, pp->end.theta,
	    pp->end.constrained ? "constrained" : "not constrained");
}

