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

/* Note that, with the call to gvParseArgs(), this application assumes that
 * a known layout algorithm is going to be specified. This can be done either
 * using argv[0] or requiring the user to run this code with a -K flag specifying
 * which layout to use. In the former case, after this program has been built as
 * 'demo', you will need to rename it as one of the installed layout engines such
 * as dot, neato, sfdp, etc. 
 */
#include <gvc.h>

int main(int argc, char **argv)
{
    Agraph_t *g;
    Agnode_t *n, *m;
    Agedge_t *e;
    GVC_t *gvc;

    /* set up a graphviz context */
    gvc = gvContext();

    /* parse command line args - minimally argv[0] sets layout engine */
    gvParseArgs(gvc, argc, argv);

    /* Create a simple digraph */
    g = agopen("g", AGDIGRAPH);
    n = agnode(g, "n");
    m = agnode(g, "m");
    e = agedge(g, n, m);

    /* Set an attribute - in this case one that affects the visible rendering */
    agsafeset(n, "color", "red", "");

    /* Compute a layout using layout engine from command line args */
    gvLayoutJobs(gvc, g);

    /* Write the graph according to -T and -o options */
    gvRenderJobs(gvc, g);

    /* Free layout data */
    gvFreeLayout(gvc, g);

    /* Free graph structures */
    agclose(g);

    /* close output file, free context, and return number of errors */
    return (gvFreeContext(gvc));
}
