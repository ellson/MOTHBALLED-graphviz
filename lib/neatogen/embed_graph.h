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



#ifndef EMBED_GRAPH_H_
#define EMBED_GRAPH_H_

#ifdef __cplusplus

    void embed_graph(vtx_data * graph, int n, int dim,
		     DistType ** (&coords), int);
    void center_coordinate(DistType ** coords, int n, int dim);
    void PCA(DistType ** coords, int dim, int n, double **(&new_coords),
	     int new_dim);
    void PCA(DistType ** coords, int dim, int n, double **(&new_coords),
	     int dim1, int dim2, bool recompute);
    void PCA_orthog(DistType ** coords, int dim, int n,
		    double **(&new_coords), int new_dim, double *orthog);
    void iterativePCA(DistType ** coords, int dim, int n,
		      double **(&new_coords));

#else
#include <defs.h>

    extern void embed_graph(vtx_data * graph, int n, int dim, DistType ***,
			    int);
    extern void center_coordinate(DistType **, int, int);

#endif

#endif

#ifdef __cplusplus
}
#endif
