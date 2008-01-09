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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#ifdef HAVE_UNISTD_H
#include	<unistd.h>
#endif
#ifdef USE_CGRAPH
#include    <cgraph.h>
#define degreeOf(n,I,O) (agdegree(n->root, n, I, O)) 
#else
#include    <agraph.h>
#define degreeOf(n,I,O) (agdegree(n, I, O)) 
#endif
#include    <ingraphs.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

static int Do_fans = 0;
static int MaxMinlen = 0;
static int ChainLimit = 0;
static int ChainSize = 0;
static Agnode_t *ChainNode;
static FILE *outFile;
static char *cmd;

static int isleaf(Agnode_t * n)
{
    return (degreeOf(n, TRUE, TRUE) == 1);
}

static int ischainnode(Agnode_t * n)
{
    return ((degreeOf(n, TRUE, FALSE) == 1)
	    && degreeOf(n, FALSE, TRUE) == 1);
}

static void adjustlen(Agedge_t * e, Agsym_t * sym, int newlen)
{
    char buf[10];

    sprintf(buf, "%d", newlen);
    agxset(e, sym, buf);
}

static Agsym_t *bindedgeattr(Agraph_t * g, char *str)
{
    return agattr(g, AGEDGE, str, "");
}

static void transform(Agraph_t * g)
{
    Agnode_t *n;
    Agedge_t *e;
    char *str;
    Agsym_t *m_ix, *s_ix;
    int cnt, d;

    m_ix = bindedgeattr(g, "minlen");
    s_ix = bindedgeattr(g, "style");

#ifdef USE_CGRAPH
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
#else
    for (n = agfstnode(g); n; n = agnxtnode(n)) {
#endif
	d = degreeOf(n, TRUE, TRUE);
	if (d == 0) {
	    if (ChainLimit < 1)
		continue;
	    if (ChainNode) {
#ifdef USE_CGRAPH
		e = agedge(g, ChainNode, n, "", TRUE);
#else
		e = agedge(ChainNode, n, "", TRUE);
#endif
		agxset(e, s_ix, "invis");
		ChainSize++;
		if (ChainSize < ChainLimit)
		    ChainNode = n;
		else {
		    ChainNode = NULL;
		    ChainSize = 0;
		}
	    } else
		ChainNode = n;
	} else if (d > 1) {
	    if (MaxMinlen < 1)
		continue;
	    cnt = 0;
#ifdef USE_CGRAPH
	    for (e = agfstin(g, n); e; e = agnxtin(g, e)) {
#else
	    for (e = agfstin(n); e; e = agnxtin(e)) {
#endif
		if (isleaf(agtail(e))) {
		    str = agxget(e, m_ix);
		    if (str[0] == 0) {
			adjustlen(e, m_ix, (cnt % MaxMinlen) + 1);
			cnt++;
		    }
		}
	    }

	    cnt = 0;
#ifdef USE_CGRAPH
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
#else
	    for (e = agfstout(n); e; e = agnxtout(e)) {
#endif
		if (isleaf(e->node) || (Do_fans && ischainnode(e->node))) {
		    str = agxget(e, m_ix);
		    if (str[0] == 0)
			adjustlen(e, m_ix, (cnt % MaxMinlen) + 1);
		    cnt++;
		}
	    }
	}
    }
}


static char *useString =
    "Usage: %s [-f?] [-l l] [-c l] [-o outfile] <files>\n\
  -o <file> - put output in <file>\n\
  -f        - adjust immediate fanout chains\n\
  -l <len>  - stagger length of leaf edges between [1,l]\n\
  -c <len>  - put disconnected nodes in chains of length l\n\
  -?        - print usage\n";

static void usage(int v)
{
    fprintf(stderr, useString, cmd);
    exit(v);
}

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
	exit(-1);
    }
    return (fp);
}

static char **scanargs(int argc, char **argv)
{
    int c, ival;

    cmd = argv[0];

    while ((c = getopt(argc, argv, ":fl:c:o:?")) != -1) {
	switch (c) {
	case 'f':
	    Do_fans = 1;
	    break;
	case 'l':
	    ival = atoi(optarg);
	    if (ival > 0)
		MaxMinlen = ival;
	    break;
	case 'c':
	    ival = atoi(optarg);
	    if (ival > 0)
		ChainLimit = ival;
	    break;
	case 'o':
	    outFile = openFile(optarg, "w");
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else {
		fprintf(stderr, "%s: option -%c unrecognized\n", cmd,
			optopt);
		usage(-1);
	    }
	    break;
	case ':':
	    fprintf(stderr, "%s: missing argument for option -%c\n",
		    cmd, optopt);
	    usage(-1);
	    break;
	}
    }
    if (Do_fans && (MaxMinlen < 1))
	fprintf(stderr, "%s: Warning: -f requires -l flag\n", cmd);
    argv += optind;
    argc -= optind;

    if (!outFile)
	outFile = stdout;
    if (argc)
	return argv;
    else
	return 0;
}

static Agraph_t *gread(FILE * fp)
{
    return agread(fp, (Agdisc_t *) 0);
}

int main(int argc, char **argv)
{
    Agraph_t *g;
    ingraph_state ig;
    char **files;

    files = scanargs(argc, argv);
    newIngraph(&ig, files, gread);
    while ((g = nextGraph(&ig))) {
	transform(g);
	agwrite(g, outFile);
    }
    return 0;
}
