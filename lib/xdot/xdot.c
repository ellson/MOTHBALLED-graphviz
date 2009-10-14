/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2007 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include <xdot.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NEW(t)           (t*)malloc(sizeof(t))
#define N_NEW(n,t)       (t*)malloc((n)*sizeof(t))

/* the parse functions should return NULL on error */
static char *parseReal(char *s, double *fp)
{
    char *p;
    double d;

    d = strtod(s, &p);
    if (p == s) return 0;
	
    *fp = d;
    return (p);
}

static char *parseInt(char *s, int *ip)
{
    int r, sz;
    r = sscanf(s, "%d%n", ip, &sz);
    if (r != 1) return 0;
    else return (s + sz);
}

static char *parsePoint(char *s, xdot_point * pp)
{
    int r, sz;
    r = sscanf(s, "%lf %lf%n", &(pp->x), &(pp->y), &sz);
    if (r != 2) return 0;
    pp->z = 0;
    return (s + sz);
}

static char *parseRect(char *s, xdot_rect * rp)
{
    int r, sz;
    r = sscanf(s, "%lf %lf %lf %lf%n", &(rp->x), &(rp->y), &(rp->w),
	       &(rp->h), &sz);
    if (r != 4) return 0;
    else return (s + sz);
}

static char *parsePolyline(char *s, xdot_polyline * pp)
{
    int i;
    xdot_point *pts;

    s = parseInt(s, &i);
    if (!s) return s;
    pts = N_NEW(i, xdot_point);
    pp->cnt = i;
    for (i = 0; i < pp->cnt; i++) {
	s = parsePoint(s, pts + i);
	if (!s) {
	    free (pts);
	    return 0;
	}
    }
    pp->pts = pts;
    return s;
}

static char *parseString(char *s, char **sp)
{
    int i;
    char *c;
    char *p;
    s = parseInt(s, &i);
    if (!s || (i <= 0)) return 0;
    while (*s && (*s != '-')) s++;
    if (*s) s++;
    else {
	return 0;
    }
    c = N_NEW(i + 1, char);
    p = c;
    while ((i > 0) && *s) {
	*p++ = *s++;
	i--;
    }
    if (i > 0) {
	free (c);
	return 0;
    }

    *p = '\0';
    *sp = c;
    return s;
}

static char *parseAlign(char *s, xdot_align * ap)
{
    int i;
    s = parseInt(s, &i);

    if (i < 0)
	*ap = xd_left;
    else if (i > 0)
	*ap = xd_right;
    else
	*ap = xd_center;
    return s;
}

#define CHK(s) if(!s){*error=1;return 0;}

static char *parseOp(xdot_op * op, char *s, drawfunc_t ops[], int* error)
{
    *error = 0;
    while (isspace(*s))
	s++;
    switch (*s++) {
    case 'E':
	op->kind = xd_filled_ellipse;
	s = parseRect(s, &op->u.ellipse);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_ellipse];
	break;

    case 'e':
	op->kind = xd_unfilled_ellipse;
	s = parseRect(s, &op->u.ellipse);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_ellipse];
	break;

    case 'P':
	op->kind = xd_filled_polygon;
	s = parsePolyline(s, &op->u.polygon);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_polygon];
	break;

    case 'p':
	op->kind = xd_unfilled_polygon;
	s = parsePolyline(s, &op->u.polygon);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_polygon];
	break;

    case 'b':
	op->kind = xd_filled_bezier;
	s = parsePolyline(s, &op->u.bezier);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_bezier];
	break;

    case 'B':
	op->kind = xd_unfilled_bezier;
	s = parsePolyline(s, &op->u.bezier);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_bezier];
	break;

    case 'c':
	op->kind = xd_pen_color;
	s = parseString(s, &op->u.color);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_pen_color];
	break;

    case 'C':
	op->kind = xd_fill_color;
	s = parseString(s, &op->u.color);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_fill_color];
	break;

    case 'L':
	op->kind = xd_polyline;
	s = parsePolyline(s, &op->u.polyline);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_polyline];
	break;

    case 'T':
	op->kind = xd_text;
	s = parseReal(s, &op->u.text.x);
	CHK(s);
	s = parseReal(s, &op->u.text.y);
	CHK(s);
	s = parseAlign(s, &op->u.text.align);
	CHK(s);
	s = parseReal(s, &op->u.text.width);
	CHK(s);
	s = parseString(s, &op->u.text.text);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_text];
	break;

    case 'F':
	op->kind = xd_font;
	s = parseReal(s, &op->u.font.size);
	CHK(s);
	s = parseString(s, &op->u.font.name);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_font];
	break;

    case 'S':
	op->kind = xd_style;
	s = parseString(s, &op->u.style);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_style];
	break;

    case 'I':
	op->kind = xd_image;
	s = parseRect(s, &op->u.image.pos);
	CHK(s);
	s = parseString(s, &op->u.image.name);
	CHK(s);
	if (ops)
	    op->drawfunc = ops[xop_image];
	break;


    case '\0':
	s = 0;
	break;

    default:
	*error = 1;
	s = 0;
	break;
    }
    return s;
}

#define XDBSIZE 100

/* parseXDotFOn:
 * Parse and append additional xops onto a given xdot object.
 * Return x.
 */ 
xdot *parseXDotFOn (char *s, drawfunc_t fns[], int sz, xdot* x)
{
    xdot_op op;
    char *ops;
    int oldsz, bufsz;
    int error;
    int initcnt;

    if (!s)
	return x;

    if (!x) {
	x = NEW(xdot);
	if (sz <= sizeof(xdot_op))
	    sz = sizeof(xdot_op);

	/* cnt, freefunc, ops, flags zeroed by NEW */
	x->sz = sz;
    }
    initcnt = x->cnt;
    sz = x->sz;

    if (initcnt == 0) {
	bufsz = XDBSIZE;
	ops = (char *) calloc(XDBSIZE, sz);
    }
    else {
	ops = (char*)(x->ops);
	bufsz = initcnt + XDBSIZE;
	ops = (char *) realloc(ops, bufsz * sz);
	memset(ops + (initcnt*sz), '\0', (bufsz - initcnt)*sz);
    }

    while ((s = parseOp(&op, s, fns, &error))) {
	if (x->cnt == bufsz) {
	    oldsz = bufsz;
	    bufsz *= 2;
	    ops = (char *) realloc(ops, bufsz * sz);
	    memset(ops + (oldsz*sz), '\0', (bufsz - oldsz)*sz);
	}
	*(xdot_op *) (ops + (x->cnt * sz)) = op;
	x->cnt++;
    }
    if (error)
	x->flags |= XDOT_PARSE_ERROR;
    if (x->cnt) {
	x->ops = (xdot_op *) realloc(ops, x->cnt * sz);
    }
    else {
	free (ops);
	free (x);
	x = NULL;
    }

    return x;

}

xdot *parseXDotF(char *s, drawfunc_t fns[], int sz)
{
    return parseXDotFOn (s, fns, sz, NULL);
}

xdot *parseXDot(char *s)
{
    return parseXDotF(s, 0, 0);
}

typedef void (*pf) (char *, void *);

static void printRect(xdot_rect * r, pf print, void *info)
{
    char buf[128];

    sprintf(buf, " %.06f %.06f %.06f %.06f", r->x, r->y, r->w, r->h);
    print(buf, info);
}

static void printPolyline(xdot_polyline * p, pf print, void *info)
{
    int i;
    char buf[512];

    sprintf(buf, " %d", p->cnt);
    print(buf, info);
    for (i = 0; i < p->cnt; i++) {
	sprintf(buf, " %.06f %.06f", p->pts[i].x, p->pts[i].y);
	print(buf, info);
    }
}

static void printString(char *p, pf print, void *info)
{
    char buf[30];

    sprintf(buf, " %d -", (int) strlen(p));
    print(buf, info);
    print(p, info);
}

static void printInt(int i, pf print, void *info)
{
    char buf[30];

    sprintf(buf, " %d", i);
    print(buf, info);
}

static void printFloat(float f, pf print, void *info)
{
    char buf[128];

    sprintf(buf, " %f", f);
    print(buf, info);
}

static void printAlign(xdot_align a, pf print, void *info)
{
    switch (a) {
    case xd_left:
	print(" -1", info);
	break;
    case xd_right:
	print(" 1", info);
	break;
    case xd_center:
	print(" 0", info);
	break;
    }
}

static void printXDot_Op(xdot_op * op, pf print, void *info)
{
    switch (op->kind) {
    case xd_filled_ellipse:
	print("E", info);
	printRect(&op->u.ellipse, print, info);
	break;
    case xd_unfilled_ellipse:
	print("e", info);
	printRect(&op->u.ellipse, print, info);
	break;
    case xd_filled_polygon:
	print("P", info);
	printPolyline(&op->u.polygon, print, info);
	break;
    case xd_unfilled_polygon:
	print("p", info);
	printPolyline(&op->u.polygon, print, info);
	break;
    case xd_filled_bezier:
	print("b", info);
	op->kind = xd_filled_bezier;
	printPolyline(&op->u.bezier, print, info);
	break;
    case xd_unfilled_bezier:
	print("B", info);
	printPolyline(&op->u.bezier, print, info);
	break;
    case xd_pen_color:
	print("c", info);
	printString(op->u.color, print, info);
	break;
    case xd_fill_color:
	print("C", info);
	printString(op->u.color, print, info);
	break;
    case xd_polyline:
	print("L", info);
	printPolyline(&op->u.polyline, print, info);
	break;
    case xd_text:
	print("T", info);
	printInt(op->u.text.x, print, info);
	printInt(op->u.text.y, print, info);
	printAlign(op->u.text.align, print, info);
	printInt(op->u.text.width, print, info);
	printString(op->u.text.text, print, info);
	break;
    case xd_font:
	print("F", info);
	op->kind = xd_font;
	printFloat(op->u.font.size, print, info);
	printString(op->u.font.name, print, info);
	break;
    case xd_style:
	print("S", info);
	printString(op->u.style, print, info);
	break;
    case xd_image:
	print("I", info);
	printRect(&op->u.image.pos, print, info);
	printString(op->u.image.name, print, info);
	break;
    }
}

static void _printXDot(xdot * x, pf print, void *info)
{
    int i;
    xdot_op *op;
    char *base = (char *) (x->ops);
    for (i = 0; i < x->cnt; i++) {
	op = (xdot_op *) (base + i * x->sz);
	printXDot_Op(op, print, info);
	if (i < x->cnt - 1)
	    print(" ", info);
    }
}

typedef struct {
    unsigned char *buf;		/* start of buffer */
    unsigned char *ptr;		/* next place to write */
    unsigned char *eptr;	/* end of buffer */
    int dyna;			/* true if buffer is malloc'ed */
} agxbuf;

#define agxbputc(X,C) ((((X)->ptr >= (X)->eptr) ? agxbmore(X,1) : 0), \
          (int)(*(X)->ptr++ = ((unsigned char)C)))
#define agxbuse(X) (agxbputc(X,'\0'),(char*)((X)->ptr = (X)->buf))

static void agxbinit(agxbuf * xb, unsigned int hint, unsigned char *init)
{
    if (init) {
	xb->buf = init;
	xb->dyna = 0;
    } else {
	if (hint == 0)
	    hint = BUFSIZ;
	xb->dyna = 1;
	xb->buf = N_NEW(hint, unsigned char);
    }
    xb->eptr = xb->buf + hint;
    xb->ptr = xb->buf;
    *xb->ptr = '\0';
}
static int agxbmore(agxbuf * xb, unsigned int ssz)
{
    int cnt;			/* current no. of characters in buffer */
    int size;			/* current buffer size */
    int nsize;			/* new buffer size */
    unsigned char *nbuf;	/* new buffer */

    size = xb->eptr - xb->buf;
    nsize = 2 * size;
    if (size + ssz > nsize)
	nsize = size + ssz;
    cnt = xb->ptr - xb->buf;
    if (xb->dyna) {
	nbuf = realloc(xb->buf, nsize);
    } else {
	nbuf = N_NEW(nsize, unsigned char);
	memcpy(nbuf, xb->buf, cnt);
	xb->dyna = 1;
    }
    xb->buf = nbuf;
    xb->ptr = xb->buf + cnt;
    xb->eptr = xb->buf + nsize;
    return 0;
}

static int agxbput(char *s, agxbuf * xb)
{
    unsigned int ssz = strlen(s);
    if (xb->ptr + ssz > xb->eptr)
	agxbmore(xb, ssz);
    memcpy(xb->ptr, s, ssz);
    xb->ptr += ssz;
    return ssz;
}

/* agxbfree:
 * Free any malloced resources.
 */
static void agxbfree(agxbuf * xb)
{
    if (xb->dyna)
	free(xb->buf);
}

char *sprintXDot(xdot * x)
{
    char *s;
    unsigned char buf[BUFSIZ];
    agxbuf xb;
    agxbinit(&xb, BUFSIZ, buf);
    _printXDot(x, (pf) agxbput, &xb);
    s = strdup(agxbuse(&xb));
    agxbfree(&xb);

    return s;
}

void fprintXDot(FILE * fp, xdot * x)
{
    _printXDot(x, (pf) fputs, fp);
}

static void freeXOpData(xdot_op * x)
{
    switch (x->kind) {
    case xd_filled_polygon:
    case xd_unfilled_polygon:
	free(x->u.polyline.pts);
	break;
    case xd_filled_bezier:
    case xd_unfilled_bezier:
	free(x->u.polyline.pts);
	break;
    case xd_polyline:
	free(x->u.polyline.pts);
	break;
    case xd_text:
	free(x->u.text.text);
	break;
    case xd_fill_color:
    case xd_pen_color:
	free(x->u.color);
	break;
    case xd_font:
	free(x->u.font.name);
	break;
    case xd_style:
	free(x->u.style);
	break;
    case xd_image:
	free(x->u.image.name);
	break;
    default:
	break;
    }
}

void freeXDot (xdot * x)
{
    int i;
    xdot_op *op;
    char *base;
    freefunc_t ff = x->freefunc;

    if (!x) return;
    base = (char *) (x->ops);
    for (i = 0; i < x->cnt; i++) {
	op = (xdot_op *) (base + i * x->sz);
	if (ff) ff (op);
	freeXOpData(op);
    }
    free(base);
    free(x);
}

#if 0
static void execOp(xdot_op * op, int param)
{
    op->drawfunc(op, param);
}
#endif
