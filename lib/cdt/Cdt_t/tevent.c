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

static int Event;

#if __STD_C
static int event(Dt_t * dt, int type, Void_t * obj, Dtdisc_t * disc)
#else
static int event(dt, type, obj, disc)
Dt_t *dt;
int type;
Void_t *obj;
Dtdisc_t *disc;
#endif
{
    Event = type;
    return 0;
}

Dtdisc_t Disc = { 0, sizeof(int), -1,
    newint, NIL(Dtfree_f), compare, hashint,
    NIL(Dtmemory_f), event
};

main()
{
    Dt_t *dt;

    if (!(dt = dtopen(&Disc, Dtset)))
	terror("Opening Dtset");
    if (Event != DT_OPEN)
	terror("No open event");
    dtmethod(dt, Dtorder);
    if (Event != DT_METH)
	terror("No meth event");

    dtdisc(dt, &Disc, 0);
    if (Event != DT_DISC)
	terror("No disc event");

    dtclose(dt);
    if (Event != DT_CLOSE)
	terror("No close event");

    return 0;
}
