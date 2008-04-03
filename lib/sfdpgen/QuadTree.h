#include "LinkedList.h"

typedef struct QuadTree_struct *QuadTree;

struct QuadTree_struct {
  /* a data structure containing coordinates of n items, their average is in "average".
     The current level is a square or cube of width "width", which is subdivided into 
     2^dim QuadTrees qts. At the last level, all coordinates are stored in a single linked list l. 
     total_weight is the combined weights of the nodes */
  int n;/* number of items */
  real total_weight;
  int dim;
  real *center;/* center of the bounding box, array of dimension dim. Allocated inside */
  real width;/* center +/- width gives the lower/upper bound, so really width is the 
		"radius" */
  real *average;/* the average coordinates. Array of length dim. Allocated inside  */
  QuadTree *qts;/* subtree . If dim = 2, there are 4, dim = 3 gives 8 */
  SingleLinkedList l;
  int max_level;
};


QuadTree QuadTree_new(int dim, real *center, real width, int max_level);

void QuadTree_delete(QuadTree q);

QuadTree QuadTree_add(QuadTree q, real *coord, real weight, int id);/* coord is copied in */

void QuadTree_print(FILE *fp, QuadTree q);

QuadTree QuadTree_new_from_point_list(int dim, int n, int max_level, real *coord, real *weight);

real point_distance(real *p1, real *p2, int dim);

void QuadTree_get_supernodes(QuadTree qt, real bh, real *point, int nodeid, int *nsuper, 
			     int *nsupermax, real **center, real **supernode_wgts, real **distances, real *counts, int *flag);

