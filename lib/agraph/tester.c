/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/


#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include "agraph.h"

#define NILgraph			NIL(Agraph_t*)
#define NILnode				NIL(Agnode_t*)
#define NILedge				NIL(Agedge_t*)
#define NILsym				NIL(Agsym_t*)
#define NILstr				NIL(char*)

main()
{
    Agraph_t *g;
    Agnode_t *n;
    Agedge_t *e;
    Agsym_t *sym;
    char *val;

    while (g = agread(stdin, NIL(Agdisc_t *))) {
	for (n = agfstnode(g); n; n = agnxtnode(n)) {
	    /*fprintf(stderr,"%s\n", agnameof(n)); */
	    for (sym = agnxtattr(g, AGNODE, 0); sym;
		 sym = agnxtattr(g, AGNODE, sym)) {
		val = agxget(n, sym);
		/*fprintf(stderr,"\t%s=%s\n",sym->name,val); */
	    }
	}
	agwrite(g, stdout);
    }
}
