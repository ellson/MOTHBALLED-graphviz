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

/* per graph structure in gvc */

#ifndef GVG_H
#define GVG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gvj.h"

    struct GVG_s {

	GVC_t *gvc;     /* parent gvc */
       
	GVJ_t *jobs;	/* linked list of jobs on this graph */
	GVJ_t *job;	/* current job */
	GVJ_t *active_jobs; /* linked list of active jobs
				(e.g. multiple open windows on same graph) */

	int emit_state;	/* current emit_state */
	graph_t *g;	/* current graph */
	graph_t *sg;	/* current subgraph/cluster */
	node_t *n;	/* current node */
	edge_t *e;	/* current edge */

	char *inputFilename;    /* NULL if from stdin */
	char *graphname;	/* name from graph */

	char **lib;

	/* pagination */
	char *pagedir;		/* pagination order */
	pointf margin;		/* margins in graph units */
	pointf pageSize;	/* pageSize in graph units, not including margins */
	point pb;		/* page size - including margins (inches) */
	boxf bb;		/* graph bb in graph units, not including margins */
	int rotation;		/* rotation - 0 = portrait, 90 = landscape */
	bool graph_sets_margin, graph_sets_pageSize, graph_sets_rotation;

	/* layers */
	char *layerDelims;	/* delimiters in layer names */
	char *layers;		/* null delimited list of layer names */
	char **layerIDs;	/* array of layer names */
	int numLayers;		/* number of layers */

        int viewNum;	        /* current view - 1 based count of views,
					all pages in all layers */
	/* default font */
	char *defaultfontname;
	double defaultfontsize;

	/* default line style */
	char **defaultlinestyle;

#define MAXNEST 4

	gvstyle_t styles[MAXNEST]; /* style stack - reused by each job */
	int SP;

	/* render defaults set from graph */
	gvcolor_t bgcolor;	/* background color */
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVG_H */
