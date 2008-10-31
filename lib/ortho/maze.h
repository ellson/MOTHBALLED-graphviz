/* vim:set shiftwidth=4 ts=8: */
#ifndef MAZE_H
#define MAZE_H

#include <sgraph.h>

enum {M_RIGHT=0, M_TOP, M_LEFT, M_BOTTOM};

#define MZ_ISNODE  1
#define MZ_VSCAN   2
#define MZ_HSCAN   4

#define IsNode(cp) (cp->flags & MZ_ISNODE)  /* cell corresponds to node */
  /* cell already inserted in vertical channel */
#define IsVScan(cp) (cp->flags & MZ_VSCAN)  
  /* cell already inserted in horizontal channel */
#define IsHScan(cp) (cp->flags & MZ_HSCAN)

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
#endif
