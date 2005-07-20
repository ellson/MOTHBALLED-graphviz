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
    graph_t *g, *prev = NULL;
    GVC_t *gvc;

    gvc = gvContext();
    parse_args(gvc, argc, argv);

    while ((g = next_input_graph())) {
	if (prev) {
	    gvlayout_cleanup(gvc, prev);
	    agclose(prev);
	}
	gvlayout_layout(gvc, g);
	emit_jobs(gvc, g);
	prev = g;
    }
    dotneato_terminate(gvc);
    return 0;
}
