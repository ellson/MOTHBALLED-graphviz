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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FDPDBG_H
#define FDPDBG_H

#ifdef DEBUG

#include <fdp.h>
#include <stdio.h>
#include <graph.h>

    extern double Scale;
    extern void outputGraph(Agraph_t *, FILE *, int);

    extern void incInd(void);
    extern void decInd(void);
    extern void prIndent(void);

    extern void dump(graph_t * g, int doAll, int doBB);
    extern void dumpE(graph_t * g, int derived);
    extern void dumpG(graph_t * g, char *fname, int);

#endif

#endif

#ifdef __cplusplus
}
#endif
