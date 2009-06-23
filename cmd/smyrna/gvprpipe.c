/* $Id$Revision: */
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

#include "gvprpipe.h"
#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <glade/glade.h>
#include <viewport.h> 

extern GladeXML* xml;	

#include <gvpr.h>
#include "topview.h"
#include "topviewsettings.h"

static void
refreshViewport ()
{
    update_graph_from_settings(view->g[view->activeGraph]);
    set_viewport_settings_from_template(view, view->g[view->activeGraph]);
    settvcolorinfo(view->g[view->activeGraph],view->Topview);
    glexpose ();
}

static ssize_t outfn (void* sp, const char *buf, size_t nbyte, void* dp)
{
    return write (1, buf, nbyte);
}

static ssize_t errfn (void* sp, const char *buf, size_t nbyte, void* dp)
{
    return write (2, buf, nbyte);
}

int run_gvpr (Agraph_t* srcGraph, int argc, char* argv[])
{
    int i, rv = 1;
    gvpropts opts;
    Agraph_t* gs[2];

    gs[0] = srcGraph;
    gs[1] = 0;
    opts.ingraphs = gs;
    opts.out = outfn;
    opts.err = errfn;
    opts.flags = GV_USE_OUTGRAPH;
    
    rv = gvpr (argc, argv, &opts);

    if (rv) {  /* error */
	fprintf (stderr, "Error in gvpr\n");
    }
    else if (opts.n_outgraphs) {
	add_graph_to_viewport(opts.outgraphs[0]);
	for (i = 1; i < opts.n_outgraphs; i++)
	    agclose (opts.outgraphs[i]);
    }
    else {
	refreshViewport ();
    }
    return rv;
}
