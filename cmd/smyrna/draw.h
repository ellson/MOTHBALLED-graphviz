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

#ifndef DRAW_H
#define DRAW_H
#include "smyrnadefs.h"
#include <gtk/gtkgl.h>
#include "xdot.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define Z_MIDDLE_PLANE  -0.00200
#define Z_BACK_PLANE	-0.00201
#define Z_FORWARD_PLANE -0.00199

/*DRAWING FUNCTIONS 
these are opengl based xdot drawing functions 
topview drawings are not here*/
void DrawBezier(GLfloat* xp,GLfloat* yp,GLfloat* zp, int filled, int param);
void DrawBeziers(xdot_op* op,int param);
void DrawEllipse(xdot_op* op,int param);
void DrawPolygon(xdot_op* op,int param);
void DrawPolyline(xdot_op* op,int param);
void DrawBitmap(GLfloat bmpX,GLfloat bmpY,GLfloat bmpW,GLfloat bmpH);
void SetFillColor(xdot_op* op,int param);
void SetPenColor(xdot_op* op,int param);
void SetStyle(xdot_op* op,int param);
void SetFont(xdot_op* op,int param);
void EmbedText(xdot_op* op,int param);
void InsertImage(xdot_op* op,int param);
void load_raw_texture ( char *file_name, int width, int height, int depth, GLenum colour_type, GLenum filter_type ); //function to load .raw files
int load_bitmap(char *filename);
void drawGraph(Agraph_t *g);
void draw_selection_box(ViewInfo* view);
void set_options(xdot_op*,int param);
void relocate_spline(xdot_op*,int param);
void draw_magnifier(ViewInfo* view);
void draw_circle(float originX,float originY,float radius);
void draw_fisheye_magnifier(ViewInfo* view);
int randomize_color(RGBColor* c,int brightness);
void drawCircle(float x,float y,float radius,float zdepth);
RGBColor GetRGBColor(char* color);
void drawBorders(ViewInfo* view);

#endif

