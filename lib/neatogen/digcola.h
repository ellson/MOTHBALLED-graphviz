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
#endif
#endif

#ifdef __cplusplus
}
#endif
