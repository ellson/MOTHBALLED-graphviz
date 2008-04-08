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

#ifndef SELECTION_H
#define SELECTION_H

#include "draw.h"
#include <GL/gl.h>

#define	SELECTION_SEGMENT_DIVIDER	5.0	//control points count to check if a line segment is withing clipping rect
#define SINGLE_SELECTION_WIDTH	10	//width of the rect clip for single selections , higher values more catches less sensitivity

extern int lineintersects(float X1, float X2, float Y1, float Y2);
extern int point_within_ellips_with_coords(float ex, float ey, float ea,
					   float eb, float px, float py);
extern int is_point_in_rectangle(float X, float Y, float RX, float RY,
				 float RW, float RH);

extern int SelectBeziers(sdot_op * op);
extern int SelectEllipse(sdot_op * op);
extern int SelectPolygon(sdot_op * op);
extern int SelectPolyline(sdot_op * op);
extern int SelectText(sdot_op * op);
extern int SelectImage(sdot_op * op);

extern int select_node(Agraph_t * g, Agnode_t *);
extern int select_edge(Agraph_t * g, Agedge_t *);
extern int select_object(Agraph_t * g, void *obj);

extern int deselect_object(Agraph_t * g, void *obj);
extern int deselect_node(Agraph_t * g, Agnode_t *);
extern int deselect_edge(Agraph_t * g, Agedge_t *);

extern int select_all_nodes(Agraph_t * g);
extern int select_all_edges(Agraph_t * g);
extern int select_all_graphs(Agraph_t * g);
extern int select_all(Agraph_t * g);

extern int deselect_all_nodes(Agraph_t * g);
extern int deselect_all_edges(Agraph_t * g);
extern int deselect_all_graphs(Agraph_t * g);
extern int deselect_all(Agraph_t * g);
int point_within_sphere_with_coords(float x0, float y0, float z0, float r,float x, float y,float z);

#endif
