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

#include	"dttest.h"

Dtdisc_t Disc = { 0, sizeof(int), -1,
    newint, NIL(Dtfree_f), compare, hashint,
    NIL(Dtmemory_f), NIL(Dtevent_f)
};

static int Count, See[10];

#if __STD_C
static visit(Dt_t * dt, Void_t * obj, Void_t * data)
#else
static visit(dt, obj, data)
Dt_t *dt;
Void_t *obj;
Void_t *data;
#endif
{
    See[(int) obj] = 1;
    Count += 1;
    return 0;
}

main()
{
    Dt_t *dt1, *dt2;
    int i;

    if (!(dt1 = dtopen(&Disc, Dtset)))
	terror("Opening Dtset");
    if (!(dt2 = dtopen(&Disc, Dtorder)))
	terror("Opening Dtorder");

    dtinsert(dt1, 1);
    dtinsert(dt1, 3);
    dtinsert(dt1, 5);

    dtinsert(dt2, 2);
    dtinsert(dt2, 4);
    dtinsert(dt2, 6);

    if ((int) dtsearch(dt1, 2) != 0)
	terror("Can't find 2 here!");

    dtview(dt1, dt2);
    if ((int) dtsearch(dt1, 2) != 2)
	terror("Should find 2 here!");

    dtwalk(dt1, visit, NIL(Void_t *));
    if (Count != 6)
	terror("Walk wrong length");
    for (i = 1; i <= 6; ++i)
	if (!See[i])
	    terror("Bad walk");

    dtinsert(dt1, 2);

    Count = 0;
    for (i = (int) dtfirst(dt1); i; i = (int) dtnext(dt1, i))
	Count++;
    if (Count != 6)
	terror("Walk wrong length2");

    Count = 0;
    for (i = (int) dtlast(dt1); i; i = (int) dtprev(dt1, i))
	Count++;
    if (Count != 6)
	terror("Walk wrong length3");

    /* dt1: 1 3 5 2
       dt2: 2 4 6 3
     */
    Count = 0;
    dtmethod(dt2, Dtset);
    dtinsert(dt2, 3);
    for (i = (int) dtfirst(dt1); i; i = (int) dtnext(dt1, i)) {
	dtsearch(dt1, 4);
	Count++;
    }
    if (Count != 6)
	terror("Walk wrong length4");

    return 0;
}
