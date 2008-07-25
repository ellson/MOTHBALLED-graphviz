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

#ifndef DELAUNAY_H
#define DELAUNAY_H

#include "sparsegraph.h"

typedef struct {
    int  nedges; /* no. of edges in triangulation */
    int* edges;  /* 2*nsegs indices of points */
    int  nfaces; /* no. of faces in triangulation */
    int* faces;  /* 3*nfaces indices of points */ 
    int* neigh;  /* 3*nfaces indices of neighbor triangles */ 
} surface_t;

v_data *delaunay_triangulation(double *x, double *y, int n);

int *delaunay_tri (double *x, double *y, int n, int* nedges);

v_data *UG_graph(double *x, double *y, int n, int accurate_computation);

surface_t* mkSurface (double *x, double *y, int n, int* segs, int nsegs);

void freeSurface (surface_t* s);

#endif
