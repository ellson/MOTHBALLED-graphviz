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

#include <stdio.h>
#include "simple.h"
#include <stdlib.h>

static void sgnarea(l, m, i)
struct vertex *l, *m;
int i[];
	/* find the sign of the area of each of the triangles
	   formed by adding a vertex of m to l  
	   also find the sign of their product  */
{
    float a, b, c, d, e, f, g, h, t;
    a = l->pos.x;
    b = l->pos.y;
    c = after(l)->pos.x - a;
    d = after(l)->pos.y - b;
    e = m->pos.x - a;
    f = m->pos.y - b;
    g = after(m)->pos.x - a;
    h = after(m)->pos.y - b;
    t = (c * f) - (d * e);
    i[0] = ((t == 0) ? 0 : (t > 0 ? 1 : -1));
    t = (c * h) - (d * g);
    i[1] = ((t == 0) ? 0 : (t > 0 ? 1 : -1));
    i[2] = i[0] * i[1];
}

static int between(f, g, h)	/* determine if g lies between f and h      */
float f, g, h;
{
    if ((f == g) || (g == h))
	return (0);
    return ((f < g) ? (g < h ? 1 : -1) : (h < g ? 1 : -1));
}

static int online(l, m, i)	/* determine if vertex i of line m is on line l     */
struct vertex *l, *m;
int i;
{
    struct position a, b, c;
    a = l->pos;
    b = after(l)->pos;
    c = (i == 0) ? m->pos : after(m)->pos;
    return ((a.x == b.x) ? ((a.x == c.x)
			    && (-1 !=
				between(a.y, c.y, b.y))) : between(a.x,
								   c.x,
								   b.x));
}

static int intpoint(l, m, x, y, cond)
struct vertex *l, *m;
float *x, *y;
int cond;			/* determine point of detected intersections  */
{
    struct position ls, le, ms, me, pt1, pt2;
    float m1, m2, c1, c2;

    if (cond <= 0)
	return (0);
    ls = l->pos;
    le = after(l)->pos;
    ms = m->pos;
    me = after(m)->pos;

    switch (cond) {

    case 3:			/* a simple intersection        */
	if (ls.x == le.x) {
	    *x = ls.x;
	    *y = me.y + SLOPE(ms, me) * (*x - me.x);
	} else if (ms.x == me.x) {
	    *x = ms.x;
	    *y = le.y + SLOPE(ls, le) * (*x - le.x);
	} else {
	    m1 = SLOPE(ms, me);
	    m2 = SLOPE(ls, le);
	    c1 = ms.y - (m1 * ms.x);
	    c2 = ls.y - (m2 * ls.x);
	    *x = (c2 - c1) / (m1 - m2);
	    *y = ((m1 * c2) - (c1 * m2)) / (m1 - m2);
	}
	break;

    case 2:			/*     the two lines  have a common segment  */
	if (online(l, m, 0) == -1) {	/* ms between ls and le */
	    pt1 = ms;
	    pt2 =
		(online(m, l, 1) ==
		 -1) ? ((online(m, l, 0) == -1) ? le : ls) : me;
	} else if (online(l, m, 1) == -1) {	/* me between ls and le */
	    pt1 = me;
	    pt2 =
		(online(l, m, 0) ==
		 -1) ? ((online(m, l, 0) == -1) ? le : ls) : ms;
	} else {
	    /* may be degenerate? */
	    if (online(m, l, 0) != -1)
		return 0;
	    pt1 = ls;
	    pt2 = le;
	}

	*x = (pt1.x + pt2.x) / 2;
	*y = (pt1.y + pt2.y) / 2;
	break;

    case 1:			/* a vertex of line m is on line l */
	if ((ls.x - le.x) * (ms.y - ls.y) == (ls.y - le.y) * (ms.x - ls.x)) {
	    *x = ms.x;
	    *y = ms.y;
	} else {
	    *x = me.x;
	    *y = me.y;
	}
    }				/* end switch  */
    return (1);
}

  /*detect whether lines l and m intersect      */
void
find_intersection(struct vertex *l,
		  struct vertex *m,
		  struct intersection ilist[], struct data *input)
{
    float x, y;
    int i[3];
    sgnarea(l, m, i);

    if (i[2] > 0)
	return;

    if (i[2] < 0) {
	sgnarea(m, l, i);
	if (i[2] > 0)
	    return;
	if (!intpoint
	    (l, m, &x, &y, (i[2] < 0) ? 3 : online(m, l, ABS(i[0]))))
	    return;
    }

    else if (!intpoint(l, m, &x, &y, (i[0] == i[1]) ?
		       2 * MAX(online(l, m, 0),
			       online(l, m, 1)) : online(l, m, ABS(i[0]))))
	return;

    if (input->ninters >= MAXINTS) {
	fprintf(stderr, "\n**ERROR**\n using too many intersections\n");
	exit(1);
    }

    ilist[input->ninters].firstv = l;
    ilist[input->ninters].secondv = m;
    ilist[input->ninters].firstp = l->poly;
    ilist[input->ninters].secondp = m->poly;
    ilist[input->ninters].x = x;
    ilist[input->ninters].y = y;
    input->ninters++;
}
