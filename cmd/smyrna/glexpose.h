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

#ifndef GLEXPOSE_H
#define GLEXPOSE_H

#include "viewport.h"
int glupdatecamera(ViewInfo * v);
int glexpose_main(ViewInfo * v);
void glexpose_grid(ViewInfo * v);
int glexpose_drawgraph(ViewInfo * view);
void drawRotatingTools();

#endif
