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

#ifdef __cplusplus
extern "C" {
#endif

#define MAXINTS  10000		/* modify this line to reflect the max no. of 
				   intersections you want reported -- 50000 seems to break the program */

#define NIL 0

#define ABS(x) (((x) > 0) ? (x) : (-x))
#define SLOPE(p,q) ( ( ( p.y ) - ( q.y ) ) / ( ( p.x ) - ( q.x ) ) )
#define MAX(a,b) ( ( a ) > ( b ) ? ( a ) : ( b ) )

#define after(v) (((v)==((v)->poly->finish))?((v)->poly->start):((v)+1))
#define prior(v) (((v)==((v)->poly->start))?((v)->poly->finish):((v)-1))

    struct position {
	float x, y;
    };


    struct vertex {
	struct position pos;
	struct polygon *poly;
	struct active_edge *active;
    };

    struct polygon {
	struct vertex *start, *finish;
    };

    struct intersection {
	struct vertex *firstv, *secondv;
	struct polygon *firstp, *secondp;
	float x, y;
    };

    struct active_edge {
	struct vertex *name;
	struct active_edge *next, *last;
    };
    struct active_edge_list {
	struct active_edge *first, *final;
	int number;
    };
    struct data {
	int nvertices, npolygons, ninters;
    };

#ifdef __cplusplus
}
#endif
