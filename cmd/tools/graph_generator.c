/* $Id$Revision: */
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
#include <math.h>
#include <graph_generator.h>

void makePath(int n, edgefn ef)
{
    int i;

    if (n == 1) {
	ef (1, 0);
	return;
    }
    for (i = 2; i <= n; i++)
	ef (i - 1, i);
}

void makeComplete(int n, edgefn ef)
{
    int i, j;

    if (n == 1) {
	ef (1, 0);
	return;
    }
    for (i = 1; i < n; i++) {
	for (j = i + 1; j <= n; j++) {
	    ef ( i, j);
	}
    }
}

void makeCircle(int n, edgefn ef)
{
    int i;

    if (n < 3) {
	fprintf(stderr, "Warning: degenerate circle of %d vertices\n", n);
	makePath(n, ef);
	return;
    }

    for (i = 1; i < n; i++)
	ef ( i, i + 1);
    ef (1, n);
}

void makeStar(int n, edgefn ef)
{
    int i;

    if (n < 3) {
	fprintf(stderr, "Warning: degenerate star of %d vertices\n", n);
	makePath(n, ef);
	return;
    }

    for (i = 2; i <= n; i++)
	ef (1, i);
}

void makeWheel(int n, edgefn ef)
{
    int i;

    if (n < 4) {
	fprintf(stderr, "Warning: degenerate wheel of %d vertices\n", n);
	makeComplete(n, ef);
	return;
    }

    makeStar(n, ef);

    for (i = 2; i < n; i++)
	ef( i, i + 1);
    ef (2, n);
}

void makeTorus(int dim1, int dim2, edgefn ef)
{
    int i, j, n = 0;

    for (i = 1; i <= dim1; i++) {
	for (j = 1; j < dim2; j++) {
	    ef( n + j, n + j + 1);
	}
	ef( n + 1, n + dim2);
	n += dim2;
    }

    for (i = 1; i <= dim2; i++) {
	for (j = 1; j < dim1; j++) {
	    ef( dim2 * (j - 1) + i, dim2 * j + i);
	}
	ef( i, dim2 * (dim1 - 1) + i);
    }
}

void makeCylinder(int dim1, int dim2, edgefn ef)
{
    int i, j, n = 0;

    for (i = 1; i <= dim1; i++) {
	for (j = 1; j < dim2; j++) {
	    ef( n + j, n + j + 1);
	}
	ef( n + 1, n + dim2);
	n += dim2;
    }

    for (i = 1; i <= dim2; i++) {
	for (j = 1; j < dim1; j++) {
	    ef( dim2 * (j - 1) + i, dim2 * j + i);
	}
    }
}

#define OUTE(h) if (tl < (hd=(h))) ef( tl, hd)

void
makeSquareGrid(int dim1, int dim2, int connect_corners, int partial, edgefn ef)
{
    int i, j, tl, hd;

    for (i = 0; i < dim1; i++)
	for (j = 0; j < dim2; j++) {
	    // write the neighbors of the node i*dim2+j+1
	    tl = i * dim2 + j + 1;
	    if (j > 0
		&& (!partial || j <= 2 * dim2 / 6 || j > 4 * dim2 / 6
		    || i <= 2 * dim1 / 6 || i > 4 * dim1 / 6)) {
		OUTE(i * dim2 + j);
	    }
	    if (j < dim2 - 1
		&& (!partial || j < 2 * dim2 / 6 || j >= 4 * dim2 / 6
		    || i <= 2 * dim1 / 6 || i > 4 * dim1 / 6)) {
		OUTE(i * dim2 + j + 2);
	    }
	    if (i > 0) {
		OUTE((i - 1) * dim2 + j + 1);
	    }
	    if (i < dim1 - 1) {
		OUTE((i + 1) * dim2 + j + 1);
	    }
	    if (connect_corners == 1) {
		if (i == 0 && j == 0) {	// upper left
		    OUTE((dim1 - 1) * dim2 + dim2);
		} else if (i == (dim1 - 1) && j == 0) {	// lower left
		    OUTE(dim2);
		} else if (i == 0 && j == (dim2 - 1)) {	// upper right
		    OUTE((dim1 - 1) * dim2 + 1);
		} else if (i == (dim1 - 1) && j == (dim2 - 1)) {	// lower right
		    OUTE(1);
		}
	    } else if (connect_corners == 2) {
		if (i == 0 && j == 0) {	// upper left
		    OUTE(dim2);
		} else if (i == (dim1 - 1) && j == 0) {	// lower left
		    OUTE((dim1 - 1) * dim2 + dim2);
		} else if (i == 0 && j == (dim2 - 1)) {	// upper right
		    OUTE(1);
		} else if (i == (dim1 - 1) && j == (dim2 - 1)) {	// lower right
		    OUTE((dim1 - 1) * dim2 + 1);
		}
	    }
	}
}

void makeBinaryTree(int depth, edgefn ef)
{
    unsigned int i;
    unsigned int n = (1 << depth) - 1;

    for (i = 0; i < n; i++) {
	ef( i + 1, 2 * i + 2);
	ef( i + 1, 2 * i + 3);
    }
}

typedef struct {
  int     nedges;
  int*    edges;
} vtx_data;

static void
constructSierpinski(int v1, int v2, int v3, int depth, vtx_data* graph)
{
    static int last_used_node_name = 3;
    int v4, v5, v6;

    int nedges;

    if (depth > 0) {
	v4 = ++last_used_node_name;
	v5 = ++last_used_node_name;
	v6 = ++last_used_node_name;
	constructSierpinski(v1, v4, v5, depth - 1, graph);
	constructSierpinski(v2, v5, v6, depth - 1, graph);
	constructSierpinski(v3, v4, v6, depth - 1, graph);
	return;
    }
    // depth==0, Construct graph:

    nedges = graph[v1].nedges;
    graph[v1].edges[nedges++] = v2;
    graph[v1].edges[nedges++] = v3;
    graph[v1].nedges = nedges;

    nedges = graph[v2].nedges;
    graph[v2].edges[nedges++] = v1;
    graph[v2].edges[nedges++] = v3;
    graph[v2].nedges = nedges;

    nedges = graph[v3].nedges;
    graph[v3].edges[nedges++] = v1;
    graph[v3].edges[nedges++] = v2;
    graph[v3].nedges = nedges;

    return;

}

#define N_NEW(n,t)       (t*)malloc((n)*sizeof(t))

void makeSierpinski(int depth, edgefn ef)
{
    vtx_data* graph;
    int* edges;
    int n;
    int nedges;
    int i, j;

    depth--;
    n = 3 * (1 + ((int) (pow(3.0, (double) depth) + 0.5) - 1) / 2);

    nedges = (int) (pow(3.0, depth + 1.0) + 0.5);

    graph = N_NEW(n + 1, vtx_data);
    edges = N_NEW(4 * n, int);

    for (i = 1; i <= n; i++) {
	graph[i].edges = edges;
	edges += 4;
	graph[i].nedges = 0;
    }

    constructSierpinski(1, 2, 3, depth, graph);

    for (i = 1; i <= n; i++) {
	int nghbr;
	// write the neighbors of the node i
	for (j = 0; j < graph[i].nedges; j++) {
	    nghbr = graph[i].edges[j];
	    if (i < nghbr) ef( i, nghbr);
	}
    }

    free(graph[1].edges);
    free(graph);
}

void makeHypercube(int dim, edgefn ef)
{
    int i, j, n;
    int neighbor;

    n = 1 << dim;

    for (i = 0; i < n; i++) {
	for (j = 0; j < dim; j++) {
	    neighbor = (i ^ (1 << j)) + 1;
	    if (i < neighbor)
		ef( i + 1, neighbor);
	}
    }
}
