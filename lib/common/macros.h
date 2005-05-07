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

#ifndef GV_MACROS_H
#define GV_MACROS_H

#define NOT(v)		(!(v))
#ifndef FALSE
#define	FALSE		0
#endif
#ifndef TRUE
#define TRUE		NOT(FALSE)
#endif

#ifndef NOTUSED
#define NOTUSED(var)      (void) var
#endif

#ifndef NIL
#define NIL(type)       ((type)0)
#endif

#ifdef DMALLOC
#define NEW(t)		 (t*)calloc(1,sizeof(t))
#define N_NEW(n,t)	 (t*)calloc((n),sizeof(t))
#define GNEW(t)		 (t*)malloc(sizeof(t))
#define N_GNEW(n,t)	 (t*)malloc((n)*sizeof(t))
#define ALLOC(size,ptr,type) (ptr? (type*)realloc(ptr,(size)*sizeof(type)):(type*)malloc((size)*sizeof(type)))
#define RALLOC(size,ptr,type) ((type*)realloc(ptr,(size)*sizeof(type)))
#define ZALLOC(size,ptr,type,osize) (ptr? (type*)recalloc(ptr,(size)*sizeof(type)):(type*)calloc((size),sizeof(type)))
#else
#define NEW(t)		 (t*)zmalloc(sizeof(t))
#define N_NEW(n,t)	 (t*)zmalloc((n)*sizeof(t))
#define GNEW(t)		 (t*)gmalloc(sizeof(t))
#define N_GNEW(n,t)	 (t*)gmalloc((n)*sizeof(t))
#define ALLOC(size,ptr,type) (ptr? (type*)grealloc(ptr,(size)*sizeof(type)):(type*)gmalloc((size)*sizeof(type)))
#define RALLOC(size,ptr,type) ((type*)grealloc(ptr,(size)*sizeof(type)))
#define ZALLOC(size,ptr,type,osize) (ptr? (type*)zrealloc(ptr,size,sizeof(type),osize):(type*)zmalloc((size)*sizeof(type)))
#endif

#ifdef MIN
#undef MIN
#endif
#define MIN(a,b)	((a)<(b)?(a):(b))

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b)	((a)>(b)?(a):(b))

#ifdef ABS
#undef ABS
#endif
#define ABS(a)		((a) >= 0 ? (a) : -(a))

#ifndef MAXINT
#define	MAXINT		((int)(~(unsigned)0 >> 1))
#endif
#ifndef MAXSHORT
#define	MAXSHORT	(0x7fff)
#endif
#ifndef MAXDOUBLE
#define MAXDOUBLE   1.7976931348623157e+308
#endif
#ifndef MAXFLOAT
#define MAXFLOAT   ((float)3.40282347e+38)
#endif

#ifdef BETWEEN
#undef BETWEEN
#endif
#define BETWEEN(a,b,c)	(((a) <= (b)) && ((b) <= (c)))

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

#define ROUND(f)        ((f>=0)?(int)(f + .5):(int)(f - .5))
#define RADIANS(deg)	((deg)/180.0 * PI)
#define DEGREES(rad)	((rad)/PI * 180.0)
#define DIST(x1,y1,x2,y2) (sqrt(((x1) - (x2))*((x1) - (x2)) + ((y1) - (y2))*((y1) - (y2))))
#define POINTS_PER_INCH	72
#define POINTS(f_inch)	(ROUND((f_inch)*POINTS_PER_INCH))
#define PS2INCH(ps)		((ps)/(double)POINTS_PER_INCH)

#define isPinned(n)     (ND_pinned(n) == P_PIN)
#define hasPos(n)       (ND_pinned(n) > 0)
#define isFixed(n)      (ND_pinned(n) > P_SET)

#define SET_CLUST_NODE(n) (ND_clustnode(n) = TRUE)
#define IS_CLUST_NODE(n)  (ND_clustnode(n))
#define HAS_CLUST_EDGE(g) (GD_flags(g) & 1)
#define SET_CLUST_EDGE(g) (GD_flags(g) |= 1)

#ifndef streq
#define streq(a,b)		(*(a)==*(b)&&!strcmp(a,b))
#endif

#define P2PF(p, pf) (pf.x = p.x, pf.y = p.y)
#define PF2P(pf, p) (p.x = ROUND (pf.x), p.y = ROUND (pf.y))
#define B2BF(b, bf) (bf.LL.x = b.LL.x, bf.LL.y = b.LL.y, bf.UR.x = b.UR.x, bf.UR.y = b.UR.y)
#define BF2B(bf, b) (b.LL.x = ROUND (bf.LL.x), b.LL.y = ROUND (bf.LL.y), b.UR.x = ROUND (bf.UR.x), b.UR.y = ROUND (bf.UR.y))

#define XPAD(d) ((d).x += 4*GAP)
#define YPAD(d) ((d).y += 2*GAP)
#define PAD(d)  {XPAD(d); YPAD(d);}

#define OTHERDIR(dir) ((dir == CCW) ? CW : CCW)

#define NEXTSIDE(side, dir) ((dir == CCW) ? \
		((side & 0x8) ? BOTTOM : (side << 1)) : \
		((side & 0x1) ? LEFT : (side >> 1)))

#define YDIR(y) (Y_invert ? (Y_off - (y)) : (y))
#define YFDIR(y) (Y_invert ? (YF_off - (y)) : (y))
#endif
