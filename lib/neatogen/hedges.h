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

#ifdef __cplusplus
extern "C" {
#endif



#ifndef HEDGES_H
#define HEDGES_H

#include "site.h"
#include "edges.h"

    typedef struct Halfedge {
	struct Halfedge *ELleft, *ELright;
	Edge *ELedge;
	int ELrefcnt;
	char ELpm;
	Site *vertex;
	double ystar;
	struct Halfedge *PQnext;
    } Halfedge;

    extern Halfedge *ELleftend, *ELrightend;

    extern void ELinitialize();
    extern void ELcleanup();
    extern int right_of(Halfedge *, Point *);
    extern Site *hintersect(Halfedge *, Halfedge *);
    extern Halfedge *HEcreate(Edge *, char);
    extern void ELinsert(Halfedge *, Halfedge *);
    extern Halfedge *ELleftbnd(Point *);
    extern void ELdelete(Halfedge *);
    extern Halfedge *ELleft(Halfedge *), *ELright(Halfedge *);
    extern Halfedge *ELleftbnd(Point *);
    extern Site *leftreg(Halfedge *), *rightreg(Halfedge *);

#endif

#ifdef __cplusplus
}
#endif
