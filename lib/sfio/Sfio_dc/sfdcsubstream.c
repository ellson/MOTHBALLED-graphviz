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


/*	Discipline to treat a contiguous segment of a stream as a stream
**	in its own right. The hard part in all this is to allow multiple
**	segments of the stream to be used as substreams at the same time.
**
**	Written by David G. Korn and Kiem-Phong Vo (03/18/1998)
*/

typedef struct _subfile_s {
    Sfdisc_t disc;		/* sfio discipline */
    Sfio_t *parent;		/* parent stream */
    Sfoff_t offset;		/* starting offset */
    Sfoff_t extent;		/* size wanted */
    Sfoff_t here;		/* current seek location */
} Subfile_t;

static ssize_t streamio(Sfio_t * f, void * buf, size_t n,
			Sfdisc_t * disc, int type)
{
    reg Subfile_t *su;
    reg Sfoff_t here, parent;
    reg ssize_t io;

    su = (Subfile_t *) disc;

    /* read just what we need */
    if (su->extent >= 0
	&& (ssize_t) n > (io = (ssize_t) (su->extent - su->here)))
	n = io;
    if (n <= 0)
	return n;

    /* save current location in parent stream */
    parent = sfseek(su->parent, (Sfoff_t) 0, 1);

    /* read data */
    here = su->here + su->offset;
    if (sfseek(su->parent, here, 0) != here)
	io = 0;
    else {
	if (type == SF_WRITE)
	    io = sfwrite(su->parent, buf, n);
	else
	    io = sfread(su->parent, buf, n);
	if (io > 0)
	    su->here += io;
    }

    /* restore parent current position */
    sfseek(su->parent, parent, 0);

    return io;
}

static ssize_t streamwrite(Sfio_t * f, const void * buf, size_t n,
			   Sfdisc_t * disc)
{
    return streamio(f, (void *) buf, n, disc, SF_WRITE);
}

static ssize_t streamread(Sfio_t * f, void * buf, size_t n,
			  Sfdisc_t * disc)
{
    return streamio(f, buf, n, disc, SF_READ);
}

static Sfoff_t streamseek(Sfio_t * f, Sfoff_t pos, int type,
			  Sfdisc_t * disc)
{
    reg Subfile_t *su;
    reg Sfoff_t here, parent;

    su = (Subfile_t *) disc;

    switch (type) {
    case 0:
	here = 0;
	break;
    case 1:
	here = su->here;
	break;
    case 2:
	if (su->extent >= 0)
	    here = su->extent;
	else {
	    parent = sfseek(su->parent, (Sfoff_t) 0, 1);
	    if ((here = sfseek(su->parent, (Sfoff_t) 0, 2)) < 0)
		return -1;
	    else
		here -= su->offset;
	    sfseek(su->parent, parent, 0);
	}
	break;
    default:
	return -1;
    }

    pos += here;
    if (pos < 0 || (su->extent >= 0 && pos >= su->extent))
	return -1;

    return (su->here = pos);
}

static int streamexcept(Sfio_t * f, int type, void * data,
			Sfdisc_t * disc)
{
    if (type == SF_FINAL || type == SF_DPOP)
	free(disc);
    return 0;
}

/**
 * @param f stream
 * @param parent parent stream
 * @param offset offset in parent stream
 * @param extent desired size
 */
int sfdcsubstream(Sfio_t * f, Sfio_t * parent, Sfoff_t offset,
		  Sfoff_t extent)
{
    reg Subfile_t *su;
    reg Sfoff_t here;

    /* establish that we can seek to offset */
    if ((here = sfseek(parent, (Sfoff_t) 0, 1)) < 0
	|| sfseek(parent, offset, 0) < 0)
	return -1;
    else
	sfseek(parent, here, 0);

    if (!(su = (Subfile_t *) malloc(sizeof(Subfile_t))))
	return -1;

    su->disc.readf = streamread;
    su->disc.writef = streamwrite;
    su->disc.seekf = streamseek;
    su->disc.exceptf = streamexcept;
    su->parent = parent;
    su->offset = offset;
    su->extent = extent;

    if (sfdisc(f, (Sfdisc_t *) su) != (Sfdisc_t *) su) {
	free(su);
	return -1;
    }

    return 0;
}
