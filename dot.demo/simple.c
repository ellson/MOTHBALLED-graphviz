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

#include <gvc.h>

int main(int argc, char **argv)
{
    GVC_t *gvc;
    graph_t *g;
    FILE *fp;

    gvc = gvContext();

    if (argc > 1)
	fp = fopen(argv[1], "r");
    else
	fp = stdin;
    g = agread(fp);

    gvLayout(gvc, g, "dot");

    gvRender(gvc, g, "plain", stdout);

    gvCleanup(gvc);

    agclose(g);
    
    return 0;
}
