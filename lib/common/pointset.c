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


#include <pointset.h>

typedef struct {
    Dtlink_t link;
    point id;
} pair;

static pair *mkPair(point p)
{
    pair *pp;

    pp = NEW(pair);
    pp->id = p;
    return pp;
}

static int cmppair(Dt_t * d, point * key1, point * key2, Dtdisc_t * disc)
{
    if (key1->x > key2->x)
	return 1;
    else if (key1->x < key2->x)
	return -1;
    else if (key1->y > key2->y)
	return 1;
    else if (key1->y < key2->y)
	return -1;
    else
	return 0;
}

Dtdisc_t intPairDisc = {
    offsetof(pair, id),
    sizeof(point),
    offsetof(pair, link),
    0,
    0,
    (Dtcompar_f) cmppair,
    0,
    0,
    0
};

PointSet *newPS(void)
{
    return (dtopen(&intPairDisc, Dtoset));
}

void freePS(PointSet * ps)
{
    dtclose(ps);
}

void insertPS(PointSet * ps, point pt)
{
    dtinsert(ps, mkPair(pt));
}

void addPS(PointSet * ps, int x, int y)
{
    point pt;

    pt.x = x;
    pt.y = y;
    dtinsert(ps, mkPair(pt));
}

int inPS(PointSet * ps, point pt)
{
    pair p;
    p.id = pt;
    return ((dtsearch(ps, &p)) ? 1 : 0);
}

int isInPS(PointSet * ps, int x, int y)
{
    pair p;
    p.id.x = x;
    p.id.y = y;
    return ((dtsearch(ps, &p)) ? 1 : 0);
}

int sizeOf(PointSet * ps)
{
    return dtsize(ps);
}

point *pointsOf(PointSet * ps)
{
    int n = dtsize(ps);
    point *pts = N_NEW(n, point);
    pair *p;
    point *pp = pts;

    for (p = (pair *) dtflatten(ps); p;
	 p = (pair *) dtlink(ps, (Dtlink_t *) p)) {
	*pp++ = p->id;
    }

    return pts;
}

typedef struct {
    Dtlink_t link;
    point id;
    int v;
} mpair;

typedef struct {
    Dtdisc_t disc;
    mpair *flist;
} MPairDisc;

static mpair *mkMPair(Dt_t * d, mpair * obj, MPairDisc * disc)
{
    mpair *ap = GNEW(mpair);

    if (disc->flist) {
	ap = disc->flist;
	disc->flist = (mpair *) (ap->link.right);
    } else
	ap = GNEW(mpair);
    ap->id = obj->id;
    ap->v = obj->v;
    return ap;
}

static void freeMPair(Dt_t * d, mpair * ap, MPairDisc * disc)
{
    ap->link.right = (Dtlink_t *) (disc->flist);
    disc->flist = ap;
}

Dtdisc_t intMPairDisc = {
    offsetof(mpair, id),
    sizeof(point),
    offsetof(mpair, link),
    (Dtmake_f) mkMPair,
    (Dtfree_f) freeMPair,
    (Dtcompar_f) cmppair,
    0,
    0,
    0
};

PointMap *newPM(void)
{
    MPairDisc *dp = GNEW(MPairDisc);

    dp->disc = intMPairDisc;
    dp->flist = 0;

    return (dtopen(&(dp->disc), Dtoset));
}

void clearPM(PointMap * ps)
{
    dtclear(ps);
}

void freePM(PointMap * ps)
{
    MPairDisc *dp = (MPairDisc *) (ps->disc);
    mpair *p;
    mpair *next;

    for (p = dp->flist; p; p = next) {
	next = (mpair *) (p->link.right);
	free(p);
    }
    dtclose(ps);
    free(dp);
}

int insertPM(PointMap * pm, int x, int y, int v)
{
    mpair *p;
    mpair dummy;

    dummy.id.x = x;
    dummy.id.y = y;
    dummy.v = v;
    p = dtinsert(pm, &dummy);
    return p->v;
}
