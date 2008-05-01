/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include "QuadTree.h"
#include "memory.h"
#include "arith.h"
#include <math.h>
#include <assert.h>

typedef struct {
    real node_weight;
    real *coord;
    real id;
} node_data;

real point_distance(real * p1, real * p2, int dim)
{
    int i;
    real dist;
    dist = 0;
    for (i = 0; i < dim; i++)
	dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    return sqrt(dist);
}

static node_data *node_data_new(int dim, real weight, real * coord, int id)
{
    node_data *nd;
    int i;
    nd = GNEW(node_data);
    nd->node_weight = weight;
    nd->coord = N_GNEW(dim, real);
    nd->id = id;
    for (i = 0; i < dim; i++)
	nd->coord[i] = coord[i];
    return nd;
}

void node_data_delete(void *d)
{
    node_data *nd = (node_data *) d;
    free(nd->coord);
    free(nd);
}

real node_data_get_weight(void *d)
{
    node_data *nd = (node_data *) d;
    return nd->node_weight;
}

real *node_data_get_coord(void *d)
{
    node_data *nd = (node_data *) d;
    return nd->coord;
}

int node_data_get_id(void *d)
{
    node_data *nd = (node_data *) d;
    return nd->id;
}

int check_or_realloc_arrays(int dim, int nsuper, int nsupermax,
			    real ** center, real ** supernode_wgts,
			    real ** distances)
{

    if (nsuper >= nsupermax) {
	nsupermax = nsuper + MAX(10, (int) 0.2 * (nsuper));
	*center = RALLOC(nsupermax * dim, *center, real);
	*supernode_wgts = RALLOC(nsupermax, *supernode_wgts, real);
	*distances = RALLOC(nsupermax, *distances, real);
    }
    return nsupermax;
}

void QuadTree_get_supernodes_internal(QuadTree  qt, real bh, real * point,
				      int nodeid, int *nsuper,
				      int *nsupermax, real ** center,
				      real ** supernode_wgts,
				      real ** distances, real * counts,
				      int *flag)
{
    SingleLinkedList l;
    real *coord, dist;
    int dim = qt->dim, i;

    (*counts)++;
    l = qt->l;
    if (l) {
	while (l) {
	    *nsupermax =
		check_or_realloc_arrays(dim, *nsuper, *nsupermax, center,
					supernode_wgts, distances);
	    if (node_data_get_id(SingleLinkedList_get_data(l)) != nodeid) {
		coord = node_data_get_coord(SingleLinkedList_get_data(l));
		for (i = 0; i < dim; i++) {
		    (*center)[dim * (*nsuper) + i] = coord[i];
		}
		(*supernode_wgts)[*nsuper] =
		    node_data_get_weight(SingleLinkedList_get_data(l));
		(*distances)[*nsuper] = point_distance(point, coord, dim);
		(*nsuper)++;
	    }
	    l = SingleLinkedList_get_next(l);
	}
    }

    if (qt->qts) {
	dist = point_distance(qt->center, point, dim);
	if (qt->width < bh * dist) {
	    *nsupermax =
		check_or_realloc_arrays(dim, *nsuper, *nsupermax, center,
					supernode_wgts, distances);
	    for (i = 0; i < dim; i++) {
		(*center)[dim * (*nsuper) + i] = qt->average[i];
	    }
	    (*supernode_wgts)[*nsuper] = qt->total_weight;
	    (*distances)[*nsuper] =
		point_distance(qt->average, point, dim);
	    (*nsuper)++;
	} else {
	    for (i = 0; i < 1 << dim; i++) {
		QuadTree_get_supernodes_internal(qt->qts[i], bh, point,
						 nodeid, nsuper, nsupermax,
						 center, supernode_wgts,
						 distances, counts, flag);
	    }
	}
    }

}

void QuadTree_get_supernodes(QuadTree  qt, real bh, real * point,
			     int nodeid, int *nsuper, int *nsupermax,
			     real ** center, real ** supernode_wgts,
			     real ** distances, double *counts, int *flag)
{
    int dim = qt->dim;

    (*counts) = 0;

    *nsuper = 0;

    *flag = 0;
    *nsupermax = 10;
    if (!*center)
	*center = N_GNEW((*nsupermax) * dim, real);
    if (!*supernode_wgts)
	*supernode_wgts = N_GNEW(*nsupermax, real);
    if (!*distances)
	*distances = N_GNEW(*nsupermax, real);
    QuadTree_get_supernodes_internal(qt, bh, point, nodeid, nsuper,
				     nsupermax, center, supernode_wgts,
				     distances, counts, flag);

}



QuadTree QuadTree_new_from_point_list(int dim, int n, int max_level,
				       real * coord, real * weight)
{
    /* form a new QuadTree data structure from a list of coordinates of n points
       coord: of length n*dim, point i sits at [i*dim, i*dim+dim - 1]
       weight: node weight of lentgth n. If NULL, unit weight assumed.
     */
    real *xmin, *xmax, *center, width;
    QuadTree qt = NULL;
    int i, k;

    xmin = N_GNEW(dim, real);
    xmax = N_GNEW(dim, real);
    center = N_GNEW(dim, real);
    if (!xmin || !xmax || !center)
	return NULL;

    for (i = 0; i < dim; i++)
	xmin[i] = coord[i];
    for (i = 0; i < dim; i++)
	xmax[i] = coord[i];

    for (i = 1; i < n; i++) {
	for (k = 0; k < dim; k++) {
	    xmin[k] = MIN(xmin[k], coord[i * dim + k]);
	    xmax[k] = MAX(xmax[k], coord[i * dim + k]);
	}
    }

    width = xmax[0] - xmin[0];
    for (i = 0; i < dim; i++) {
	center[i] = (xmin[i] + xmax[i]) * 0.5;
	width = MAX(width, xmax[i] - xmin[i]);
    }
    width *= 0.52;
    qt = QuadTree_new(dim, center, width, max_level);

    if (weight) {
	for (i = 0; i < n; i++) {
	    qt = QuadTree_add(qt, &(coord[i * dim]), weight[i], i);
	}
    } else {
	for (i = 0; i < n; i++) {
	    qt = QuadTree_add(qt, &(coord[i * dim]), 1, i);
	}
    }


    free(xmin);
    free(xmax);
    free(center);
    return qt;
}

QuadTree QuadTree_new(int dim, real * center, real width, int max_level)
{
    QuadTree q;
    int i;
    q = GNEW(struct QuadTree_s);
    q->dim = dim;
    q->n = 0;
    q->center = N_GNEW(dim, real);
    for (i = 0; i < dim; i++)
	q->center[i] = center[i];
    assert(width > 0);
    q->width = width;
    q->total_weight = 0;
    q->average = NULL;
    q->qts = NULL;
    q->l = NULL;
    q->max_level = max_level;
    return q;
}

void QuadTree_delete(QuadTree  q)
{
    int i, dim = q->dim;
    if (!q)
	return;

    free(q->center);
    free(q->average);
    if (q->qts) {
	for (i = 0; i < 1 << dim; i++) {
	    QuadTree_delete(q->qts[i]);
	}
	free(q->qts);
    }
    SingleLinkedList_delete(q->l, node_data_delete);
    free(q);
}

static int QuadTree_get_quadrant(int dim, real * center, real * coord)
{
    /* find the quadrant that a point of coordinates coord is going into with reference to the center.
       if coord - center == {+,-,+,+} = {1,0,1,1}, then it will sit in the i-quadrant where
       i's binary representation is 1011 (that is, decimal 11).
     */
    int d = 0, i;

    for (i = dim - 1; i >= 0; i--) {
	if (coord[i] - center[i] < 0) {
	    d = 2 * d;
	} else {
	    d = 2 * d + 1;
	}
    }
    return d;
}

static QuadTree QuadTree_add_internal(QuadTree  q, real * coord,
				       real weight, int id, int level)
{
    int i, dim = q->dim, ii, k;
    node_data *nd = NULL;
    real *center, width;
    int max_level = q->max_level;

    /* Make sure that coord is within bounding box */
    for (i = 0; i < q->dim; i++) {
	if (coord[i] < q->center[i] - q->width
	    || coord[i] > q->center[i] + q->width)
	    return NULL;
    }

    if (q->n == 0) {
	/* if this node is empty right now */
	q->n = 1;
	q->total_weight = weight;
	q->average = N_GNEW(dim, real);
	for (i = 0; i < q->dim; i++)
	    q->average[i] = coord[i];
	nd = node_data_new(q->dim, weight, coord, id);
	assert(!(q->l));
	q->l = SingleLinkedList_new(nd);
    } else if (level < max_level) {
	/* otherwise open up into 2^dim quadtrees unless the level is too high */
	q->total_weight += weight;
	for (i = 0; i < q->dim; i++)
	    q->average[i] =
		((q->average[i]) * q->n + coord[i]) / (q->n + 1);
	if (!q->qts) {
	    q->qts = N_GNEW(1 << dim, QuadTree );
	    for (i = 0; i < 1 << dim; i++) {
		width = (q->width) / 2;
		q->qts[i] =
		    QuadTree_new(q->dim, q->center, width, max_level);
		center = (q->qts[i])->center;	/* right now this has the center for the parent */
		ii = i;
		for (k = 0; k < dim; k++) {	/* decompose child id into binary, if {1,0}, say, then
						   add {width/2, -width/2} to the parents' center
						   to get the child's center. */
		    if (ii % 2 == 0) {
			center[k] -= width;
		    } else {
			center[k] += width;
		    }
		    ii = (ii - ii % 2) / 2;
		}
	    }
	}

	/* done adding new quadtree, now add points to them */
	/* insert the old node (if exist) and the current node into the appropriate child quadtree */
	ii = QuadTree_get_quadrant(dim, q->center, coord);
	assert(ii < 1 << dim && ii >= 0);
	q->qts[ii] =
	    QuadTree_add_internal(q->qts[ii], coord, weight, id,
				  level + 1);
	assert(q->qts[ii]);

	if (q->l) {
	    assert(q->n == 1);
	    coord = node_data_get_coord(SingleLinkedList_get_data(q->l));
	    weight = node_data_get_weight(SingleLinkedList_get_data(q->l));
	    ii = QuadTree_get_quadrant(dim, q->center, coord);
	    assert(ii < 1 << dim && ii >= 0);
	    q->qts[ii] =
		QuadTree_add_internal(q->qts[ii], coord, weight, id,
				      level + 1);
	    assert(q->qts[ii]);

	    /* delete the old node data on parent */
	    SingleLinkedList_delete(q->l, node_data_delete);
	    q->l = NULL;
	}

	(q->n)++;
    } else {
	assert(!(q->qts));
	/* level is too high, append data in the linked list */
	(q->n)++;
	q->total_weight += weight;
	for (i = 0; i < q->dim; i++)
	    q->average[i] =
		((q->average[i]) * q->n + coord[i]) / (q->n + 1);
	nd = node_data_new(q->dim, weight, coord, id);
	assert(q->l);
	q->l = SingleLinkedList_prepend(q->l, nd);
    }
    return q;
}


QuadTree QuadTree_add(QuadTree  q, real * coord, real weight, int id)
{
    if (!q)
	return q;
    return QuadTree_add_internal(q, coord, weight, id, 0);

}

static void draw_polygon(FILE * fp, int dim, real * center, real width)
{
    /* pliot the enclosing square */
    if (dim < 2 || dim > 3)
	return;
    fprintf(fp, "(*in c*){Line[{");

    if (dim == 2) {
	fprintf(fp, "{%f, %f}", center[0] + width, center[1] + width);
	fprintf(fp, ",{%f, %f}", center[0] - width, center[1] + width);
	fprintf(fp, ",{%f, %f}", center[0] - width, center[1] - width);
	fprintf(fp, ",{%f, %f}", center[0] + width, center[1] - width);
	fprintf(fp, ",{%f, %f}", center[0] + width, center[1] + width);
    } else if (dim == 3) {
	/* top */
	fprintf(fp, "{");
	fprintf(fp, "{%f, %f, %f}", center[0] + width, center[1] + width,
		center[2] + width);
	fprintf(fp, ",{%f, %f, %f}", center[0] - width, center[1] + width,
		center[2] + width);
	fprintf(fp, ",{%f, %f, %f}", center[0] - width, center[1] - width,
		center[2] + width);
	fprintf(fp, ",{%f, %f, %f}", center[0] + width, center[1] - width,
		center[2] + width);
	fprintf(fp, ",{%f, %f, %f}", center[0] + width, center[1] + width,
		center[2] + width);
	fprintf(fp, "},");
	/* bot */
	fprintf(fp, "{");
	fprintf(fp, "{%f, %f, %f}", center[0] + width, center[1] + width,
		center[2] - width);
	fprintf(fp, ",{%f, %f, %f}", center[0] - width, center[1] + width,
		center[2] - width);
	fprintf(fp, ",{%f, %f, %f}", center[0] - width, center[1] - width,
		center[2] - width);
	fprintf(fp, ",{%f, %f, %f}", center[0] + width, center[1] - width,
		center[2] - width);
	fprintf(fp, ",{%f, %f, %f}", center[0] + width, center[1] + width,
		center[2] - width);
	fprintf(fp, "},");
	/* for sides */
	fprintf(fp, "{");
	fprintf(fp, "{%f, %f, %f}", center[0] + width, center[1] + width,
		center[2] - width);
	fprintf(fp, ",{%f, %f, %f}", center[0] + width, center[1] + width,
		center[2] + width);
	fprintf(fp, "},");

	fprintf(fp, "{");
	fprintf(fp, "{%f, %f, %f}", center[0] - width, center[1] + width,
		center[2] - width);
	fprintf(fp, ",{%f, %f, %f}", center[0] - width, center[1] + width,
		center[2] + width);
	fprintf(fp, "},");

	fprintf(fp, "{");
	fprintf(fp, "{%f, %f, %f}", center[0] + width, center[1] - width,
		center[2] - width);
	fprintf(fp, ",{%f, %f, %f}", center[0] + width, center[1] - width,
		center[2] + width);
	fprintf(fp, "},");

	fprintf(fp, "{");
	fprintf(fp, "{%f, %f, %f}", center[0] - width, center[1] - width,
		center[2] - width);
	fprintf(fp, ",{%f, %f, %f}", center[0] - width, center[1] - width,
		center[2] + width);
	fprintf(fp, "}");
    }


    fprintf(fp, "}]}(*end C*)");


}
static void QuadTree_print_internal(FILE * fp, QuadTree  q, int level)
{
    /* dump a quad tree in Mathematica format. */
    SingleLinkedList l;
    SingleLinkedList l0;
    real *coord;
    int i, dim = q->dim;

    draw_polygon(fp, q->dim, q->center, q->width);

    l0 = l = q->l;
    if (l) {
	printf(",(*a*) {Red,");
	while (l) {
	    if (l != l0)
		printf(",");
	    coord = node_data_get_coord(SingleLinkedList_get_data(l));
	    fprintf(fp, "Point[{");
	    for (i = 0; i < dim; i++) {
		if (i != 0)
		    printf(",");
		fprintf(fp, "%f", coord[i]);
	    }
	    fprintf(fp, "}]");
	    l = SingleLinkedList_get_next(l);
	}
	fprintf(fp, "}");
    }

    if (q->qts) {
	for (i = 0; i < 1 << dim; i++) {
	    fprintf(fp, ",(*b*){");
	    QuadTree_print_internal(fp, q->qts[i], level + 1);
	    fprintf(fp, "}");
	}
    }


}

void QuadTree_print(FILE * fp, QuadTree  q)
{
    if (!fp)
	return;
    if (q->dim == 2) {
	fprintf(fp, "Graphics[{");
    } else if (q->dim == 3) {
	fprintf(fp, "Graphics3D[{");
    } else {
	return;
    }
    QuadTree_print_internal(fp, q, 0);
    if (q->dim == 2) {
	fprintf(fp,
		"}, PlotRange -> All, Frame -> True, FrameTicks -> True]\n");
    } else {
	fprintf(fp, "}, PlotRange -> All]\n");
    }
}
