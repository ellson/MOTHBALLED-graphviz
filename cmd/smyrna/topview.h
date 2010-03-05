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

#ifndef TOPVIEW_H
#define TOPVIEW_H

#include "smyrnadefs.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
    extern void cleartopview(topview * t);
    extern void preparetopview(Agraph_t * g, topview * t);
    extern void update_topview(Agraph_t * g, topview * t, int init);
    extern void drawTopViewGraph(Agraph_t * g);
    extern int move_TVnodes(void);
    extern void local_zoom(topview * t);
    extern void originate_distorded_coordinates(topview * t);
    extern float calcfontsize(float totaledgelength, int totallabelsize,
		       int edgecount, int totalnodecount);
    extern void select_with_regex(char *exp);
    extern void settvcolorinfo(Agraph_t * g, topview * t);
    extern void setMultiedges(Agraph_t * g, char *attrname);
#endif
#ifdef __cplusplus
}				/* end extern "C" */
#endif
#endif
