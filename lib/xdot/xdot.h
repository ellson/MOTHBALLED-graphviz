/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2007 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#ifndef XDOT_H
#define XDOT_H
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "cgraph.h"

typedef enum {
    xd_left, xd_center, xd_right
} xdot_align;

typedef struct {
    int x, y,z;		//z is constant 
} xdot_point;

typedef struct {
    int x, y, w, h;
} xdot_rect;

typedef struct {
    int cnt;
    xdot_point* pts;
} xdot_polyline;

typedef struct {
  int x, y;
  xdot_align align;
  int width;
  char* text;
} xdot_text;

typedef struct {
    xdot_rect pos;
    char* name;
} xdot_image;

typedef struct {
    float size;
    char* name;
} xdot_font;

typedef enum {
    xd_filled_ellipse, xd_unfilled_ellipse,
    xd_filled_polygon, xd_unfilled_polygon,
    xd_filled_bezier,  xd_unfilled_bezier,
    xd_polyline,       xd_text,
    xd_fill_color,     xd_pen_color, xd_font, xd_style, xd_image
} xdot_kind; 
    
typedef enum {
    xop_ellipse,
    xop_polygon,
    xop_bezier,
    xop_polyline,       xop_text,
    xop_fill_color,     xop_pen_color, xop_font, xop_style, xop_image
} xop_kind; 
    
typedef struct _xdot_op xdot_op;
typedef void (*drawfunc_t)(xdot_op*, int);

struct _xdot_op {
    xdot_kind kind;
    union {
      xdot_rect ellipse;       /* xd_filled_ellipse, xd_unfilled_ellipse */
      xdot_polyline polygon;   /* xd_filled_polygon, xd_unfilled_polygon */
      xdot_polyline polyline;  /* xd_polyline */
      xdot_polyline bezier;    /* xd_filled_bezier,  xd_unfilled_bezier */
      xdot_text text;          /* xd_text */
      xdot_image image;        /* xd_image */
      char* color;             /* xd_fill_color, xd_pen_color */
      xdot_font font;          /* xd_font */
      char* style;             /* xd_style */
    } u;
    drawfunc_t drawfunc;
};

typedef struct {
    int cnt;
    xdot_op* ops;
} xdot;

/* ops are indexed by xop_kind */
extern xdot* parseXDotF (char*, drawfunc_t ops[], int sz);
extern xdot* parseXDot (char*);
extern char* sprintXDot (xdot*);
extern void fprintXDot (FILE*, xdot*);
extern void freeXDot (xdot*);
#endif
