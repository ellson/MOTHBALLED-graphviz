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



#ifndef SITE_H
#define SITE_H

#include "geometry.h"

    /* Sites are also used as vertices on line segments */
    typedef struct Site {
	Point coord;
	int sitenbr;
	int refcnt;
    } Site;

    extern int siteidx;
    extern Site *bottomsite;

    extern void siteinit(void);
    extern Site *getsite(void);
    extern double dist(Site *, Site *);	/* Distance between two sites */
    extern void deref(Site *);	/* Increment refcnt of site  */
    extern void ref(Site *);	/* Decrement refcnt of site  */
    extern void makevertex(Site *);	/* Transform a site into a vertex */
#endif

#ifdef __cplusplus
}
#endif
