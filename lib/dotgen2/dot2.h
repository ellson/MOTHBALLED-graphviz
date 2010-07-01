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


#ifndef         DOT2_H
#define         DOT2_H

#include        "render.h"
#include        "dot2procs.h"

/* tags for representative (model) objects */
/* 0 is reserved for undefined */
#define STDNODE         1       /* a real node on only one rank */
#define TALLNODE        2       /* a real node on two or more ranks */
#define EXTNODE         3       /* a node external to a cluster */
#define SKELETON        4
#define PATH            5
#define INTNODE         6       /*internal temporary nodes to fill rank gaps*/
#define LBLNODE         7       /*internal temp nodes for edge labels*/

#endif                          /* DOT_H */
