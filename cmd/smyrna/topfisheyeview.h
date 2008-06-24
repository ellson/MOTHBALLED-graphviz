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
#ifndef TOPFISHEYEVIEW_H
#define TOPFISHEYEVIEW_H

#include "smyrnadefs.h"
#include "hier.h"

void fisheye_polar(double x_focus, double y_focus, topview * t);
void fisheye_spherical(double x_focus, double y_focus, double z_focus,
		       topview * t);
void prepare_topological_fisheye(topview*);
void drawtopologicalfisheye(topview * t);
void drawtopologicalfisheye2(topview * t);
void changetopfishfocus(topview * t, float *x, float *y,
				   float *z, int num_foci);
void refresh_old_values(topview* t);
int get_temp_coords(topview* t,int level,int v,double* coord_x,double* coord_y,float *R,float *G,float *B);
void get_interpolated_coords(double x0,double y0,double x1,double y1,int fr,int total_fr, double* x,double* y);
int get_active_frame(topview* t);
#endif
