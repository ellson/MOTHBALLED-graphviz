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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "delaunay.h"
#include "memory.h"
#include "logic.h"

#ifdef HAVE_TRIANGLE
#define TRILIBRARY
#include "triangle.c"

// maybe it should be replaced by RNG - relative neigborhood graph, or by GG - gabriel graph
int* 
delaunay_tri (double *x, double *y, int n, int* nedges)
{
    triangulateio in, out;
    int i;

    in.pointlist = N_GNEW(2 * n, REAL);
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

    *nedges = out.numberofedges;
    free(in.pointlist);
    return out.edgelist;
}

v_data *delaunay_triangulation(double *x, double *y, int n)
{
    v_data *delaunay;
    int nedges;
    int *edges;
    int source, dest;
    int* edgelist = delaunay_tri (x, y, n, &nedges);
    int i;

    delaunay = N_GNEW(n, v_data);
    edges = N_GNEW(2 * nedges + n, int);

    for (i = 0; i < n; i++) {
	delaunay[i].ewgts = NULL;
	delaunay[i].nedges = 1;
    }

    for (i = 0; i < 2 * nedges; i++)
	delaunay[edgelist[i]].nedges++;

    for (i = 0; i < n; i++) {
	delaunay[i].edges = edges;
	edges += delaunay[i].nedges;
	delaunay[i].edges[0] = i;
	delaunay[i].nedges = 1;
    }
    for (i = 0; i < nedges; i++) {
	source = edgelist[2 * i];
	dest = edgelist[2 * i + 1];
	delaunay[source].edges[delaunay[source].nedges++] = dest;
	delaunay[dest].edges[delaunay[dest].nedges++] = source;
    }

    free(edgelist);
    return delaunay;
}

#elif HAVE_GTS
#include <gts.h>

static gboolean triangle_is_hole(GtsTriangle * t)
{
    GtsEdge *e1, *e2, *e3;
    GtsVertex *v1, *v2, *v3;

    gts_triangle_vertices_edges(t, NULL, &v1, &v2, &v3, &e1, &e2, &e3);

    if ((GTS_IS_CONSTRAINT(e1) && GTS_SEGMENT(e1)->v1 != v1) ||
	(GTS_IS_CONSTRAINT(e2) && GTS_SEGMENT(e2)->v1 != v2) ||
	(GTS_IS_CONSTRAINT(e3) && GTS_SEGMENT(e3)->v1 != v3))
	return TRUE;
    return FALSE;
}

static guint delaunay_remove_holes(GtsSurface * surface)
{
    return gts_surface_foreach_face_remove(surface,
				    (GtsFunc) triangle_is_hole, NULL);
}

typedef struct {
    GtsVertex v;
    int idx;
} GVertex;

typedef struct {
    GtsVertexClass parent_class;
} GVertexClass;

static GVertexClass *g_vertex_class(void)
{
    static GVertexClass *klass = NULL;

    if (klass == NULL) {
	GtsObjectClassInfo vertex_info = {
	    "GVertex",
	    sizeof(GVertex),
	    sizeof(GVertexClass),
	    (GtsObjectClassInitFunc) NULL,
	    (GtsObjectInitFunc) NULL,
	    (GtsArgSetFunc) NULL,
	    (GtsArgGetFunc) NULL
	};
	klass = gts_object_class_new(GTS_OBJECT_CLASS(gts_vertex_class()),
				     &vertex_info);
    }

    return klass;
}

static GtsSurface *tri(double *x, double *y, int npt, int *segs, int nsegs)
{
    int i;
    GtsSurface *surface = gts_surface_new(gts_surface_class(),
					  gts_face_class(),
					  gts_edge_class(),
					  gts_vertex_class());
    GVertex **vertices = N_GNEW(npt, GVertex *);
    GtsEdge *c;
    GSList *list = NULL;
    GtsVertex *v1, *v2, *v3;
    GtsTriangle *t;
    GtsVertexClass *vcl = (GtsVertexClass *) g_vertex_class();

    for (i = 0; i < npt; i++) {
	GVertex *p = (GVertex *) gts_vertex_new(vcl, x[i], y[i], 0);
	p->idx = i;
	vertices[i] = p;
    }

    for (i = 0; i < npt; i++)
	list = g_slist_prepend(list, vertices[i]);
    t = gts_triangle_enclosing(gts_triangle_class(), list, 100.);
    g_slist_free(list);

    gts_triangle_vertices(t, &v1, &v2, &v3);

    gts_surface_add_face(surface, gts_face_new(gts_face_class(),
					       t->e1, t->e2, t->e3));

    for (i = 0; i < npt; i++) {
	GtsVertex *v1 = (GtsVertex *) vertices[i];
	GtsVertex *v = gts_delaunay_add_vertex(surface, v1, NULL);
	if (v) {
	    fprintf(stderr, "Error adding point %d\n", i);
	    exit(1);
	}
    }

    for (i = 0; i < nsegs; i++) {
	c = gts_edge_new(GTS_EDGE_CLASS(gts_constraint_class()),
			 (GtsVertex *) (vertices[2 * i]),
			 (GtsVertex *) (vertices[2 * i + 1]));
	gts_delaunay_add_constraint(surface, GTS_CONSTRAINT(c));
    }

    /* destroy enclosing triangle */
    gts_allow_floating_vertices = TRUE;
    gts_object_destroy(GTS_OBJECT(v1));
    gts_object_destroy(GTS_OBJECT(v2));
    gts_object_destroy(GTS_OBJECT(v3));
    gts_allow_floating_vertices = FALSE;

    if (nsegs)
	delaunay_remove_holes(surface);

    free(vertices);
    return surface;
}

typedef struct {
    int n;
    v_data *delaunay;
} estats;
    
static void cnt_edge (GtsSegment * e, estats* sp)
{
    sp->n++;
    if (sp->delaunay) {
	sp->delaunay[((GVertex*)(e->v1))->idx].nedges++;
	sp->delaunay[((GVertex*)(e->v2))->idx].nedges++;
    }
}

static void
edgeStats (GtsSurface* s, estats* sp)
{
    gts_surface_foreach_edge (s, (GtsFunc) cnt_edge, sp);
}

static void add_edge (GtsSegment * e, v_data* delaunay)
{
    int source = ((GVertex*)(e->v1))->idx;
    int dest = ((GVertex*)(e->v2))->idx;

    delaunay[source].edges[delaunay[source].nedges++] = dest;
    delaunay[dest].edges[delaunay[dest].nedges++] = source;
}

v_data *delaunay_triangulation(double *x, double *y, int n)
{
    v_data *delaunay;
    GtsSurface* s = tri(x, y, n, NULL, 0);
    int i, nedges;
    int* edges;
    estats stats;

    if (!s) return NULL;

    delaunay = N_GNEW(n, v_data);

    for (i = 0; i < n; i++) {
	delaunay[i].ewgts = NULL;
	delaunay[i].nedges = 1;
    }

    stats.n = 0;
    stats.delaunay = delaunay;
    edgeStats (s, &stats);
    nedges = stats.n;
    edges = N_GNEW(2 * nedges + n, int);

    for (i = 0; i < n; i++) {
	delaunay[i].edges = edges;
	edges += delaunay[i].nedges;
	delaunay[i].edges[0] = i;
	delaunay[i].nedges = 1;
    }
    gts_surface_foreach_edge (s, (GtsFunc) add_edge, delaunay);

    gts_object_destroy (GTS_OBJECT (s));

    return delaunay;
}

typedef struct {
    int n;
    int* edges;
} estate;

static void addEdge (GtsSegment * e, estate* es)
{
    int source = ((GVertex*)(e->v1))->idx;
    int dest = ((GVertex*)(e->v2))->idx;

    es->edges[2*(es->n)] = source;
    es->edges[2*(es->n)+1] = dest;
    es->n += 1;
}

int *delaunay_tri(double *x, double *y, int n, int* pnedges)
{
    GtsSurface* s = tri(x, y, n, NULL, 0);
    int nedges;
    int* edges;
    estats stats;
    estate state;

    if (!s) return NULL;

    stats.n = 0;
    stats.delaunay = NULL;
    edgeStats (s, &stats);
    *pnedges = nedges = stats.n;
    edges = N_GNEW(2 * nedges, int);

    state.n = 0;
    state.edges = edges;
    gts_surface_foreach_edge (s, (GtsFunc) addEdge, &state);

    gts_object_destroy (GTS_OBJECT (s));

    return edges;
}
#else
v_data *delaunay_triangulation(double *x, double *y, int n)
{
    fprintf(stderr, "Graphviz built without triangulation library\n");
    return 0;
}
int *delaunay_tri(double *x, double *y, int n, int* nedges)
{
    fprintf(stderr, "Graphviz built without triangulation library\n");
    return 0;
}
#endif

static void remove_edge(v_data * graph, int source, int dest)
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

v_data *UG_graph(double *x, double *y, int n, int accurate_computation)
{
    v_data *delaunay;
    int i;
    double dist_ij, dist_ik, dist_jk, x_i, y_i, x_j, y_j;
    int j, k, neighbor_j, neighbor_k;
    int removed;

    if (n == 2) {
	int *edges = N_GNEW(4, int);
	delaunay = N_GNEW(n, v_data);
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
	int *edges = N_GNEW(1, int);
	delaunay = N_GNEW(n, v_data);
	delaunay[0].ewgts = NULL;
	delaunay[0].edges = edges;
	delaunay[0].nedges = 1;
	delaunay[0].edges[0] = 0;
	return delaunay;
    }

    delaunay = delaunay_triangulation(x, y, n);

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
		removed = FALSE;
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
			    removed = TRUE;
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
		removed = FALSE;
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
			    removed = TRUE;
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

void freeGraph (v_data * graph)
{
    if (graph != NULL) {
	if (graph[0].edges != NULL)
	    free(graph[0].edges);
	if (graph[0].ewgts != NULL)
	    free(graph[0].ewgts);
	free(graph);
    }
}

void freeGraphData(vtx_data * graph)
{
    if (graph != NULL) {
	if (graph[0].edges != NULL)
	    free(graph[0].edges);
	if (graph[0].ewgts != NULL)
	    free(graph[0].ewgts);
#ifdef USE_STYLES
	if (graph[0].styles != NULL)
	    free(graph[0].styles);
#endif
#ifdef DIGCOLA
	if (graph[0].edists != NULL)
	    free(graph[0].edists);
#endif
	free(graph);
    }
}

