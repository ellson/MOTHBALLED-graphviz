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

/*	Read an unsigned long value coded portably for a given range.
**
**	Written by Kiem-Phong Vo
*/

#if __STD_C
Sfulong_t sfgetm(reg Sfio_t *f, Sfulong_t m)
#else
Sfulong_t sfgetm(f, m) reg Sfio_t *f;
Sfulong_t m;
#endif
{
  Sfulong_t v;
  reg uchar *s, *ends, c;
  reg int p;

  SFMTXSTART(f, (Sfulong_t)(-1));

  if (f->mode != SF_READ && _sfmode(f, SF_READ, 0) < 0)
    SFMTXRETURN(f, (Sfulong_t)(-1));

  SFLOCK(f, 0);

  for (v = 0;;) {
    if (SFRPEEK(f, s, p) <= 0) {
      f->flags |= SF_ERROR;
      v = (Sfulong_t)(-1);
      goto done;
    }
    for (ends = s + p; s < ends;) {
      c = *s++;
      v = (v << SF_BBITS) | SFBVALUE(c);
      if ((m >>= SF_BBITS) <= 0) {
        f->next = s;
        goto done;
      }
    }
    f->next = s;
  }
done:
  SFOPEN(f, 0);
  SFMTXRETURN(f, v);
}
