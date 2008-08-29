/* $Id$Revision: */
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

#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

typedef void (*edgefn)(int, int);

extern void makeCircle(int , edgefn);
extern void makeComplete(int , edgefn);
extern void makePath(int , edgefn);
extern void makeStar(int , edgefn);
extern void makeWheel (int, edgefn);
extern void makeTorus(int , int , edgefn);
extern void makeCylinder(int , int , edgefn);
extern void makeSquareGrid(int , int , int, int, edgefn);
extern void makeBinaryTree(int , edgefn);
extern void makeSierpinski(int , edgefn);
extern void makeHypercube(int , edgefn);

#endif
