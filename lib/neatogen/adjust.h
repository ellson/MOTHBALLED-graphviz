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



#ifndef ADJUST_H
#define ADJUST_H

#ifdef __cplusplus
extern "C" {
#endif

    extern void adjustNodes(graph_t * G);
    extern void normalize(graph_t * g);
    extern void removeOverlap(graph_t * G);

#ifdef __cplusplus
}
#endif
#endif
