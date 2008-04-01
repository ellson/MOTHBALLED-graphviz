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
    int *foci_nodes;  /* Nodes in real graph */ 
    double *x_foci;   /* Universal coordinates */
    double *y_foci;
} focus_t;

/* Conversion from logical to physical coordinates:
 * NoRescale - simple copy
 * For Scale, Polar and Rectilinear, the coordinates are all
 * scaled and translated to map into the rectangle
 *   ((margin,margin),(w,h)) 
 * where w = width*graphSize/100 - 2*margin
 * and   h = height*graphSize/100 - 2*margin
 * 
 * For Scale, this is all that is done.
 * For Polar and Rectilinear, more space is provided around the foci. 
 */
typedef enum {NoRescale, Scale, Polar, Rectilinear} RescaleType;

typedef struct {
    int width;      /* viewport width */
    int height;     /* viewport height */
    int margin;     /* viewport margin */
    int graphSize;  /* viewport scale : 0 -- 100 */
    RescaleType rescale;
} hierparms_t;

void positionAllItems(Hierarchy * hp, focus_t * fs, hierparms_t * parms);
vtx_data *makeGraph(topview * tv, int *nedges);
Hierarchy *makeHier(int nnodes, int nedges, vtx_data *, double *,
		    double *);
focus_t *initFocus(int ncnt);

#endif
