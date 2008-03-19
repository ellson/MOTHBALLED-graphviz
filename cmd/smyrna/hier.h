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

#ifndef HIER_H
#define HIER_H

#include "hierarchy.h"
#include "topview.h"
typedef struct {
    int num_foci;
    int *foci_nodes;
    double *x_foci;
    double *y_foci;
} focus_t;

typedef struct {
    int graphSize;
    int ClientWidth;
    int ClientHeight;
    int margin;
    RescaleType rescale_type;	// use Polar by default
} hierparms_t;

void positionAllItems(Hierarchy * hp, focus_t * fs, hierparms_t * parms);
vtx_data *makeGraph(topview * tv, int *nedges);
Hierarchy *makeHier(int nnodes, int nedges, vtx_data *, double *,
		    double *);
focus_t *initFocus(int ncnt);

#endif
