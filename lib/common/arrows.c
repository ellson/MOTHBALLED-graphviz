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


#include	"render.h"
#include	"gvc.h"

#define sqr(a) ((double) (a) * (a))
#define dstsq(a, b) (sqr (a.x - b.x) + sqr (a.y - b.y))
#define dst(a, b) sqrt (dstsq (a, b))

#define EPSILON .0001

/* standard arrow length in points */
#define ARROW_LENGTH 10.

/* arrow types */
#define ARR_TYPE_NONE	  (ARR_NONE)
#define ARR_TYPE_NORM	  1
#define ARR_TYPE_CROW	  2
#define ARR_TYPE_TEE	  3
#define ARR_TYPE_BOX	  4
#define ARR_TYPE_DIAMOND  5
#define ARR_TYPE_DOT      6

/* arrow mods */
#define ARR_MOD_OPEN      (1<<8)
#define ARR_MOD_INV       (1<<9)
#define ARR_MOD_LEFT      (1<<10)
#define ARR_MOD_RIGHT     (1<<11)

typedef struct arrowdir_t {
    char *dir;
    int sflag;
    int eflag;
} arrowdir_t;

static arrowdir_t Arrowdirs[] = {
    {"forward", ARR_TYPE_NONE, ARR_TYPE_NORM},
    {"back", ARR_TYPE_NORM, ARR_TYPE_NONE},
    {"both", ARR_TYPE_NORM, ARR_TYPE_NORM},
    {"none", ARR_TYPE_NONE, ARR_TYPE_NONE},
    {(char *) 0, ARR_TYPE_NONE, ARR_TYPE_NONE}
};

typedef struct arrowname_t {
    char *name;
    int type;
} arrowname_t;

static arrowname_t Arrowsynonyms[] = {
    /* synonyms for deprecated arrow names - included for backward compatibility */
    /*  evaluated before primary names else "invempty" would give different results */
    {"invempty", (ARR_TYPE_NORM | ARR_MOD_INV | ARR_MOD_OPEN)},	/* oinv     */
    {(char *) 0, (ARR_TYPE_NONE)}
};

static arrowname_t Arrowmods[] = {
    {"o", ARR_MOD_OPEN},
    {"r", ARR_MOD_RIGHT},
    {"l", ARR_MOD_LEFT},
    /* deprecated alternates for backward compat */
    {"e", ARR_MOD_OPEN},	/* o  - needed for "ediamond" */
    {"half", ARR_MOD_LEFT},	/* l  - needed for "halfopen" */
    {(char *) 0, ARR_TYPE_NONE}
};

static arrowname_t Arrownames[] = {
    {"normal", ARR_TYPE_NORM},
    {"crow", ARR_TYPE_CROW},
    {"tee", ARR_TYPE_TEE},
    {"box", ARR_TYPE_BOX},
    {"diamond", ARR_TYPE_DIAMOND},
    {"dot", ARR_TYPE_DOT},
    {"none", ARR_TYPE_NONE},
    /* ARR_MOD_INV is used only here to define two additional shapes
       since not all types can use it */
    {"inv", (ARR_TYPE_NORM | ARR_MOD_INV)},
    {"vee", (ARR_TYPE_CROW | ARR_MOD_INV)},
    /* WARNING ugly kludge to deal with "o" v "open" conflict */
    /* Define "open" as just "pen" since "o" already taken as ARR_MOD_OPEN */
    /* Note that ARR_MOD_OPEN has no meaning for ARR_TYPE_CROW shape */
    {"pen", (ARR_TYPE_CROW | ARR_MOD_INV)},
    /* WARNING ugly kludge to deal with "e" v "empty" conflict */
    /* Define "empty" as just "mpty" since "e" already taken as ARR_MOD_OPEN */
    /* Note that ARR_MOD_OPEN has expected meaning for ARR_TYPE_NORM shape */
    {"mpty", ARR_TYPE_NORM},
    {(char *) 0, ARR_TYPE_NONE}
};

typedef struct arrowtype_t {
    int type;
    double lenfact;		/* ratio of length of this arrow type to standards arrow */
    void (*gen) (GVC_t * gvc, pointf p, pointf u, int flag);	/* generator function for type */
} arrowtype_t;

/* forward declaration of functions used in Arrowtypes[] */
static void arrow_type_normal(GVC_t * gvc, pointf p, pointf u, int flag);
static void arrow_type_crow(GVC_t * gvc, pointf p, pointf u, int flag);
static void arrow_type_tee(GVC_t * gvc, pointf p, pointf u, int flag);
static void arrow_type_box(GVC_t * gvc, pointf p, pointf u, int flag);
static void arrow_type_diamond(GVC_t * gvc, pointf p, pointf u, int flag);
static void arrow_type_dot(GVC_t * gvc, pointf p, pointf u, int flag);

static arrowtype_t Arrowtypes[] = {
    {ARR_TYPE_NORM, 1.0, arrow_type_normal},
    {ARR_TYPE_CROW, 1.0, arrow_type_crow},
    {ARR_TYPE_TEE, 0.5, arrow_type_tee},
    {ARR_TYPE_BOX, 1.0, arrow_type_box},
    {ARR_TYPE_DIAMOND, 1.2, arrow_type_diamond},
    {ARR_TYPE_DOT, 0.8, arrow_type_dot},
    {ARR_TYPE_NONE, 0.0, NULL}
};

static char *arrow_match_name_frag(char *name, arrowname_t * arrownames,
				   int *flag)
{
    arrowname_t *arrowname;
    int namelen = 0;
    char *rest = name;

    for (arrowname = arrownames; arrowname->name; arrowname++) {
	namelen = strlen(arrowname->name);
	if (strncmp(name, arrowname->name, namelen) == 0) {
	    *flag |= arrowname->type;
	    rest += namelen;
	    break;
	}
    }
    return rest;
}

static char *arrow_match_shape(char *name, int *flag)
{
    char *next, *rest;
    int f = ARR_TYPE_NONE;

    rest = arrow_match_name_frag(name, Arrowsynonyms, &f);
    if (rest == name) {
	do {
	    next = rest;
	    rest = arrow_match_name_frag(next, Arrowmods, &f);
	} while (next != rest);
	rest = arrow_match_name_frag(rest, Arrownames, &f);
    }
    if (f && !(f & ((1 << 8) - 1)))
	f |= ARR_TYPE_NORM;
    *flag |= f;
    return rest;
}

static void arrow_match_name(char *name, int *flag)
{
    char *rest;
    int f1 = ARR_TYPE_NONE, f2 = ARR_TYPE_NONE;

    rest = arrow_match_shape(name, &f1);
    rest = arrow_match_shape(rest, &f2);
    *flag = f1 | (f2 << 16);
}

void arrow_flags(edge_t * e, int *sflag, int *eflag)
{
    char *attr;
    arrowdir_t *arrowdir;

    *sflag = ARR_TYPE_NONE;
    *eflag =
	AG_IS_DIRECTED(e->tail->graph) ? ARR_TYPE_NORM : ARR_TYPE_NONE;
    if (E_dir && ((attr = agxget(e, E_dir->index)))[0]) {
	for (arrowdir = Arrowdirs; arrowdir->dir; arrowdir++) {
	    if (streq(attr, arrowdir->dir)) {
		*sflag = arrowdir->sflag;
		*eflag = arrowdir->eflag;
		break;
	    }
	}
    }
    if (E_arrowhead && ((attr = agxget(e, E_arrowhead->index)))[0])
	arrow_match_name(attr, eflag);
    if (E_arrowtail && ((attr = agxget(e, E_arrowtail->index)))[0])
	arrow_match_name(attr, sflag);
    if (ED_conc_opp_flag(e)) {
	edge_t *f;
	int s0, e0;
	/* pick up arrowhead of opposing edge */
	f = agfindedge(e->tail->graph, e->head, e->tail);
	arrow_flags(f, &s0, &e0);
	*eflag = *eflag | s0;
	*sflag = *sflag | e0;
    }
}

double arrow_length(edge_t * e, int flag)
{
    arrowtype_t *arrowtype;
    double lenfact = 0.0;
    int f;

    /* we don't simply index with flag because arrowtypes are not necessarily sorted */
    f = flag & ((1 << 8) - 1);
    for (arrowtype = Arrowtypes; arrowtype->gen; arrowtype++) {
	if (f == arrowtype->type) {
	    lenfact += arrowtype->lenfact;
	    break;
	}
    }
    f = (flag >> 16) & ((1 << 8) - 1);
    for (arrowtype = Arrowtypes; arrowtype->gen; arrowtype++) {
	if (f == arrowtype->type) {
	    lenfact += arrowtype->lenfact;
	    break;
	}
    }
    /* The original was missing the factor E_arrowsz, but I believe it
       should be here for correct arrow clipping */
    return ARROW_LENGTH * lenfact * late_double(e, E_arrowsz, 1.0, 0.0);
}

/* inside function for calls to bezier_clip */
static boolean inside(inside_t * inside_context, pointf p)
{
    return dstsq(p, inside_context->a.p[0]) <= inside_context->a.r[0];
}

int arrowEndClip(edge_t* e, point * ps, int startp,
		 int endp, bezier * spl, int eflag)
{
    inside_t inside_context;
    pointf sp[4];
    double elen, elen2;

    elen = arrow_length(e, eflag);
    elen2 = elen * elen;
    spl->eflag = eflag, spl->ep = ps[endp + 3];
    if (endp > startp && dstsq(ps[endp], ps[endp + 3]) < elen2) {
	endp -= 3;
    }
    P2PF(ps[endp], sp[3]);
    P2PF(ps[endp + 1], sp[2]);
    P2PF(ps[endp + 2], sp[1]);
    P2PF(spl->ep, sp[0]);	/* ensure endpoint starts inside */

    inside_context.a.p = &sp[0];
    inside_context.a.r = &elen2;
    bezier_clip(&inside_context, inside, sp, TRUE);

    PF2P(sp[3], ps[endp]);
    PF2P(sp[2], ps[endp + 1]);
    PF2P(sp[1], ps[endp + 2]);
    PF2P(sp[0], ps[endp + 3]);
    return endp;
}

int arrowStartClip(edge_t* e, point * ps, int startp,
		   int endp, bezier * spl, int sflag)
{
    inside_t inside_context;
    pointf sp[4];
    double slen, slen2;

    slen = arrow_length(e, sflag);
    slen2 = slen * slen;
    spl->sflag = sflag, spl->sp = ps[startp];
    if (endp > startp && dstsq(ps[startp], ps[startp + 3]) < slen2) {
	startp += 3;
    }
    P2PF(ps[startp + 3], sp[0]);
    P2PF(ps[startp + 2], sp[1]);
    P2PF(ps[startp + 1], sp[2]);
    P2PF(spl->sp, sp[3]);	/* ensure endpoint starts inside */

    inside_context.a.p = &sp[3];
    inside_context.a.r = &slen2;
    bezier_clip(&inside_context, inside, sp, FALSE);

    PF2P(sp[3], ps[startp]);
    PF2P(sp[2], ps[startp + 1]);
    PF2P(sp[1], ps[startp + 2]);
    PF2P(sp[0], ps[startp + 3]);
    return startp;
}

/* FIXME - codegens should accept floats directly */
static void arrow_codegen_polygon(GVC_t * gvc, pointf p[], int np,
				  int fill)
{
    point P[16];		/* ugly - but this should be enough for arrows */
    int i;
    for (i = 0; i < np; i++)
	PF2P(p[i], P[i]);
    gvrender_polygon(gvc, P, np, fill);
}

/* FIXME - codegens should accept floats directly */
static void arrow_codegen_polyline(GVC_t * gvc, pointf p[], int np)
{
    point P[16];		/* ugly - but this should be enough for arrows */
    int i;
    for (i = 0; i < np; i++)
	PF2P(p[i], P[i]);
    gvrender_polyline(gvc, P, np);
}

/* FIXME - codegens should accept floats directly */
static void arrow_codegen_ellipse(GVC_t * gvc, pointf p, pointf r,
				  int fill)
{
    point P, R;
    PF2P(p, P);
    PF2P(r, R);
    gvrender_ellipse(gvc, P, R.x, R.y, fill);
}

static void arrow_type_normal(GVC_t * gvc, pointf p, pointf u, int flag)
{
    pointf q, v, a[5];

    v.x = -u.y * 0.35;
    v.y = u.x * 0.35;
    q.x = p.x + u.x;
    q.y = p.y + u.y;
    if (flag & ARR_MOD_INV) {
	a[0] = a[4] = p;
	a[1].x = p.x - v.x;
	a[1].y = p.y - v.y;
	a[2] = q;
	a[3].x = p.x + v.x;
	a[3].y = p.y + v.y;
    } else {
	a[0] = a[4] = q;
	a[1].x = q.x - v.x;
	a[1].y = q.y - v.y;
	a[2] = p;
	a[3].x = q.x + v.x;
	a[3].y = q.y + v.y;
    }
    if (flag & ARR_MOD_LEFT)
	arrow_codegen_polygon(gvc, a, 3, !(flag & ARR_MOD_OPEN));
    else if (flag & ARR_MOD_RIGHT)
	arrow_codegen_polygon(gvc, &a[2], 3, !(flag & ARR_MOD_OPEN));
    else
	arrow_codegen_polygon(gvc, &a[1], 3, !(flag & ARR_MOD_OPEN));
}

static void arrow_type_crow(GVC_t * gvc, pointf p, pointf u, int flag)
{
    pointf m, n, q, v, a[7];

    v.x = -u.y * 0.45;
    v.y = u.x * 0.45;
    q.x = p.x + u.x;
    q.y = p.y + u.y;
    m.x = p.x + u.x * 0.3;
    m.y = p.y + u.y * 0.3;
    n.x = p.x + u.x * 0.7;
    n.y = p.y + u.y * 0.7;
    if (flag & ARR_MOD_INV) {
	a[0] = a[6] = p;
	a[1].x = q.x - v.x;
	a[1].y = q.y - v.y;
	a[2] = a[4] = m;
	a[3] = q;
	a[5].x = q.x + v.x;
	a[5].y = q.y + v.y;
    } else {
	a[0] = a[6] = q;
	a[1].x = p.x - v.x;
	a[1].y = p.y - v.y;
	a[2] = a[4] = n;
	a[3] = p;
	a[5].x = p.x + v.x;
	a[5].y = p.y + v.y;
    }
    if (flag & ARR_MOD_LEFT)
	arrow_codegen_polygon(gvc, a, 5, 1);
    else if (flag & ARR_MOD_RIGHT)
	arrow_codegen_polygon(gvc, &a[2], 5, 1);
    else
	arrow_codegen_polygon(gvc, a, 7, 1);
}

static void arrow_type_tee(GVC_t * gvc, pointf p, pointf u, int flag)
{
    pointf m, n, q, v, a[4];

    v.x = -u.y;
    v.y = u.x;
    q.x = p.x + u.x;
    q.y = p.y + u.y;
    m.x = p.x + u.x * 0.2;
    m.y = p.y + u.y * 0.2;
    n.x = p.x + u.x * 0.6;
    n.y = p.y + u.y * 0.6;
    a[0].x = m.x + v.x;
    a[0].y = m.y + v.y;
    a[1].x = m.x - v.x;
    a[1].y = m.y - v.y;
    a[2].x = n.x - v.x;
    a[2].y = n.y - v.y;
    a[3].x = n.x + v.x;
    a[3].y = n.y + v.y;
    if (flag & ARR_MOD_LEFT) {
	a[0] = m;
	a[3] = n;
    } else if (flag & ARR_MOD_RIGHT) {
	a[1] = m;
	a[2] = n;
    }
    arrow_codegen_polygon(gvc, a, 4, 1);
    a[0] = p;
    a[1] = q;
    arrow_codegen_polyline(gvc, a, 2);
}

static void arrow_type_box(GVC_t * gvc, pointf p, pointf u, int flag)
{
    pointf m, q, v, a[4];

    v.x = -u.y * 0.4;
    v.y = u.x * 0.4;
    m.x = p.x + u.x * 0.8;
    m.y = p.y + u.y * 0.8;
    q.x = p.x + u.x;
    q.y = p.y + u.y;
    a[0].x = p.x + v.x;
    a[0].y = p.y + v.y;
    a[1].x = p.x - v.x;
    a[1].y = p.y - v.y;
    a[2].x = m.x - v.x;
    a[2].y = m.y - v.y;
    a[3].x = m.x + v.x;
    a[3].y = m.y + v.y;
    if (flag & ARR_MOD_LEFT) {
	a[0] = p;
	a[3] = m;
    } else if (flag & ARR_MOD_RIGHT) {
	a[1] = p;
	a[2] = m;
    }
    arrow_codegen_polygon(gvc, a, 4, !(flag & ARR_MOD_OPEN));
    a[0] = m;
    a[1] = q;
    arrow_codegen_polyline(gvc, a, 2);
}

static void arrow_type_diamond(GVC_t * gvc, pointf p, pointf u, int flag)
{
    pointf q, r, v, a[5];

    v.x = -u.y / 3.;
    v.y = u.x / 3.;
    r.x = p.x + u.x / 2.;
    r.y = p.y + u.y / 2.;
    q.x = p.x + u.x;
    q.y = p.y + u.y;
    a[0] = a[4] = q;
    a[1].x = r.x + v.x;
    a[1].y = r.y + v.y;
    a[2] = p;
    a[3].x = r.x - v.x;
    a[3].y = r.y - v.y;
    if (flag & ARR_MOD_LEFT)
	arrow_codegen_polygon(gvc, &a[2], 3, !(flag & ARR_MOD_OPEN));
    else if (flag & ARR_MOD_RIGHT)
	arrow_codegen_polygon(gvc, a, 3, !(flag & ARR_MOD_OPEN));
    else
	arrow_codegen_polygon(gvc, a, 4, !(flag & ARR_MOD_OPEN));
}

static void arrow_type_dot(GVC_t * gvc, pointf p, pointf u, int flag)
{
    pointf r;

    r.x = r.y = sqrt(u.x * u.x + u.y * u.y) / 2.;
    p.x += u.x / 2.;
    p.y += u.y / 2.;
    arrow_codegen_ellipse(gvc, p, r, !(flag & ARR_MOD_OPEN));
}

static pointf arrow_gen_type(GVC_t * gvc, pointf p, pointf u, int flag)
{
    int f;
    arrowtype_t *arrowtype;

    f = flag & ((1 << 8) - 1);
    for (arrowtype = Arrowtypes; arrowtype->type; arrowtype++) {
	if (f == arrowtype->type) {
	    u.x *= arrowtype->lenfact;
	    u.y *= arrowtype->lenfact;
	    (arrowtype->gen) (gvc, p, u, flag);
	    p.x = p.x + u.x;
	    p.y = p.y + u.y;
	    break;
	}
    }
    return p;
}

void arrow_newgen(GVC_t * gvc, pointf p, pointf u, double scale, int flag)
{
    double s;
    int f;

    /* Dotted and dashed styles on the arrowhead are ugly (dds) */
    /* linewidth needs to be reset */
    gvrender_begin_context(gvc);
    gvrender_set_style(gvc, gvc->defaultlinestyle);

    /* generate arrowhead vector */
    u.x -= p.x;
    u.y -= p.y;
    /* the EPSILONs are to keep this stable as length of u approaches 0.0 */
    s = ARROW_LENGTH * scale / (sqrt(u.x * u.x + u.y * u.y) + EPSILON);
    u.x += (u.x >= 0.0) ? EPSILON : -EPSILON;
    u.y += (u.y >= 0.0) ? EPSILON : -EPSILON;
    u.x *= s;
    u.y *= s;

    /* arrow head closest to node */
    f = flag & ((1 << 16) - 1);
    p = arrow_gen_type(gvc, p, u, f);

    /* arrow head furthest from node */
    /*   start where first one ended */
    f = (flag >> 16) & ((1 << 16) - 1);
    arrow_gen_type(gvc, p, u, f);

    gvrender_end_context(gvc);
}

/* FIXME emit.c and output.c require wrapper for int point coords */
void arrow_gen(GVC_t * gvc, point p, point u, double scale, int flag)
{
    pointf P, U;

    P2PF(p, P);
    P2PF(u, U);
    arrow_newgen(gvc, P, U, scale, flag);
}
