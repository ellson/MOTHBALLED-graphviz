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

#ifndef XLAYOUT_H
#define XLAYOUT_H

#include <fdp.h>

    typedef struct {
	int numIters;
	double T0;
	double K;
	double C;
	int loopcnt;
        int tries;
    } xparams;

    extern void fdp_xLayout(graph_t *, xparams *);

#endif

#ifdef __cplusplus
}
#endif
