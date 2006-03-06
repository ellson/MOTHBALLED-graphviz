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

/*
    A gvgraph (GVG_t*) is a single graph within a gvcontext (GVC_t *) 
    which can support multiple graphs.

    Each gvgraph (GVG_t *) may have multiple gvjobs (GVJ_t *) which correspond
    to the renderings of that graph.
 */

#include "render.h"
#include "gvcint.h"

static FILE *gvNextInputFile(GVC_t *gvc)
{
    static int ctr = 0;
    FILE *rv = NULL;

    if (Files[0] == NULL) {
        if (ctr++ == 0)
            rv = stdin;
    } else {
        rv = NULL;
        while (Files[ctr]) {
            if ((rv = fopen(Files[ctr++], "r")))
                break;
            else {
                agerr(AGERR, "%s: can't open %s\n", CmdName,
                      Files[ctr - 1]);
                graphviz_errors++;
            }
        }
    }
    if (rv) {
        agsetfile(Files[0] ? Files[ctr - 1] : "<stdin>");
	if (Files[0])
	    gvc->gvg->inputFilename = Files[ctr - 1];
	else
	    gvc->gvg->inputFilename = NULL;
    }
    return rv;
}

graph_t *gvNextInputGraph(GVC_t *gvc)
{
    graph_t *g;
    static FILE *fp;

    if (fp == NULL)
        fp = gvNextInputFile(gvc);
    g = NULL;

    while (fp != NULL) {
        if ((g = agread(fp)))
            break;
        fp = gvNextInputFile(gvc);
    }
    return g;
}

