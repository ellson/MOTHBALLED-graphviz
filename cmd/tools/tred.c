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
 * Written by Stephen North
 * Updated by Emden Gansner
 */

/*
 * reads a sequence of graphs on stdin, and writes their
 * transitive reduction on stdout
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cgraph.h"
#include <stdlib.h>

typedef struct {
    Agrec_t h;
    int mark;
} Agnodeinfo_t;

#define agrootof(n) ((n)->root)

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "ingraphs.h"

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

char **Files;
char *CmdName;
#define MARK(n)  (((Agnodeinfo_t*)(n->base.data))->mark)

static int dfs(Agnode_t * n, Agedge_t * link, int warn)
{
    Agedge_t *e;
    Agedge_t *f;
    Agraph_t *g = agrootof(n);

    MARK(n) = 1;

    for (e = agfstin(g, n); e; e = f) {
	f = agnxtin(g, e);
	if (e == link)
	    continue;
	if (MARK(agtail(e)))
	    agdelete(g, e);
    }

    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	if (MARK(aghead(e))) {
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

    MARK(n) = 0;
    return warn;
}

static char *useString = "Usage: %s [-?] <files>\n\
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
    while ((c = getopt(argc, argv, ":?")) != -1) {
	switch (c) {
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

static void process(Agraph_t * g)
{
    Agnode_t *n;
    int warn = 0;

    aginit(g, AGNODE, "info", sizeof(Agnodeinfo_t), TRUE);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	warn = dfs(n, 0, warn);
    }
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

    init(argc, argv);
    newIngraph(&ig, Files, gread);

    while ((g = nextGraph(&ig)) != 0) {
	if (agisdirected(g))
	    process(g);
	agclose(g);
    }

    return 0;
}

