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

#ifndef _HIERARCHY_H_
#define _HIERARCHY_H_

#include "defs.h"

typedef enum {Polar,  Rectilinear, NoRescale} RescaleType;

typedef struct _ex_vtx_data {
	int nedges;
	int *edges;
	int size;
	int active_level;
	int globalIndex;

	// "logical" coordinates of node (result of algorithm):
	float x_coord;
	float y_coord;

	// coordinates of node after making local layout:
	float local_x_coord; 
	float local_y_coord;

	// actual coordinates of (active) node in drawing area	
	float physical_x_coord;
	float physical_y_coord;	

	// for animation
	int old_active_level;
	int new_active_level;
	float old_physical_x_coord;
	float old_physical_y_coord;
	float new_physical_x_coord;
	float new_physical_y_coord;
} ex_vtx_data;


typedef struct _Hierarchy {
	int nlevels;
	vtx_data ** graphs;
	ex_vtx_data ** geom_graphs;
	int * nvtxs;
	int * nedges;
	int ** v2cv;
	int ** cv2v;
	int maxNodeIndex;
} Hierarchy;

void release(Hierarchy*);
Hierarchy* create_hierarchy(vtx_data * graph, int nvtxs, int nedges, 
    ex_vtx_data* geom_graph, int ngeom_edges, int min_nvtxs);
	
void set_active_levels(Hierarchy*, int*, int);
void set_horizontal_active_level(Hierarchy* hierarchy, int cur_level);					
double find_closest_active_node(Hierarchy*, double x, double y, int*);
int find_leftmost_descendant(Hierarchy*, int node, int level, int min_level);

int extract_active_logical_coords(Hierarchy * hierarchy, int node, int level, 
    double *x_coords, double *y_coords, int counter);
int set_active_physical_coords(Hierarchy *, int node, int level,
    double *x_coords, double *y_coords, int counter);

// For animation
int extract_new_active_logical_coords(Hierarchy *, int node, int level, 
    double *x_coords, double *y_coords, int counter);
int set_new_active_physical_coords(Hierarchy * hierarchy, int node, int level,
    double *x_coords, double *y_coords, int counter);
void derive_old_new_active_physical_coords(Hierarchy *, int, int , 
    double new_x, double new_y, double old_x, double old_y);
int count_active_nodes(Hierarchy *);

// creating a geometric graph:
int init_ex_graph(vtx_data * graph1, vtx_data * graph2, int n,
   double *x_coords, double *y_coords, ex_vtx_data ** gp);

vtx_data *delaunay_triangulation(double *x, double *y, int n);

vtx_data *UG_graph(double *x, double *y, int n, int accurate_computation);

// layout distortion:
void rescale_layout(double *x_coords, double *y_coords,
    int n, int interval, int ClientWidth, int ClientHeight,
    int margin);

void rescale_layout_polar(double * x_coords, double * y_coords, 
    double * x_foci, double * y_foci, int num_foci,
    int n, int interval, int ClientWidth, int ClientHeight, int margin);

void find_physical_coords(Hierarchy*, int, int, double *x, double *y);
void find_new_physical_coords(Hierarchy*, int, int, double *x, double *y);
void find_old_physical_coords(Hierarchy*, int, int, double *x, double *y);
int find_active_ancestor(Hierarchy*, int, int, int*);
int locateByIndex(Hierarchy*, int, int*, int*);
int findGlobalIndexesOfActiveNeighbors(Hierarchy*, int, int**);

void freeGraph(vtx_data * graph);

#endif
