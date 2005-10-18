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

#ifdef __cplusplus
extern "C" {
#endif

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
    extern double dist2(pointf p, pointf q);

    extern point flip_pt(point p, int rankdir);
    extern pointf flip_ptf(pointf p, int rankdir);
    extern point invflip_pt(point p, int rankdir);

#ifdef __cplusplus
}
#endif
