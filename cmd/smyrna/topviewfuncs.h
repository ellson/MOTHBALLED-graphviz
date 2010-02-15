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

#ifndef TOPVIEWFUNCS_H
#define TOPVIEWFUNCS_H

#include "smyrnadefs.h"

#ifdef __cplusplus
extern "C" {
#endif
extern void pick_object_xyz(Agraph_t* g,topview* t,GLfloat x,GLfloat y,GLfloat z) ;
extern topview* initSmGraph(Agraph_t * g);
extern void renderSmGraph(Agraph_t * g,topview* t);
extern void freeSmGraph(Agraph_t * g,topview* t);
extern void cacheSelectedEdges(Agraph_t * g,topview* t);
extern void cacheSelectedNodes(Agraph_t * g,topview* t);

#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
