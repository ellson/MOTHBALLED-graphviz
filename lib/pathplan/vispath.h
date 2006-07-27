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



#ifndef _VIS_INCLUDE
#define _VIS_INCLUDE

#include <pathgeom.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_BLD_pathplan) && defined(__EXPORT__)
#   define extern __EXPORT__
#endif

/* open a visibility graph */
    extern vconfig_t *Pobsopen(Ppoly_t ** obstacles, int n_obstacles);

/* close a visibility graph, freeing its storage */
    extern void Pobsclose(vconfig_t * config);

/* route a polyline from p0 to p1, avoiding obstacles.
 * if an endpoint is inside an obstacle, pass the polygon's index >=0
 * if the endpoint is not inside an obstacle, pass POLYID_NONE
 * if the endpoint location is not known, pass POLYID_UNKNOWN
 */

    extern int Pobspath(vconfig_t * config, Ppoint_t p0, int poly0,
			Ppoint_t p1, int poly1,
			Ppolyline_t * output_route);

#define POLYID_NONE		-1111
#define POLYID_UNKNOWN	-2222

#undef extern

#ifdef __cplusplus
}
#endif
#endif
