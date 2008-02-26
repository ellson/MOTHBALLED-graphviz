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

#include "viewport.h"

#define	SELECTION_SEGMENT_DIVIDER	5.0	//control points count to check if a line segment is withing clipping rect
#define SINGLE_SELECTION_WIDTH	10	//width of the rect clip for single selections , higher values more catches less sensitivity

typedef struct {
    double x, y;
} pointf;
extern int intersect (pointf a, pointf b, pointf c, pointf d, pointf* x);

int lineintersects(float X1,float X2,float Y1,float Y2);
int rectintersects(float x,float y,float W,float H);
int point_within_ellipse( xdot_op* op);
int point_within_ellips_with_coords(float ex,float ey,float ea,float eb,float px,float py);
int point_within_polygon(xdot_op* op);
int line_intersects (float* x,float* y,float* X,float* Y);

int is_point_in_rectangle(float X,float Y,float RX,float RY,float RW,float RH);
int within_bezier(GLfloat* xp,GLfloat* yp,GLfloat* zp,int isx);

int ellipse_in_rect(xdot_op* op);
int ellipse_x_rect(xdot_op* op);


int SelectBeziers(xdot_op* op);
int SelectEllipse(xdot_op* op);
int SelectPolygon(xdot_op* op);
int SelectPolyline(xdot_op* op);
int SelectText(xdot_op* op);
int SelectImage(xdot_op* op);



int spline_in_rect(xdot_op* op);
int spline_x_rect(xdot_op* op);

int polygon_int_rect(xdot_op* op);
int polygon_x_rect(xdot_op* op);

int polyline_in_rect(xdot_op* op);
int polyline_x_rect(xdot_op* op);

int text_in_rect(xdot_op* op);
int text_x_rect(xdot_op* op);

int image_in_rect(xdot_op* op);
int image_x_rect(xdot_op* op);


//select functions
int select_graph(Agraph_t* g,Agraph_t*);
int select_node(Agraph_t* g,Agnode_t*);
int select_edge(Agraph_t* g,Agedge_t*);

int select_object (Agraph_t* g,void* obj);
int deselect_object (Agraph_t* g,void* obj);
int deselect_graph(Agraph_t* g,Agraph_t*);
int deselect_node(Agraph_t* g,Agnode_t*);
int deselect_edge(Agraph_t* g,Agedge_t*);

int select_all_nodes(Agraph_t* g);
int select_all_edges(Agraph_t* g);
int select_all_graphs(Agraph_t* g);


int deselect_all_nodes(Agraph_t* g);
int deselect_all_edges(Agraph_t* g);
int deselect_all_gprahs(Agraph_t* g);

int select_all(Agraph_t* g);
int deselect_all(Agraph_t* g);


#endif
