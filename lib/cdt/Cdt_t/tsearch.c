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

Dtdisc_t Rdisc = { 0, sizeof(int), -1,
    newint, NIL(Dtfree_f), rcompare, hashint,
    NIL(Dtmemory_f), NIL(Dtevent_f)
};

main()
{
    Dt_t *dt;
    Dtlink_t *link;
    int i, k, count[10];

    /* testing Dtorder */
    dt = dtopen(&Disc, Dtorder);
    if ((int) dtinsert(dt, 7) != 7)
	terror("Insert 7");
    if ((int) dtinsert(dt, 1) != 1)
	terror("Insert 1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Insert 3");
    if ((int) dtinsert(dt, 4) != 4)
	terror("Insert 4");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Insert 2");
    if ((int) dtinsert(dt, 6) != 6)
	terror("Insert 6");
    if ((int) dtinsert(dt, 7) != 7)
	terror("Insert 7,2");
    if ((int) dtinsert(dt, 5) != 5)
	terror("Insert 5");

    for (i = 1; i <= 7; ++i)
	if ((int) dtsearch(dt, i) != i)
	    terror("Dtorder search");
    for (link = dtflatten(dt), i = 1; link;
	 link = dtlink(dt, link), i += 1)
	if ((int) dtobj(dt, link) != i)
	    terror("Dtorder flatten");
    for (i = (int) dtlast(dt), k = 7; k >= 1;
	 i = (int) dtprev(dt, i), k -= 1)
	if (i != k)
	    terror("Dtorder backwalk");

    /* reverse ordering */
    dtdisc(dt, &Rdisc, 0);
    for (i = 7; i >= 1; --i)
	if ((int) dtsearch(dt, i) != i)
	    terror("Dtorder search 2");
    for (i = (int) dtlast(dt), k = 1; k <= 7;
	 i = (int) dtprev(dt, i), k += 1)
	if (i != k)
	    terror("Dtorder backwalk 2");
    for (link = dtflatten(dt), i = 7; link;
	 link = dtlink(dt, link), i -= 1)
	if ((int) dtobj(dt, link) != i)
	    terror("Dtorder flatten 2");

    if (!(link = dtextract(dt)))
	terror("Fail extracting Dtorder");
    if (dtrestore(dt, link) < 0)
	terror("Fail restoring Dtorder");
    if (dtsize(dt) != 7)
	terror("Dtorder size after extract");
    for (link = dtflatten(dt), i = 7; link;
	 link = dtlink(dt, link), i -= 1)
	if ((int) dtobj(dt, link) != i)
	    terror("Dtorder flatten after extract");

    /* change to hashing */
    dtmethod(dt, Dtset);
    for (i = 1; i <= 7; ++i)
	if ((int) dtsearch(dt, i) != i)
	    terror("Dtset search");
    for (link = dtflatten(dt), i = 0; link; link = dtlink(dt, link))
	i += 1;
    if (i != 7)
	terror("Dtset flatten");
    for (i = (int) dtlast(dt), k = 0; i != 0; i = (int) dtprev(dt, i))
	k += 1;
    if (k != 7)
	terror("Dtset flatten 2");

    if (!(link = dtextract(dt)))
	terror("Fail extracting Dtset");
    if (dtrestore(dt, link) < 0)
	terror("Fail restoring Dtset");
    if (dtsize(dt) != 7)
	terror("Dtset size after extract");
    for (i = (int) dtlast(dt), k = 0; i != 0; i = (int) dtprev(dt, i))
	k += 1;
    if (k != 7)
	terror("Dtset flatten after extract");

    dtdisc(dt, &Disc, 0);
    for (i = 1; i <= 7; ++i)
	if ((int) dtsearch(dt, i) != i)
	    terror("Dtset search 2");
    for (link = dtflatten(dt), i = 0; link; link = dtlink(dt, link))
	i += 1;
    if (i != 7)
	terror("Dtset flatten 2");

    dtclear(dt);
    if (dtsize(dt) != 0)
	terror("Dtsize");

    /* testing Dtlist */
    dtmethod(dt, Dtlist);
    if ((int) dtinsert(dt, 1) != 1)
	terror("Dtlist insert 1.1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtlist insert 3.1");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Dtlist insert 2.1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtlist insert 3.2");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Dtlist insert 2.2");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtlist insert 3.3");
    if ((int) dtinsert(dt, 1) != 1)
	terror("Dtlist insert 1.2");

    /* check multiplicities */
    for (i = 1; i <= 3; ++i)
	count[i] = 0;
    for (i = (int) dtlast(dt); i != 0; i = (int) dtprev(dt, i))
	count[i] += 1;
    if (count[1] != 2)
	terror("Dtlist count 1");
    if (count[2] != 2)
	terror("Dtlist count 2");
    if (count[3] != 3)
	terror("Dtlist count 3");

    dtclear(dt);
    if (dtsize(dt) != 0)
	terror("Dtsize");

    /* testing Dtbag */
    dtmethod(dt, Dtbag);
    if ((int) dtinsert(dt, 1) != 1)
	terror("Dtlist insert 1.1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtlist insert 3.1");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Dtlist insert 2.1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtlist insert 3.2");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Dtlist insert 2.2");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtlist insert 3.3");
    if ((int) dtinsert(dt, 1) != 1)
	terror("Dtlist insert 1.2");

    /* check multiplicities */
    for (i = 1; i <= 3; ++i)
	count[i] = 0;
    for (i = (int) dtlast(dt); i != 0; i = (int) dtprev(dt, i))
	count[i] += 1;
    if (count[1] != 2)
	terror("Dtbag count 1");
    if (count[2] != 2)
	terror("Dtbag count 2");
    if (count[3] != 3)
	terror("Dtbag count 3");

    /* test consecutive  1's */
    if ((int) dtsearch(dt, 1) != 1)
	terror("Dtbag search 1");
    if ((int) dtnext(dt, 1) != 1)
	terror("Dtbag next 1");
    if ((int) dtnext(dt, 1) == 1)
	terror("Dtbag next not expecting 1");

    /* test consecutive  2's */
    if ((int) dtsearch(dt, 2) != 2)
	terror("Dtbag search 2");
    if ((int) dtnext(dt, 2) != 2)
	terror("Dtbag next 2");
    if ((int) dtnext(dt, 2) == 2)
	terror("Dtbag next not expecting 2");

    /* test consecutive 3's */
    if ((int) dtsearch(dt, 3) != 3)
	terror("Dtbag search 3");
    if ((int) dtnext(dt, 3) != 3)
	terror("Dtbag next 3");
    if ((int) dtnext(dt, 3) != 3)
	terror("Dtbag next 3.2");
    if ((int) dtnext(dt, 3) == 3)
	terror("Dtbag next not expecting 3");

    /* change method to Dtobag */
    dtmethod(dt, Dtobag);

    /* check multiplicities */
    for (i = 1; i <= 3; ++i)
	count[i] = 0;
    for (i = (int) dtfirst(dt); i != 0; i = (int) dtnext(dt, i))
	count[i] += 1;
    if (count[1] != 2)
	terror("Dtobag count 1");
    if (count[2] != 2)
	terror("Dtobag count 2");
    if (count[3] != 3)
	terror("Dtobag count 3");

    /* test consecutive  1's */
    if ((int) dtsearch(dt, 1) != 1)
	terror("Dtobag search 1");
    if ((int) dtnext(dt, 1) != 1)
	terror("Dtobag next 1");
    if ((int) dtnext(dt, 1) != 2)
	terror("Dtobag next should be 2");

    /* test consecutive  2's */
    if ((int) dtsearch(dt, 2) != 2)
	terror("Dtobag search 2");
    if ((int) dtnext(dt, 2) != 2)
	terror("Dtobag next 2");
    if ((int) dtnext(dt, 2) != 3)
	terror("Dtobag next should be 3");

    /* test consecutive 3's */
    if ((int) dtsearch(dt, 3) != 3)
	terror("Dtobag search 3");
    if ((int) dtnext(dt, 3) != 3)
	terror("Dtobag next 3");
    if ((int) dtnext(dt, 3) != 3)
	terror("Dtobag next 3.2");
    if ((int) dtnext(dt, 3) == 3)
	terror("Dtobag next not expecting 3");

    /* test large set of values */
    dtclear(dt);
    dtmethod(dt, Dtset);
    for (i = 1; i < 20000; ++i)
	if ((int) dtinsert(dt, i) != i)
	    terror("Can't insert");
    dtmethod(dt, Dtoset);
    for (i = 1, k = (int) dtfirst(dt); i < 20000;
	 ++i, k = (int) dtnext(dt, k))
	if (i != k)
	    terror("Bad value");

    return 0;
}
