#ifndef HIER_H
#define HIER_H

#include "hierarchy.h"
#include "topview.h"
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
Hierarchy* makeHier (int nnodes, int nedges, vtx_data*, double*, double*);
focus_t* initFocus (int ncnt);

#endif
