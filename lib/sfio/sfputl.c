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

#include "sfhdr.h"

/*	Write out a long value in a portable format
**
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int _sfputl(reg Sfio_t *f, Sflong_t v)
#else
int _sfputl(f, v) reg Sfio_t *f; /* write a portable long to this stream */
Sflong_t v;                      /* the value to be written */
#endif
{
#define N_ARRAY (2 * sizeof(Sflong_t))
  reg uchar *s, *ps;
  reg ssize_t n, p;
  uchar c[N_ARRAY];

  SFMTXSTART(f, -1);
  if (f->mode != SF_WRITE && _sfmode(f, SF_WRITE, 0) < 0) SFMTXRETURN(f, -1);
  SFLOCK(f, 0);

  s = ps = &(c[N_ARRAY - 1]);
  if (v < 0) {/* add 1 to avoid 2-complement problems with -SF_MAXINT */
    v = -(v + 1);
    *s = (uchar)(SFSVALUE(v) | SF_SIGN);
  } else
    *s = (uchar)(SFSVALUE(v));
  v = (Sfulong_t)v >> SF_SBITS;

  while (v > 0) {
    *--s = (uchar)(SFUVALUE(v) | SF_MORE);
    v = (Sfulong_t)v >> SF_UBITS;
  }
  n = (ps - s) + 1;

  if (n > 8 || SFWPEEK(f, ps, p) < n)
    n = SFWRITE(f, (Void_t *)s, n); /* write the hard way */
  else {
    switch (n) {
      case 8:
        *ps++ = *s++;
      case 7:
        *ps++ = *s++;
      case 6:
        *ps++ = *s++;
      case 5:
        *ps++ = *s++;
      case 4:
        *ps++ = *s++;
      case 3:
        *ps++ = *s++;
      case 2:
        *ps++ = *s++;
      case 1:
        *ps++ = *s++;
    }
    f->next = ps;
  }

  SFOPEN(f, 0);
  SFMTXRETURN(f, n);
}
