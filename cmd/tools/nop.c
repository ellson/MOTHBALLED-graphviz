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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef USE_CGRAPH
#include <cgraph.h>
#else
typedef char Agnodeinfo_t;
typedef char Agedgeinfo_t;
typedef char Agraphinfo_t;
#include <graph.h>
#endif
#include <ingraphs.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include	<unistd.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

char **Files;
int chkOnly;

static char *useString = "Usage: nop [-p?] <files>\n\
  -p - check for valid DOT\n\
  -? - print usage\n\
If no files are specified, stdin is used\n";

static void usage(int v)
{
    printf(useString);
    exit(v);
}

static void init(int argc, char *argv[])
{
    int c;

    while ((c = getopt(argc, argv, ":p?")) != -1) {
	switch (c) {
	case 'p':
	    chkOnly = 1;
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr, "nop: option -%c unrecognized - ignored\n",
			optopt);
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    if (argc)
	Files = argv;
}

static Agraph_t *gread(FILE * fp)
{
#ifdef USE_CGRAPH
    return agread(fp, (Agdisc_t *) 0);
#else
    return agread(fp);
#endif
}

int main(int argc, char **argv)
{
    Agraph_t *g;
    ingraph_state ig;

    init(argc, argv);
#ifndef USE_CGRAPH
    aginit ();
#endif
    newIngraph(&ig, Files, gread);

    while ((g = nextGraph(&ig)) != 0) {
	if (!chkOnly) agwrite(g, stdout);
	agclose(g);
    }

    return(ig.errors | agerrors());
}
