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
#include <pack.h>

int main (int argc, char* argv[])
{
    graph_t *g;
    graph_t *sg;
    FILE *fp;
    graph_t** cc;
    int       i, ncc;
    GVC_t *gvc;

    gvc = gvContext();

    if (argc > 1)
        fp = fopen(argv[1], "r");
    else
        fp = stdin;
    g = agread(fp);

    cc = ccomps(g, &ncc, (char*)0);

    for (i = 0; i < ncc; i++) {
        sg = cc[i];
        nodeInduce (sg);
        gvLayout(gvc, sg, "neato");
    }
    pack_graph (ncc, cc, g, 0);

    gvRender(gvc, g, "ps", stdout);

    for (i = 0; i < ncc; i++) {
        sg = cc[i];
        gvFreeLayout(gvc, sg);
        agdelete(g, sg);
    }

    agclose(g);

    return (gvFreeContext(gvc));

}
