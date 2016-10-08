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

/*	Discipline to make an unseekable read stream seekable
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 03/18/1998.
*/

typedef struct _skable_s {
    Sfdisc_t disc;		/* sfio discipline */
    Sfio_t *shadow;		/* to shadow data */
    int eof;			/* if eof has been reached */
} Seek_t;

/**
 * @param f stream involved
 * @param buf buffer to read into
 * @param n number of bytes to read
 * @param disc discipline
 */
static ssize_t skwrite(Sfio_t * f, const void * buf, size_t n,
		       Sfdisc_t * disc)
{
    return (ssize_t) (-1);
}

/**
 * @param f stream involved
 * @param buf buffer to read into
 * @param n number of bytes to read
 * @param disc discipline
 */
static ssize_t skread(Sfio_t * f, void * buf, size_t n, Sfdisc_t * disc)
{
    Seek_t *sk;
    Sfio_t *sf;
    Sfoff_t addr, extent;
    ssize_t r, w;

    sk = (Seek_t *) disc;
    sf = sk->shadow;
    if (sk->eof)
	return sfread(sf, buf, n);

    addr = sfseek(sf, (Sfoff_t) 0, 1);
    extent = sfsize(sf);

    if (addr + n <= extent)
	return sfread(sf, buf, n);

    if ((r = (ssize_t) (extent - addr)) > 0) {
	if ((w = sfread(sf, buf, r)) != r)
	    return w;
	buf = (char *) buf + r;
	n -= r;
    }

    /* do a raw read */
    if ((w = sfrd(f, buf, n, disc)) <= 0) {
	sk->eof = 1;
	w = 0;
    } else if (sfwrite(sf, buf, w) != w)
	sk->eof = 1;

    return r + w;
}

static Sfoff_t skseek(Sfio_t * f, Sfoff_t addr, int type, Sfdisc_t * disc)
{
    Sfoff_t extent;
    Seek_t *sk;
    Sfio_t *sf;
    char buf[SF_BUFSIZE];
    ssize_t r, w;

    if (type < 0 || type > 2)
	return (Sfoff_t) (-1);

    sk = (Seek_t *) disc;
    sf = sk->shadow;

    extent = sfseek(sf, (Sfoff_t) 0, 2);
    if (type == 1)
	addr += sftell(sf);
    else if (type == 2)
	addr += extent;

    if (addr < 0)
	return (Sfoff_t) (-1);
    else if (addr > extent) {
	if (sk->eof)
	    return (Sfoff_t) (-1);

	/* read enough to reach the seek point */
	while (addr > extent) {
	    if (addr > extent + sizeof(buf))
		w = sizeof(buf);
	    else
		w = (int) (addr - extent);
	    if ((r = sfrd(f, buf, w, disc)) <= 0)
		w = r - 1;
	    else if ((w = sfwrite(sf, buf, r)) > 0)
		extent += r;
	    if (w != r) {
		sk->eof = 1;
		break;
	    }
	}

	if (addr > extent)
	    return (Sfoff_t) (-1);
    }

    return sfseek(sf, addr, 0);
}

/* on close, remove the discipline */
static int skexcept(Sfio_t * f, int type, void * data, Sfdisc_t * disc)
{
    if (type == SF_FINAL || type == SF_DPOP) {
	sfclose(((Seek_t *) disc)->shadow);
	free(disc);
    }
    return 0;
}

int sfdcseekable(Sfio_t * f)
{
    reg Seek_t *sk;

    /* see if already seekable */
    if (sfseek(f, (Sfoff_t) 0, 1) >= 0)
	return 0;

    if (!(sk = (Seek_t *) malloc(sizeof(Seek_t))))
	return -1;

    sk->disc.readf = skread;
    sk->disc.writef = skwrite;
    sk->disc.seekf = skseek;
    sk->disc.exceptf = skexcept;
    sk->shadow = sftmp(SF_BUFSIZE);
    sk->eof = 0;

    if (sfdisc(f, (Sfdisc_t *) sk) != (Sfdisc_t *) sk) {
	sfclose(sk->shadow);
	free(sk);
	return -1;
    }

    return 0;
}
