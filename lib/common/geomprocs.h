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

/* geometric functions (e.g. on points and boxes) with application to, but
 * no specific dependance on graphs */

#ifndef GV_GEOMPROCS_H
#define GV_GEOMPROCS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <geom.h>

extern point pointof(int, int);
extern pointf pointfof(double, double);

extern pointf cvt2ptf(point);
extern point cvt2pt(pointf);

extern point add_points(point, point);
extern pointf add_pointfs(pointf, pointf);

extern point sub_points(point, point);
extern pointf sub_pointfs(pointf, pointf);

extern point exch_xy(point p);
extern pointf exch_xyf(pointf p);

extern box boxof(int llx, int lly, int urx, int ury);
extern boxf boxfof(double llx, double lly, double urx, double ury);

extern box mkbox(point, point);
extern boxf mkboxf(pointf, pointf);

extern box box_bb(box, box);
extern boxf boxf_bb(boxf, boxf);

extern box box_intersect(box, box);
extern boxf boxf_intersect(boxf, boxf);

extern int box_overlap(box, box);
extern int boxf_overlap(boxf, boxf);

extern int box_contains(box, box);
extern int boxf_contains(boxf, boxf);

extern box flip_rec_box(box b, point p);
extern boxf flip_rec_boxf(boxf b, pointf p);

extern double ptToLine2 (pointf l1, pointf l2, pointf p);

extern int lineToBox(pointf p1, pointf p2, boxf b);

extern point ccwrotatep(point p, int ccwrot);
extern pointf ccwrotatepf(pointf p, int ccwrot);

extern point cwrotatep(point p, int cwrot);
extern pointf cwrotatepf(pointf p, int cwrot);

extern void rect2poly(pointf *p);
extern pointf scale(double c, pointf v);
extern pointf perp(pointf v);
extern pointf subPt(pointf v, pointf w);
extern pointf addPt(pointf v, pointf w);

extern int seg_intersect (pointf a, pointf b, pointf c, pointf d, pointf* x);
#ifdef __cplusplus
}
#endif

#endif
