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

#include "mem.h"
#include "site.h"
#include <math.h>


int siteidx;
Site *bottomsite;

static Freelist sfl;
static int nvertices;

void siteinit()
{
    /* double sn; */

    freeinit(&sfl, sizeof(Site));
    nvertices = 0;
    /* sn = nsites+4; */
    /* sqrt_nsites = sqrt(sn); */
}


Site *getsite()
{
    return ((Site *) getfree(&sfl));
}

double dist(Site * s, Site * t)
{
    double ans;
    double dx, dy;

    dx = s->coord.x - t->coord.x;
    dy = s->coord.y - t->coord.y;
    ans = sqrt(dx * dx + dy * dy);
    return ans;
}


void makevertex(Site * v)
{
    v->sitenbr = nvertices;
    nvertices += 1;
#ifdef STANDALONE
    out_vertex(v);
#endif
}


void deref(Site * v)
{
    v->refcnt -= 1;
    if (v->refcnt == 0)
	makefree(v, &sfl);
}

void ref(Site * v)
{
    v->refcnt += 1;
}
