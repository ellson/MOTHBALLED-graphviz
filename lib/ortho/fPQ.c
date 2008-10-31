/* $Id$Revision: */
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

/* Priority Queue Code for shortest path in graph */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fPQ.h>
#include <memory.h>
#include <assert.h>

static snode**  pq;
static int     PQcnt;
static snode    guard;
static int     PQsize;

void
PQgen(int sz)
{
  if (!pq) {
    pq = N_NEW(sz+1,snode*);
    pq[0] = &guard;
    PQsize = sz;
  }
  PQcnt = 0;
}

void
PQfree()
{
  free (pq);
  pq = NULL;
  PQcnt = 0;
}

void
PQinit()
{
  PQcnt = 0;
}

void
PQcheck ()
{
  int i;
 
  for (i = 1; i <= PQcnt; i++) {
    if (N_IDX(pq[i]) != i) {
      assert (0);
    }
  }
}

void
PQupheap(int k)
{
  snode* x = pq[k];
  int     v = x->n_val;
  int     next = k/2;
  snode*  n;
  
  while (N_VAL(n = pq[next]) < v) {
    pq[k] = n;
    N_IDX(n) = k;
    k = next;
    next /= 2;
  }
  pq[k] = x;
  N_IDX(x) = k;
}

void
PQ_insert(snode* np)
{
  if (PQcnt == PQsize) {
    fprintf (stderr, "Heap overflow\n");
    exit (1);
  }
  PQcnt++;
  pq[PQcnt] = np;
  PQupheap (PQcnt);
  PQcheck();
}

void
PQdownheap (int k)
{
  snode*    x = pq[k];
  int      v = N_VAL(x);
  int      lim = PQcnt/2;
  snode*    n;
  int      j;

  while (k <= lim) {
    j = k+k;
    n = pq[j];
    if (j < PQcnt) {
      if (N_VAL(n) < N_VAL(pq[j+1])) {
        j++;
        n = pq[j];
      }
    }
    if (v >= N_VAL(n)) break;
    pq[k] = n;
    N_IDX(n) = k;
    k = j;
  }
  pq[k] = x;
  N_IDX(x) = k;
}

snode*
PQremove ()
{
  snode* n;

  if (PQcnt) {
    n = pq[1];
    pq[1] = pq[PQcnt];
    PQcnt--;
    if (PQcnt) PQdownheap (1);
    PQcheck();
    return n;
  }
  else return 0;
}

void
PQupdate (snode* n, int d)
{
  N_VAL(n) = d;
  PQupheap (n->n_idx);
  PQcheck();
}

void
PQprint ()
{
  int    i;
  snode*  n;

  fprintf (stderr, "Q: ");
  for (i = 1; i <= PQcnt; i++) {
    n = pq[i];
    fprintf (stderr, "%d(%d:%d) ",  
      n->index, N_IDX(n), N_VAL(n));
  }
  fprintf (stderr, "\n");
}
