#ifndef HIER_H
#define HIER_H

#include "topview.h"
#include "hierarchy.h"

typedef struct {
    int num_foci;
    int* foci_nodes;
    double* x_foci;
    double* y_foci;
} focus_t;

typedef struct {
    int graphSize;
    int ClientWidth;
    int ClientHeight;
    int margin;
    RescaleType rescale_type; // use Polar by default
} hierparms_t;

void positionAllItems (Hierarchy* hp, focus_t* fs, hierparms_t* parms);
vtx_data* makeGraph (topview* tv, int* nedges);
Hierarchy* makeHier (topview*, vtx_data*, double*, double*);

#endif
