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



#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include "defs.h"

#ifdef __cplusplus
    void dijkstra(int vertex, vtx_data * graph, int n, DistType * dist);

/* Dijkstra bounded to nodes in *unweighted* radius */
    void dijkstra_bounded(int vertex, vtx_data * graph, int n,
			  DistType * dist, int bound, int *visited_nodes,
			  int &num_visited_nodes);

#else
    extern void dijkstra(int, vtx_data *, int, DistType *);
    extern void dijkstra_f(int, vtx_data *, int, float *);

    /* Dijkstra bounded to nodes in *unweighted* radius */
    extern int dijkstra_bounded(int, vtx_data *, int, DistType *, int,
				int *);
#endif

#endif

#ifdef __cplusplus
}
#endif
