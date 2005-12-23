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
 * Written by Stephen North and Eleftherios Koutsofios.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gvc.h"
#include "globals.h"

#include <time.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(HAVE_FENV_H) && defined(HAVE_FEENABLEEXCEPT)
/* _GNU_SOURCE is needed for feenableexcept to be defined in fenv.h on GNU
 * systems.   Presumably it will do no harm on other systems. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
# include <fenv.h>
#elif HAVE_FPU_CONTROL_H
# include <fpu_control.h>
#elif HAVE_SYS_FPU_H
# include <sys/fpu.h>
#endif

char *Info[] = {
    "dot",			/* Program */
    VERSION,			/* Version */
    BUILDDATE			/* Build Date */
};

static GVC_t *Gvc;
static graph_t * G;

#ifndef MSWIN32
static void intr(int s)
{
    if (G)
	gvRenderJobs(Gvc, G);
    exit (gvFreeContext(Gvc));
}

#ifndef NO_FPERR
static void fperr(int s)
{
    fprintf(stderr, "caught SIGFPE %d\n", s);
    /* signal (s, SIG_DFL); raise (s); */
    exit(1);
}

static void fpinit(void)
{
#if defined(HAVE_FENV_H) && defined(HAVE_FEENABLEEXCEPT)
    int exc = 0;
# ifdef FE_DIVBYZERO
    exc |= FE_DIVBYZERO;
# endif
# ifdef FE_OVERFLOW
    exc |= FE_OVERFLOW;
# endif
# ifdef FE_INVALID
    exc |= FE_INVALID;
# endif
    feenableexcept(exc);

#ifdef HAVE_FESETENV
#ifdef FE_NONIEEE_ENV
    fesetenv (FE_NONIEEE_ENV);
#endif
#endif

#elif  HAVE_FPU_CONTROL_H
    /* On s390-ibm-linux, the header exists, but the definitions
     * of the masks do not.  I assume this is temporary, but until
     * there's a real implementation, it's probably safest to not
     * adjust the FPU on this platform.
     */
# if defined(_FPU_MASK_IM) && defined(_FPU_MASK_DM) && defined(_FPU_MASK_ZM) && defined(_FPU_GETCW)
    fpu_control_t fpe_flags = 0;
    _FPU_GETCW(fpe_flags);
    fpe_flags &= ~_FPU_MASK_IM;	/* invalid operation */
    fpe_flags &= ~_FPU_MASK_DM;	/* denormalized operand */
    fpe_flags &= ~_FPU_MASK_ZM;	/* zero-divide */
    /*fpe_flags &= ~_FPU_MASK_OM;        overflow */
    /*fpe_flags &= ~_FPU_MASK_UM;        underflow */
    /*fpe_flags &= ~_FPU_MASK_PM;        precision (inexact result) */
    _FPU_SETCW(fpe_flags);
# endif
#endif
}
#endif
#endif

static graph_t *create_test_graph(void)
{
#define NUMNODES 5

    Agnode_t *node[NUMNODES];
    Agraph_t *g;
    int j, k;
    char name[10];

    /* Create a new graph */
    g = agopen("new_graph", AGDIGRAPH);

    /* Add nodes */
    for (j = 0; j < NUMNODES; j++) {
	sprintf(name, "%d", j);
	node[j] = agnode(g, name);
    }

    /* Connect nodes */
    for (j = 0; j < NUMNODES; j++) {
	for (k = j + 1; k < NUMNODES; k++) {
	    agedge(g, node[j], node[k]);
	}
    }
    return g;
}

int main(int argc, char **argv)
{
    graph_t *prev = NULL;

    Gvc = gvNEWcontext(Info, gvUsername());
    gvParseArgs(Gvc, argc, argv);

#ifndef MSWIN32
    signal(SIGUSR1, gvToggle);
    signal(SIGINT, intr);
#ifndef NO_FPERR
    fpinit();
    signal(SIGFPE, fperr);
#endif
#endif

    if (MemTest) {
	while (1) {
	    /* Create a test graph */
	    G = create_test_graph();

	    /* Perform layout and cleanup */
	    gvLayoutJobs(Gvc, G);  /* take layout engine from command line */
	    gvFreeLayout(Gvc, G);

	    /* Delete graph */
	    agclose(G);
	}
	assert(0);		/* should never exit loop */
    } else {
	while ((G = next_input_graph())) {
	    if (prev) {
		gvFreeLayout(Gvc, prev);
		agclose(prev);
	    }
	    gvLayoutJobs(Gvc, G);  /* take layout engine from command line */
	    gvRenderJobs(Gvc, G);
	    prev = G;
	}
    }
    return (gvFreeContext(Gvc));
}
