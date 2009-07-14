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
 * gvpr: graph pattern recognizer
 *
 * Written by Emden Gansner
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include "compat_unistd.h"
#endif



#include "gvpr.h"

#ifdef DEBUG
#include "sfio.h"
static ssize_t outfn (void* sp, const char *buf, size_t nbyte, void* dp)
{
    write (1, "<stdout> ", 8);
    return write (1, buf, nbyte);
}

static ssize_t errfn (void* sp, const char *buf, size_t nbyte, void* dp)
{
    write (2, "<stderr> ", 8);
    return write (2, buf, nbyte);
}

static int iofread(void *chan, char *buf, int bufsize)
{
    return read(sffileno((Sfio_t *) chan), buf, bufsize);
}

static int ioputstr(void *chan, char *str)
{
    return sfputr((Sfio_t *) chan, str, -1);
}

static int ioflush(void *chan)
{
    return sfsync((Sfio_t *) chan);
}

static Agiodisc_t gprIoDisc = { iofread, ioputstr, ioflush };

static Agdisc_t gprDisc = { &AgMemDisc, &AgIdDisc, &gprIoDisc };

int
main (int argc, char* argv[])
{
    Agraph_t* gs[2];
    Agraph_t* g = agread (sfstdin, &gprDisc);
    int rv, i;
    gvpropts opts;

    gs[0] = g;
    gs[1] = 0;
    opts.ingraphs = gs;
    opts.out = outfn;
    opts.err = errfn;
    opts.flags = GV_USE_OUTGRAPH;
    
    rv = gvpr (argc, argv, &opts);

    sfprintf (sfstderr, "rv %d\n", rv);
    /* for (i = 0; i < opts.n_outgraphs; i++) */
	/* agwrite (opts.outgraphs[i], sfstdout); */

    rv = gvpr (argc, argv, &opts);

    return rv;
}

#else
int
main (int argc, char* argv[])
{
    gvpropts opts;
    opts.ingraphs = 0;
    opts.out = 0;
    opts.err = 0;
    opts.flags = GV_USE_EXIT;
    
    return gvpr (argc, argv, &opts);
}

#endif

