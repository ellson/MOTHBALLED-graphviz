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

typedef struct {
    int mark;
} Agnodeinfo_t;
typedef char Agedgeinfo_t;
typedef char Agraphinfo_t;

#include <graph.h>
#ifdef HAVE_UNISTD_H
#include	<unistd.h>
#endif
#include <ingraphs.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

char **Files;
char *CmdName;

#define MARK(n)     ((n)->u.mark)

static int dfs(Agnode_t * n, Agedge_t * link, int warn)
{
    Agedge_t *e;
    Agedge_t *f;
    Agraph_t *g = n->graph;

    MARK(n) = 1;

    for (e = agfstin(g, n); e; e = f) {
	f = agnxtin(g, e);
	if (e == link)
	    continue;
	if (MARK(e->tail))
	    agdelete(g, e);
    }

    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	if (MARK(e->head)) {
	    if (!warn) {
		warn++;
		fprintf(stderr,
			"warning: %s has cycle(s), transitive reduction not unique\n",
			g->name);
		fprintf(stderr, "cycle involves edge %s -> %s",
			e->tail->name, e->head->name);
	    }
	} else
	    warn = dfs(e->head, e, warn);
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

    aginit();
    CmdName = argv[0];
    while ((c = getopt(argc, argv, ":")) != -1) {
	switch (c) {
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr, "%s: option -%c unrecognized - ignored\n",
			CmdName, c);
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

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	warn = dfs(n, 0, warn);
    }
    agwrite(g, stdout);
    fflush(stdout);
}

int main(int argc, char **argv)
{
    Agraph_t *g;
    ingraph_state ig;

    init(argc, argv);
    newIngraph(&ig, Files, agread);

    while ((g = nextGraph(&ig)) != 0) {
	if (AG_IS_DIRECTED(g))
	    process(g);
	agclose(g);
    }

    return 0;
}
