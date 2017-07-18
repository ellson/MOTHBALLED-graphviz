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

#if defined(__STDPP__directive) && defined(__STDPP__hide)
__STDPP__directive pragma pp:hide getpagesize
#else
#define getpagesize	______getpagesize
#endif

#include	"sfhdr.h"

#if defined(__STDPP__directive) && defined(__STDPP__hide)
__STDPP__directive pragma pp:nohide getpagesize
#else
#undef	getpagesize
#endif

#ifdef HAVE_GETPAGESIZE
#ifdef __cplusplus
extern "C" {
#endif
	extern int getpagesize(void);
#ifdef __cplusplus
}
#endif
#endif
/*	Set a (new) buffer for a stream.
**	If size < 0, it is assigned a suitable value depending on the
**	kind of stream. The actual buffer size allocated is dependent
**	on how much memory is available.
**
**	Written by Kiem-Phong Vo.
*/
#ifndef HAVE_SYS_STAT_H
    struct stat {
    int st_mode;
    int st_size;
};
#define fstat(fd,st)	(-1)
#endif /*HAVE_SYS_STAT_H*/

/**
 * @param f stream to be buffered
 * @param buf new buffer
 * @param size buffer size, -1 for default size
 */
void *sfsetbuf(reg Sfio_t * f, reg void * buf, reg size_t size)
{
    reg int sf_malloc;
    reg uchar *obuf;
    reg Sfdisc_t *disc;
    reg ssize_t osize, blksize;
    reg int oflags, init, local;
    Stat_t st;

    SFONCE();

    SFMTXSTART(f, NIL(void *));

    GETLOCAL(f, local);

    if (size == 0 && buf) {	/* special case to get buffer info */
	_Sfi = f->val =
	    (f->bits & SF_MMAP) ? (f->endb - f->data) : f->size;
	SFMTXRETURN(f, (void *) f->data);
    }

    /* cleanup actions already done, don't allow write buffering any more */
    if (_Sfexiting && !(f->flags & SF_STRING) && (f->mode & SF_WRITE)) {
	buf = NIL(void *);
	size = 0;
    }

    if ((init = f->mode & SF_INIT)) {
	if (!f->pool && _sfsetpool(f) < 0)
	    SFMTXRETURN(f, NIL(void *));
    } else if ((f->mode & SF_RDWR) != SFMODE(f, local)
	       && _sfmode(f, 0, local) < 0)
	SFMTXRETURN(f, NIL(void *));

    if (init)
	f->mode = (f->mode & SF_RDWR) | SF_LOCK;
    else {
	int rv;

	/* make sure there is no hidden read data */
	if (f->proc && (f->flags & SF_READ) && (f->mode & SF_WRITE) &&
	    _sfmode(f, SF_READ, local) < 0)
	    SFMTXRETURN(f, NIL(void *));

	/* synchronize first */
	SFLOCK(f, local);
	rv = SFSYNC(f);
	SFOPEN(f, local);
	if (rv < 0)
	    SFMTXRETURN(f, NIL(void *));

	/* turn off the SF_SYNCED bit because buffer is changing */
	f->mode &= ~SF_SYNCED;
    }

    SFLOCK(f, local);

    blksize = 0;
    oflags = f->flags;

    if (f->data == f->tiny) {
	f->data = NIL(uchar *);
	f->size = 0;
    }
    obuf = f->data;
    osize = f->size;

    f->flags &= ~SF_MALLOC;
    f->bits &= ~SF_MMAP;

    /* pure read/string streams must have a valid string */
    if ((f->flags & (SF_RDWR | SF_STRING)) == SF_RDSTR &&
	(size == (size_t) SF_UNBOUND || !buf))
	size = 0;

    /* set disc to the first discipline with a seekf */
    for (disc = f->disc; disc; disc = disc->disc)
	if (disc->seekf)
	    break;

    if ((init || local) && !(f->flags & SF_STRING)) {	/* ASSERT(f->file >= 0) */
	st.st_mode = 0;

	/* if has discipline, set size by discipline if possible */
	if (!HAVE_SYS_STAT_H || disc) {
	    if ((f->here = SFSK(f, (Sfoff_t) 0, SEEK_CUR, disc)) < 0)
		goto unseekable;
	    else {
		Sfoff_t e;
		if ((e = SFSK(f, (Sfoff_t) 0, SEEK_END, disc)) >= 0)
		    f->extent = e > f->here ? e : f->here;
		(void) SFSK(f, f->here, SEEK_SET, disc);
		goto setbuf;
	    }
	}

	/* get file descriptor status */
	if (fstat((int) f->file, &st) < 0)
	    f->here = -1;
	else {
#if defined(HAVE_SYS_STAT_H) && _stat_blksize	/* preferred io block size */
	    if ((blksize = (ssize_t) st.st_blksize) > 0)
		while ((blksize + (ssize_t) st.st_blksize) <= SF_PAGE)
		    blksize += (ssize_t) st.st_blksize;
#endif
	    if (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
		f->here = SFSK(f, (Sfoff_t) 0, SEEK_CUR, f->disc);
	    else
		f->here = -1;
	}

	if (f->here >= 0) {
	    f->extent = (Sfoff_t) st.st_size;

	    /* seekable std-devices are share-public by default */
	    if (f == sfstdin || f == sfstdout || f == sfstderr)
		f->flags |= SF_SHARE | SF_PUBLIC;
	} else {
	  unseekable:
	    f->extent = -1;
	    f->here = 0;

	    if (init) {
		if (S_ISCHR(st.st_mode)) {
		    int oerrno = errno;

		    blksize = SF_GRAIN;

		    /* set line mode for terminals */
		    if (!(f->flags & SF_LINE) && isatty(f->file))
			f->flags |= SF_LINE;
#ifdef HAVE_SYS_STAT_H
		    else {	/* special case /dev/null */
			reg int dev, ino;
			dev = (int) st.st_dev;
			ino = (int) st.st_ino;
			if (stat(DEVNULL, &st) >= 0 &&
			    dev == (int) st.st_dev &&
			    ino == (int) st.st_ino)
			    SFSETNULL(f);
		    }
#endif
		    errno = oerrno;
		}

		/* initialize side buffer for r+w unseekable streams */
		if (!f->proc && (f->bits & SF_BOTH))
		    (void) _sfpopen(f, -1, -1, 0);
	    }
	}

	/* set page size, this is also the desired default buffer size */
	if (_Sfpage <= 0) {
#ifdef HAVE_GETPAGESIZE
	    if ((_Sfpage = (size_t) getpagesize()) <= 0)
#endif
		_Sfpage = SF_PAGE;
	}
    }

    /* get buffer space */
  setbuf:
    if (size == (size_t) SF_UNBOUND) {	/* define a default size suitable for block transfer */
	if (init && osize > 0)
	    size = osize;
	else if (f == sfstderr && (f->mode & SF_WRITE))
	    size = 0;
	else if (f->flags & SF_STRING)
	    size = SF_GRAIN;
	else if ((f->flags & SF_READ) && !(f->bits & SF_BOTH) &&
		 f->extent > 0 && f->extent < (Sfoff_t) _Sfpage)
	    size =
		(((size_t) f->extent + SF_GRAIN -
		  1) / SF_GRAIN) * SF_GRAIN;
	else if ((ssize_t) (size = _Sfpage) < blksize)
	    size = blksize;

	buf = NIL(void *);
    }

    sf_malloc = 0;
    if (size > 0 && !buf && !(f->bits & SF_MMAP)) {	/* try to allocate a buffer */
	if (obuf && size == (size_t) osize && init) {
	    buf = (void *) obuf;
	    obuf = NIL(uchar *);
	    sf_malloc = (oflags & SF_MALLOC);
	}
	if (!buf) {		/* do allocation */
	    while (!buf && size > 0) {
		if ((buf = (void *) malloc(size)))
		    break;
		else
		    size /= 2;
	    }
	    if (size > 0)
		sf_malloc = SF_MALLOC;
	}
    }

    if (size == 0 && !(f->flags & SF_STRING) && !(f->bits & SF_MMAP) && (f->mode & SF_READ)) {	/* use the internal buffer */
	size = sizeof(f->tiny);
	buf = (void *) f->tiny;
    }

    /* set up new buffer */
    f->size = size;
    f->next = f->data = f->endr = f->endw = (uchar *) buf;
    f->endb = (f->mode & SF_READ) ? f->data : f->data + size;
    if (f->flags & SF_STRING) {	/* these fields are used to test actual size - see sfseek() */
	f->extent = (!sf_malloc &&
		     ((f->flags & SF_READ)
		      || (f->bits & SF_BOTH))) ? size : 0;
	f->here = 0;

	/* read+string stream should have all data available */
	if ((f->mode & SF_READ) && !sf_malloc)
	    f->endb = f->data + size;
    }

    f->flags = (f->flags & ~SF_MALLOC) | sf_malloc;

    if (obuf && obuf != f->data && osize > 0 && (oflags & SF_MALLOC)) {
	free((void *) obuf);
	obuf = NIL(uchar *);
    }

    _Sfi = f->val = obuf ? osize : 0;

    SFOPEN(f, local);

    SFMTXRETURN(f, (void *) obuf);
}
