/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *************************************************************************/


/*
 * Written by Stephen North
 * Updated by Emden Gansner
 */

/*
 * reads a sequence of graphs on stdin, and writes their
 * transitive reduction on stdout
 */

#include "config.h"

#include "cgraph.h"
#include "arith.h"
#include "timing.h"
#include <stdlib.h>

#define NEW(t)           (t*)malloc(sizeof(t))
#define N_NEW(n,t)       (t*)malloc((n)*sizeof(t))

typedef struct {
    unsigned char on_stack;
    unsigned char dist;
} nodeinfo_t;

#define ON_STACK(ninfo,n) (ninfo[AGSEQ(n)].on_stack) 
#define DIST(ninfo,n) (ninfo[AGSEQ(n)].dist) 
#define agrootof(n) ((n)->root)

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "ingraphs.h"

#include <getopt.h>

static char **Files;
static char *CmdName;
static int Verbose;

typedef struct blk_t {
    Agedge_t **data;
    Agedge_t **endp;
    struct blk_t *prev;
    struct blk_t *next;
} blk_t;

typedef struct {
    blk_t *fstblk;
    blk_t *curblk;
    Agedge_t **curp;
} stk_t;

#define SMALLBUF 1024
#define BIGBUF 1000000

typedef struct {
    Agedge_t *base[SMALLBUF];
    blk_t Blk;
    stk_t Stk;
} estack_t;


static void initStk(estack_t* sp)
{
    sp->Blk.data = sp->base;
    sp->Blk.endp = sp->Blk.data + SMALLBUF;
    sp->Stk.curblk = sp->Stk.fstblk = &(sp->Blk);
    sp->Stk.curp = sp->Stk.curblk->data;
}

static void push(estack_t* sp, Agedge_t * ep, nodeinfo_t* ninfo)
{
    if (sp->Stk.curp == sp->Stk.curblk->endp) {
	if (sp->Stk.curblk->next == NULL) {
	    blk_t *bp = NEW(blk_t);
	    if (bp == 0) {
		fprintf(stderr, "%s: Out of memory\n", CmdName);
		exit(1);
	    }
	    bp->prev = sp->Stk.curblk;
	    bp->next = NULL;
	    bp->data = N_NEW(BIGBUF, Agedge_t *);
	    if (bp->data == 0) {
		fprintf(stderr, "%s: Out of memory\n", CmdName);
		exit(1);
	    }
	    bp->endp = bp->data + BIGBUF;
	    sp->Stk.curblk->next = bp;
	}
	sp->Stk.curblk = sp->Stk.curblk->next;
	sp->Stk.curp = sp->Stk.curblk->data;
    }
    ON_STACK(ninfo, aghead(ep)) = 1;
    *sp->Stk.curp++ = ep;
}

static Agedge_t *pop(estack_t* sp, nodeinfo_t* ninfo)
{
    Agedge_t* e;
    if (sp->Stk.curp == sp->Stk.curblk->data) {
	if (sp->Stk.curblk == sp->Stk.fstblk)
	    return 0;
	sp->Stk.curblk = sp->Stk.curblk->prev;
	sp->Stk.curp = sp->Stk.curblk->endp;
    }
    sp->Stk.curp--;
    e = *sp->Stk.curp;
    ON_STACK(ninfo,aghead(e)) = 0;
    return e;
}

static Agedge_t *top(estack_t* sp)
{
    Agedge_t** pp;
    if (sp->Stk.curp == sp->Stk.curblk->data) {
	if (sp->Stk.curblk == sp->Stk.fstblk)
	    return 0;
	pp = sp->Stk.curblk->prev->endp-1;
    }
    else 
	pp = sp->Stk.curp-1;
    return *pp;
}

/* dfs:
 * Main function for transitive reduction.
 * This does a DFS starting at node n. Each node records the length of 
 * its largest simple path from n. We only care if the length is > 1. Node
 * n will have distance 0; outneighbors of n will have distance 1 or 2; all
 * others will have distance 2. 
 *
 * During the DFS, we only push edges on the stack whose head has distance 0 
 * (i.e., hasn't been visited yet), setting its distance to the distance of the
 * tail node plus one. If we find a head node with distance 1, we don't push the
 * edge, since it has already been in a DFS, but we update its distance. We also
 * check for back edges and report these.
 * 
 * After the DFS, we check all outedges of n. Those edges whose head has
 * distance 2 we delete. We also delete all but one copy of any edges with the
 * same head.
 */ 
static int dfs(Agnode_t * n, nodeinfo_t* ninfo, int warn, estack_t* sp)
{
    Agraph_t *g = agrootof(n);
    Agedgepair_t dummy;
    Agedge_t* link;
    Agedge_t* next;
    Agedge_t* prev;
    Agedge_t* e;
    Agedge_t* f;
    Agnode_t* v;
    Agnode_t* hd;
    Agnode_t* oldhd;

    dummy.out.base.tag.objtype = AGOUTEDGE;
    dummy.out.node = n;
    dummy.in.base.tag.objtype = AGINEDGE;
    dummy.in.node = NULL;

    push(sp, &dummy.out, ninfo);
    prev = 0;

    while ((link = top(sp))) {
	v = aghead(link);
	if (prev)
	    next = agnxtout(g, prev);
	else
	    next = agfstout(g, v);
	for (; next; next = agnxtout(g, next)) {
	    hd = aghead(next);
	    if (hd == v) continue; // Skip a loop
	    if (ON_STACK(ninfo,hd)) {
		if (!warn) {
		    warn++;
		    fprintf(stderr,
			"warning: %s has cycle(s), transitive reduction not unique\n",
			agnameof(g));
		    fprintf(stderr, "cycle involves edge %s -> %s\n",
			agnameof(v), agnameof(hd));
		}
	    }
	    else if (DIST(ninfo,hd) == 0) {
		DIST(ninfo,hd) = MIN(1,DIST(ninfo,v))+1;
	        break;
	    }
	    else if (DIST(ninfo,hd) == 1) {
		DIST(ninfo,hd) = MIN(1,DIST(ninfo,v))+1;
	    }
	}
	if (next) {
	    push(sp, next, ninfo);
            prev = 0;
	}
	else {
	    prev = pop(sp, ninfo);
	}
    }
    oldhd = NULL; 
    for (e = agfstout(g, n); e; e = f) {
	f = agnxtout(g, e);
	hd = aghead(e);
        if (oldhd == hd)
	    agdelete(g, e);
        else {
            oldhd = hd;
	    if (DIST(ninfo, hd)>1) agdelete(g, e);
	}
    }
    return warn;
}

static char *useString = "Usage: %s [-v?] <files>\n\
  -v - verbose\n\
  -? - print usage\n\
If no files are specified, stdin is used\n";

static void usage(int v)
{
    printf(useString, CmdName);
    exit(v);
}

static void init(int argc, char *argv[])
{
    int c;

    CmdName = argv[0];
    opterr = 0;
    while ((c = getopt(argc, argv, "v")) != -1) {
	switch (c) {
	case 'v':
	    Verbose = 1;
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr, "%s: option -%c unrecognized - ignored\n",
			CmdName, optopt);
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    if (argc)
	Files = argv;
}

/* process:
 * Do a DFS for each vertex in graph g, so the time
 * complexity is O(|V||E|).
 */
static void process(Agraph_t * g, estack_t* sp)
{
    Agnode_t *n;
    int cnt = 0;
    int warn = 0;
    double secs;
    double total_secs = 0;
    nodeinfo_t* ninfo;
    size_t infosize;

    infosize = (agnnodes(g)+1)*sizeof(nodeinfo_t);
    ninfo = (nodeinfo_t*)malloc(infosize);

    if (Verbose)
	fprintf(stderr, "Processing graph %s\n", agnameof(g));
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	memset(ninfo, 0, infosize);
	if (Verbose) start_timer();
	warn = dfs(n, ninfo, warn, sp);
	if (Verbose) {
	    secs = elapsed_sec();
            total_secs += secs;
	    cnt++;
	    if ((cnt%1000) == 0) fprintf (stderr, "[%d]\n", cnt);
	}
    }
    if (Verbose)
	fprintf(stderr, "Finished graph %s: %.02f secs.\n", agnameof(g), total_secs);
    free (ninfo);
    agwrite(g, stdout);
    fflush(stdout);
}

static Agraph_t *gread(FILE * fp)
{
    return agread(fp, (Agdisc_t *) 0);
}

int main(int argc, char **argv)
{
    Agraph_t *g;
    ingraph_state ig;
    estack_t estk;

    init(argc, argv);
    newIngraph(&ig, Files, gread);
    initStk(&estk);

    while ((g = nextGraph(&ig)) != 0) {
	if (agisdirected(g))
	    process(g, &estk);
	agclose(g);
    }

    return 0;
}

