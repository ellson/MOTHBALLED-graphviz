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


#include "render.h"
#include <stdio.h>

#include "mem.h"
#include "hedges.h"
#include "heap.h"


static Halfedge *PQhash;
static int PQhashsize;
static int PQcount;
static int PQmin;

static int PQbucket(Halfedge * he)
{
    int bucket;

    bucket = (he->ystar - ymin) / deltay * PQhashsize;
    if (bucket < 0)
	bucket = 0;
    if (bucket >= PQhashsize)
	bucket = PQhashsize - 1;
    if (bucket < PQmin)
	PQmin = bucket;
    return (bucket);
}

void PQinsert(Halfedge * he, Site * v, double offset)
{
    Halfedge *last, *next;

    he->vertex = v;
    ref(v);
    he->ystar = v->coord.y + offset;
    last = &PQhash[PQbucket(he)];
    while ((next = last->PQnext) != (struct Halfedge *) NULL &&
	   (he->ystar > next->ystar ||
	    (he->ystar == next->ystar
	     && v->coord.x > next->vertex->coord.x))) {
	last = next;
    }
    he->PQnext = last->PQnext;
    last->PQnext = he;
    PQcount += 1;
}

void PQdelete(Halfedge * he)
{
    Halfedge *last;

    if (he->vertex != (Site *) NULL) {
	last = &PQhash[PQbucket(he)];
	while (last->PQnext != he)
	    last = last->PQnext;
	last->PQnext = he->PQnext;
	PQcount -= 1;
	deref(he->vertex);
	he->vertex = (Site *) NULL;
    }
}


int PQempty()
{
    return (PQcount == 0);
}


Point PQ_min()
{
    Point answer;

    while (PQhash[PQmin].PQnext == (struct Halfedge *) NULL) {
	PQmin += 1;
    }
    answer.x = PQhash[PQmin].PQnext->vertex->coord.x;
    answer.y = PQhash[PQmin].PQnext->ystar;
    return (answer);
}

Halfedge *PQextractmin()
{
    Halfedge *curr;

    curr = PQhash[PQmin].PQnext;
    PQhash[PQmin].PQnext = curr->PQnext;
    PQcount -= 1;
    return (curr);
}

void PQcleanup()
{
    free(PQhash);
    PQhash = NULL;
}

void PQinitialize()
{
    int i;

    PQcount = 0;
    PQmin = 0;
    PQhashsize = 4 * sqrt_nsites;
    if (PQhash == NULL)
	PQhash = N_GNEW(PQhashsize, Halfedge);
    for (i = 0; i < PQhashsize; i += 1)
	PQhash[i].PQnext = (Halfedge *) NULL;
}

static void PQdumphe(Halfedge * p)
{
    printf("  [%p] %p %p %d %d %d %d %f\n",
	   p, p->ELleft, p->ELright, p->ELedge->edgenbr,
	   p->ELrefcnt, p->ELpm, (p->vertex ? p->vertex->sitenbr : -1),
	   p->ystar);
}

void PQdump()
{
    int i;
    Halfedge *p;

    for (i = 0; i < PQhashsize; i += 1) {
	printf("[%d]\n", i);
	p = PQhash[i].PQnext;
	while (p != NULL) {
	    PQdumphe(p);
	    p = p->PQnext;
	}
    }

}
