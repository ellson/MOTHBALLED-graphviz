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

    /* testing Dtobag */
    dt = dtopen(&Disc, Dtobag);
    if ((int) dtinsert(dt, 5) != 5)
	terror("Insert 5.1");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Insert 2.1");
    if ((int) dtinsert(dt, 5) != 5)
	terror("Insert 5.2");
    for (k = 0, i = (int) dtfirst(dt); i; k = i, i = (int) dtnext(dt, i))
	if (i < k)
	    terror("Wrong order1\n");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Insert 3.1");
    if ((int) dtinsert(dt, 5) != 5)
	terror("Insert 5.3");
    for (k = 0, i = (int) dtfirst(dt); i; k = i, i = (int) dtnext(dt, i))
	if (i < k)
	    terror("Wrong order2\n");
    if ((int) dtinsert(dt, 4) != 4)
	terror("Insert 4.1");
    if ((int) dtinsert(dt, 1) != 1)
	terror("Insert 1");
    for (k = 0, i = (int) dtfirst(dt); i; k = i, i = (int) dtnext(dt, i))
	if (i < k)
	    terror("Wrong order3\n");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Insert 2.2");
    if ((int) dtinsert(dt, 5) != 5)
	terror("Insert 5.4");
    if ((int) dtinsert(dt, 4) != 4)
	terror("Insert 4.2");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Insert 3.2");
    for (k = 0, i = (int) dtfirst(dt); i; k = i, i = (int) dtnext(dt, i))
	if (i < k)
	    terror("Wrong order4\n");
    if ((int) dtinsert(dt, 4) != 4)
	terror("Insert 4.3");
    if ((int) dtinsert(dt, 5) != 5)
	terror("Insert 5.5");
    for (k = 0, i = (int) dtfirst(dt); i; k = i, i = (int) dtnext(dt, i))
	if (i < k)
	    terror("Wrong order5\n");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Insert 3.3");
    if ((int) dtinsert(dt, 4) != 4)
	terror("Insert 4.4");

    for (k = 0, i = (int) dtfirst(dt); i; k = i, i = (int) dtnext(dt, i))
	if (i < k)
	    terror("Wrong order5\n");

    for (i = 0; i <= 5; ++i)
	count[i] = 0;
    for (i = (int) dtfirst(dt); i; i = (int) dtnext(dt, i))
	count[i] += 1;
    for (i = 0; i <= 5; ++i)
	if (count[i] != i)
	    terror("Wrong count\n");

    for (i = 0; i <= 5; ++i)
	count[i] = 0;
    for (i = (int) dtlast(dt); i; i = (int) dtprev(dt, i))
	count[i] += 1;
    for (i = 0; i <= 5; ++i)
	if (count[i] != i)
	    terror("Wrong count2\n");

    for (k = 0, i = (int) dtfirst(dt); i; k = i, i = (int) dtnext(dt, i))
	if (i < k)
	    terror("Wrong order6\n");

    for (link = dtflatten(dt), i = 1; link; ++i) {
	for (k = 1; k <= i; ++k, link = dtlink(dt, link))
	    if (i != (int) dtobj(dt, link))
		terror("Bad element\n");
    }

    return 0;
}
