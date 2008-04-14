#include <stdio.h>
#include <stdlib.h>
#include "general.h"
#include "SparseMatrix.h"
#include "DotIO.h"

int Verbose;

static FILE* openF (char* fname, char* mode)
{
    FILE* f = fopen (fname, mode);
    if (!f) {
        fprintf (stderr, "Could not open %s for %s\n", fname,
            ((*mode == 'r') ? "reading" : "writing"));
        exit (1);
    }
    return f;
}

real distance(real *x, int dim, int i, int j){
  int k;
  real dist = 0.;
  for (k = 0; k < dim; k++) dist += (x[i*dim+k] - x[j*dim + k])*(x[i*dim+k] - x[j*dim + k]);
  dist = sqrt(dist);
  return dist;
}


int main(int argc, char *argv[])
{
  Agraph_t* g = 0;
  char *infile;
  FILE *f;
  real *x;
  real *label_sizes = NULL;
  int i;
  SparseMatrix A;

  /* ======================= read graph ==================== */
  if (argc < 2) {
    fprintf(stderr, "usage: dot2coord dotfile");
    exit(1);
  }
  infile = argv[1];
  f = openF (infile, "r");

  aginit ();
  g = agread (f);
  A = SparseMatrix_import_dot(g, 2, &label_sizes, &x, FORMAT_CSR);

  for (i = 0; i < A->m; i++) fprintf(stdout, "%lf %lf\n",x[2*i],x[2*i+1]);

  SparseMatrix_delete(A);
  FREE(x);
  if (label_sizes) FREE(label_sizes);
  return 0;
}
