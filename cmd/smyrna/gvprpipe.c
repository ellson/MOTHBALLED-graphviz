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
#include "const.h"
#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <glade/glade.h>
#include <gtk/gtk.h>
#include "draw.h"

#include <viewport.h> 
//#include <gltemplate.h> 

#include <gvpr.h>
extern GladeXML *xml;			//global libglade vars
static void append_textview(GtkTextView* textv, const char* s, size_t bytes)
{

    GtkTextIter endit;
    GtkTextBuffer * gtkbuf;
	/*get text view buffer*/
	gtkbuf = gtk_text_view_get_buffer(textv);
	/*set iterator to the end of the buffer*/
    gtk_text_buffer_get_end_iter (gtkbuf,&endit);
	/* insert buf to the end */
	gtk_text_buffer_insert(gtkbuf,&endit,s,bytes);

}
static ssize_t outfn (void* sp, const char *buf, size_t nbyte, void* dp)
{

	append_textview((GtkTextView*) glade_xml_get_widget(xml,"gvprtextoutput"),buf,nbyte);	
	append_textview((GtkTextView*) glade_xml_get_widget(xml,"mainconsole"),buf,nbyte);	
	return nbyte;
}

#ifdef UNUSED
static ssize_t errfn (void* sp, const char *buf, size_t nbyte, void* dp)
{
	return 0;
}
#endif

int run_gvpr (Agraph_t* srcGraph, int argc, char* argv[])
{
    int i, rv = 1;
    gvpropts opts;
    Agraph_t* gs[2];
    static int count;
    char buf[SMALLBUF];

    gs[0] = srcGraph;
    gs[1] = 0;
    opts.ingraphs = gs;
    opts.out = outfn;
    opts.err = outfn;
    opts.flags = GV_USE_OUTGRAPH;
   
    rv = gvpr (argc, argv, &opts);

    if (rv) {  /* error */
		fprintf (stderr, "Error in gvpr\n");
    }
    else if (opts.n_outgraphs) {
	refreshViewport (0);
	sprintf (buf, "<%d>", ++count);
	if (opts.outgraphs[0] != view->g[view->activeGraph])
	    add_graph_to_viewport(opts.outgraphs[0], buf);
	if (opts.n_outgraphs > 1)
	    fprintf (stderr, "Warning: multiple output graphs-discarded\n"); 
	for (i = 1; i < opts.n_outgraphs; i++) {
	    agclose (opts.outgraphs[i]);
	}
    }
    else {
	refreshViewport (0);
    }
    return rv;
}
