#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "mmio.h"
#include "SparseMatrix.h"
#include "matrix_market.h"
#include "QuadTree.h"
#include "spring_electrical.h"
#include "Multilevel.h"
#include "general.h"
#ifdef HAVE_DOT
#include "DotIO.h"
#endif

#ifdef HAVE_DOT
#define FMT_NONE 0
#define FMT_MATHEMATICA 1
#define FMT_DOT 2
#define FMT_COORDINATES 3

int format = FMT_DOT;
double scale = 1;
#endif

int Verbose;

#ifdef HAVE_DOT
#if 0
static void scale_to_box(real xmin, real ymin, real xmax, real ymax, int n, int dim, real *x){
  real min[3], max[3], min0[3], ratio = 1;
  int i, k;

  for (i = 0; i < dim; i++) {
    min[i] = x[i];
    max[i] = x[i];
  }

  for (i = 0; i < n; i++){
    for (k = 0; k < dim; k++) {
      min[k] = MIN(x[i*dim+k], min[k]);
      max[k] = MAX(x[i*dim+k], max[k]);
    }
  }

  if (max[0] - min[0] != 0) {
    ratio = (xmax-xmin)/(max[0] - min[0]);
  }
  if (max[1] - min[1] != 0) {
    ratio = MIN(ratio, (ymax-ymin)/(max[1] - min[1]));
  }
  
  min0[0] = xmin;
  min0[1] = ymin;
  min0[2] = 0;
  for (i = 0; i < n; i++){
    for (k = 0; k < dim; k++) {
      x[i*dim+k] = min0[k] + (x[i*dim+k] - min[k])*ratio;
    }
  }
  
  
}
#endif
#endif
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

static void usage (char* cmd, int eval)
{
    fprintf(stderr, "Usage: %s <options> filename\n", cmd);
    fprintf(stderr, "Options are\n");
    fprintf(stderr, "-m nlevels     number of multilevels, default infinity\n");
    fprintf(stderr, "-p x           power of repulsive force, default is usually -1, sometimes -1.8. Must be a negative real number.\n");
    /*    fprintf(stderr, "-b             whether to beautify leaf nodes. Not used by default.\n");*/
    fprintf(stderr, "-u             whether the graph is assumed to be undriected with no loops. Default is directed.\n");
    fprintf(stderr, "-d             dimension to generate layout. 2 (default) or 3.\n");
    fprintf(stderr, "-n             whether to use node weights. Not used by default.\n");
    fprintf(stderr, "-v             verbose output.\n");
    fprintf(stderr, "-e             whether to expand the final layout for even edge length: 0 (no), 1 (using stress majorization), 2 (using a combined spring-lectrical and spring-lectrical and sprinf model., 3 (using triangular graph), 4 (using neighborhood graph)\n");
    fprintf(stderr, "-l             whether to remove overlap, integer, which specify the number of iterations\n");
#ifdef HAVE_DOT
    fprintf(stderr, "-s x           scale output in dot by x.\n");
#endif
    fprintf(stderr, "-o file        output file name\n");
#ifdef HAVE_DOT
    fprintf(stderr, "-T fmt         set output format: m(athematica) d(ot) c(oordinate)\n");
#endif
    exit(eval);
}



static void init(int argc, char *argv[], spring_electrical_control ctrl, char **infile, char **outfile, int *dim, int *undirected){

  unsigned int c;
  real p;    
  int m, ee, l;
  char* cmd = argv[0];
  
  *infile = NULL;
  *outfile = NULL;
  Verbose = FALSE;
  *dim = 2;
  *undirected = FALSE;

  ctrl->p = AUTOP;
  ctrl->multilevels = MAXINT;
  ctrl->beautify_leaves = FALSE;
  ctrl->use_node_weights = FALSE;
  
#ifdef HAVE_DOT
  while ((c = getopt(argc, argv, ":ubs:vno:p:m:T:d:e:l:")) != -1) {
#else
  while ((c = getopt(argc, argv, ":ubvno:p:m:d:e:l:")) != -1) {
#endif
    switch (c) {
    case 'u':
      *undirected = TRUE;
      break;
    case 'b':
      ctrl->beautify_leaves = TRUE;
      break;
    case 'n':
      ctrl->use_node_weights = TRUE;
      break;
    case 'v':
      Verbose = TRUE;
      break;
    case 'o':
      *outfile = optarg;
      break;
#ifdef HAVE_DOT
    case 's':
      if ((sscanf(optarg,"%lf",&p) <= 0) || (p <= 0)) {
	fprintf(stderr, "-s arg %s must be a positive real number\n",optarg);
      }
      else
	scale = p;
      break;
    case 'T':
      switch (*optarg) {
      case 'm' :
	format = FMT_MATHEMATICA;
	break;
      case 'd' :
	format = FMT_DOT;
	break;
      case 'c' :
	format = FMT_COORDINATES;
	break;
      default :
	fprintf(stderr, "option -T%s unrecognized - ignored\n", optarg);
	break;
      }
      break;
#endif
    case 'l':
      if ((sscanf(optarg,"%d",&l) <= 0) || (l < 0)) {
	fprintf(stderr, "-l arg %s must be a non-negative integer\n",optarg);
      }
      else
	ctrl->overlap = l;
      break;
    case 'p':
      if ((sscanf(optarg,"%lf",&p) <= 0) || (p >= 0)) {
	fprintf(stderr, "-p arg %s must be a negative real number\n",optarg);
      }
      else
	ctrl->p = p;
      break;
    case 'e':
      if (sscanf(optarg,"%d",&ee) <= 0) {
	fprintf(stderr, "-e arg %s must be an integer, 0 (no expansion), 1 (sm expansion graph dist), 2 (sm avg dist), 3 (sm power dist), 4 (spring expansion), 5 (triangularization), 6 (RNG)\n",optarg);
      }
      else
	ctrl->smoothing = ee;
      break;
    case 'd':
      if (sscanf(optarg,"%d",dim) <= 0) {
	fprintf(stderr, "-d arg %s must be an integer\n",optarg);
	*dim = 2;
      }
      break;
    case 'm':
      if (sscanf(optarg,"%d",&m) <= 0) {
	fprintf(stderr, "-m arg %s must be an integer\n",optarg);
	ctrl->multilevels = 1<<30;
      }
      else
	ctrl->multilevels = m;
      break;
    case '?':
      if (optopt == '?')
	usage(cmd, 0);
      else
	fprintf(stderr, "gc: option -%c unrecognized - ignored\n",
		optopt);
      break;
    }
  }
  argv += optind;
  argc -= optind;
  
  if (argc)
    *infile = argv[0];
  
}

#ifdef HAVE_DOT
static int
isDotFile (char* fname)
{
  char* dotp = strrchr (fname, '.');
  return (dotp && !strcmp (dotp+1, "dot"));
}
#endif

int main(int argc, char *argv[])
{
#ifdef HAVE_DOT
  Agraph_t* g = 0;
  int dotFile = 0;
#endif
  char *infile, *outfile;
  FILE *f;
  SparseMatrix A = NULL;
  spring_electrical_control ctrl = spring_electrical_control_new();
  int dim;
  real *x;
  int flag, undirected;
  real *label_sizes = NULL;
  /*  int with_color = FALSE, with_label = FALSE;*/
  int with_color = FALSE, with_label = FALSE;
  int i;

  init(argc, argv, ctrl, &infile, &outfile, &dim, &undirected);

  /* ======================= read graph ==================== */
  if (infile) {
#ifdef HAVE_DOT
    dotFile = isDotFile (infile);
#endif
    f = openF (infile, "r");
  } else {
    f = stdin;
  }

#ifdef HAVE_DOT
  if (dotFile) {
    aginit ();
    g = agread (f);
    A = SparseMatrix_import_dot(g, dim, &label_sizes, &x, FORMAT_CSR);
  } else {
#endif
    A = SparseMatrix_import_matrix_market(f, FORMAT_CSR);
#ifdef HAVE_DOT
  }
#endif

  if (!A) {
#ifdef HAVE_DOT
    if (f == stdin){
      aginit ();
      g = agread (f);
      A = SparseMatrix_import_dot(g, dim, &label_sizes, &x, FORMAT_CSR);
      if (A) dotFile = TRUE;
    }
#endif
    if (!A){
      fprintf(stderr,"can not open file %s\n",infile);
      exit(1);
    }
  }

  if (A && !label_sizes){
    label_sizes = MALLOC(sizeof(real)*2*A->m);
    for (i = 0; i < 2*A->m; i++) label_sizes[i] = 72;
  }
  if (infile) fclose(f);


  /* ====== layout ==========*/

  if (!SparseMatrix_connectedQ(A)) fprintf(stderr,"Warning: the graph is disconnected!!\n");
  if (!x) {
    x = MALLOC(sizeof(real)*A->m*dim);
  } else {
#ifdef OVERLAP
      FILE *fp;
      fp = fopen("x0","w");
      for (i = 0; i < A->m; i++) fprintf(fp, "%lf %lf\n",x[2*i],x[2*i+1]);
      fclose(fp);
#endif
  }

  /* using node degree as node weight */
#if 0
  {real *nodewgts;
  int i;
  nodewgts = MALLOC(sizeof(real)*A->m);
  for (i = 0; i < A->m; i++) nodewgts[i] = A->ia[i+1]-A->ia[i];
  multilevel_spring_electrical_embedding(dim, A, ctrl, nodewgts, label_sizes, x, &flag);
  }
#else

  multilevel_spring_electrical_embedding(dim, A, ctrl, NULL, label_sizes, x, &flag);
#endif
  spring_electrical_control_delete(ctrl); 

  if (undirected) {
    SparseMatrix B;
    B = SparseMatrix_make_undirected(A);
    SparseMatrix_delete(A);
    A = B;
#ifdef HAVE_DOT
    if (g) {
      /* free g first!! */
      g = makeDotGraph (A, infile, dim, x, with_color, with_label);
    } else {
      g = makeDotGraph (A, infile, dim, x, with_color, with_label);
    }
#endif
  }

  if (outfile)
      f = openF (outfile,"w");
  else
      f = stdout;
  
#ifdef HAVE_DOT
  if (format == FMT_NONE) {
    if (dotFile) 
      format = FMT_DOT;
    else 
      format = FMT_MATHEMATICA;
  }  

  switch (format){
  case FMT_DOT:
    if (!dotFile){ 
      g = makeDotGraph (A, infile, dim, x, with_color, with_label);
      
    } 
    
    /*    scale_to_box(0,0,7*70,10*70,A->m,dim,x);*/
    /*    scale_to_box(0,0,21*70,30*70,A->m,dim,x);*/
    attach_embedding(g, dim, scale, x);
    
    agwrite (g, f);
    break;
 case FMT_MATHEMATICA:
#endif
   export_embedding(f, dim, A, x, NULL);
   break;
  case FMT_COORDINATES:{
    int i, k;
    for (i = 0; i < A->m; i++){
      for (k = 0; k < dim; k++){
	fprintf(f, "%f ",x[i*dim+k]);
      }
      fprintf(f,"\n");
    }
  }
  }
  if (outfile) fclose(f);

#ifdef OVERLAP
  fprintf(stderr, "%d nodes and %d edges\n",A->m,A->nz);
#endif

  SparseMatrix_delete(A);
  FREE(x);
  if (label_sizes) FREE(label_sizes);
  return 0;
}
