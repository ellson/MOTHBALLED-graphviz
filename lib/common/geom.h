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

#include "arith.h"

#ifndef GV_GEOM_H
#define GV_GEOM_H

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct { int x, y; } point;

typedef struct pointf_s { double x, y; } pointf;

/* tell pathplan/pathgeom.h */
#define HAVE_POINTF_S

typedef struct { point LL, UR; } box;

typedef struct { pointf LL, UR; } boxf;

/* true if point p is inside box b */
#define INSIDE(p,b)	(BETWEEN((b).LL.x,(p).x,(b).UR.x) && BETWEEN((b).LL.y,(p).y,(b).UR.y))
/* true if boxes b0 and b1 overlap */
#define OVERLAP(b0,b1)	(((b0).UR.x >= (b1).LL.x) && ((b1).UR.x >= (b0).LL.x) && ((b0).UR.y >= (b1).LL.y) && ((b1).UR.y >= (b0).LL.y))
/* true if box b0 completely contains b1*/
#define CONTAINS(b0,b1)	(((b0).UR.x >= (b1).UR.x) && ((b0).UR.y >= (b1).UR.y) && ((b0).LL.x <= (b1).LL.x) && ((b0).LL.y <= (b1).LL.y))

/* expand box b as needed to enclose point p */
#define EXPANDBP(b, p)	(b.LL.x = MIN(b.LL.x, p.x), b.LL.y = MIN(b.LL.y, p.y), b.UR.x = MAX(b.UR.x, p.x), b.UR.y = MAX(b.UR.y, p.y))
/* expand box b0 as needed to enclose box b1 */
#define EXPANDBB(b0, b1) (b0.LL.x = MIN(b0.LL.x, b1.LL.x), b0.LL.y = MIN(b0.LL.y, b1.LL.y), b0.UR.x = MAX(b0.UR.x, b1.UR.x), b0.UR.y = MAX(b0.UR.y, b1.UR.y))

#define DIST2(p1,p2) (SQR((p1.x) - (p2.x))) + (SQR((p1.y) - (p2.y)))
#define DIST(p1,p2) (sqrt(DIST2((p1),(p2))))

#define POINTS_PER_INCH	72
#define POINTS(f_inch)	(ROUND((f_inch)*POINTS_PER_INCH))
#define PS2INCH(ps)		((ps)/(double)POINTS_PER_INCH)

#define P2PF(p, pf) (pf.x = p.x, pf.y = p.y)
#define PF2P(pf, p) (p.x = ROUND (pf.x), p.y = ROUND (pf.y))
#define B2BF(b, bf) (bf.LL.x = b.LL.x, bf.LL.y = b.LL.y, bf.UR.x = b.UR.x, bf.UR.y = b.UR.y)
#define BF2B(bf, b) (b.LL.x = ROUND (bf.LL.x), b.LL.y = ROUND (bf.LL.y), b.UR.x = ROUND (bf.UR.x), b.UR.y = ROUND (bf.UR.y))

extern point pointof(int, int);
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
extern boolean box_overlap(box, box);
extern boolean boxf_overlap(boxf, boxf);
extern boolean box_contains(box, box);
extern boolean boxf_contains(boxf, boxf);
extern box flip_rec_box(box b, point p);

extern int lineToBox(pointf p1, pointf p2, boxf b);

extern point ccwrotatep(point p, int ccwrot);
extern pointf ccwrotatepf(pointf p, int ccwrot);
extern point cwrotatep(point p, int cwrot);
extern pointf cwrotatepf(pointf p, int cwrot);

#ifdef __cplusplus
}
#endif

#endif
