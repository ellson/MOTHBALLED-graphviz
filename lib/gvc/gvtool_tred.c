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


/*
 * Written by Stephen North
 * Updated by Emden Gansner
 * Adapted to gvToolTred(g) by John Ellson
 */

/*
 * performs an inplace transitive reduction on a graph
 */

#include "config.h"
#include <stdio.h>
#include "cgraph.h"
#include "gvc.h"

#define agrootof(n) ((n)->root)

static int dfs(Agnode_t * n, Agedge_t * link, int warn)
{
    Agedge_t *e;
    Agedge_t *f;
    Agraph_t *g = agrootof(n);

    ND_mark(n) = 1;

    for (e = agfstin(g, n); e; e = f) {
	f = agnxtin(g, e);
	if (e == link)
	    continue;
	if (ND_mark(agtail(e)))
	    agdelete(g, e);
    }

    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	if (ND_mark(aghead(e))) {
	    if (!warn) {
		warn++;
		fprintf(stderr,
			"warning: %s has cycle(s), transitive reduction not unique\n",
			agnameof(g));
		fprintf(stderr, "cycle involves edge %s -> %s\n",
			agnameof(agtail(e)), agnameof(aghead(e)));
	    }
	} else
	    warn = dfs(aghead(e), AGOUT2IN(e), warn);
    }

    ND_mark(n) = 0;
    return warn;
}

void gvToolTred(Agraph_t * g)
{
    Agnode_t *n;
    int warn = 0;

    if (agisdirected(g)) {
        for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    warn = dfs(n, NULL, warn);
        }
    } else {
	fprintf(stderr, "warning: %s is not a directed graph, not attempting tred\n",
			agnameof(g));
    }
}
