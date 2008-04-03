#ifdef HAVE_DOT
#include "DotIO.h"
#include "spring_electrical.h"

#define ND_id(n) (n)->u.id

real Hue2RGB(real v1, real v2, real H) {
  if(H < 0.0) H += 1.0;
  if(H > 1.0) H -= 1.0;
  if((6.0*H) < 1.0) return (v1 + (v2 - v1) * 6.0 * H);
  if((2.0*H) < 1.0) return v2;
  if((3.0*H) < 2.0) return (v1 + (v2 - v1) * ((2.0/3.0) - H) * 6.0);
  return v1;
}

char *hex[16]={"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};

char * hue2rgb(real hue, char *color){
  real v1, v2, lightness = .5, saturation = 1;
  int red, blue, green;

  if(lightness < 0.5) 
    v2 = lightness * (1.0 + saturation);
  else
    v2 = (lightness + saturation) - (saturation * lightness);

  v1 = 2.0 * lightness - v2;

  red =   (int)(255.0 * Hue2RGB(v1, v2, hue + (1.0/3.0)) + 0.5);
  green = (int)(255.0 * Hue2RGB(v1, v2, hue) + 0.5);
  blue =  (int)(255.0 * Hue2RGB(v1, v2, hue - (1.0/3.0)) + 0.5);
  color[0] = '#';
  sprintf(color+1,"%s",hex[red/16]);
  sprintf(color+2,"%s",hex[red%16]);
  sprintf(color+3,"%s",hex[green/16]);
  sprintf(color+4,"%s",hex[green%16]);
  sprintf(color+5,"%s",hex[blue/16]);
  sprintf(color+6,"%s",hex[blue%16]);
  color[7] = '\0';
  return color;
}

static void
posStr (char* buf, int dim, real* x, double sc)
{
  if (dim== 3){
    sprintf (buf, "%f,%f,%f", sc*x[0], sc*x[1], sc*x[2]);
  } else {
    sprintf (buf, "%f,%f", sc*x[0], sc*x[1]);
  }
}

void 
attach_embedding (Agraph_t* g, int dim, double sc, real *x)
{
  Agsym_t* sym = agfindattr(g->proto->n, "pos"); 
  Agnode_t* n;
  char buf[1024];
  int i = 0;

  if (!sym)
    sym = agnodeattr (g, "pos", "");

  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    assert (i == ND_id(n));
    posStr (buf, dim, x + i*dim, sc);
    agxset (n, sym->index, buf);
    i++;
  }
  
}

/* SparseMatrix_import_dot:
 * Assumes g is connected and simple, i.e., we can have a->b and b->a
 * but not a->b and a->b
 */
SparseMatrix 
SparseMatrix_import_dot (Agraph_t* g, int dim, real **label_sizes, real **x, int format)
{
  SparseMatrix A = 0;
  Agnode_t* n;
  Agedge_t* e;
  Agsym_t* sym;
  int nnodes;
  int nedges;
  int i, row;
  int* I;
  int* J;
  real* val;
  real v;
  int type = MATRIX_TYPE_REAL;
  real padding = 10;

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

  sym = agfindattr(g->proto->e, "wt"); 
  i = 0;
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    row = ND_id(n);
    for (e = agfstout (g, n); e; e = agnxtout (g, e)) {
      I[i] = row;
      J[i] = ND_id(e->head);
      if (sym) {
        if (sscanf (agxget (e, sym->index), "%lf", &v) != 1)
          v = 1;
      }
      else
        v = 1;
      val[i] = v;
      i++;
    }
  }
  
  *label_sizes = MALLOC(sizeof(real)*2*nnodes);
  for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
    real sz;
    i = ND_id(n);
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
    *x = MALLOC(sizeof(real)*dim*nnodes);
    for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
      real xx,yy;
      i = ND_id(n);
      if (agget(n, "pos")){
	sscanf(agget(n, "pos"), "%lf,%lf", &xx, &yy);
	(*x)[i*dim] = xx;
	(*x)[i*dim+1] = yy;
      } else {
	(*x)[i*dim] = 0;
	(*x)[i*dim+1] = 0;
      }
    }
  }

  A = SparseMatrix_from_coordinate_arrays(nedges, nnodes, nnodes, I, J, val, type);

  FREE(I);
  FREE(J);
  FREE(val);

  return A;
}

char *strip_dir(char *s){
  int i, first = TRUE;
  for (i = strlen(s); i >= 0; i--) {
    if (first && s[i] == '.') {/* get rid of .mtx */
      s[i] = '\0';
      first = FALSE;
    }
    if (s[i] == '/') return (char*) &(s[i+1]);
  }
  return s;
}

Agraph_t* 
makeDotGraph (SparseMatrix A, char *name, int dim, real *x, int with_color, int with_label)
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
  aginit ();
  if (SparseMatrix_known_undirected(A)){
    g = agopen ("G", AGRAPH);
  } else {
    g = agopen ("G", AGDIGRAPH);
  }
  sprintf (buf, "%f", 1.0);

  label_string = strcpy(label_string, name);
  label_string = strcat(label_string, ". ");
  sprintf (buf, "%d", A->m);
  label_string = strcat(label_string, buf);
  label_string = strcat(label_string, " nodes, ");
  sprintf (buf, "%d", A->nz);
  label_string = strcat(label_string, buf);
  label_string = strcat(label_string, " edges.");


  if (with_label) sym = agraphattr(g, "label", label_string); 
  sym = agraphattr(g, "fontcolor", "#808090");
  if (with_color) sym = agraphattr(g, "bgcolor", "black"); 
  sym = agraphattr(g, "outputorder", "edgesfirst"); 

  if (A->m > 10000) {
    /* -Estyle=setlinewidth(0.0)' -Eheadclip=false -Etailclip=false -Nstyle=invis*/
    agnodeattr(g, "style", "invis"); 
  } else {
    /*    width=0, height = 0, label="", style=filled*/
    agnodeattr(g, "shape", "point"); 
    agnodeattr(g, "width", "0"); 
    agnodeattr(g, "label", ""); 
    agnodeattr(g, "style", "filled"); 
    agnodeattr(g, "color", "#000000"); 
  }
  agedgeattr(g, "headclip", "false"); 
  agedgeattr(g, "tailclip", "false"); 
  agedgeattr(g, "style", "setlinewidth(0.0)"); 

  if (with_color) {
    sym2 = agedgeattr(g, "color", ""); 
    sym3 = agedgeattr(g, "wt", ""); 
  }
  for (i = 0; i < A->m; i++) {
    sprintf (buf, "%d", i);
    n = agnode (g, buf);
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
      if (A->type != MATRIX_TYPE_REAL){
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
	sprintf (buf, "%f", val[j]);
	if (with_color) sprintf (buf2, hue2rgb(.65*color[j], cstring));
      } else {
	sprintf (buf, "%f", 1.);
 	if (with_color) sprintf (buf2, hue2rgb(.65*color[j], cstring));
     }
      e = agedge (g, n, h);
      if (with_color) {
	agxset (e, sym2->index, buf2);
	sprintf (buf2, "%f", color[j]);
	agxset (e, sym3->index, buf2);
      }
    }
  }
  
  FREE(color);
  FREE (arr);
  FREE(label_string);
  return g;
}

#endif
