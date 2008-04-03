#ifndef DOTIO_H
#define DOTIO_H

typedef char Agraphinfo_t;
typedef struct {
  int id;
} Agnodeinfo_t;
typedef char Agedgeinfo_t;

#include <graph.h>
#include "SparseMatrix.h"

extern void attach_embedding(Agraph_t *g, int dim, double sc, real *x);
extern SparseMatrix SparseMatrix_import_dot(Agraph_t *g, int dim, real **label_sizes, real **x, int format);
extern Agraph_t* makeDotGraph (SparseMatrix, char *title, int dim, real *x, int with_color, int with_label);

#endif
