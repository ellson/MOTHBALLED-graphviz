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

    /* testing Dtqueue */
    if (!(dt = dtopen(&Disc, Dtqueue)))
	terror("dtopen queue");
    if ((int) dtinsert(dt, 1) != 1)
	terror("Dtqueue insert 1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtqueue insert 3.1");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Dtqueue insert 2.1");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtqueue insert 3.2");
    if ((int) dtinsert(dt, 2) != 2)
	terror("Dtqueue insert 2.2");
    if ((int) dtinsert(dt, 3) != 3)
	terror("Dtqueue insert 3.3");

    if ((int) dtlast(dt) != 3)
	terror("Dtqueue dtlast");
    if ((int) dtprev(dt, 3) != 2)
	terror("Dtqueue dtprev 3.3");
    if ((int) dtprev(dt, 2) != 3)
	terror("Dtqueue dtprev 2.2");
    if ((int) dtprev(dt, 3) != 2)
	terror("Dtqueue dtprev 3.2");
    if ((int) dtprev(dt, 2) != 3)
	terror("Dtqueue dtprev 2.1");
    if ((int) dtprev(dt, 3) != 1)
	terror("Dtqueue dtprev 3.1");
    if ((int) dtprev(dt, 1) != 0)
	terror("Dtqueue dtprev 1");

    if ((int) dtdelete(dt, NIL(Void_t *)) != 1)
	terror("Dtqueue pop 1");
    if ((int) dtdelete(dt, NIL(Void_t *)) != 3)
	terror("Dtqueue delete 3.1");
    if ((int) dtdelete(dt, NIL(Void_t *)) != 2)
	terror("Dtqueue delete 2");
    if ((int) dtdelete(dt, NIL(Void_t *)) != 3)
	terror("Dtqueue delete 3.2");
    if ((int) dtdelete(dt, NIL(Void_t *)) != 2)
	terror("Dtqueue delete 2.1");
    if ((int) dtdelete(dt, NIL(Void_t *)) != 3)
	terror("Dtqueue delete 3.3");

    if (dtsize(dt) != 0)
	terror("Dtqueue size");

    return 0;
}
