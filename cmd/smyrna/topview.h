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
#ifdef	WIN32			//this is needed on WIN32 to get libglade see the callback
#define _BB  __declspec(dllexport)
#else
#define _BB
#endif

void cleartopview(topview * t);
void preparetopview(Agraph_t * g, topview * t);
void drawTopViewGraph(Agraph_t * g);
int set_update_required(topview * t);
int move_TVnodes();
void local_zoom(topview * t);
void originate_distorded_coordinates(topview * t);

#endif
