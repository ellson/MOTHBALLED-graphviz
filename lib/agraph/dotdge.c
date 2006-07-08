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


#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include "agraph.h"

#define NILgraph			NIL(Agraph_t*)
#define NILnode				NIL(Agnode_t*)
#define NILedge				NIL(Agedge_t*)
#define NILsym				NIL(Agsym_t*)
#define NILstr				NIL(char*)

void main(int argc, char **argv)
{
    Agraph_t *g;
    Agnode_t *n;
    Agedge_t *e;

    while (g = agread(stdin, NIL(Agdisc_t *))) {
	fprintf(stderr, "nodes %d edges %d\n", agnnodes(g), agnedges(g));
	for (n = agfstnode(g); n; n = agnxtnode(n)) {
	    if (agdegree(n, TRUE, TRUE) == 0)
		printf("u . \"%s\" {}\n", agnameof(n));
	    else {
		for (e = agfstout(n); e; e = agnxtout(e)) {
		    printf("u .> \"%s\" {} e%x {} ", agnameof(agtail(e)),
			   e);
		    printf("\"%s\" {}\n", agnameof(aghead(e)));
		}
	    }
	}
    }
}
