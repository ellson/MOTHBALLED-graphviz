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

v_data *delaunay_triangulation(double *x, double *y, int n);

int *delaunay_tri (double *x, double *y, int n, int* nedges);

v_data *UG_graph(double *x, double *y, int n, int accurate_computation);

#endif
