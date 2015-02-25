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

#include "sfdchdr.h"

/*	Discipline to turn on direct IO capability.
**	This currently only works for XFS on SGI's.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 03/18/1998.
*/

typedef struct _direct_s {
  Sfdisc_t disc; /* Sfio discipline      */
  int cntl;      /* file control flags   */
#ifdef HAVE_STRUCT_DIOATTR
  struct dioattr dio; /* direct IO params     */
#endif
} Direct_t;

/* convert a pointer to an int */
#define P2I(p) (Sfulong_t)((char *)(p) - (char *)0)

#ifdef HAVE_STRUCT_DIOATTR
#if __STD_C
static ssize_t diordwr(Sfio_t *f, Void_t *buf, size_t n, Direct_t *di, int type)
#else
static ssize_t diordwr(f, buf, n, di, type) Sfio_t *f;
Void_t *buf;
size_t n;
Direct_t *di;
int type;
#endif
{
  size_t rw, done;
  ssize_t rv = 0;

  done = 0; /* amount processed by direct IO */

  if ((P2I(buf) % di->dio.d_mem) == 0 && (f->here % di->dio.d_miniosz) == 0 &&
      n >= di->dio.d_miniosz) {/* direct IO ok, make sure we're in the right
                                  mode */
    if (!(di->cntl & FDIRECT)) {
      di->cntl |= FDIRECT;
      (void)fcntl(f->file, F_SETFL, di->cntl);
    }

    for (rw = (n / di->dio.d_miniosz) * di->dio.d_miniosz;;) {
      size_t io;

      if ((io = rw) > di->dio.d_maxiosz) io = di->dio.d_maxiosz;
      if (type == SF_READ)
        rv = read(f->file, buf, io);
      else
        rv = write(f->file, buf, io);

      if (rv > 0) {
        rw -= rv;
        done += rv;
        buf = (Void_t *)((char *)buf + rv);
      }

      if (rv < io || rw < di->dio.d_miniosz) break;
    }
  }

  if (done < n &&
      (di->cntl & FDIRECT)) {/* turn off directIO for remaining IO operation */
    di->cntl &= ~FDIRECT;
    (void)fcntl(f->file, F_SETFL, di->cntl);
  }

  if ((rw = n - done) > 0 &&
      (rv = type == SF_READ ? read(f->file, buf, rw)
                            : write(f->file, buf, rw)) > 0)
    done += rv;

  return done ? done : rv;
}

#if __STD_C
static ssize_t dioread(Sfio_t *f, Void_t *buf, size_t n, Sfdisc_t *disc)
#else
static ssize_t dioread(f, buf, n, disc) Sfio_t *f;
Void_t *buf;
size_t n;
Sfdisc_t *disc;
#endif
{
  return diordwr(f, buf, n, (Direct_t *)disc, SF_READ);
}

#if __STD_C
static ssize_t diowrite(Sfio_t *f, const Void_t *buf, size_t n, Sfdisc_t *disc)
#else
static ssize_t diowrite(f, buf, n, disc) Sfio_t *f;
Void_t *buf;
size_t n;
Sfdisc_t *disc;
#endif
{
  return diordwr(f, (Void_t *)buf, n, (Direct_t *)disc, SF_WRITE);
}

#if __STD_C
static int dioexcept(Sfio_t *f, int type, Void_t *data, Sfdisc_t *disc)
#else
static int dioexcept(f, type, data, disc) Sfio_t *f;
int type;
Void_t *data;
Sfdisc_t *disc;
#endif
{
  Direct_t *di = (Direct_t *)disc;

  if (type == SF_FINAL || type == SF_DPOP) {
    if (di->cntl & FDIRECT) {
      di->cntl &= ~FDIRECT;
      (void)fcntl(f->file, F_SETFL, di->cntl);
    }
    free(disc);
  }

  return 0;
}
#endif /*HAVE_STRUCT_DIOATTR */

#if __STD_C
int sfdcdio(Sfio_t *f, size_t bufsize)
#else
int sfdcdio(f, bufsize) Sfio_t *f;
size_t bufsize;
#endif
{
#ifndef HAVE_STRUCT_DIOATTR
  return -1;
#else
  int cntl;
  struct dioattr dio;
  Void_t *buf;
  Direct_t *di;

  if (f->extent < 0 || (f->flags & SF_STRING)) return -1;

  if ((cntl = fcntl(f->file, F_GETFL, 0)) < 0) return -1;

  if (!(cntl & FDIRECT)) {
    cntl |= FDIRECT;
    if (fcntl(f->file, F_SETFL, cntl) < 0) return -1;
  }

  if (fcntl(f->file, F_DIOINFO, &dio) < 0) goto no_direct;

  if (bufsize > 0) bufsize = (bufsize / dio.d_miniosz) * dio.d_miniosz;
  if (bufsize <= 0) bufsize = dio.d_miniosz * 64;
  if (bufsize > dio.d_maxiosz) bufsize = dio.d_maxiosz;

  if (!(di = (Direct_t *)malloc(sizeof(Direct_t)))) goto no_direct;

  if (!(buf = (Void_t *)memalign(dio.d_mem, bufsize))) {
    free(di);
    goto no_direct;
  }

  sfsetbuf(f, buf, bufsize);
  if (sfsetbuf(f, buf, 0) == buf)
    sfset(f, SF_MALLOC, 1);
  else {
    free(buf);
    free(di);
    goto no_direct;
  }

  di->disc.readf = dioread;
  di->disc.writef = diowrite;
  di->disc.seekf = NIL(Sfseek_f);
  di->disc.exceptf = dioexcept;
  di->cntl = cntl;
  di->dio = dio;

  if (sfdisc(f, (Sfdisc_t *)di) != (Sfdisc_t *)di) {
    free(di);
  no_direct:
    cntl &= ~FDIRECT;
    (void)fcntl(f->file, F_SETFL, cntl);
    return -1;
  }

  return 0;

#endif /*FDIRECT*/
}
