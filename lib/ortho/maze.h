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

#ifndef MAZE_H
#define MAZE_H

#include <sgraph.h>

enum {M_RIGHT=0, M_TOP, M_LEFT, M_BOTTOM};

#define MZ_ISNODE   1
#define MZ_VSCAN    2
#define MZ_HSCAN    4
#define MZ_SMALLV   8
#define MZ_SMALLH  16

  /* cell corresponds to node */
#define IsNode(cp) (cp->flags & MZ_ISNODE)  
  /* cell already inserted in vertical channel */
#define IsVScan(cp) (cp->flags & MZ_VSCAN)  
  /* cell already inserted in horizontal channel */
#define IsHScan(cp) (cp->flags & MZ_HSCAN)
  /* cell has small height corresponding to a small height node */
#define IsSmallV(cp) (cp->flags & MZ_SMALLV)
  /* cell has small width corresponding to a small width node */
#define IsSmallH(cp) (cp->flags & MZ_SMALLH)

typedef struct cell {
  int flags;
  int nedges;
  sedge* edges[6];
  int nsides;
  snode** sides;
  boxf  bb;
} cell;

typedef struct {
  int ncells, ngcells;
  cell* cells;     /* cells not corresponding to graph nodes */
  cell* gcells;    /* cells corresponding to graph nodes */
  sgraph* sg;
  Dt_t* hchans;
  Dt_t* vchans;
} maze;

extern maze* mkMaze (graph_t*);
extern void freeMaze (maze*);
void updateWts (sgraph* g, cell* cp, sedge* ep);
#ifdef DEBUG
extern int odb_flags;
#define ODB_MAZE   1
#define ODB_SGRAPH 2
#define ODB_ROUTE  4
#define ODB_CHANG  8
#endif
#endif
