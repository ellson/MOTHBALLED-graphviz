/* $Id$ $Revision$ */
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

/*
 * this implements the resistor circuit current model for
 * computing node distance, as an alternative to shortest-path.
 * likely it could be improved by using edge weights, somehow.
 * Return 1 if successful; 0 otherwise (e.g., graph is disconnected).
 */
#include	"neato.h"
#include "cgraph.h"

int solveCircuit(int nG, double **Gm, double **Gm_inv)
{
    double sum;
    int i, j;

    if (Verbose)
	fprintf(stderr, "Calculating circuit model");

    /* set diagonal entries to sum of conductances but ignore nth node */
    for (i = 0; i < nG; i++) {
	sum = 0.0;
	for (j = 0; j < nG; j++)
	    if (i != j)
		sum += Gm[i][j];
	Gm[i][i] = -sum;
    }
    return matinv(Gm, Gm_inv, nG - 1);
}

int circuit_model(graph_t * g, int nG)
{

#ifdef WITH_CGRAPH
	double **Gm;
    double **Gm_inv;
    int rv, i, j;
    node_t *v;
    edge_t *e;
	
    Gm = new_array(nG, nG, 0.0);
    Gm_inv = new_array(nG, nG, 0.0);

    /* set non-diagonal entries */
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	for (e = agfstedge(g, v); e; e = agnxtedge(g, e, v)) {

		
		i = ND_id(agtail(e));
	    j = ND_id(aghead(e));
	    if (i == j)
		continue;
	    /* conductance is 1/resistance */
	    Gm[i][j] = Gm[j][i] = -1.0 / ED_dist(e);	/* negate */
	}
    }

    rv = solveCircuit(nG, Gm, Gm_inv);

    if (rv)
	for (i = 0; i < nG; i++) {
	    for (j = 0; j < nG; j++) {
		GD_dist(g)[i][j] =
		    Gm_inv[i][i] + Gm_inv[j][j] - 2.0 * Gm_inv[i][j];
	    }
	}
    free_array(Gm);
    free_array(Gm_inv);
    return rv;

#else	
	double **Gm;
    double **Gm_inv;
    int rv, i, j;
    node_t *v;
    edge_t *e;

    Gm = new_array(nG, nG, 0.0);
    Gm_inv = new_array(nG, nG, 0.0);

    /* set non-diagonal entries */
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	for (e = agfstedge(g, v); e; e = agnxtedge(g, e, v)) {
	    i = ND_id(e->tail);
	    j = ND_id(e->head);
	    if (i == j)
		continue;
	    /* conductance is 1/resistance */
	    Gm[i][j] = Gm[j][i] = -1.0 / ED_dist(e);	/* negate */
	}
    }

    rv = solveCircuit(nG, Gm, Gm_inv);

    if (rv)
	for (i = 0; i < nG; i++) {
	    for (j = 0; j < nG; j++) {
		GD_dist(g)[i][j] =
		    Gm_inv[i][i] + Gm_inv[j][j] - 2.0 * Gm_inv[i][j];
	    }
	}
    free_array(Gm);
    free_array(Gm_inv);
    return rv;
#endif
}
