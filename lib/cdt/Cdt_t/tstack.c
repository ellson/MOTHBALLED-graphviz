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

main()
{
    Dt_t *dt;
    int i;

    /* testing Dtstack */
    if (!(dt = dtopen(&Disc, Dtstack)))
	terror("dtopen stack");
    if ((int) dtinsert(dt, 1) != 1)
	terror("Dtstack insert 1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtstack insert 3.1");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Dtstack insert 2.1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtstack insert 3.2");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Dtstack insert 2.2");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtstack insert 3.3");

    if ((int) dtlast(dt) != 1)
	terror("Dtstack dtlast");
    if ((int) dtprev(dt, 1) != 3)
	terror("Dtstack dtprev 1");
    if ((int) dtprev(dt, 3) != 2)
	terror("Dtstack dtprev 3.1");
    if ((int) dtprev(dt, 2) != 3)
	terror("Dtstack dtprev 2.1");
    if ((int) dtprev(dt, 3) != 2)
	terror("Dtstack dtprev 3.2");
    if ((int) dtprev(dt, 2) != 3)
	terror("Dtstack dtprev 2.2");
    if ((int) dtprev(dt, 3) != 0)
	terror("Dtstack dtprev 3.2");

    if ((int) dtdelete(dt, NIL(Void_t *)) != 3)
	terror("Dtstack pop 3.3");

    /* search to one of the 3 */
    if ((int) dtsearch(dt, 3) != 3)
	terror("Dtstack search 3.2");
    if ((int) dtdelete(dt, 3) != 3)
	terror("Dtstack delete 3.2");

    if ((int) dtdelete(dt, NIL(Void_t *)) != 2)
	terror("Dtstack pop 2.2");
    if ((int) dtdelete(dt, NIL(Void_t *)) != 2)
	terror("Dtstack pop 2.1");
    if ((int) dtdelete(dt, NIL(Void_t *)) != 3)
	terror("Dtstack pop 3.1");
    if ((int) dtdelete(dt, NIL(Void_t *)) != 1)
	terror("Dtstack pop 1");

    if (dtsize(dt) != 0)
	terror("Dtstack size");

    return 0;
}
