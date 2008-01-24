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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "memory.h"
#include "triangle.h"
#include "hierarchy.h"

// maybe it should be replaced by RNG - relative neigborhood graph, or by GG - gabriel graph

vtx_data *delaunay_triangulation(double *x, double *y, int n)
{
    vtx_data *delaunay;
    triangulateio in, out;
    int i;
    int nedges;
    int *edges;
    int source, dest;

    in.pointlist = N_NEW(2 * n, REAL);
    for (i = 0; i < n; i++) {
	in.pointlist[2 * i] = x[i];
	in.pointlist[2 * i + 1] = y[i];
    }

    in.pointattributelist = NULL;
    in.pointmarkerlist = NULL;
    in.numberofpoints = n;
    in.numberofpointattributes = 0;
    in.trianglearealist = NULL;
    in.triangleattributelist = NULL;
    in.numberoftriangleattributes = 0;
    in.neighborlist = NULL;
    in.segmentlist = NULL;
    in.segmentmarkerlist = NULL;
    in.holelist = NULL;
    in.numberofholes = 0;
    in.regionlist = NULL;
    in.edgelist = NULL;
    in.edgemarkerlist = NULL;
    in.normlist = NULL;

    out.pointattributelist = NULL;
    out.pointmarkerlist = NULL;
    out.numberofpoints = n;
    out.numberofpointattributes = 0;
    out.trianglearealist = NULL;
    out.triangleattributelist = NULL;
    out.numberoftriangleattributes = 0;
    out.neighborlist = NULL;
    out.segmentlist = NULL;
    out.segmentmarkerlist = NULL;
    out.holelist = NULL;
    out.numberofholes = 0;
    out.regionlist = NULL;
    out.edgelist = NULL;
    out.edgemarkerlist = NULL;
    out.normlist = NULL;

    triangulate("zQNEeB", &in, &out, NULL);

    nedges = out.numberofedges;

    delaunay = N_NEW(n, vtx_data);
    edges = N_NEW(2 * nedges + n, int);
    for (i = 0; i < n; i++) {
	delaunay[i].ewgts = NULL;
	delaunay[i].nedges = 1;
    }

    for (i = 0; i < 2 * nedges; i++)
	delaunay[out.edgelist[i]].nedges++;

    for (i = 0; i < n; i++) {
	delaunay[i].edges = edges;
	edges += delaunay[i].nedges;
	delaunay[i].edges[0] = i;
	delaunay[i].nedges = 1;
    }
    for (i = 0; i < nedges; i++) {
	source = out.edgelist[2 * i];
	dest = out.edgelist[2 * i + 1];
	delaunay[source].edges[delaunay[source].nedges++] = dest;
	delaunay[dest].edges[delaunay[dest].nedges++] = source;
    }

    free(in.pointlist);
    free(out.edgelist);
    return delaunay;
}

static void remove_edge(vtx_data * graph, int source, int dest)
{
    int i;
    for (i = 1; i < graph[source].nedges; i++) {
	if (graph[source].edges[i] == dest) {
	    graph[source].edges[i] =
		graph[source].edges[--graph[source].nedges];
	    break;
	}
    }
}

vtx_data *UG_graph(double *x, double *y, int n, int accurate_computation)
{
    triangulateio in, out;
    vtx_data *delaunay = N_NEW(n, vtx_data);
    int i;
    int nedges;
    int *edges;
    double dist_ij, dist_ik, dist_jk, x_i, y_i, x_j, y_j;
    int j, k, neighbor_j, neighbor_k;
    int removed;

    in.pointlist = N_NEW(2 * n, REAL);
    for (i = 0; i < n; i++) {
	in.pointlist[2 * i] = x[i];
	in.pointlist[2 * i + 1] = y[i];
    }

    in.pointattributelist = NULL;
    in.pointmarkerlist = NULL;
    in.numberofpoints = n;
    in.numberofpointattributes = 0;
    in.trianglearealist = NULL;
    in.triangleattributelist = NULL;
    in.numberoftriangleattributes = 0;
    in.neighborlist = NULL;
    in.segmentlist = NULL;
    in.segmentmarkerlist = NULL;
    in.holelist = NULL;
    in.numberofholes = 0;
    in.regionlist = NULL;
    in.edgelist = NULL;
    in.edgemarkerlist = NULL;
    in.normlist = NULL;

    out.pointattributelist = NULL;
    out.pointmarkerlist = NULL;
    out.numberofpoints = n;
    out.numberofpointattributes = 0;
    out.trianglearealist = NULL;
    out.triangleattributelist = NULL;
    out.numberoftriangleattributes = 0;
    out.neighborlist = NULL;
    out.segmentlist = NULL;
    out.segmentmarkerlist = NULL;
    out.holelist = NULL;
    out.numberofholes = 0;
    out.regionlist = NULL;
    out.edgelist = NULL;
    out.edgemarkerlist = NULL;
    out.normlist = NULL;

    if (n == 2) {
	int *edges = N_NEW(4, int);
	delaunay[0].ewgts = NULL;
	delaunay[0].edges = edges;
	delaunay[0].nedges = 2;
	delaunay[0].edges[0] = 0;
	delaunay[0].edges[1] = 1;
	delaunay[1].edges = edges + 2;
	delaunay[1].ewgts = NULL;
	delaunay[1].nedges = 2;
	delaunay[1].edges[0] = 1;
	delaunay[1].edges[1] = 0;
	return delaunay;
    } else if (n == 1) {
	int *edges = N_NEW(1, int);
	delaunay[0].ewgts = NULL;
	delaunay[0].edges = edges;
	delaunay[0].nedges = 1;
	delaunay[0].edges[0] = 0;
	return delaunay;
    }

    triangulate("zQNEeB", &in, &out, NULL);

    nedges = out.numberofedges;

    edges = N_NEW(2 * nedges + n, int);
    for (i = 0; i < n; i++) {
	delaunay[i].ewgts = NULL;
	delaunay[i].nedges = 1;
    }

    for (i = 0; i < 2 * nedges; i++)
	delaunay[out.edgelist[i]].nedges++;

    for (i = 0; i < n; i++) {
	delaunay[i].edges = edges;
	edges += delaunay[i].nedges;
	delaunay[i].edges[0] = i;
	delaunay[i].nedges = 1;
    }
    int source, dest;
    for (i = 0; i < nedges; i++) {
	source = out.edgelist[2 * i];
	dest = out.edgelist[2 * i + 1];
	delaunay[source].edges[delaunay[source].nedges++] = dest;
	delaunay[dest].edges[delaunay[dest].nedges++] = source;
    }

    free(in.pointlist);
    free(out.edgelist);

    if (accurate_computation) {
	for (i = 0; i < n; i++) {
	    x_i = x[i];
	    y_i = y[i];
	    for (j = 1; j < delaunay[i].nedges;) {
		neighbor_j = delaunay[i].edges[j];
		if (neighbor_j < i) {
		    j++;
		    continue;
		}
		x_j = x[neighbor_j];
		y_j = y[neighbor_j];
		dist_ij =
		    (x_j - x_i) * (x_j - x_i) + (y_j - y_i) * (y_j - y_i);
		removed = 0;
		for (k = 0; k < n && !removed; k++) {
		    dist_ik =
			(x[k] - x_i) * (x[k] - x_i) + (y[k] -
						       y_i) * (y[k] - y_i);
		    if (dist_ik < dist_ij) {
			dist_jk =
			    (x[k] - x_j) * (x[k] - x_j) + (y[k] -
							   y_j) * (y[k] -
								   y_j);
			if (dist_jk < dist_ij) {
			    // remove the edge beteween i and neighbor j
			    delaunay[i].edges[j] =
				delaunay[i].edges[--delaunay[i].nedges];
			    remove_edge(delaunay, neighbor_j, i);
			    removed = 1;
			}
		    }
		}
		if (!removed) {
		    j++;
		}
	    }
	}
    } else {
	// remove all edges v-u if there is w, neighbor of u or v, that is closer to both u and v than dist(u,v)
	for (i = 0; i < n; i++) {
	    x_i = x[i];
	    y_i = y[i];
	    for (j = 1; j < delaunay[i].nedges;) {
		neighbor_j = delaunay[i].edges[j];
		x_j = x[neighbor_j];
		y_j = y[neighbor_j];
		dist_ij =
		    (x_j - x_i) * (x_j - x_i) + (y_j - y_i) * (y_j - y_i);
		// now look at i'th neighbors to see whether there is a node in the "forbidden region"
		// we will also go through neighbor_j's neighbors when we traverse the edge from its other side
		removed = 0;
		for (k = 1; k < delaunay[i].nedges && !removed; k++) {
		    neighbor_k = delaunay[i].edges[k];
		    dist_ik =
			(x[neighbor_k] - x_i) * (x[neighbor_k] - x_i) +
			(y[neighbor_k] - y_i) * (y[neighbor_k] - y_i);
		    if (dist_ik < dist_ij) {
			dist_jk =
			    (x[neighbor_k] - x_j) * (x[neighbor_k] - x_j) +
			    (y[neighbor_k] - y_j) * (y[neighbor_k] - y_j);
			if (dist_jk < dist_ij) {
			    // remove the edge beteween i and neighbor j
			    delaunay[i].edges[j] =
				delaunay[i].edges[--delaunay[i].nedges];
			    remove_edge(delaunay, neighbor_j, i);
			    removed = 1;
			}
		    }
		}
		if (!removed) {
		    j++;
		}
	    }
	}
    }
    return delaunay;
}
