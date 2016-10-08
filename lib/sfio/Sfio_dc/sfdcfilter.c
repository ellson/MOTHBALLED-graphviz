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

#include	"sfdchdr.h"

/*	Discipline to invoke UNIX processes as data filters.
**	These processes must be able to fit in pipelines.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 03/18/1998.
*/

#if !defined(FNDELAY) && defined(O_NDELAY)
#define FNDELAY	O_NDELAY
#endif

typedef struct _filter_s {
    Sfdisc_t disc;		/* discipline structure */
    Sfio_t *filter;		/* the filter stream */
    char raw[1024];		/* raw data buffer */
    char *next;			/* remainder of data unwritten to pipe */
    char *endb;			/* end of data */
} Filter_t;

/* read data from the filter */
/**
 * @param f stream reading from
 * @param buf buffer to read into
 * @param n number of bytes requested
 * @param disc discipline
 */
static ssize_t filterread(Sfio_t * f, void * buf, size_t n,
			  Sfdisc_t * disc)
{
    Filter_t *fi;
    ssize_t r, w;

    fi = (Filter_t *) disc;
    for (;;) {
	if (!fi->next)
	    fi->next = fi->endb = fi->raw;
	else {			/* try to get data from filter, if any */
	    errno = 0;
	    if ((r = sfread(fi->filter, buf, n)) > 0)
		return r;
	    if (errno != EWOULDBLOCK)
		return 0;
	}

	/* get some raw data to stuff down the pipe */
	if (fi->next >= fi->endb) {
	    if ((r = sfrd(f, fi->raw, sizeof(fi->raw), disc)) > 0) {
		fi->next = fi->raw;
		fi->endb = fi->raw + r;
	    } else {		/* eof, close write end of pipes */
		sfset(fi->filter, SF_READ, 0);
		close(sffileno(fi->filter));
		sfset(fi->filter, SF_READ, 1);
	    }
	}

	if ((w = fi->endb - fi->next) > 0) {
	    errno = 0;
	    if ((w = sfwrite(fi->filter, fi->next, w)) > 0)
		fi->next += w;
	    else if (errno != EWOULDBLOCK)
		return 0;
	    /* pipe is full, sleep for a while, then continue */
	    else
		sleep(1);
	}
    }
}

/**
 * @param f stream reading from
 * @param buf buffer to read into
 * @param n number of bytes requested
 * @param disc discipline
 */
static ssize_t filterwrite(Sfio_t * f, const void * buf, size_t n,
			   Sfdisc_t * disc)
{
    return -1;
}

/* for the duration of this discipline, the stream is unseekable */
static Sfoff_t filterseek(Sfio_t * f, Sfoff_t addr, int offset,
			  Sfdisc_t * disc)
{
    f = NIL(Sfio_t *);
    addr = 0;
    offset = 0;
    disc = NIL(Sfdisc_t *);
    return (Sfoff_t) (-1);
}

/* on close, remove the discipline */
static int filterexcept(Sfio_t * f, int type, void * data,
			Sfdisc_t * disc)
{
    if (type == SF_FINAL || type == SF_DPOP) {
	sfclose(((Filter_t *) disc)->filter);
	free(disc);
    }

    return 0;
}

/**
 * @param f stream to filter data
 * @param cmd program to run as a filter
 */
int sfdcfilter(Sfio_t * f, const char *cmd)
{
    reg Filter_t *fi;
    reg Sfio_t *filter;

    /* open filter for read&write */
    if (!(filter = sfpopen(NIL(Sfio_t *), cmd, "r+")))
	return -1;

    /* unbuffered so that write data will get to the pipe right away */
    sfsetbuf(filter, NIL(void *), 0);

    /* make the write descriptor nonblocking */
    sfset(filter, SF_READ, 0);
    fcntl(sffileno(filter), F_SETFL, FNDELAY);
    sfset(filter, SF_READ, 1);

    /* same for the read descriptor */
    sfset(filter, SF_WRITE, 0);
    fcntl(sffileno(filter), F_SETFL, FNDELAY);
    sfset(filter, SF_WRITE, 1);

    if (!(fi = (Filter_t *) malloc(sizeof(Filter_t)))) {
	sfclose(filter);
	return -1;
    }

    fi->disc.readf = filterread;
    fi->disc.writef = filterwrite;
    fi->disc.seekf = filterseek;
    fi->disc.exceptf = filterexcept;
    fi->filter = filter;
    fi->next = fi->endb = NIL(char *);

    if (sfdisc(f, (Sfdisc_t *) fi) != (Sfdisc_t *) fi) {
	sfclose(filter);
	free(fi);
	return -1;
    }

    return 0;
}
