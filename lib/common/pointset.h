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



#ifndef _POINTSET_H
#define _POINTSET_H 1

#include <render.h>

    typedef Dict_t PointSet;
    typedef Dict_t PointMap;

    extern PointSet *newPS();
    extern void freePS(PointSet *);
    extern void insertPS(PointSet *, point);
    extern void addPS(PointSet *, int, int);
    extern int inPS(PointSet *, point);
    extern int isInPS(PointSet *, int, int);
    extern int sizeOf(PointSet *);
    extern point *pointsOf(PointSet *);

    extern PointMap *newPM();
    extern void clearPM(PointMap *);
    extern void freePM(PointMap *);
    extern int insertPM(PointMap *, int, int, int);
#endif


#ifdef __cplusplus
}
#endif
