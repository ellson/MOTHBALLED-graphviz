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

#ifndef INGRAPHS_H
#define INGRAPHS_H

/* The ingraphs library works with both libagraph and with 
 * libgraph, with all user-supplied data. For this to work,
 * the include file relies upon its context to supply a
 * definition of Agraph_t.
 */

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef Agraph_t *(*opengfn) (FILE *);

    typedef struct {
	void *(*openf) (char *);
	Agraph_t *(*readf) (void *);
	int (*closef) (void *);
	void *dflt;
    } ingdisc;

    typedef struct {
	char **Files;
	int ctr;
	void *fp;
	ingdisc *fns;
	char heap;
    } ingraph_state;

    extern ingraph_state *newIngraph(ingraph_state *, char **, opengfn);
    extern ingraph_state *newIng(ingraph_state *, char **, ingdisc *);
    extern void closeIngraph(ingraph_state * sp);
    extern Agraph_t *nextGraph(ingraph_state *);
    extern char *fileName(ingraph_state *);

#ifdef __cplusplus
}
#endif
#endif
