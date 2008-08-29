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

/*
 * Written by Emden Gansner
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <ctype.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif
#include "graph_generator.h"

typedef enum { unknown, grid, circle, complete, path, tree, torus, cylinder,
    sierpinski, hypercube, star, wheel
} GraphType;

typedef struct {
    int graphSize1;
    int graphSize2;
    int Verbose;
    int isPartial;
    int foldVal;
    int directed;
    FILE *outfile;
} opts_t;

static char *cmd;

static FILE *openFile(char *name, char *mode)
{
    FILE *fp;
    char *modestr;

    fp = fopen(name, mode);
    if (!fp) {
	if (*mode == 'r')
	    modestr = "reading";
	else
	    modestr = "writing";
	fprintf(stderr, "%s: could not open file %s for %s\n",
		cmd, name, modestr);
	exit(1);
    }
    return fp;
}

static char *Usage = "Usage: %s [-dV?] [options]\n\
 -c<n>         : cycle \n\
 -C<x,y>       : cylinder \n\
 -g[f]<h,w>    : grid (folded if f is used)\n\
 -G[f]<h,w>    : partial grid (folded if f is used)\n\
 -h<x>         : hypercube \n\
 -k<x>         : complete \n\
 -o<outfile>   : put output in <outfile> (stdout)\n\
 -p<x>         : path \n\
 -s<x>         : star\n\
 -S<x>         : sierpinski\n\
 -t<x>         : binary tree \n\
 -T<x,y>       : torus \n\
 -w<x>         : wheel\n\
 -d            : directed graph\n\
 -V            : verbose mode\n\
 -?            : print usage\n";

static void usage(int v)
{
    fprintf(v ? stderr : stdout, Usage, cmd);
    exit(v);
}

static void errexit(char opt)
{
    fprintf(stderr, "in flag -%c\n", opt);
    usage(1);
}

static int readPos(char *s, char **e, int min)
{
    int d;

    d = strtol(s, e, 10);
    if (s == *e) {
	fprintf(stderr, "ill-formed integer \"%s\" ", s);
	return -1;
    }
    if (d < min) {
	fprintf(stderr, "integer \"%s\" less than %d", s, min);
	return -1;
    }
    return d;
}

/* setOne:
 * Return non-zero on error.
 */
static int setOne(char *s, opts_t* opts)
{
    int d;
    char *next;

    d = readPos(s, &next, 1);
    if (d > 0) {
	opts->graphSize1 = d;
	return 0;
    }
    else return d;
}

/* setOne:
 * Return non-zero on error.
 */
static int setTwo(char *s, opts_t* opts)
{
    int d;
    char *next;

    d = readPos(s, &next, 1);
    if (d < 0)
	return d;
    opts->graphSize1 = d;

    if (*next != ',') {
	fprintf(stderr, "ill-formed int pair \"%s\" ", s);
	return -1;
    }

    s = next + 1;
    d = readPos(s, &next, 1);
    if (d > 1) {
	opts->graphSize2 = d;
	return 0;
    }
    else return d;
}

static char* setFold(char *s, opts_t* opts)
{
    char *next;

    if (*s == 'f') {
	next = s+1;
	opts->foldVal = 1;
    }
    else
	next = s;

    return next;
}

static char *optList = ":c:C:dg:G:h:k:o:p:s:S:t:T:Vw:?";

static GraphType init(int argc, char *argv[], opts_t* opts)
{
    int c;
    GraphType graphType = unknown;

    cmd = argv[0];
    while ((c = getopt(argc, argv, optList)) != -1) {
	switch (c) {
	case 'c':
	    graphType = circle;
	    if (setOne(optarg, opts))
		errexit(c);
	    break;
	case 'C':
	    graphType = cylinder;
	    if (setTwo(optarg, opts))
		errexit(c);
	    break;
	case 'd':
	    opts->directed = 1;
	    break;
	case 'G':
	    opts->isPartial = 1;
	case 'g':
	    graphType = grid;
	    optarg = setFold (optarg, opts);
	    if (setTwo(optarg, opts))
		errexit(c);
	    break;
	case 'h':
	    graphType = hypercube;
	    if (setOne(optarg, opts))
		errexit(c);
	    break;
	case 'k':
	    graphType = complete;
	    if (setOne(optarg, opts))
		errexit(c);
	    break;
	case 'o':
	    opts->outfile = openFile(optarg, "w");
	    break;
	case 'p':
	    graphType = path;
	    if (setOne(optarg, opts))
		errexit(c);
	    break;
	case 'S':
	    graphType = sierpinski;
	    if (setOne(optarg, opts))
		errexit(c);
	    break;
	case 's':
	    graphType = star;
	    if (setOne(optarg, opts))
		errexit(c);
	    break;
	case 't':
	    graphType = tree;
	    if (setOne(optarg, opts))
		errexit(c);
	    break;
	case 'T':
	    graphType = torus;
	    if (setTwo(optarg, opts))
		errexit(c);
	    break;
	case 'V':
	    opts->Verbose = 1;
	    break;
	case 'w':
	    graphType = wheel;
	    if (setOne(optarg, opts))
		errexit(c);
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr, "Unrecognized flag \"-%c\" - ignored\n",
			optopt);
	    break;
	}
    }

    argc -= optind;
    argv += optind;
    if (!opts->outfile)
	opts->outfile = stdout;
    if (graphType == unknown) {
	fprintf(stderr, "Graph type not set\n");
	usage(1);
    }

    return graphType;
}

static opts_t opts;

static void dirfn (int t, int h)
{
    if (h > 0)
	fprintf (opts.outfile, "  %d -> %d\n", t, h);
    else
	fprintf (opts.outfile, "  %d\n", t);
}

static void undirfn (int t, int h)
{
    if (h > 0)
	fprintf (opts.outfile, "  %d -- %d\n", t, h);
    else
	fprintf (opts.outfile, "  %d\n", t);
}

int main(int argc, char *argv[])
{
    GraphType graphType = init(argc, argv, &opts);
    edgefn ef;

    if (opts.directed) {
	fprintf(opts.outfile, "digraph {\n");
	ef = dirfn;
    }
    else {
	fprintf(opts.outfile, "graph {\n");
	ef = undirfn;
    }

    switch (graphType) {
    case grid:
	makeSquareGrid(opts.graphSize1, opts.graphSize2,
		       opts.foldVal, opts.isPartial, ef);
	break;
    case circle:
	makeCircle(opts.graphSize1, ef);
	break;
    case path:
	makePath(opts.graphSize1, ef);
	break;
    case tree:
	makeBinaryTree(opts.graphSize1, ef);
	break;
    case torus:
	makeTorus(opts.graphSize1, opts.graphSize2, ef);
	break;
    case cylinder:
	makeCylinder(opts.graphSize1, opts.graphSize2, ef);
	break;
    case sierpinski:
	makeSierpinski(opts.graphSize1, ef);
	break;
    case complete:
	makeComplete(opts.graphSize1, ef);
	break;
    case hypercube:
	makeHypercube(opts.graphSize1, ef);
	break;
    case star:
	makeStar(opts.graphSize1, ef);
	break;
    case wheel:
	makeWheel(opts.graphSize1, ef);
	break;
    default:
	/* can't happen */
	break;
    }
    fprintf(opts.outfile, "}\n");

    exit(0);
}
