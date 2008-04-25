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

#ifndef _DEFS_H_
#define _DEFS_H_

#include "neato.h"

#include "sparsegraph.h"

#ifdef DIGCOLA
#ifdef IPSEPCOLA
    typedef struct cluster_data {
	int nvars;         /* total count of vars in clusters */
        int nclusters;     /* number of clusters */
        int *clustersizes; /* number of vars in each cluster */
        int **clusters;    /* list of var indices for constituents of each c */
	int ntoplevel;     /* number of nodes not in any cluster */
	int *toplevel;     /* array of nodes not in any cluster */
	boxf *bb;	   /* bounding box of each cluster */
    } cluster_data;
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif
