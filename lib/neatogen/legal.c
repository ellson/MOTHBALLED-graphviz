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

#include "neato.h"
#include "simple.h"


extern void
find_ints(struct vertex vertex_list[],
	  struct polygon polygon_list[],
	  struct data *input, struct intersection ilist[]);
int Plegal_arrangement(Ppoly_t ** polys, int n_polys)
{

    int i, j, vno, nverts, rv;

    struct vertex *vertex_list;
    struct polygon *polygon_list;
    struct data input;
    struct intersection ilist[MAXINTS];

    polygon_list = N_GNEW(n_polys, struct polygon);

    for (i = nverts = 0; i < n_polys; i++)
	nverts += polys[i]->pn;

    vertex_list = N_GNEW(nverts, struct vertex);

    for (i = vno = 0; i < n_polys; i++) {
	polygon_list[i].start = &vertex_list[vno];
	for (j = 0; j < polys[i]->pn; j++) {
	    vertex_list[vno].pos.x = polys[i]->ps[j].x;
	    vertex_list[vno].pos.y = polys[i]->ps[j].y;
	    vertex_list[vno].poly = &polygon_list[i];
	    vertex_list[vno].active = 0;
	    vno++;
	}
	polygon_list[i].finish = &vertex_list[vno - 1];
    }

    input.nvertices = nverts;
    input.npolygons = n_polys;

    find_ints(vertex_list, polygon_list, &input, ilist);


#define EQ_PT(v,w) (((v).x == (w).x) && ((v).y == (w).y))
    rv = 1;
    {
	int i;
	struct position vft, vsd, avft, avsd;
	for (i = 0; i < input.ninters; i++) {
	    vft = ilist[i].firstv->pos;
	    avft = after(ilist[i].firstv)->pos;
	    vsd = ilist[i].secondv->pos;
	    avsd = after(ilist[i].secondv)->pos;
	    if (((vft.x != avft.x) && (vsd.x != avsd.x)) ||
		((vft.x == avft.x) &&
		 !EQ_PT(vft, ilist[i]) &&
		 !EQ_PT(avft, ilist[i])) ||
		((vsd.x == avsd.x) &&
		 !EQ_PT(vsd, ilist[i]) && !EQ_PT(avsd, ilist[i]))) {
		rv = 0;
		if (Verbose > 1) {
		    fprintf(stderr, "\nintersection %d at %.3f %.3f\n",
			    i, ilist[i].x, ilist[i].y);
		    fprintf(stderr, "seg#1 : (%.3f, %.3f) (%.3f, %.3f)\n",
			    (double) (ilist[i].firstv->pos.x)
			    , (double) (ilist[i].firstv->pos.y)
			    , (double) (after(ilist[i].firstv)->pos.x)
			    , (double) (after(ilist[i].firstv)->pos.y));
		    fprintf(stderr, "seg#2 : (%.3f, %.3f) (%.3f, %.3f)\n",
			    (double) (ilist[i].secondv->pos.x)
			    , (double) (ilist[i].secondv->pos.y)
			    , (double) (after(ilist[i].secondv)->pos.x)
			    , (double) (after(ilist[i].secondv)->pos.y));
		}
	    }
	}
    }
    free(polygon_list);
    free(vertex_list);
    return rv;
}
