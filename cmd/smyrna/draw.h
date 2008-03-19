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

typedef struct {
    xdot_op op;
    void *obj;
    int size;
} sdot_op;

/* DRAWING FUNCTIONS 
 * these are opengl based xdot drawing functions 
 * topview drawings are not here
 */
extern drawfunc_t OpFns[];
extern void drawGraph(Agraph_t * g);
void draw_selection_box(ViewInfo * view);
void draw_magnifier(ViewInfo * view);
void draw_fisheye_magnifier(ViewInfo * view);
extern int randomize_color(RGBColor * c, int brightness);
extern void drawCircle(float x, float y, float radius, float zdepth);
extern RGBColor GetRGBColor(char *color);
extern void drawBorders(ViewInfo * view);

#endif
