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
#include "viewport.h"
#include <gtk/gtkgl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pango/pangoft2.h>

//#include "xdot.h"

#define ANGLE   30.0
/* tan (ANGLE * PI / 180.0) */
#define TANGENT 0.57735

#define TEXT_Z_NEAR  2.0
#define TEXT_Z_FAR  0.0
#define TEXT_Z_DIFF  0.005

static GLfloat text_z = TEXT_Z_NEAR;
static const char *text = "ABCD adasd";
static PangoContext *ft2_context=NULL;

//DRAWING FUNCTIONS
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
void draw_selection_box();
void set_options(xdot_op*,int param);
void relocate_spline(xdot_op*,int param);
void draw_letter(GLfloat x,GLfloat y,char c);
int font_display_list();
void draw_cached_letter(GLfloat x,GLfloat y,int letter);
void draw_magnifier();
void draw_circle(float originX,float originY,float radius);
void draw_fisheye_magnifier();
//int point_within_ellipse(float ex,float ey,float ea,float eb,float px,float py);
#endif

