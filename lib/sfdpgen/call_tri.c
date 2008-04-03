#include "general.h"
#include "SparseMatrix.h"


#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */

#include "triangle.h"



SparseMatrix call_tri(int n, int dim, real *x)
{
  /* always contains a self edge and is symmetric */
  struct triangulateio in, mid, vorout;
  real one = 1;
  int i, ii, jj;
  SparseMatrix A, B;

  /* Define input points. */

  in.numberofpoints = n;
  in.numberofpointattributes = 0;
  in.pointlist = (REAL *) MALLOC(in.numberofpoints * 2 * sizeof(REAL));
  
  for (i = 0; i < n; i++){
    in.pointlist[i*2] = x[i*2];
    in.pointlist[i*2 + 1] = x[i*2 + 1];
  }
  in.pointattributelist = NULL;
  in.pointmarkerlist = NULL;
  in.numberofsegments = 0;
  in.numberofholes = 0;
  in.numberofregions = 0;
  in.regionlist = NULL;
  mid.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
  mid.pointattributelist = (REAL *) NULL;
  mid.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
  mid.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
  mid.triangleattributelist = (REAL *) NULL;
  mid.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
  mid.segmentlist = (int *) NULL;
  mid.segmentmarkerlist = (int *) NULL;
  mid.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
  mid.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */
  vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
  vorout.pointattributelist = (REAL *) NULL;
  vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
  vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

  /* Triangulate the points.  Switches are chosen to read and write a  */
  /*   PSLG (p), preserve the convex hull (c), number everything from  */
  /*   zero (z), assign a regional attribute to each element (A), and  */
  /*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
  /*   neighbor list (n).                                              */

  if (n > 2) {
    triangulate("Qe", &in, &mid, &vorout);
  } else {
    vorout.numberofedges = 0;
  }

  A = SparseMatrix_new(n, n, 1, MATRIX_TYPE_REAL, FORMAT_COORD);
  for (i = 0; i < vorout.numberofedges; i++) {
    ii = mid.edgelist[i * 2] - 1;
    jj = mid.edgelist[i * 2+1] - 1;
    SparseMatrix_coordinate_form_add_entries(A, 1, &(ii), &(jj), &one);
  }
  if (n == 2) {/* if two points, add edge i->j */
    ii = 0;
    jj = 1;
    SparseMatrix_coordinate_form_add_entries(A, 1, &(ii), &(jj), &one);
  }
  for (i = 0; i < n; i++) {
    SparseMatrix_coordinate_form_add_entries(A, 1, &i, &i, &one);
  }
  B = SparseMatrix_from_coordinate_format(A);
  B = SparseMatrix_symmetrize(B, FALSE);
  SparseMatrix_delete(A);

  return B;
}



struct vtx_data_struct{
  int nedges;
  int *edges;
  float *ewgts;
};

typedef struct vtx_data_struct vtx_data;

// maybe it should be replaced by RNG - relative neiborhood graph, or by GG - gabriel graph

void delaunay_triangulation(double *x, double *y, int n, vtx_data *delaunay) {
  int source,dest;
	struct triangulateio in ,out;
	int i, nedges;
	int *edges;

	in.pointlist=malloc(sizeof(REAL)*2*n);
	for (i=0; i<n; i++) {
		in.pointlist[2*i]=x[i];
		in.pointlist[2*i+1]=y[i];
	}

	in.pointattributelist=NULL;
	in.pointmarkerlist=NULL;
	in.numberofpoints=n;
	in.numberofpointattributes=0;
	in.trianglearealist=NULL;
	in.triangleattributelist=NULL;
	in.numberoftriangleattributes=0;
	in.neighborlist=NULL;
	in.segmentlist=NULL;
	in.segmentmarkerlist=NULL;
	in.holelist=NULL;
	in.numberofholes=0;
	in.regionlist=NULL;
	in.edgelist=NULL;
	in.edgemarkerlist=NULL;
	in.normlist=NULL;
	
	out.pointattributelist=NULL;
	out.pointmarkerlist=NULL;
	out.numberofpoints=n;
	out.numberofpointattributes=0;
	out.trianglearealist=NULL;
	out.triangleattributelist=NULL;
	out.numberoftriangleattributes=0;
	out.neighborlist=NULL;
	out.segmentlist=NULL;
	out.segmentmarkerlist=NULL;
	out.holelist=NULL;
	out.numberofholes=0;
	out.regionlist=NULL;
	out.edgelist=NULL;
	out.edgemarkerlist=NULL;
	out.normlist=NULL;

	triangulate("zQNEeB", &in, &out, NULL);

	nedges=out.numberofedges;

	delaunay = malloc(sizeof(vtx_data)*n);
	edges = malloc(sizeof(int)*(2*nedges+n));

	for (i=0; i<n; i++) {
		delaunay[i].ewgts = NULL;
		delaunay[i].nedges = 1;
	}
	
	for (i=0; i<2*nedges; i++)
		delaunay[out.edgelist[i]].nedges++;

	for (i=0; i<n; i++) {
		delaunay[i].edges = edges;
		edges += delaunay[i].nedges;
		delaunay[i].edges[0] = i;
		delaunay[i].nedges = 1;
	}
	for (i=0; i<nedges; i++) {
		source=out.edgelist[2*i];
		dest=out.edgelist[2*i+1];
		delaunay[source].edges[delaunay[source].nedges++] = dest;
		delaunay[dest].edges[delaunay[dest].nedges++] = source;
	}

	free(in.pointlist);
	free(out.edgelist);
}


inline void remove_edge(vtx_data * graph, int source, int dest) {
	int i;
	for (i=1; i<graph[source].nedges; i++) {
		if (graph[source].edges[i]==dest) {
			graph[source].edges[i]=graph[source].edges[--graph[source].nedges];
			break;
		}
	}
}

void UG_graph(double *x, double *y, int n, vtx_data ** delaunay0, int accurate_computation) {
  struct triangulateio in ,out;
  int i, nedges, *edges;
  int source,dest;
  double dist_ij, dist_ik, dist_jk, x_i, y_i, x_j, y_j;
  int j,k, neighbor_j, neighbor_k;
  int removed, false = 0, true = 1;
  vtx_data *delaunay;
  
  
  in.pointlist=malloc(sizeof(REAL)*2*n);
  for (i=0; i<n; i++) {
    in.pointlist[2*i]=x[i];
    in.pointlist[2*i+1]=y[i];
  }

	in.pointattributelist=NULL;
	in.pointmarkerlist=NULL;
	in.numberofpoints=n;
	in.numberofpointattributes=0;
	in.trianglearealist=NULL;
	in.triangleattributelist=NULL;
	in.numberoftriangleattributes=0;
	in.neighborlist=NULL;
	in.segmentlist=NULL;
	in.segmentmarkerlist=NULL;
	in.holelist=NULL;
	in.numberofholes=0;
	in.regionlist=NULL;
	in.edgelist=NULL;
	in.edgemarkerlist=NULL;
	in.normlist=NULL;
	
	out.pointattributelist=NULL;
	out.pointmarkerlist=NULL;
	out.numberofpoints=n;
	out.numberofpointattributes=0;
	out.trianglearealist=NULL;
	out.triangleattributelist=NULL;
	out.numberoftriangleattributes=0;
	out.neighborlist=NULL;
	out.segmentlist=NULL;
	out.segmentmarkerlist=NULL;
	out.holelist=NULL;
	out.numberofholes=0;
	out.regionlist=NULL;
	out.edgelist=NULL;
	out.edgemarkerlist=NULL;
	out.normlist=NULL;

	if (n==2) {
		int * edges = malloc(sizeof(int)*4);
		delaunay = malloc(sizeof(vtx_data)*n);
		*delaunay0 = delaunay;
		delaunay[0].ewgts = NULL;
		delaunay[0].edges = edges;
		delaunay[0].nedges = 2;
		delaunay[0].edges[0]=0; delaunay[0].edges[1]=1;
		delaunay[1].edges = edges+2;
		delaunay[1].ewgts = NULL;
		delaunay[1].nedges = 2;
		delaunay[1].edges[0]=1; delaunay[1].edges[1]=0;
		return;
	}
	else if (n==1) {
		int * edges = malloc(sizeof(int)*4);
		delaunay = malloc(sizeof(vtx_data)*n);
		*delaunay0 = delaunay;
		delaunay[0].ewgts = NULL;
		delaunay[0].edges = edges;
		delaunay[0].nedges = 1;
		delaunay[0].edges[0]=0; 
		return;
	}


	triangulate("zQNEeB", &in, &out, NULL);
	


	nedges=out.numberofedges;

	delaunay = malloc(sizeof(vtx_data)*n);
	*delaunay0 = delaunay;
	edges = malloc(sizeof(int)*(2*nedges+n));
	for (i=0; i<n; i++) {
		delaunay[i].ewgts = NULL;
		delaunay[i].nedges = 1;
	}
	
	for (i=0; i<2*nedges; i++)
		delaunay[out.edgelist[i]].nedges++;

	for (i=0; i<n; i++) {
		delaunay[i].edges = edges;
		edges += delaunay[i].nedges;
		delaunay[i].edges[0] = i;
		delaunay[i].nedges = 1;
	}
	for (i=0; i<nedges; i++) {
		source=out.edgelist[2*i];
		dest=out.edgelist[2*i+1];
		delaunay[source].edges[delaunay[source].nedges++] = dest;
		delaunay[dest].edges[delaunay[dest].nedges++] = source;
	}

	free(in.pointlist);
	free(out.edgelist);

	
	if (accurate_computation) {
		for (i=0; i<n; i++) {
			x_i= x[i]; y_i = y[i];
			for (j=1; j<delaunay[i].nedges;) {
				neighbor_j = delaunay[i].edges[j];
				if (neighbor_j<i) {
					j++;
					continue;
				}
				x_j= x[neighbor_j]; y_j = y[neighbor_j];
				dist_ij = (x_j-x_i)*(x_j-x_i)+(y_j-y_i)*(y_j-y_i);
				removed = false;
				for (k=0; k<n && !removed; k++) {
					dist_ik = (x[k]-x_i)*(x[k]-x_i)+(y[k]-y_i)*(y[k]-y_i);
					if (dist_ik < dist_ij) {
						dist_jk = (x[k]-x_j)*(x[k]-x_j)+(y[k]-y_j)*(y[k]-y_j);
						if (dist_jk < dist_ij) {
							// remove the edge beteween i and neighbor j
							delaunay[i].edges[j]=delaunay[i].edges[--delaunay[i].nedges];
							remove_edge(delaunay, neighbor_j, i);
							removed=true;
						}					
					}
				}
				if (!removed) {
					j++;
				}
			}
		}
	}
	else {
		// remove all edges v-u if there is w, neighbor of u or v, that is closer to both u and v than dist(u,v)
		for (i=0; i<n; i++) {
			x_i= x[i]; y_i = y[i];
			for (j=1; j<delaunay[i].nedges;) {
				neighbor_j = delaunay[i].edges[j];
				x_j= x[neighbor_j]; y_j = y[neighbor_j];
				dist_ij = (x_j-x_i)*(x_j-x_i)+(y_j-y_i)*(y_j-y_i);
				// now look at i'th neighbors to see whether there is a node in the "forbidden region"
				// we will also go through neighbor_j's neighbors when we traverse the edge from its other side
				removed = false;
				for (k=1; k<delaunay[i].nedges && !removed; k++) {
					neighbor_k = delaunay[i].edges[k];
					dist_ik = (x[neighbor_k]-x_i)*(x[neighbor_k]-x_i)+(y[neighbor_k]-y_i)*(y[neighbor_k]-y_i);
					if (dist_ik < dist_ij) {
						dist_jk = (x[neighbor_k]-x_j)*(x[neighbor_k]-x_j)+(y[neighbor_k]-y_j)*(y[neighbor_k]-y_j);
						if (dist_jk < dist_ij) {
							// remove the edge beteween i and neighbor j
							delaunay[i].edges[j]=delaunay[i].edges[--delaunay[i].nedges];
							remove_edge(delaunay, neighbor_j, i);
							removed=true;
						}					
					}
				}
				if (!removed) {
					j++;
				}
			}
		}
	}
}


SparseMatrix call_tri2(int n, int dim, real *xx){
  real *x, *y;
  vtx_data *delaunay;
  int i, j;
  SparseMatrix A, B;
  real one = 1;
  x = MALLOC(sizeof(real)*n);
  y = MALLOC(sizeof(real)*n);

  for (i = 0; i < n; i++){
    x[i] = xx[dim*i];
    y[i] = xx[dim*i+1];
  }

  UG_graph(x, y, n, &delaunay, 0);

  A = SparseMatrix_new(n, n, 1, MATRIX_TYPE_REAL, FORMAT_COORD);

  for (i = 0; i < n; i++){
    for (j = 1; j < delaunay[i].nedges; j++){
      SparseMatrix_coordinate_form_add_entries(A, 1, &i, &(delaunay[i].edges[j]), &one);
    } 
  }
  for (i = 0; i < n; i++) {
    SparseMatrix_coordinate_form_add_entries(A, 1, &i, &i, &one);
  }
  B = SparseMatrix_from_coordinate_format(A);
  B = SparseMatrix_symmetrize(B, FALSE);
  SparseMatrix_delete(A);
  return B;

}
