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

#ifndef NEATOPROCS_H
#define NEATOPROCS_H

#ifdef __cplusplus
extern "C" {
#endif

    extern int allow_edits(int);
    extern void avoid_cycling(graph_t *, Agnode_t *, double *);
    extern int checkStart(graph_t * G, int nG, int);
    extern Agnode_t *choose_node(graph_t *, int);
    extern int circuit_model(graph_t *, int);
    extern void D2E(Agraph_t *, int, int, double *);
    extern void diffeq_model(graph_t *, int);
    extern double distvec(double *, double *, double *);
    extern void do_graph_label(Agraph_t *);
    extern void final_energy(graph_t *, int);
    extern double doubleattr(void *, int, double);
    extern double fpow32(double);
    extern Ppolyline_t getPath(edge_t *, vconfig_t *, int, Ppoly_t **,
			       int);
    extern void heapdown(Agnode_t *);
    extern void heapup(Agnode_t *);
    extern void initial_positions(graph_t *, int);
    extern int init_port(Agnode_t *, Agedge_t *, char *, boolean);
    extern void jitter3d(Agnode_t *, int);
    extern void jitter_d(Agnode_t *, int, int);
    extern Ppoly_t *makeObstacle(node_t * n, double SEP);
    extern void makeSelfArcs(path * P, edge_t * e, int stepx);
    extern void makeSpline(edge_t *, Ppoly_t **, int, boolean);
    extern void make_spring(graph_t *, Agnode_t *, Agnode_t *, double);
    extern void move_node(graph_t *, int, Agnode_t *);
    extern int init_nop(graph_t * g);
    extern void neato_nodesize(node_t * n, boolean flip);
    extern void neato_cleanup(graph_t * g);
    extern void neato_cleanup_edge(edge_t * e);
    extern void neato_cleanup_graph(graph_t * g);
    extern void neato_cleanup_node(node_t * n);
    extern node_t *neato_dequeue(void);
    extern void neato_enqueue(node_t *);
    extern void neato_free_splines(edge_t * e);
    extern void neato_init_node_edge(Agraph_t *);
    extern void neato_init_edge(Agedge_t *);
    extern void neato_init_node(Agnode_t *);
    extern void neato_layout(Agraph_t * g);
    extern void neatoLayout(Agraph_t * g, int layoutMode, int layoutModel);
    extern void neato_init_graph(graph_t * g);
    extern void neato_init_graphn(graph_t * g, int);
    extern int Plegal_arrangement(Ppoly_t ** polys, int n_polys);
    extern void randompos(Agnode_t *, int);
    extern void s1(graph_t *, node_t *);
    extern int scan_graph(graph_t *);
    extern int scan_graph_mode(graph_t * G, int mode);
    extern void free_scan_graph(graph_t *);
    extern void shortest_path(graph_t *, int);
    extern void solve(double *, double *, double *, int);
    extern void solve_model(graph_t *, int);
    extern int solveCircuit(int nG, double **Gm, double **Gm_inv);
    extern void spline_edges(Agraph_t *);
    extern void spline_edges0(Agraph_t *);
    extern int spline_edges1(graph_t * g, int);
    extern int splineEdges(graph_t *,
			   int (*edgefn) (graph_t *, double, int), int);
    extern void neato_set_aspect(graph_t * g);
    extern void toggle(int);
    extern int user_pos(Agsym_t *, Agsym_t *, Agnode_t *, int);
    extern double **new_array(int i, int j, double val);
    extern void free_array(double **rv);
    extern int matinv(double **A, double **Ainv, int n);

#ifdef __cplusplus
}
#endif
#endif
