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

#ifndef DIGCOLA_H
#define DIGCOLA_H

#include <defs.h>
#ifdef DIGCOLA
extern void compute_y_coords(vtx_data*, int, double*, int);
extern double compute_hierarchy(vtx_data*, int, double, double, 
                                double*, int**, int**, int*); 
extern void IMDS_given_dim(vtx_data*, int, double*, double*, double);
extern int stress_majorization_with_hierarchy(vtx_data*, int, int, double**, 
                                              int, int, int, int, double);
typedef struct ipsep_options {
    int diredges;       /* 1=generate directed edge constraints */
                        /* 2=generate directed hierarchy level constraints (DiG-CoLa) */
    double edge_gap;    /* amount to force vertical separation of */
                        /* start/end nodes */
    int noverlap;       /* 1=generate non-overlap constraints */
                        /* 2=remove overlaps after layout */
    pointf gap;         /* hor and vert gap to enforce when removing overlap*/
    pointf* nsize;      /* node widths and heights */
    cluster_data* clusters;
                        /* list of node indices for each cluster */
#ifdef MOSEK
    int mosek;          /* use Mosek as constraint optimization engine */
#endif //MOSEK
} ipsep_options;

// stress majorization, for Constraint Layout
extern int stress_majorization_cola(vtx_data*, int, int, double**, int, int, int, ipsep_options*);
#endif
#endif

#ifdef __cplusplus
}
#endif
