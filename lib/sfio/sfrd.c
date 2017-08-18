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

#include	"sfhdr.h"

/*	Internal function to do a hard read.
**	This knows about discipline and memory mapping, peek read.
**
**	Written by Kiem-Phong Vo.
*/

/* synchronize unseekable write streams */
static void _sfwrsync(void)
{
    reg Sfpool_t *p;
    reg Sfio_t *f;
    reg int n;

    /* sync all pool heads */
    for (p = _Sfpool.next; p; p = p->next) {
	if (p->n_sf <= 0)
	    continue;
	f = p->sf[0];
	if (!SFFROZEN(f) && f->next > f->data &&
	    (f->mode & SF_WRITE) && f->extent < 0)
	    (void) _sfflsbuf(f, -1);
    }

    /* and all the ones in the discrete pool */
    for (n = 0; n < _Sfpool.n_sf; ++n) {
	f = _Sfpool.sf[n];

	if (!SFFROZEN(f) && f->next > f->data &&
	    (f->mode & SF_WRITE) && f->extent < 0)
	    (void) _sfflsbuf(f, -1);
    }
}

ssize_t sfrd(reg Sfio_t * f, reg void * buf, reg size_t n,
	     Sfdisc_t * disc)
{
    Sfoff_t r;
    reg Sfdisc_t *dc;
    reg int local, rcrv, dosync, oerrno;

    SFMTXSTART(f, -1);

    GETLOCAL(f, local);
    if ((rcrv = f->mode & (SF_RC | SF_RV)))
	f->mode &= ~(SF_RC | SF_RV);
    f->bits &= ~SF_JUSTSEEK;

    if (f->mode & SF_PKRD)
	SFMTXRETURN(f, -1);

    if (!local && !(f->bits & SF_DCDOWN)) {	/* an external user's call */
	if (f->mode != SF_READ && _sfmode(f, SF_READ, 0) < 0)
	    SFMTXRETURN(f, -1);
	if (f->next < f->endb) {
	    if (SFSYNC(f) < 0)
		SFMTXRETURN(f, -1);
	    f->next = f->endb = f->endr = f->endw = f->data;
	}
    }

    for (dosync = 0;;) {	/* stream locked by sfsetfd() */
	if (!(f->flags & SF_STRING) && f->file < 0)
	    SFMTXRETURN(f, 0);

	f->flags &= ~(SF_EOF | SF_ERROR);

	dc = disc;
	if (f->flags & SF_STRING) {
	    if ((r = (f->data + f->extent) - f->next) < 0)
		r = 0;
	    if (r <= 0)
		goto do_except;
	    SFMTXRETURN(f, (ssize_t) r);
	}

	/* warn that a read is about to happen */
	SFDISC(f, dc, readf);
	if (dc && dc->exceptf && (f->flags & SF_IOCHECK)) {
	    reg int rv;
	    if (local)
		SETLOCAL(f);
	    if ((rv = _sfexcept(f, SF_READ, n, dc)) > 0)
		n = rv;
	    else if (rv < 0) {
		f->flags |= SF_ERROR;
		SFMTXRETURN(f, (ssize_t) rv);
	    }
	}

	/* sync unseekable write streams to prevent deadlock */
	if (!dosync && f->extent < 0) {
	    dosync = 1;
	    _sfwrsync();
	}

	/* make sure file pointer is right */
	if (f->extent >= 0 && (f->flags & SF_SHARE)) {
	    if (!(f->flags & SF_PUBLIC))
		f->here = SFSK(f, f->here, SEEK_SET, dc);
	    else
		f->here = SFSK(f, (Sfoff_t) 0, SEEK_CUR, dc);
	}

	oerrno = errno;
	errno = 0;

	if (dc && dc->readf) {
	    int share = f->flags & SF_SHARE;

	    if (rcrv)		/* pass on rcrv for possible continuations */
		f->mode |= rcrv;
	    /* tell readf that no peeking necessary */
	    else
		f->flags &= ~SF_SHARE;

	    SFDCRD(f, buf, n, dc, r);

	    /* reset flags */
	    if (rcrv)
		f->mode &= ~rcrv;
	    else
		f->flags |= share;
	} else if (SFISNULL(f))
	    r = 0;
	else if (f->extent < 0 && (f->flags & SF_SHARE) && rcrv) {	/* try peek read */
	    r = sfpkrd(f->file, (char *) buf, n,
		       (rcrv & SF_RC) ? (int) f->getr : -1,
		       -1L, (rcrv & SF_RV) ? 1 : 0);
	    if (r > 0) {
		if (rcrv & SF_RV)
		    f->mode |= SF_PKRD;
		else
		    f->mode |= SF_RC;
	    }
	} else
	    r = read(f->file, buf, n);

	if (errno == 0)
	    errno = oerrno;

	if (r > 0) {
	    if (!(f->bits & SF_DCDOWN)) {	/* not a continuation call */
		if (!(f->mode & SF_PKRD)) {
		    f->here += r;
		    if (f->extent >= 0 && f->extent < f->here)
			f->extent = f->here;
		}
		if ((uchar *) buf >= f->data &&
		    (uchar *) buf < f->data + f->size)
		    f->endb = f->endr = ((uchar *) buf) + r;
	    }

	    SFMTXRETURN(f, (ssize_t) r);
	}

      do_except:
	if (local)
	    SETLOCAL(f);
	switch (_sfexcept(f, SF_READ, (ssize_t) r, dc)) {
	case SF_ECONT:
	    goto do_continue;
	case SF_EDONE:
	    n = local ? 0 : (ssize_t) r;
	    SFMTXRETURN(f, n);
	case SF_EDISC:
	    if (!local && !(f->flags & SF_STRING))
		goto do_continue;
	    /* else fall through */
	case SF_ESTACK:
	    SFMTXRETURN(f, -1);
	}

      do_continue:
	for (dc = f->disc; dc; dc = dc->disc)
	    if (dc == disc)
		break;
	disc = dc;
    }
}
