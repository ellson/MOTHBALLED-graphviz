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

#ifndef TLAYOUT_H
#define TLAYOUT_H

#include "fdp.h"
#include "xlayout.h"

typedef enum {
  seed_unset, seed_val, seed_time, seed_regular
} seedMode;

    typedef struct {
	int useGrid;		/* use grid for speed up */
	int useNew;		/* encode x-K into attractive force */
	int numIters;		/* actual iterations in layout */
	int unscaled;		/* % of iterations used in pass 1 */
	double C;		/* Repulsion factor in xLayout */
	double Tfact;		/* scale temp from default expression */
	double K;		/* spring constant; ideal distance */
	double T0;              /* initial temperature */
    } fdpParms_t;

    extern fdpParms_t fdp_parms;

    extern void fdp_initParams(graph_t *);
    extern void fdp_tLayout(graph_t *, xparams *);

#endif

#ifdef __cplusplus
}
#endif
