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


#ifndef VISIBILITY_H
#define VISIBILITY_H

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <vispath.h>
#include <pathutil.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef COORD **array2;
    typedef unsigned char boolean;

#define	OBSCURED	0.0
#define EQ(p,q)		((p.x == q.x) && (p.y == q.y))
#define NEQ(p,q)	(!EQ(p,q))
#define NIL(p)		((p)0)
#define	CW			0
#define	CCW			1

    struct vconfig_s {
	int Npoly;
	int N;			/* number of points in walk of barriers */
	Ppoint_t *P;		/* barrier points */
	int *start;
	int *next;
	int *prev;

	/* this is computed from the above */
	array2 vis;
    };

    extern COORD *ptVis(vconfig_t *, int, Ppoint_t);
    extern int directVis(Ppoint_t, int, Ppoint_t, int, vconfig_t *);
    extern void visibility(vconfig_t *);
    extern int *makePath(Ppoint_t p, int pp, COORD * pvis,
			 Ppoint_t q, int qp, COORD * qvis,
			 vconfig_t * conf);

#ifdef __cplusplus
}
#endif
#endif
