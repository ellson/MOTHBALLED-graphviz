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

#if __STD_C
static ssize_t skwrite(Sfio_t * f, const Void_t * buf, size_t n,
		       Sfdisc_t * disc)
#else
static ssize_t skwrite(f, buf, n, disc)
Sfio_t *f;			/* stream involved */
Void_t *buf;			/* buffer to read into */
size_t n;			/* number of bytes to read */
Sfdisc_t *disc;			/* discipline */
#endif
{
    return (ssize_t) (-1);
}

#if __STD_C
static ssize_t skread(Sfio_t * f, Void_t * buf, size_t n, Sfdisc_t * disc)
#else
static ssize_t skread(f, buf, n, disc)
Sfio_t *f;			/* stream involved */
Void_t *buf;			/* buffer to read into */
size_t n;			/* number of bytes to read */
Sfdisc_t *disc;			/* discipline */
#endif
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

#if __STD_C
static Sfoff_t skseek(Sfio_t * f, Sfoff_t addr, int type, Sfdisc_t * disc)
#else
static Sfoff_t skseek(f, addr, type, disc)
Sfio_t *f;
Sfoff_t addr;
int type;
Sfdisc_t *disc;
#endif
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
#if __STD_C
static int skexcept(Sfio_t * f, int type, Void_t * data, Sfdisc_t * disc)
#else
static int skexcept(f, type, data, disc)
Sfio_t *f;
int type;
Void_t *data;
Sfdisc_t *disc;
#endif
{
    if (type == SF_FINAL || type == SF_DPOP) {
	sfclose(((Seek_t *) disc)->shadow);
	free(disc);
    }
    return 0;
}

#if __STD_C
int sfdcseekable(Sfio_t * f)
#else
int sfdcseekable(f)
Sfio_t *f;
#endif
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
