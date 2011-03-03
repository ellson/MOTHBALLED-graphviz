/* $Id$Revision: */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#define STANDALONE
#include "general.h"
#include "DotIO.h"
#include "clustering.h"
#include "mq.h"
#include "spring_electrical.h"
#include "color_palette.h"
#include "colorutil.h"

typedef struct {
    Agrec_t h;
    unsigned int id;
} Agnodeinfo_t;

#define ND_id(n)  (((Agnodeinfo_t*)((n)->base.data))->id)

#if 0
static void
posStr (char* buf, int dim, real* x, double sc)
{
  if (dim== 3){
    sprintf (buf, "%f,%f,%f", sc*x[0], sc*x[1], sc*x[2]);
  } else if (dim == 2) {
    sprintf (buf, "%f,%f", sc*x[0], sc*x[1]);
  } else if (dim == 1) {
    sprintf (buf, "%f", sc*x[0]);
  } else {
    assert(0);
  }
}

static void 
attach_embedding (Agraph_t* g, int dim, double sc, real *x)
{
  Agsym_t* sym = agattr(g, AGNODE, "pos", NULL); 
  Agnode_t* n;
  char buf[1024];
  int i = 0;

  if (!sym)
    sym = agattr (g, AGNODE, "pos", "");

  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    assert (i == ND_id(n));
    posStr (buf, dim, x + i*dim, sc);
    agxset (n, sym, buf);
    i++;
  }
  
}
#endif

/* SparseMatrix_import_dot:
 * Assumes g is connected and simple, i.e., we can have a->b and b->a
 * but not a->b and a->b
 */
SparseMatrix 
SparseMatrix_import_dot (Agraph_t* g, int dim, real **label_sizes, real **x, int *n_edge_label_nodes, int **edge_label_nodes, int format, SparseMatrix *D)
{
  SparseMatrix A = 0;
  Agnode_t* n;
  Agedge_t* e;
  Agsym_t *sym, *symD = NULL;
  int nnodes;
  int nedges;
  int i, row;
  int* I;
  int* J;
  real *val, *valD = NULL;
  real v;
  int type = MATRIX_TYPE_REAL;
  real padding = 10;
  int nedge_nodes = 0;

  if (!g) return NULL;
  nnodes = agnnodes (g);
  nedges = agnedges (g);
  if (format != FORMAT_CSR) {
    fprintf (stderr, "Format %d not supported\n", format);
    exit (1);
  }

  /* Assign node ids */
  i = 0;
  for (n = agfstnode (g); n; n = agnxtnode (g, n))
    ND_id(n) = i++;

  I = N_NEW(nedges, int);
  J = N_NEW(nedges, int);
  val = N_NEW(nedges, real);

  sym = agattr(g, AGEDGE, "weight", NULL); 
  if (D) {
    symD = agattr(g, AGEDGE, "len", NULL); 
    valD = N_NEW(nedges, real);
  }
  i = 0;
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    if (edge_label_nodes && strncmp(agnameof(n), "|edgelabel|",11)==0) nedge_nodes++;
    row = ND_id(n);
    for (e = agfstout (g, n); e; e = agnxtout (g, e)) {
      I[i] = row;
      J[i] = ND_id(aghead(e));

      /* edge weight */
      if (sym) {
        if (sscanf (agxget(e,sym), "%lf", &v) != 1) v = 1;
      } else {
        v = 1;
      }
      val[i] = v;

      /* edge length */
      if (symD) {
        if (sscanf (agxget(e,symD), "%lf", &v) != 1) v = 1;
	valD[i] = v;
      }

      i++;
    }
  }
  
  if (edge_label_nodes) {
    *edge_label_nodes = MALLOC(sizeof(int)*nedge_nodes);
    nedge_nodes = 0;
  }

  *label_sizes = MALLOC(sizeof(real)*2*nnodes);
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    real sz;
    i = ND_id(n);
    if (edge_label_nodes && strncmp(agnameof(n), "|edgelabel|",11)==0) {
      (*edge_label_nodes)[nedge_nodes++] = i;
    }
    if (agget(n, "width") && agget(n, "height")){
      sscanf(agget(n, "width"), "%lf", &sz);
      /*      (*label_sizes)[i*2] = POINTS(sz)*.6;*/
      (*label_sizes)[i*2] = POINTS(sz)*.5 + padding*0.5;
      sscanf(agget(n, "height"), "%lf", &sz);
      /*(*label_sizes)[i*2+1] = POINTS(sz)*.6;*/
      (*label_sizes)[i*2+1] = POINTS(sz)*.5  + padding*0.5;
    } else {
      (*label_sizes)[i*2] = 4*POINTS(0.75)*.5;
      (*label_sizes)[i*2+1] = 4*POINTS(0.5)*.5;
    }
 }

  if (x){
    if (!(*x)) {
      *x = MALLOC(sizeof(real)*dim*nnodes);
      assert(*x);
    }
    for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
      real xx,yy, zz,ww;
      int nitems, k;
      i = ND_id(n);
      if (agget(n, "pos")){
	if (dim == 2){
	  nitems = sscanf(agget(n, "pos"), "%lf,%lf", &xx, &yy);
	  if (nitems != 2) return NULL;
	  (*x)[i*dim] = xx;
	  (*x)[i*dim+1] = yy;
	} else if (dim == 3){
	  nitems = sscanf(agget(n, "pos"), "%lf,%lf,%lf", &xx, &yy, &zz);
	  if (nitems != 3) return NULL;
	  (*x)[i*dim] = xx;
	  (*x)[i*dim+1] = yy;
	  (*x)[i*dim+2] = zz;
	} else if (dim == 4){
	  nitems = sscanf(agget(n, "pos"), "%lf,%lf,%lf,%lf", &xx, &yy, &zz,&ww);
	  if (nitems != 4) return NULL;
	  (*x)[i*dim] = xx;
	  (*x)[i*dim+1] = yy;
	  (*x)[i*dim+2] = zz;
	  (*x)[i*dim+3] = ww;
	} else if (dim == 1){
	  nitems = sscanf(agget(n, "pos"), "%lf", &xx);
	  if (nitems != 1) return NULL;
	  (*x)[i*dim] = xx;
	} else {
	  assert(0);
	}
      } else {
	for (k = 0; k < dim; k++) (*x)[i*dim + k] = 0;
      }
    }
  }

  A = SparseMatrix_from_coordinate_arrays(nedges, nnodes, nnodes, I, J, val, type);
  if (edge_label_nodes) *n_edge_label_nodes = nedge_nodes;

  if (D) *D = SparseMatrix_from_coordinate_arrays(nedges, nnodes, nnodes, I, J, valD, type);

  FREE(I);
  FREE(J);
  FREE(val);
  if (valD) FREE(valD);

  return A;
}


void dump_coordinates(char *name, int n, int dim, real *x){
  char fn[1000];
  FILE *fp;
  int i, k;

  if (!name){
    name = "";
  } else {
    name = strip_dir(name);
  }
  
  strcpy(fn, name);
  strcat(fn,".x");
  fp = fopen(fn,"w");
  fprintf(fp, "%d %d\n",n, dim);
  for (i = 0; i < n; i++){
    for (k = 0; k < dim; k++){
      fprintf(fp,"%f ",x[i*dim+k]);
    }
    fprintf(fp,"\n");
  }
  fclose(fp);

}

static Agnode_t*
mkNode (Agraph_t* g, char* name)
{
  Agnode_t* n = agnode(g, name, 1);
  agbindrec(n, "info", sizeof(Agnodeinfo_t), TRUE);
  return n;
}

Agraph_t* 
makeDotGraph (SparseMatrix A, char *name, int dim, real *x, int with_color, int with_label, int  use_matrix_value)
{
  Agraph_t* g;
  Agnode_t* n;
  Agnode_t* h;
  Agedge_t* e;
  int i, j;
  char buf[1024], buf2[1024];
  Agsym_t *sym, *sym2 = NULL, *sym3 = NULL;
  int* ia=A->ia;
  int* ja=A->ja;
  real* val = (real*)(A->a);
  Agnode_t** arr = N_NEW (A->m, Agnode_t*);
  real *color = NULL;
  char cstring[8];
  char *label_string;

  if (!name){
    name = "stdin";
  } else {
    name = strip_dir(name);
  }
  label_string = MALLOC(sizeof(char)*1000);

  if (SparseMatrix_known_undirected(A)){
    g = agopen ("G", Agundirected, 0);
  } else {
    g = agopen ("G", Agdirected, 0);
  }
  sprintf (buf, "%f", 1.0);

  label_string = strcpy(label_string, name);
  label_string = strcat(label_string, ". ");
  sprintf (buf, "%d", A->m);
  label_string = strcat(label_string, buf);
  label_string = strcat(label_string, " nodes, ");
  sprintf (buf, "%d", A->nz);
  label_string = strcat(label_string, buf);
  /*  label_string = strcat(label_string, " edges. Created by Yifan Hu");*/
  label_string = strcat(label_string, " edges.");


  if (with_label) sym = agattr(g, AGRAPH, "label", label_string); 
  sym = agattr(g, AGRAPH, "fontcolor", "#808090");
  if (with_color) sym = agattr(g, AGRAPH, "bgcolor", "black"); 
  sym = agattr(g, AGRAPH, "outputorder", "edgesfirst"); 

  if (A->m > 100) {
    /* -Estyle=setlinewidth(0.0)' -Eheadclip=false -Etailclip=false -Nstyle=invis*/
    agattr(g, AGNODE, "style", "invis"); 
  } else {
    /*    width=0, height = 0, label="", style=filled*/
    agattr(g, AGNODE, "shape", "point"); 
    if (A->m < 50){
      agattr(g, AGNODE, "width", "0.03"); 
    } else {
      agattr(g, AGNODE, "width", "0"); 
    }
    agattr(g, AGNODE, "label", ""); 
    agattr(g, AGNODE, "style", "filled"); 
    if (with_color) {
      agattr(g, AGNODE, "color", "#FF0000"); 
    } else {
      agattr(g, AGNODE, "color", "#000000"); 
    }
  }

  agattr(g, AGEDGE, "headclip", "false"); 
  agattr(g, AGEDGE, "tailclip", "false"); 
  if (A->m < 50){
    agattr(g, AGEDGE, "style", "setlinewidth(2)"); 
  } else if (A->m < 500){
    agattr(g, AGEDGE, "style", "setlinewidth(0.5)"); 
  } else if (A->m < 5000){
    agattr(g, AGEDGE, "style", "setlinewidth(0.1)"); 
  } else {
    agattr(g, AGEDGE, "style", "setlinewidth(0.0)"); 
  }

  if (with_color) {
    sym2 = agattr(g, AGEDGE, "color", ""); 
    sym3 = agattr(g, AGEDGE, "wt", ""); 
  }
  for (i = 0; i < A->m; i++) {
    sprintf (buf, "%d", i);
    n = mkNode (g, buf);
    ND_id(n) = i;
    arr[i] = n;
  }

  if (with_color){
    real maxdist = 0.;
    real mindist = 0.;
    int first = TRUE;
    color = malloc(sizeof(real)*A->nz);
    for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
      i = ND_id(n);
      if (A->type != MATRIX_TYPE_REAL || use_matrix_value){
	for (j = ia[i]; j < ia[i+1]; j++) {
	  color[j] = distance(x, dim, i, ja[j]);
	  if (i != ja[j]){
	    if (first){
	      mindist = color[j];
	      first = FALSE;
	    } else {
	      mindist = MIN(mindist, color[j]);
	    }
	  }
	  maxdist = MAX(color[j], maxdist);
	}
      } else {
	for (j = ia[i]; j < ia[i+1]; j++) {
	  color[j] = ABS(val[j]);
	  if (i != ja[j]){
	    if (first){
	      mindist = color[j];
	      first = FALSE;
	    } else {
	      mindist = MIN(mindist, color[j]);
	    }
	  }
	  maxdist = MAX(color[j], maxdist);
	}
      }
    }
    for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
      i = ND_id(n);
      for (j = ia[i]; j < ia[i+1]; j++) {
	color[j] = ((color[j] - mindist)/MAX(maxdist-mindist, 0.000001));
      }
    }
  }

  i = 0;
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    i = ND_id(n);
    for (j = ia[i]; j < ia[i+1]; j++) {
      h = arr [ja[j]];
      if (val){
	switch (A->type){
	case MATRIX_TYPE_REAL:
	  sprintf (buf, "%f", ((real*)A->a)[j]);
	  break;
	case MATRIX_TYPE_INTEGER:
	  sprintf (buf, "%d", ((int*)A->a)[j]);
	  break;
	case MATRIX_TYPE_COMPLEX:/* take real part as weight */
	  sprintf (buf, "%f", ((real*)A->a)[2*j]);
	  break;
	}
	if (with_color) sprintf (buf2, "%s", hue2rgb(.65*color[j], cstring));
      } else {
	sprintf (buf, "%f", 1.);
 	if (with_color) sprintf (buf2, "%s", hue2rgb(.65*color[j], cstring));
     }
      e = agedge (g, n, h, NULL, 1);
      if (with_color) {
	agxset (e, sym2, buf2);
	sprintf (buf2, "%f", color[j]);
	agxset (e, sym3, buf2);
      }
    }
  }
  
  FREE(color);
  FREE (arr);
  FREE(label_string);
  return g;
}


char *cat_string(char *s1, char *s2){
  char *s;
  s = malloc(sizeof(char)*(strlen(s1)+strlen(s2)+1+1));
  strcpy(s,s1);
  strcat(s,"|");
  strcat(s,s2);
  return s;
}

char *cat_string3(char *s1, char *s2, char *s3, int id){
  char *s;
  char sid[1000];
  sprintf(sid,"%d",id);
  s = malloc(sizeof(char)*(strlen(s1)+strlen(s2)+strlen(s3)+strlen(sid)+1+3));
  strcpy(s,s1);
  strcat(s,"|");
  strcat(s,s2);
  strcat(s,"|");
  strcat(s,s3);
  strcat(s,"|");
  strcat(s,sid);
  return s;
}


Agraph_t *convert_edge_labels_to_nodes(Agraph_t* g){
  Agnode_t *n, *newnode;
  Agraph_t *dg;

  int nnodes;
  int nedges;


  Agedge_t *ep, *e;
  int i = 0;
  Agnode_t **ndmap = NULL;
  char *s;
  char *elabel;
  int id = 0;

  Agsym_t* sym = agattr(g, AGEDGE, "label", NULL);

  dg = agopen("test", g->desc, 0);

  if (!g) return NULL;
  nnodes = agnnodes (g);
  nedges = agnedges (g);

  ndmap = malloc(sizeof(Agnode_t *)*nnodes);

  agattr(dg, AGNODE, "label", "\\N"); 
  agattr(dg, AGNODE, "shape", "ellipse"); 
  agattr(dg, AGNODE, "width","0.00001");
  agattr(dg, AGNODE, "height", "0.00001");
  agattr(dg, AGNODE, "margin","0.");
  agattr(dg, AGEDGE, "arrowsize", "0.5"); 

  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    newnode = mkNode(dg, agnameof(n));
    agset(newnode,"shape","box");
    ndmap[i] = newnode;
    ND_id(n) = i++;
  }


  /*
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    for (ep = agfstedge(g, n); ep; ep = agnxtedge(g, ep, n)) {
      if (agtail(ep) == n) continue;
      agedge(dg, ndmap[ND_id(agtail(ep))], ndmap[ND_id(aghead(ep))]);
    }
  }
  */



  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    for (ep = agfstedge(g, n); ep; ep = agnxtedge(g, ep, n)) {
      if (agtail(ep) == n && (aghead(ep) != n)) continue;
      if (sym && (elabel = agxget(ep,sym)) && strcmp(elabel,"") != 0) {
	s = cat_string3("|edgelabel",agnameof(agtail(ep)), agnameof(aghead(ep)), id++);
	newnode = mkNode(dg,s);
	agset(newnode,"label",elabel);
	agset(newnode,"shape","plaintext");
	e = agedge(dg, ndmap[ND_id(agtail(ep))], newnode, NULL, 1);
	agset(e, "arrowsize","0");
	agedge(dg, newnode, ndmap[ND_id(aghead(ep))], NULL, 1);
	free(s);
      } else {
	agedge(dg, ndmap[ND_id(agtail(ep))], ndmap[ND_id(aghead(ep))], NULL, 1);
      }
    }
  }

  free(ndmap);
  return dg;
 }

Agraph_t* assign_random_edge_color(Agraph_t* g){
  char cstring[8];
  char buf2[1024];
  Agsym_t *sym2;
  Agnode_t* n;
  Agedge_t *ep;

  sym2 = agattr(g, AGEDGE, "color", ""); 
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    for (ep = agfstedge(g, n); ep; ep = agnxtedge(g, ep, n)) {
      sprintf (buf2, "%s", hue2rgb(0.65*drand(), cstring));
      agxset (ep, sym2, buf2);
    }
  }

  return g;
 }

void Dot_SetClusterColor(Agraph_t* g, float *rgb_r,  float *rgb_g,  float *rgb_b, int *clusters){

  Agnode_t* n;
  char scluster[20];
  int i;
  Agsym_t* clust_clr_sym = agattr(g, AGNODE, "clustercolor", NULL); 

  if (!clust_clr_sym) clust_clr_sym = agattr(g, AGNODE, "clustercolor", "-1"); 
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    i = ND_id(n);
    if (rgb_r && rgb_g && rgb_b) {
      rgb2hex((rgb_r)[(clusters)[i]],(rgb_g)[(clusters)[i]],(rgb_b)[(clusters)[i]], scluster);
      //sprintf(scluster,"#%2x%2x%2x", (int) (255*((rgb_r)[(clusters)[i]])), (int) (255*((rgb_g)[(clusters)[i]])), (int) (255*((rgb_b)[(clusters)[i]])));
    }
    agxset(n,clust_clr_sym,scluster);
  }
}

SparseMatrix Import_coord_clusters_from_dot(Agraph_t* g, int maxcluster, int dim, int *nn, real **label_sizes, real **areas, real **x, int **clusters, float **rgb_r,  float **rgb_g,  float **rgb_b,  float **fsz, char ***labels, int default_color_scheme, int clustering_scheme){
  SparseMatrix A = 0;
  Agnode_t* n;
  Agedge_t* e;
  Agsym_t* sym;
  Agsym_t* clust_sym;
  Agsym_t* clust_clr_sym;
  int nnodes;
  int nedges;
  int i, row, ic,nc, j;
  int* I;
  int* J;
  real* val;
  real v;
  int type = MATRIX_TYPE_REAL;
  char scluster[100];
  float ff;
  real* areap = NULL;

  int MAX_GRPS, MIN_GRPS, noclusterinfo = FALSE, first = TRUE;
  float *pal;

  switch (default_color_scheme){
  case COLOR_SCHEME_BLUE_YELLOW:
    pal = &(palette_blue_to_yellow[0][0]);
    break;
  case COLOR_SCHEME_WHITE_RED:
    pal = &(palette_white_to_red[0][0]);
    break;
  case COLOR_SCHEME_GREY_RED:
    pal = &(palette_grey_to_red[0][0]);
    break;
  case COLOR_SCHEME_GREY:
    pal = &(palette_grey[0][0]);
    break;
  case COLOR_SCHEME_PASTEL:
    pal = &(palette_pastel[0][0]);
    break;
  case COLOR_SCHEME_SEQUENTIAL_SINGLEHUE_RED:
    fprintf(stderr," HERE!\n");
    pal = &(palette_sequential_singlehue_red[0][0]);
    break;
  case COLOR_SCHEME_SEQUENTIAL_SINGLEHUE_RED_LIGHTER:
    fprintf(stderr," HERE!\n");
    pal = &(palette_sequential_singlehue_red_lighter[0][0]);
    break;
  case COLOR_SCHEME_PRIMARY:
    pal = &(palette_primary[0][0]);
    break;
  case COLOR_SCHEME_NONE:
    pal = NULL;
    break;
  default:
    pal = &(palette_pastel[0][0]);
    break;
  }
    
  if (!g) return NULL;
  nnodes = agnnodes (g);
  nedges = agnedges (g);
  *nn = nnodes;

  /* Assign node ids */
  i = 0;
  for (n = agfstnode (g); n; n = agnxtnode (g, n))
    ND_id(n) = i++;

  /* form matrix */  
  I = N_NEW(nedges, int);
  J = N_NEW(nedges, int);
  val = N_NEW(nedges, real);

  sym = agattr(g, AGEDGE, "weight", NULL); 
  clust_sym = agattr(g, AGNODE, "cluster", NULL); 
  clust_clr_sym = agattr(g, AGNODE, "clustercolor", NULL); 
  //sym = agattr(g, AGEDGE, "wt", NULL); 
  i = 0;
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    row = ND_id(n);
    for (e = agfstout (g, n); e; e = agnxtout (g, e)) {
      I[i] = row;
      J[i] = ND_id(aghead(e));
      if (sym) {
        if (sscanf (agxget(e,sym), "%lf", &v) != 1)
          v = 1;
      }
      else
        v = 1;
      val[i] = v;
      i++;
    }
  }
  A = SparseMatrix_from_coordinate_arrays(nedges, nnodes, nnodes, I, J, val, type);

  /* get clustering info */
  nc = 1;
  MIN_GRPS = 0;
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    i = ND_id(n);
    if (clust_sym && (sscanf(agxget(n,clust_sym), "%d", &ic)>0)) {
      nc = MAX(nc, ic);
      if (first){
	MIN_GRPS = ic;
	first = FALSE;
      } else {
	MIN_GRPS = MIN(MIN_GRPS, ic);
      }
    } else {
      noclusterinfo = TRUE;
    }
  }
  MAX_GRPS = nc;

  *clusters = MALLOC(sizeof(int)*nnodes);

  if (noclusterinfo) {
    int use_value = TRUE, flag = 0;
    real modularity;
    if (!clust_sym) clust_sym = agattr(g,AGNODE,"cluster","-1");

    if (clustering_scheme == CLUSTERING_MQ){
      mq_clustering(A, FALSE, maxcluster, use_value,
		    &nc, clusters, &modularity, &flag);
    } else if (clustering_scheme == CLUSTERING_MODULARITY){ 
      modularity_clustering(A, FALSE, maxcluster, use_value,
		    &nc, clusters, &modularity, &flag);
    } else {
      assert(0);
    }
    for (i = 0; i < nnodes; i++) (*clusters)[i]++;/* make into 1 based */
    for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
      i = ND_id(n);
      sprintf(scluster,"%d",(*clusters)[i]);
      agxset(n,clust_sym,scluster);
    }
    MIN_GRPS = 1;
    MAX_GRPS = nc;
    if (Verbose){
      fprintf(stderr," no complement clustering info in dot file, using modularity clustering. Modularity = %f, ncluster=%d\n",modularity, nc);
    }
  }

  *label_sizes = MALLOC(sizeof(real)*dim*nnodes);
  if (pal){
    *rgb_r = MALLOC(sizeof(float)*(1+MAX_GRPS));
    *rgb_g = MALLOC(sizeof(float)*(1+MAX_GRPS));
    *rgb_b = MALLOC(sizeof(float)*(1+MAX_GRPS));
  } else {
    *rgb_r = NULL;
    *rgb_g = NULL;
    *rgb_b = NULL;
  }
  *fsz = MALLOC(sizeof(float)*nnodes);
  *labels = MALLOC(sizeof(char*)*nnodes);

  if (areas){
    *areas = areap = MALLOC(sizeof(real)*nnodes);
  }

  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    gvcolor_t color;
    real sz;
    i = ND_id(n);
    if (agget(n, "width") && agget(n, "height")){
      sscanf(agget(n, "width"), "%lf", &sz);
      (*label_sizes)[i*2] = POINTS(sz*0.5);
      sscanf(agget(n, "height"), "%lf", &sz);
      (*label_sizes)[i*2+1] = POINTS(sz*0.5);
    } else {
      (*label_sizes)[i*2] = POINTS(0.75/2);
      (*label_sizes)[i*2+1] = POINTS(0.5*2);
    }

    if (areap) {
      char* s;
      double d;
      if ((s = agget(n, "area")) && sscanf(s, "%lf", &d))
        areap[i] = d;
      else
        areap[i] = 1.0;
    }

    if (!noclusterinfo && (sscanf(agxget(n, clust_sym), "%d", &ic)>0)) {
      (*clusters)[i] = ic;
    }


   if (agget(n, "fontsize")){
      sscanf(agget(n, "fontsize"), "%f", &ff);
      (*fsz)[i] = ff;
    } else {
     (*fsz)[i] = 14;
    }

   if (agget(n, "label") && strcmp(agget(n, "label"), "") != 0 && strcmp(agget(n, "label"), "\\N") != 0){
     char *lbs;
     lbs = agget(n, "label");
      (*labels)[i] = MALLOC(sizeof(char)*(strlen(lbs)+1));
      strcpy((*labels)[i], lbs);
    } else {
     (*labels)[i] = MALLOC(sizeof(char)*(strlen(agnameof(n))+1));
      strcpy((*labels)[i], agnameof(n));
    }



   j = (*clusters)[i];
   if (MAX_GRPS-MIN_GRPS < MAX_COLOR) {
     j = (j-MIN_GRPS)*((int)((MAX_COLOR-1)/MAX((MAX_GRPS-MIN_GRPS),1)));
   } else {
     j = (j-MIN_GRPS)%MAX_COLOR;
   }

   if (pal){
     //     assert((*clusters)[i] >= 0 && (*clusters)[i] <= MAX_GRPS);
     (*rgb_r)[(*clusters)[i]] = pal[3*j+0];
     (*rgb_g)[(*clusters)[i]] =  pal[3*j+1];
     (*rgb_b)[(*clusters)[i]] = pal[3*j+2];
   }

   if (!noclusterinfo && clust_clr_sym && (colorxlate(agxget(n,clust_clr_sym),&color,RGBA_DOUBLE) == COLOR_OK)) {
     (*rgb_r)[(*clusters)[i]] = color.u.RGBA[0];
     (*rgb_g)[(*clusters)[i]] = color.u.RGBA[1];
     (*rgb_b)[(*clusters)[i]] = color.u.RGBA[2];
   }


  }


  if (x){
    *x = MALLOC(sizeof(real)*dim*nnodes);
    for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
      real xx,yy;
      i = ND_id(n);
      if (agget(n, "pos")){
	sscanf(agget(n, "pos"), "%lf,%lf", &xx, &yy);
	(*x)[i*dim] = xx;
	(*x)[i*dim+1] = yy;
      } else {
	fprintf(stderr,"WARNING: pos field missing for node %d, set to origin\n",i);
	(*x)[i*dim] = 0;
	(*x)[i*dim+1] = 0;
      }
    }
  }


  FREE(I);
  FREE(J);
  FREE(val);

  return A;
}


void initDotIO (Agraph_t *g)
{
  aginit(g, AGNODE, "info", sizeof(Agnodeinfo_t), TRUE);
}

