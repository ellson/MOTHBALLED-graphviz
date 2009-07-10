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

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "types.h"
#ifdef WITH_CGRAPH
#include "cgraph.h"
#else
#include "graph.h"
#endif
#include "rawgraph.h"

typedef struct {
    double p1, p2;
} paird;

typedef struct {
    int a,b;
} pair;

typedef struct {
	pair t1, t2;
} pair2;

typedef enum {B_NODE, B_UP, B_LEFT, B_DOWN, B_RIGHT} bend;

/* Example : segment connecting maze point (3,2) 
 * and (3,8) has isVert = 1, common coordinate = 3, p1 = 2, p2 = 8
 */
typedef struct segment {
  boolean isVert;
  boolean flipped;
  double comm_coord;  /* the common coordinate */
  paird p;      /* end points */
  bend l1, l2; 
  int ind_no;      /* index number of this segment in its channel */
  int track_no;    /* track number assigned in the channel */
  struct segment* prev;
  struct segment* next;
} segment;

typedef struct {
  int n;
  segment* segs;
} route;

typedef struct {
  Dtlink_t link;
  paird p;   /* extrema of channel */
  int cnt;   /* number of segments */
  segment** seg_list; /* array of segment pointers */
  rawgraph* G;
  struct cell* cp;
} channel;

#if 0
typedef struct {
  int i1, i2, j;
  int cnt;
  int* seg_list;  /* list of indices of the segment list */

  rawgraph* G;
} hor_channel;

typedef struct {
	hor_channel* hs;
	int cnt;
} vhor_channel;

typedef struct {
  int i, j1, j2;
  int cnt;
  int* seg_list;  /* list of indices of the segment list */

  rawgraph* G;
} vert_channel;

typedef struct {
	vert_channel* vs;
	int cnt;
} vvert_channel;
#endif

#define N_DAD(n) (n)->n_dad

#endif
