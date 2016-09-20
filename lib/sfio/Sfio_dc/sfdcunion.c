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


/*	Make a sequence of streams act like a single stream.
**	This is for reading only.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 03/18/1998.
*/

#define	UNSEEKABLE	1

typedef struct _file_s {
    Sfio_t *f;			/* the stream           */
    Sfoff_t lower;		/* its lowest end       */
} File_t;

typedef struct _union_s {
    Sfdisc_t disc;		/* discipline structure */
    short type;			/* type of streams      */
    short c;			/* current stream       */
    short n;			/* number of streams    */
    Sfoff_t here;		/* current location     */
    File_t f[1];		/* array of streams     */
} Union_t;

/**
 * @param f stream involved
 * @param buf buffer to read into
 * @param n number of bytes to read
 * @param disc discipline
 */
static ssize_t unwrite(Sfio_t * f, const void * buf, size_t n,
		       Sfdisc_t * disc)
{
    return -1;
}

/**
 * @param f stream involved
 * @param buf buffer to read into
 * @param n number of bytes to read
 * @param disc discipline
 */
static ssize_t unread(Sfio_t * f, void * buf, size_t n, Sfdisc_t * disc)
{
    reg Union_t *un;
    reg ssize_t r, m;

    un = (Union_t *) disc;
    m = n;
    f = un->f[un->c].f;
    while (1) {
	if ((r = sfread(f, buf, m)) < 0 || (r == 0 && un->c == un->n - 1))
	    break;

	m -= r;
	un->here += r;

	if (m == 0)
	    break;

	buf = (char *) buf + r;
	if (sfeof(f) && un->c < un->n - 1)
	    f = un->f[un->c += 1].f;
    }
    return n - m;
}

static Sfoff_t unseek(Sfio_t * f, Sfoff_t addr, int type, Sfdisc_t * disc)
{
    reg Union_t *un;
    reg int i;
    reg Sfoff_t extent, s;

    un = (Union_t *) disc;
    if (un->type & UNSEEKABLE)
	return -1L;

    if (type == 2) {
	extent = 0;
	for (i = 0; i < un->n; ++i)
	    extent += (sfsize(un->f[i].f) - un->f[i].lower);
	addr += extent;
    } else if (type == 1)
	addr += un->here;

    if (addr < 0)
	return -1;

    /* find the stream where the addr could be in */
    extent = 0;
    for (i = 0; i < un->n - 1; ++i) {
	s = sfsize(un->f[i].f) - un->f[i].lower;
	if (addr < extent + s)
	    break;
	extent += s;
    }

    s = (addr - extent) + un->f[i].lower;
    if (sfseek(un->f[i].f, s, 0) != s)
	return -1;

    un->c = i;
    un->here = addr;

    for (i += 1; i < un->n; ++i)
	sfseek(un->f[i].f, un->f[i].lower, 0);

    return addr;
}

/* on close, remove the discipline */
static int unexcept(Sfio_t * f, int type, void * data, Sfdisc_t * disc)
{
    if (type == SF_FINAL || type == SF_DPOP)
	free(disc);

    return 0;
}

int sfdcunion(Sfio_t * f, Sfio_t ** array, int n)
{
    reg Union_t *un;
    reg int i;

    if (n <= 0)
	return -1;

    if (!
	(un =
	 (Union_t *) malloc(sizeof(Union_t) + (n - 1) * sizeof(File_t))))
	return -1;

    un->disc.readf = unread;
    un->disc.writef = unwrite;
    un->disc.seekf = unseek;
    un->disc.exceptf = unexcept;
    un->type = 0;
    un->c = 0;
    un->n = n;
    un->here = 0;

    for (i = 0; i < n; ++i) {
	un->f[i].f = array[i];
	if (!(un->type & UNSEEKABLE)) {
	    un->f[i].lower = sfseek(array[i], (Sfoff_t) 0, 1);
	    if (un->f[i].lower < 0)
		un->type |= UNSEEKABLE;
	}
    }

    if (sfdisc(f, (Sfdisc_t *) un) != (Sfdisc_t *) un) {
	free(un);
	return -1;
    }

    return 0;
}
