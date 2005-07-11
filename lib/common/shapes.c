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

/* for sincos */
#define _GNU_SOURCE 1

#include	"render.h"
#include	"gvc.h"
#include	"htmltable.h"
#include	<limits.h>

#ifndef DISABLE_CODEGENS
extern codegen_t VRML_CodeGen;
#endif

#define FILLED 	(1 << 0)
#define ROUNDED (1 << 1)
#define DIAGONALS (1 << 2)
#define AUXLABELS (1 << 3)
#define INVISIBLE (1 << 4)
#define RBCONST 12
#define RBCURVE .5

#ifdef HAVE_SINCOS
extern void sincos(double x, double *s, double *c);
#else
# define sincos(x,s,c) *s = sin(x); *c = cos(x)
#endif

static port Center = { {0, 0}, -1, 0, 0, 0, 1, 0, 0 };

#define ATTR_SET(a,n) ((a) && (*(agxget(n,a->index)) != '\0'))
#define DEF_POINT 0.05
  /* extra null character needed to avoid style emitter from thinking
   * there are arguments.
   */
static char *point_style[3] = { "invis\0", "filled\0", 0 };
static shape_desc *point_desc;

/* forward declarations of functions used in shapes tables */

static void poly_init(node_t * n);
static void poly_free(node_t * n);
static port poly_port(node_t * n, char *portname, char *);
static boolean poly_inside(inside_t * inside_context, pointf p);
static int poly_path(node_t* n, port* p, int side, box rv[], int *kptr);
static void poly_gencode(GVJ_t * job, node_t * n);

static void record_init(node_t * n);
static void record_free(node_t * n);
static port record_port(node_t * n, char *portname, char *);
static boolean record_inside(inside_t * inside_context, pointf p);
static int record_path(node_t* n, port* p, int side, box rv[], int *kptr);
static void record_gencode(GVJ_t * job, node_t * n);

static void point_init(node_t * n);

static boolean epsf_inside(inside_t * inside_context, pointf p);
static void epsf_gencode(GVJ_t * job, node_t * n);

/* polygon descriptions.  "polygon" with 0 sides takes all user control */

/*				      regul perip sides orien disto skew */
static polygon_t p_polygon = { FALSE, 1, 0, 0., 0., 0. };

/* builtin polygon descriptions */
static polygon_t p_ellipse = { FALSE, 1, 1, 0., 0., 0. };
static polygon_t p_circle = { TRUE, 1, 1, 0., 0., 0. };
static polygon_t p_egg = { FALSE, 1, 1, 0., -.3, 0. };
static polygon_t p_triangle = { FALSE, 1, 3, 0., 0., 0. };
static polygon_t p_box = { FALSE, 1, 4, 0., 0., 0. };
static polygon_t p_plaintext = { FALSE, 0, 4, 0., 0., 0. };
static polygon_t p_diamond = { FALSE, 1, 4, 45., 0., 0. };
static polygon_t p_trapezium = { FALSE, 1, 4, 0., -.4, 0. };
static polygon_t p_parallelogram = { FALSE, 1, 4, 0., 0., .6 };
static polygon_t p_house = { FALSE, 1, 5, 0., -.64, 0. };
static polygon_t p_pentagon = { FALSE, 1, 5, 0., 0., 0. };
static polygon_t p_hexagon = { FALSE, 1, 6, 0., 0., 0. };
static polygon_t p_septagon = { FALSE, 1, 7, 0., 0., 0. };
static polygon_t p_octagon = { FALSE, 1, 8, 0., 0., 0. };

/* redundant and undocumented builtin polygons */
static polygon_t p_doublecircle = { TRUE, 2, 1, 0., 0., 0. };
static polygon_t p_invtriangle = { FALSE, 1, 3, 180., 0., 0. };
static polygon_t p_invtrapezium = { FALSE, 1, 4, 180., -.4, 0. };
static polygon_t p_invhouse = { FALSE, 1, 5, 180., -.64, 0. };
static polygon_t p_doubleoctagon = { FALSE, 2, 8, 0., 0., 0. };
static polygon_t p_tripleoctagon = { FALSE, 3, 8, 0., 0., 0. };
static polygon_t p_Mdiamond =
    { FALSE, 1, 4, 45., 0., 0., DIAGONALS | AUXLABELS };
static polygon_t p_Msquare = { TRUE, 1, 4, 0., 0., 0., DIAGONALS };
static polygon_t p_Mcircle =
    { TRUE, 1, 1, 0., 0., 0., DIAGONALS | AUXLABELS };

/*
 * every shape has these functions:
 *
 * void		SHAPE_init(node_t *n)
 *			initialize the shape (usually at least its size).
 * void		SHAPE_free(node_t *n)
 *			free all memory used by the shape
 * port		SHAPE_port(node_t *n, char *portname)
 *			return the aiming point and slope (if constrained)
 *			of a port.
 * int		SHAPE_inside(inside_t *inside_context, pointf p, edge_t *e);
 *			test if point is inside the node shape which is
 *			assumed convex.
 *			the point is relative to the node center.  the edge
 *			is passed in case the port affects spline clipping.
 * int		SHAPE_path(node *n, edge_t *e, int pt, box path[], int *nbox)
 *			create a path for the port of e that touches n,
 *			return side
 * void		SHAPE_gencode(GVJ_t *job, node_t *n)
 *			generate graphics code for a node.
 *
 * some shapes, polygons in particular, use additional shape control data *
 *
 */

static shape_functions poly_fns = {
    poly_init,
    poly_free,
    poly_port,
    poly_inside,
    poly_path,
    poly_gencode
};
static shape_functions point_fns = {
    point_init,
    poly_free,
    poly_port,
    poly_inside,
    NULL,
    poly_gencode
};
static shape_functions record_fns = {
    record_init,
    record_free,
    record_port,
    record_inside,
    record_path,
    record_gencode
};
static shape_functions epsf_fns = {
    epsf_init,
    epsf_free,
    poly_port,
    epsf_inside,
    NULL,
    epsf_gencode
};

static shape_desc Shapes[] = {	/* first entry is default for no such shape */
    {"box", &poly_fns, &p_box},
    {"polygon", &poly_fns, &p_polygon},
    {"ellipse", &poly_fns, &p_ellipse},
    {"circle", &poly_fns, &p_circle},
    {"point", &point_fns, &p_circle},
    {"egg", &poly_fns, &p_egg},
    {"triangle", &poly_fns, &p_triangle},
    {"none", &poly_fns, &p_plaintext},
    {"plaintext", &poly_fns, &p_plaintext},
    {"diamond", &poly_fns, &p_diamond},
    {"trapezium", &poly_fns, &p_trapezium},
    {"parallelogram", &poly_fns, &p_parallelogram},
    {"house", &poly_fns, &p_house},
    {"pentagon", &poly_fns, &p_pentagon},
    {"hexagon", &poly_fns, &p_hexagon},
    {"septagon", &poly_fns, &p_septagon},
    {"octagon", &poly_fns, &p_octagon},
    {"rect", &poly_fns, &p_box},
    {"rectangle", &poly_fns, &p_box},
    {"doublecircle", &poly_fns, &p_doublecircle},
    {"doubleoctagon", &poly_fns, &p_doubleoctagon},
    {"tripleoctagon", &poly_fns, &p_tripleoctagon},
    {"invtriangle", &poly_fns, &p_invtriangle},
    {"invtrapezium", &poly_fns, &p_invtrapezium},
    {"invhouse", &poly_fns, &p_invhouse},
    {"Mdiamond", &poly_fns, &p_Mdiamond},
    {"Msquare", &poly_fns, &p_Msquare},
    {"Mcircle", &poly_fns, &p_Mcircle},
/*  *** shapes other than polygons  *** */
    {"record", &record_fns, NULL},
    {"Mrecord", &record_fns, NULL},
    {"epsf", &epsf_fns, NULL},
    {NULL, NULL, NULL}
};

static void unrecognized(node_t * n, char *p)
{
    agerr(AGWARN, "node %s, port %s unrecognized\n", n->name, p);
}

static double quant(double val, double q)
{
    int i;
    i = val / q;
    if (i * q + .00001 < val)
	i++;
    return i * q;
}

static int same_side(pointf p0, pointf p1, pointf L0, pointf L1)
{
    int s0, s1;
    double a, b, c;

    /* a x + b y = c */
    a = -(L1.y - L0.y);
    b = (L1.x - L0.x);
    c = a * L0.x + b * L0.y;

    s0 = (a * p0.x + b * p0.y - c >= 0);
    s1 = (a * p1.x + b * p1.y - c >= 0);
    return (s0 == s1);
}

static
void activepencolor(GVJ_t * job, node_t * n)
{
    char *color;

    color = late_nnstring(n, N_activepencolor, DEFAULT_ACTIVEPENCOLOR);
    gvrender_set_pencolor(job, color);
}

static
void activefillcolor(GVJ_t * job, node_t * n)
{
    char *color;

    color = late_nnstring(n, N_activefillcolor, DEFAULT_ACTIVEFILLCOLOR);
    gvrender_set_fillcolor(job, color);
}

static
void pencolor(GVJ_t * job, node_t * n)
{
    char *color;

    color = late_nnstring(n, N_color, "");
    if (color[0])
	gvrender_set_pencolor(job, color);
    else
	gvrender_set_pencolor(job, DEFAULT_COLOR);
}

static
char *findFill(node_t * n)
{
    char *color;

    color = late_nnstring(n, N_fillcolor, "");
    if (!color[0]) {
	/* for backward compatibilty, default fill is same as pen */
	color = late_nnstring(n, N_color, "");
	if (!color[0]) {
	    if (ND_shape(n) == point_desc) {
		color = "black";
	    }
#ifndef DISABLE_CODEGENS
	    else {
		color = (Output_lang == MIF ? "black" : DEFAULT_FILL);
	    }
#endif
	}
    }
    return color;
}

static void fillcolor(GVJ_t * job, node_t * n)
{
    gvrender_set_fillcolor(job, findFill(n));
}

static char **checkStyle(node_t * n, int *flagp)
{
    char *style;
    char **pstyle = 0;
    int istyle = 0;
    polygon_t *poly;

    style = late_nnstring(n, N_style, "");
    if (style[0]) {
	char **pp;
	char **qp;
	char *p;
	pp = pstyle = parse_style(style);
	while ((p = *pp)) {
	    if (strcmp(p, "filled") == 0) {
		istyle |= FILLED;
		pp++;
	    } else if (strcmp(p, "rounded") == 0) {
		istyle |= ROUNDED;
		qp = pp; /* remove rounded from list passed to renderer */
		do {
		    qp++;
		    *(qp-1) = *qp;
		} while (*qp);
	    } else if (strcmp(p, "diagonals") == 0) {
		istyle |= DIAGONALS;
		qp = pp; /* remove diagonals from list passed to renderer */
		do {
		    qp++;
		    *(qp-1) = *qp;
		} while (*qp);
	    } else if (strcmp(p, "invis") == 0) {
		istyle |= INVISIBLE;
		pp++;
	    }
	    else pp++;
	}
    }
    if ((poly = ND_shape(n)->polygon))
	istyle |= poly->option;

    *flagp = istyle;
    return pstyle;
}

static int stylenode(GVJ_t * job, node_t * n)
{
    char **pstyle;
    int istyle;

    if ((pstyle = checkStyle(n, &istyle)))
	gvrender_set_style(job, pstyle);
    return istyle;
}

static void Mcircle_hack(GVJ_t * job, node_t * n)
{
    double x, y;
    point A[2], p;

    y = .7500;
    x = .6614;			/* x^2 + y^2 = 1.0 */
    p.y = y * ND_ht_i(n) / 2.0;
    p.x = ND_rw_i(n) * x;	/* assume node is symmetric */

    A[0] = add_points(p, ND_coord_i(n));
    A[1].y = A[0].y;
    A[1].x = A[0].x - 2 * p.x;
    gvrender_polyline(job, A, 2);
    A[0].y -= 2 * p.y;
    A[1].y = A[0].y;
    gvrender_polyline(job, A, 2);
}

static point interpolate(double t, point p0, point p1)
{
    point rv;
    rv.x = p0.x + t * (p1.x - p0.x);
    rv.y = p0.y + t * (p1.y - p0.y);
    return rv;
}

static void round_corners(GVJ_t * job, node_t * n, point * A, int sides,
			  int style)
{
    point *B, C[2], p0, p1;
    pointf BF[4];
    double d, dx, dy, t;
    int i, seg, mode;

    if (style & DIAGONALS)
	mode = DIAGONALS;
    else
	mode = ROUNDED;
    B = N_NEW(4 * sides + 4, point);
    i = 0;
    for (seg = 0; seg < sides; seg++) {
	p0 = A[seg];
	if (seg < sides - 1)
	    p1 = A[seg + 1];
	else
	    p1 = A[0];
	dx = p1.x - p0.x;
	dy = p1.y - p0.y;
	d = sqrt(dx * dx + dy * dy);
	/*t = ((mode == ROUNDED)? RBCONST / d : .5); */
	t = RBCONST / d;
	if (mode != ROUNDED)
	    B[i++] = p0;
	if (mode == ROUNDED)
	    B[i++] = interpolate(RBCURVE * t, p0, p1);
	B[i++] = interpolate(t, p0, p1);
	B[i++] = interpolate(1.0 - t, p0, p1);
	if (mode == ROUNDED)
	    B[i++] = interpolate(1.0 - RBCURVE * t, p0, p1);
    }
    B[i++] = B[0];
    B[i++] = B[1];
    B[i++] = B[2];

    if (mode == ROUNDED) {
	if (style & FILLED) {
	    int j = 0;
	    char* fillc = findFill(n);
	    point* pts = N_GNEW(2*sides,point);
    	    gvrender_begin_context(job);
	    gvrender_set_pencolor (job, fillc);
	    gvrender_set_fillcolor (job, fillc);
	    for (seg = 0; seg < sides; seg++) {
		pts[j++] = B[4 * seg + 1];
		pts[j++] = B[4 * seg + 2];
	    }
	    gvrender_polygon(job, pts, 2*sides, TRUE);
	    free (pts);
	    for (seg = 0; seg < sides; seg++) {
		for (i = 0; i < 4; i++)
		    P2PF(B[4 * seg + 2 + i], BF[i]);
		gvrender_beziercurve(job, BF, 4, FALSE, FALSE, TRUE);
	    }
    	    gvrender_end_context(job);
	}
	pencolor(job, n);
	for (seg = 0; seg < sides; seg++) {
	    gvrender_polyline(job, B + 4 * seg + 1, 2);

	    /* convert to floats for gvrender api */
	    for (i = 0; i < 4; i++)
		P2PF(B[4 * seg + 2 + i], BF[i]);
	    gvrender_beziercurve(job, BF, 4, FALSE, FALSE, FALSE);
	}
    } else {			/* diagonals are weird.  rewrite someday. */
	pencolor(job, n);
	if (style & FILLED)
	    fillcolor(job, n);	/* emit fill color */
	gvrender_polygon(job, A, sides, style & FILLED);
	for (seg = 0; seg < sides; seg++) {
#ifdef NOTDEF
	    C[0] = B[3 * seg];
	    C[1] = B[3 * seg + 3];
	    gvrender_polyline(job, C, 2);
#endif
	    C[0] = B[3 * seg + 2];
	    C[1] = B[3 * seg + 4];
	    gvrender_polyline(job, C, 2);
	}
    }
    free(B);
}

/*=============================poly start=========================*/

/* userSize;
 * Return maximum size, in points, of width and height supplied
 * by user, if any. Return 0 otherwise.
 */
static double userSize(node_t * n)
{
    double w, h;
    w = late_double(n, N_width, 0.0, MIN_NODEWIDTH);
    h = late_double(n, N_height, 0.0, MIN_NODEHEIGHT);
    return POINTS(MAX(w, h));
}

shape_kind shapeOf(node_t * n)
{
    shape_desc *sh = ND_shape(n);
    void (*ifn) (node_t *);

    if (!sh)
	return SH_UNSET;
    ifn = ND_shape(n)->fns->initfn;
    if (ifn == poly_init)
	return SH_POLY;
    else if (ifn == record_init)
	return SH_RECORD;
    else if (ifn == point_init)
	return SH_POINT;
    else
	return SH_USER;
}

boolean isPolygon(node_t * n)
{
    return (ND_shape(n) && (ND_shape(n)->fns->initfn == poly_init));
}

static void poly_init(node_t * n)
{
    pointf dimen, bb;
    pointf P, Q, R;
    pointf *vertices;
    char *p;
    double temp, alpha, beta, gamma, delta;
    double orientation, distortion, skew;
    double sectorangle, sidelength, skewdist, gdistortion, gskew;
    double angle, sinx, cosx, xmax, ymax, scalex, scaley;
    double width, height, marginx, marginy;
    int regular, peripheries, sides;
    int i, j, outp;
    polygon_t *poly = NEW(polygon_t);

    regular = ND_shape(n)->polygon->regular;
    peripheries = ND_shape(n)->polygon->peripheries;
    sides = ND_shape(n)->polygon->sides;
    orientation = ND_shape(n)->polygon->orientation;
    skew = ND_shape(n)->polygon->skew;
    distortion = ND_shape(n)->polygon->distortion;
    regular |= mapbool(agget(n, "regular"));

    /* all calculations in floating point POINTS */

    /* make x and y dimensions equal if node is regular
     *   If the user has specified either width or height, use the max.
     *   Else use minimum default value.
     * If node is not regular, use the current width and height.
     */
    if (regular) {
	double sz = userSize(n);
	if (sz > 0.0)
	    width = height = sz;
	else {
	    width = ND_width(n);
	    height = ND_height(n);
	    width = height = POINTS(MIN(width, height));
	}
    } else {
	width = POINTS(ND_width(n));
	height = POINTS(ND_height(n));
    }

    peripheries = late_int(n, N_peripheries, peripheries, 0);
    orientation += late_double(n, N_orientation, 0.0, -360.0);
    if (sides == 0) {		/* not for builtins */
	skew = late_double(n, N_skew, 0.0, -100.0);
	sides = late_int(n, N_sides, 4, 0);
	distortion = late_double(n, N_distortion, 0.0, -100.0);
    }

    /* get label dimensions */
    dimen = ND_label(n)->dimen;

    /* minimal whitespace around label */
    if ((dimen.x > 0.0) || (dimen.y > 0.0)) {
	/* padding */
	if ((p = agget(n, "margin"))) {
	    i = sscanf(p, "%lf,%lf", &marginx, &marginy);
	    if (i > 0) {
		dimen.x += 2 * POINTS(marginx);
		if (i > 1)
		    dimen.y += 2 * POINTS(marginy);
		else
		    dimen.y += 2 * POINTS(marginx);
	    } else
		PAD(dimen);
	} else
	    PAD(dimen);
    }

    if (mapbool(late_string(n, N_fixed, "false"))) {
	if ((width < dimen.x) || (height < dimen.y))
	    agerr(AGWARN,
		  "node '%s', graph '%s' size too small for label\n",
		  n->name, n->graph->name);
	dimen.x = dimen.y = 0;
    } else {
	if (ND_shape(n)->usershape) {
	    point imagesize;
	    char *sfile = agget(n, "shapefile");

	    imagesize = user_shape_size(n, sfile);
	    if ((imagesize.x == -1) && (imagesize.y == -1)) {
		agerr(AGERR,
		      "No or improper shapefile=\"%s\" for node \"%s\"\n",
		      (sfile ? sfile : "<nil>"), n->name);
	    } else
		GD_has_images(n->graph) = 1;
	    dimen.x = MAX(dimen.x, imagesize.x);
	    dimen.y = MAX(dimen.y, imagesize.y);
	}
    }

    /* quantization */
    if ((temp = GD_drawing(n->graph)->quantum) > 0.0) {
	temp = POINTS(temp);
	dimen.x = quant(dimen.x, temp);
	dimen.y = quant(dimen.y, temp);
    }

    /* If regular, make label dimensions the same.
     * Need this to guarantee final node size is regular.
     */
    if (regular) {
	dimen.x = dimen.y = MAX(dimen.x, dimen.y);
    }

    /* I don't know how to distort or skew ellipses in postscript */
    /* Convert request to a polygon with a large number of sides */
    if ((sides <= 2) && ((distortion != 0.) || (skew != 0.))) {
	sides = 120;
    }

    /* add extra padding to allow for the shape */
    if (sides <= 2) {
	/* for ellipses, add padding based on the smaller radii */
	if (dimen.y > dimen.x)
	    temp = dimen.x * (sqrt(2.) - 1.);
	else
	    temp = dimen.y * (sqrt(2.) - 1.);
	dimen.x += temp;
	dimen.y += temp;
    } else if (sides == 4 && (ROUND(orientation) % 90) == 0
	       && distortion == 0. && skew == 0.) {
	/* for regular boxes the fit should be exact */
    } else {
	/* for all other polygon shapes, compute the inner ellipse
	   and then pad for that  */
	temp = cos(PI / sides);
	dimen.x /= temp;
	dimen.y /= temp;
	/* add padding based on the smaller radii */
	if (dimen.y > dimen.x)
	    temp = dimen.x * (sqrt(2.) - 1.);
	else
	    temp = dimen.y * (sqrt(2.) - 1.);
	dimen.x += temp;
	dimen.y += temp;
    }

    /* increase node size to width/height if needed */
    bb.x = width = MAX(width, dimen.x);
    bb.y = height = MAX(height, dimen.y);

    /* adjust text justification */
    if (!mapbool(late_string(n, N_nojustify, "false"))) {
	ND_label(n)->d.x = bb.x - dimen.x;
	ND_label(n)->d.y = bb.y - dimen.y;
    }

    outp = peripheries;
    if (peripheries < 1)
	outp = 1;
    if (sides < 3) {		/* ellipses */
	sides = 1;
	vertices = N_NEW(outp, pointf);
	P.x = bb.x / 2.;
	P.y = bb.y / 2.;
	vertices[0] = P;
	if (peripheries > 1) {
	    for (j = 1; j < peripheries; j++) {
		P.x += GAP;
		P.y += GAP;
		vertices[j] = P;
	    }
	    bb.x = 2. * P.x;
	    bb.y = 2. * P.y;
	}
    } else {
	vertices = N_NEW(outp * sides, pointf);
	sectorangle = 2. * PI / sides;
	sidelength = sin(sectorangle / 2.);
	skewdist = hypot(fabs(distortion) + fabs(skew), 1.);
	gdistortion = distortion * sqrt(2.) / cos(sectorangle / 2.);
	gskew = skew / 2.;
	angle = (sectorangle - PI) / 2.;
	sincos(angle, &sinx, &cosx);
	R.x = .5 * cosx;
	R.y = .5 * sinx;
	xmax = ymax = 0.;
	angle += (PI - sectorangle) / 2.;
	for (i = 0; i < sides; i++) {

	    /*next regular vertex */
	    angle += sectorangle;
	    sincos(angle, &sinx, &cosx);
	    R.x += sidelength * cosx;
	    R.y += sidelength * sinx;

	    /*distort and skew */
	    P.x = R.x * (skewdist + R.y * gdistortion) + R.y * gskew;
	    P.y = R.y;

	    /*orient P.x,P.y */
	    alpha = RADIANS(orientation) + atan2(P.y, P.x);
	    sincos(alpha, &sinx, &cosx);
	    P.x = P.y = hypot(P.x, P.y);
	    P.x *= cosx;
	    P.y *= sinx;

	    /*scale for label */
	    P.x *= bb.x;
	    P.y *= bb.y;

	    /*find max for bounding box */
	    xmax = MAX(fabs(P.x), xmax);
	    ymax = MAX(fabs(P.y), ymax);

	    /* store result in array of points */
	    vertices[i] = P;
	}

	/* apply minimum dimensions */
	xmax *= 2.;
	ymax *= 2.;
	bb.x = MAX(width, xmax);
	bb.y = MAX(height, ymax);
	scalex = bb.x / xmax;
	scaley = bb.y / ymax;

	for (i = 0; i < sides; i++) {
	    P = vertices[i];
	    P.x *= scalex;
	    P.y *= scaley;
	    vertices[i] = P;
	}

	if (peripheries > 1) {
	    Q = vertices[(sides - 1)];
	    R = vertices[0];
	    beta = atan2(R.y - Q.y, R.x - Q.x);
	    for (i = 0; i < sides; i++) {

		/*for each vertex find the bisector */
		P = Q;
		Q = R;
		R = vertices[(i + 1) % sides];
		alpha = beta;
		beta = atan2(R.y - Q.y, R.x - Q.x);
		gamma = (alpha + PI - beta) / 2.;

		/*find distance along bisector to */
		/*intersection of next periphery */
		temp = GAP / sin(gamma);

		/*convert this distance to x and y */
		delta = alpha - gamma;
		sincos(delta, &sinx, &cosx);
		sinx *= temp;
		cosx *= temp;

		/*save the vertices of all the */
		/*peripheries at this base vertex */
		for (j = 1; j < peripheries; j++) {
		    Q.x += cosx;
		    Q.y += sinx;
		    vertices[i + j * sides] = Q;
		}
	    }
	    for (i = 0; i < sides; i++) {
		P = vertices[i + (peripheries - 1) * sides];
		bb.x = MAX(2. * fabs(P.x), bb.x);
		bb.y = MAX(2. * fabs(P.y), bb.y);
	    }
	}
    }
    poly->regular = regular;
    poly->peripheries = peripheries;
    poly->sides = sides;
    poly->orientation = orientation;
    poly->skew = skew;
    poly->distortion = distortion;
    poly->vertices = vertices;

    ND_width(n) = PS2INCH(bb.x);
    ND_height(n) = PS2INCH(bb.y);
    ND_shape_info(n) = (void *) poly;
}

static void poly_free(node_t * n)
{
    polygon_t *p = ND_shape_info(n);

    if (p) {
	free(p->vertices);
	free(p);
    }
}

#define GET_PORT_BOX(n,e) ((n) == (e)->head ? ED_head_port(e).bp : ED_tail_port(e).bp)

static boolean poly_inside(inside_t * inside_context, pointf p)
{
    static node_t *lastn;	/* last node argument */
    static polygon_t *poly;
    static int last, outp, sides;
    static pointf O;		/* point (0,0) */
    static pointf *vertex;
    static double xsize, ysize, scalex, scaley, box_URx, box_URy;

    int i, i1, j, s;
    pointf P, Q, R;
    box *bp = inside_context->s.bp;
    node_t *n = inside_context->s.n;

    P = flip_ptf(p, GD_rankdir(n->graph));

    /* Quick test if port rectangle is target */
    if (bp) {
	box bbox = *bp;
	return INSIDE(P, bbox);
    }

    if (n != lastn) {
	poly = (polygon_t *) ND_shape_info(n);
	vertex = poly->vertices;
	sides = poly->sides;

	/* get point and node size adjusted for rankdir=LR */
	if (GD_flip(n->graph)) {
	    ysize = ND_lw_i(n) + ND_rw_i(n);
	    xsize = ND_ht_i(n);
	} else {
	    xsize = ND_lw_i(n) + ND_rw_i(n);
	    ysize = ND_ht_i(n);
	}

	/* scale */
	if (xsize == 0.0)
	    xsize = 1.0;
	if (ysize == 0.0)
	    ysize = 1.0;
	scalex = POINTS(ND_width(n)) / xsize;
	scaley = POINTS(ND_height(n)) / ysize;
	box_URx = POINTS(ND_width(n)) / 2.0;
	box_URy = POINTS(ND_height(n)) / 2.0;

	/* index to outer-periphery */
	outp = (poly->peripheries - 1) * sides;
	if (outp < 0)
	    outp = 0;
	lastn = n;
    }

    /* scale */
    P.x *= scalex;
    P.y *= scaley;

    /* inside bounding box? */
    if ((fabs(P.x) > box_URx) || (fabs(P.y) > box_URy))
	return FALSE;

    /* ellipses */
    if (sides <= 2)
	return (hypot(P.x / box_URx, P.y / box_URy) < 1.);

    /* use fast test in case we are converging on a segment */
    i = last % sides;		/*in case last left over from larger polygon */
    i1 = (i + 1) % sides;
    Q = vertex[i + outp];
    R = vertex[i1 + outp];
    if (!(same_side(P, O, Q, R)))
	return FALSE;
    if ((s = same_side(P, Q, R, O)) && (same_side(P, R, O, Q)))
	return TRUE;
    for (j = 1; j < sides; j++) {
	if (s) {
	    i = i1;
	    i1 = (i + 1) % sides;
	} else {
	    i1 = i;
	    i = (i + sides - 1) % sides;
	}
	if (!(same_side(P, O, vertex[i + outp], vertex[i1 + outp]))) {
	    last = i;
	    return FALSE;
	}
    }
    last = i;			/* in case next edge is to same side */
    return TRUE;
}

/* poly_path:
 * Generate box path from port to border.
 * Store boxes in rv and number of boxes in kptr.
 * side gives preferred side of bounding box for last node.
 * Return actual side. Returning 0 indicates nothing done.
 */
static int poly_path(node_t* n, port* p, int side, box rv[], int *kptr)
{
    side = 0;

    if (ND_label(n)->html && ND_has_port(n)) {
	side = html_path(n, p, side, rv, kptr);
    }
    return side;
}

/* invflip_side:
 */
static int invflip_side (int side, int rankdir)
{
    switch (rankdir) {
    case RANKDIR_TB:
	break;
    case RANKDIR_BT:
	switch (side) {
	case TOP:
	    side = BOTTOM;
	    break;
	case BOTTOM:
	    side = TOP;
	    break;
	default:
	    break;
	}
	break;
    case RANKDIR_LR:
	switch (side) {
	case TOP:
	    side = RIGHT;
	    break;
	case BOTTOM:
	    side = LEFT;
	    break;
	case LEFT:
	    side = TOP;
	    break;
	case RIGHT:
	    side = BOTTOM;
	    break;
	}
	break;
    case RANKDIR_RL:
	switch (side) {
	case TOP:
	    side = RIGHT;
	    break;
	case BOTTOM:
	    side = LEFT;
	    break;
	case LEFT:
	    side = BOTTOM;
	    break;
	case RIGHT:
	    side = TOP;
	    break;
	}
	break;
    }
    return side;
}

/* invflip_angle:
 */
static double invflip_angle (double angle, int rankdir)
{
    switch (rankdir) {
    case RANKDIR_TB:
	break;
    case RANKDIR_BT:
	angle *= -1; 
	break;
    case RANKDIR_LR:
	angle -= PI * 0.5;
	break;
    case RANKDIR_RL:
	if (angle == PI)
	    angle = -0.5 * PI;
	else if (angle == PI * 0.75)
	    angle = -0.25 * PI;
	else if (angle == PI * 0.5)
	    angle = 0;
	else if (angle == PI * 0.25)
	    angle = angle;
	else if (angle == 0)
	    angle = PI * 0.5;
	else if (angle == PI * -0.25)
	    angle = PI * 0.75;
	else if (angle == PI * -0.5)
	    angle = PI;
	else if (angle == PI * -0.75)
	    angle = angle;
	break;
    }
    return angle;
}

/* would like = 0, but spline router croaks
 * Basically, if the point is actually on a box, the curve fitter
 * may, at some point, compute a curve which lies entirely outside
 * the polygon except at the two endpoints. Thus, by guaranteeing that
 * the point lies in the interior, such bad cases can't happen.
 */
#define MARGIN  1

/* compassPoint:
 * Compute compass points for non-trivial shapes.
 * It finds where the ray ((0,0),(x,y)) hits the boundary and
 * return it.
 * Assumes ictxt and ictxt->n are non-NULL.
 */
static point 
compassPoint(inside_t* ictxt, double y, double x)
{
    point  p;
    pointf curve[4];  /* bezier control points for a straight line */
    node_t* n = ictxt->s.n;

    curve[0].x = 0;
    curve[0].y = 0;
    curve[1].x = x/3;
    curve[1].y = y/3;
    curve[2].x = 2*x/3;
    curve[2].y = 2*y/3;
    curve[3].x = x;
    curve[3].y = y;

    bezier_clip(ictxt, ND_shape(n)->fns->insidefn, curve, 1);

    p.x = ROUND(curve[0].x);
    p.y = ROUND(curve[0].y);

    return p;
}

/* compassPort:
 * Attach a compass point to a port pp, and fill in remaining fields.
 * n is the corresponding node; bp is the bounding box of the port.
 * compass is the compass point
 * Return 1 if unrecognized compass point, in which case we
 * use the center.
 * This function also finishes initializing the port structure,
 * even if no compass point is involved.
 * The sides value gives the set of sides shared by the port. This
 * is used with a compass point to indicate if the port is exposed, to
 * set the port's side value.
 * 
 * FIX: For purposes, of rankdir=BT or RL, this assumes nodes are up-down
 * symmetric, left-right symmetric, and convex.
 */
static int 
compassPort(node_t* n, box* bp, port* pp, char* compass, int sides, inside_t* ictxt)
{
    box b;
    point p, ctr;
    int rv = 0;
    double theta = 0.0;
    int constrain = 0;
    int side = 0;
    int clip = TRUE;
    int defined;

    if (bp) {
	b = *bp;
	p = pointof((b.LL.x + b.UR.x) / 2, (b.LL.y + b.UR.y) / 2);
	defined = TRUE;
    } else {
	p.x = p.y = 0;
	if (GD_flip(n->graph)) {
	    b.UR.x = ND_ht_i(n) / 2;
	    b.LL.x = -b.UR.x;
	    b.UR.y = ND_lw_i(n);
	    b.LL.y = -b.UR.y;
	} else {
	    b.UR.y = ND_ht_i(n) / 2;
	    b.LL.y = -b.UR.y;
	    b.UR.x = ND_lw_i(n);
	    b.LL.x = -b.UR.x;
	}
	defined = FALSE;
    }
    ctr = p;
    if (compass && *compass) {
	switch (*compass++) {
	case 'e':
	    p.x = b.UR.x + MARGIN;
	    theta = 0.0;
	    constrain = 1;
	    defined = TRUE;
	    clip = FALSE;
	    side = sides & RIGHT;
	    break;
	case 's':
	    p.y = b.LL.y - MARGIN;
	    constrain = 1;
	    clip = FALSE;
	    side = sides & BOTTOM;
	    switch (*compass) {
	    case '\0':
		theta = -PI * 0.5;
		defined = TRUE;
		break;
	    case 'e':
		theta = -PI * 0.25;
		defined = TRUE;
		if (ictxt) p = compassPoint (ictxt, -MAXINT, MAXINT);
		else p.x = b.UR.x + MARGIN;
		break;
	    case 'w':
		theta = -PI * 0.75;
		defined = TRUE;
		if (ictxt) p = compassPoint (ictxt, -MAXINT, -MAXINT);
		else p.x = b.LL.x - MARGIN;
		break;
	    default:
		p.y = ctr.y;
		constrain = 0;
		clip = TRUE;
		rv = 1;
		break;
	    }
	    break;
	case 'w':
	    p.x = b.LL.x - MARGIN;
	    theta = PI;
	    constrain = 1;
	    defined = TRUE;
	    clip = FALSE;
	    side = sides & LEFT;
	    break;
	case 'n':
	    p.y = b.UR.y + MARGIN;
	    constrain = 1;
	    clip = FALSE;
	    side = sides & TOP;
	    switch (*compass) {
	    case '\0':
		defined = TRUE;
		theta = PI * 0.5;
		break;
	    case 'e':
		defined = TRUE;
		theta = PI * 0.25;
		if (ictxt) p = compassPoint (ictxt, MAXINT, MAXINT);
		else p.x = b.UR.x + MARGIN;
		break;
	    case 'w':
		defined = TRUE;
		theta = PI * 0.75;
		if (ictxt) p = compassPoint (ictxt, MAXINT, -MAXINT);
		else p.x = b.LL.x - MARGIN;
		break;
	    default:
		p.y = ctr.y;
		constrain = 0;
		clip = TRUE;
		rv = 1;
		break;
	    }
	    break;
	default:
	    rv = 1;
	    break;
	}
    }
    p = invflip_pt(p, GD_rankdir(n->graph));
    pp->side = invflip_side(side, GD_rankdir(n->graph));
    pp->bp = bp;
    pp->p = p;
    pp->theta = invflip_angle(theta, GD_rankdir(n->graph));
    if ((p.x == 0) && (p.y == 0))
	pp->order = MC_SCALE/2;
    else {
	/* compute angle with 0 at north pole, increasing CCW */
	double angle = atan2(p.y,p.x) + 1.5*PI;
	if (angle >= 2*PI) angle -= 2*PI;
	pp->order = (int)((MC_SCALE * angle) / 2*PI);
    }
    pp->constrained = constrain;
    pp->defined = defined;
    pp->clip = clip;
    return rv;
}

static port poly_port(node_t * n, char *portname, char *compass)
{
    port rv;
    box *bp;
    int  sides;    /* bitmap of which sides the port lies along */

    if (portname[0] == '\0')
	return Center;

    sides = BOTTOM | RIGHT | TOP | LEFT; 
    if ((ND_label(n)->html) && (bp = html_port(n, portname, &sides))) {
	if (compassPort(n, bp, &rv, compass, sides, NULL)) {
	    agerr(AGWARN,
		"node %s, port %s, unrecognized compass point '%s' - ignored\n",
		      n->name, portname, compass);
	}
    } 
    else {
	inside_t ictxt;

	ictxt.s.n = n;
	ictxt.s.bp = NULL;
	if (compassPort(n, NULL, &rv, portname, sides, &ictxt))
	    unrecognized(n, portname);
    }

    return rv;
}

/* generic polygon gencode routine */
static void poly_gencode(GVJ_t * job, node_t * n)
{
    polygon_t *poly;
    double xsize, ysize;
    int i, j, peripheries, sides, style;
    pointf P, *vertices;
    static point *A;
    static int A_size;
    boolean filled;
    extern int xdemitState;

    xdemitState = EMIT_DRAW;
    poly = (polygon_t *) ND_shape_info(n);
    vertices = poly->vertices;
    sides = poly->sides;
    peripheries = poly->peripheries;
    if (A_size < sides) {
	A_size = sides + 5;
	A = ALLOC(A_size, A, point);
    }

    ND_label(n)->p = ND_coord_i(n);
/* prescale by 16.0 to help rounding trick below */
    xsize = (16.0 * (ND_lw_i(n) + ND_rw_i(n)) / POINTS(ND_width(n)));
    ysize = (16.0 * ND_ht_i(n) / POINTS(ND_height(n)));

#if !defined(DISABLE_CODEGENS) && defined(HAVE_GD_PNG)
    /* this is bad, but it's because of how the VRML driver works */
    if ((job->codegen == &VRML_CodeGen) && (peripheries == 0)) {
	peripheries = 1;
    }
#endif

    if (ND_shape(n) == point_desc) {
	checkStyle(n, &style);
	if (style & INVISIBLE)
	    gvrender_set_style(job, point_style);
	else
	    gvrender_set_style(job, &point_style[1]);
	style = FILLED;
    } else {
	style = stylenode(job, n);
    }
    if (ND_active(n)) {
	activefillcolor(job,n);
	filled = TRUE;
	activepencolor(job,n);
    }
    else {
        if (style & FILLED) {
	    fillcolor(job, n);	/* emit fill color */
	    filled = TRUE;
        } else {
	    filled = FALSE;
        }
        pencolor(job, n);	/* emit pen color */
    }

    if (ND_shape(n)->usershape) {
	for (i = 0; i < sides; i++) {
	    P = vertices[i];
/* simple rounding produces random results around .5 
 * this trick should clip off the random part. 
 * (note xsize/ysize prescaled by 16.0 above) */
	    A[i].x = ROUND(P.x * xsize) / 16;
	    A[i].y = ROUND(P.y * ysize) / 16;
	    if (sides > 2) {
		A[i].x += ND_coord_i(n).x;
		A[i].y += ND_coord_i(n).y;
	    }
	}
	gvrender_user_shape(job, ND_shape(n)->name, A, sides, filled);
	filled = FALSE;
    }
    /* if no boundary but filled, set boundary color to fill color */
    if ((peripheries == 0) && filled) {
	char *color;
	peripheries = 1;
	color = findFill(n);
	if (color[0])
	    gvrender_set_pencolor(job, color);
    }
    for (j = 0; j < peripheries; j++) {
	for (i = 0; i < sides; i++) {
	    P = vertices[i + j * sides];
/* simple rounding produces random results around .5 
 * this trick should clip off the random part. 
 * (note xsize/ysize prescaled by 16.0 above) */
	    A[i].x = ROUND(P.x * xsize) / 16;
	    A[i].y = ROUND(P.y * ysize) / 16;
	    if (sides > 2) {
		A[i].x += ND_coord_i(n).x;
		A[i].y += ND_coord_i(n).y;
	    }
	}
	if (sides <= 2) {
	    gvrender_ellipse(job, ND_coord_i(n), A[0].x, A[0].y, filled);
	    if (style & DIAGONALS) {
		Mcircle_hack(job, n);
	    }
	} else if (style & (ROUNDED | DIAGONALS)) {
	    round_corners(job, n, A, sides, style);
	} else {
	    gvrender_polygon(job, A, sides, filled);
	}
	/* fill innermost periphery only */
	filled = FALSE;
    }

    xdemitState = EMIT_LABEL;
    emit_label(job, ND_label(n), (void *) n);
}

/*=======================end poly======================================*/

/*===============================point start========================*/

/* point_init:
 * shorthand for shape=circle, style=filled, width=0.05, label=""
 */
static void point_init(node_t * n)
{
    textlabel_t *p;

    if (!point_desc) {
	shape_desc *ptr;
	for (ptr = Shapes; ptr->name; ptr++)
	    if (!strcmp(ptr->name, "point")) {
		point_desc = ptr;
		break;
	    }
	assert(point_desc);
    }

    /* adjust label to "" */
    p = ND_label(n);
    free_label(p);
    ND_label(n) = NEW(textlabel_t);	/* allocate empty label that can be freed later */
    ND_label(n)->text = strdup("");

    /* set width and height, and make them equal
     * if user has set weight or height, use it.
     * if both are set, use smallest.
     * if neither, use default
     */
    if (ATTR_SET(N_width, n)) {
	if (ATTR_SET(N_height, n)) {
	    ND_width(n) = ND_height(n) = MIN(ND_width(n), ND_height(n));
	} else
	    ND_height(n) = ND_width(n);
    } else if (ATTR_SET(N_height, n)) {
	ND_width(n) = ND_height(n);
    } else
	ND_width(n) = ND_height(n) = DEF_POINT;

    poly_init(n);
}


/* the "record" shape is a rudimentary table formatter */

#define HASTEXT 1
#define HASPORT 2
#define HASTABLE 4
#define INTEXT 8
#define INPORT 16

#define ISCTRL(c) ((c) == '{' || (c) == '}' || (c) == '|' || (c) == '<' || (c) == '>')

static char *reclblp;

static field_t*
parse_reclbl(node_t * n, int LR, int flag, char *text)
{
    field_t *fp, *rv = NEW(field_t);
    char *tsp, *psp, *hstsp, *hspsp, *sp;
    char port[SMALLBUF];
    int maxf, cnt, mode, wflag, ishardspace, fi;

    fp = NULL;
    for (maxf = 1, cnt = 0, sp = reclblp; *sp; sp++) {
	if (*sp == '\\') {
	    sp++;
	    if (*sp && (*sp == '{' || *sp == '}' || *sp == '|' || *sp == '\\'))
		continue;
	}
	if (*sp == '{')
	    cnt++;
	else if (*sp == '}')
	    cnt--;
	else if (*sp == '|' && cnt == 0)
	    maxf++;
	if (cnt < 0)
	    break;
    }
    rv->fld = N_NEW(maxf, field_t *);
    rv->LR = LR;
    mode = 0;
    fi = 0;
    hstsp = tsp = text, hspsp = psp = &port[0];
    wflag = TRUE;
    ishardspace = FALSE;
    while (wflag) {
	switch (*reclblp) {
	case '<':
	    if (mode & (HASTABLE | HASPORT))
		return NULL;
	    mode |= (HASPORT | INPORT);
	    reclblp++;
	    break;
	case '>':
	    if (!(mode & INPORT))
		return NULL;
	    mode &= ~INPORT;
	    reclblp++;
	    break;
	case '{':
	    reclblp++;
	    if (mode != 0 || !*reclblp)
		return NULL;
	    mode = HASTABLE;
	    if (!(rv->fld[fi++] = parse_reclbl(n, NOT(LR), FALSE, text)))
		return NULL;
	    break;
	case '}':
	case '|':
	case '\000':
	    if ((!*reclblp && !flag) || (mode & INPORT))
		return NULL;
	    if (!(mode & HASTABLE))
		fp = rv->fld[fi++] = NEW(field_t);
	    if (mode & HASPORT) {
		if (psp > &port[0] + 1 &&
		    psp - 1 != hspsp && *(psp - 1) == ' ')
		    psp--;
		*psp = '\000';
		fp->id = strdup(&port[0]);
		hspsp = psp = &port[0];
	    }
	    if (!(mode & (HASTEXT | HASTABLE)))
		mode |= HASTEXT, *tsp++ = ' ';
	    if (mode & HASTEXT) {
		if (tsp > text + 1 &&
		    tsp - 1 != hstsp && *(tsp - 1) == ' ')
		    tsp--;
		*tsp = '\000';
		fp->lp =
		    make_label(0, strdup(text), ND_label(n)->fontsize,
			       ND_label(n)->fontname,
			       ND_label(n)->fontcolor, n->graph);
		fp->LR = TRUE;
		hstsp = tsp = text;
	    }
	    if (*reclblp) {
		if (*reclblp == '}') {
		    reclblp++;
		    rv->n_flds = fi;
		    return rv;
		}
		mode = 0;
		reclblp++;
	    } else
		wflag = FALSE;
	    break;
	case '\\':
	    if (*(reclblp + 1)) {
		if (ISCTRL(*(reclblp + 1)))
		    reclblp++;
                else if (*reclblp == '\\') { /* handles escaped \ */
                    *tsp++ = '\\';
                    mode |= (INTEXT | HASTEXT);
                    reclblp++;
                }
		else if (*(reclblp + 1) == ' ')
		    ishardspace = TRUE, reclblp++;
	    }
	    /* falling through ... */
	default:
	    if ((mode & HASTABLE) && *reclblp != ' ')
		return NULL;
	    if (!(mode & (INTEXT | INPORT)) && *reclblp != ' ')
		mode |= (INTEXT | HASTEXT);
	    if (mode & INTEXT) {
		if (!(*reclblp == ' ' && !ishardspace &&
		      *(tsp - 1) == ' '))
		    *tsp++ = *reclblp;
		if (ishardspace)
		    hstsp = tsp - 1;
	    } else if (mode & INPORT) {
		if (!(*reclblp == ' ' && !ishardspace &&
		      (psp == &port[0] || *(psp - 1) == ' ')))
		    *psp++ = *reclblp;
		if (ishardspace)
		    hspsp = psp - 1;
	    }
	    reclblp++;
	    while (*reclblp & 128)
		*tsp++ = *reclblp++;
	    break;
	}
    }
    rv->n_flds = fi;
    return rv;
}

static point size_reclbl(node_t * n, field_t * f)
{
    int i;
    char *p;
    double marginx, marginy;
    point d, d0;
    pointf dimen;

    if (f->lp) {
	dimen = f->lp->dimen;

	/* minimal whitespace around label */
	if ((dimen.x > 0.0) || (dimen.y > 0.0)) {
	    /* padding */
	    if ((p = agget(n, "margin"))) {
		i = sscanf(p, "%lf,%lf", &marginx, &marginy);
		if (i > 0) {
		    dimen.x += 2 * POINTS(marginx);
		    if (i > 1)
			dimen.y += 2 * POINTS(marginy);
		    else
			dimen.y += 2 * POINTS(marginy);
		} else
		    PAD(dimen);
	    } else
		PAD(dimen);
	}
	PF2P(dimen, d);
    } else {
	d.x = d.y = 0;
	for (i = 0; i < f->n_flds; i++) {
	    d0 = size_reclbl(n, f->fld[i]);
	    if (f->LR) {
		d.x += d0.x;
		d.y = MAX(d.y, d0.y);
	    } else {
		d.y += d0.y;
		d.x = MAX(d.x, d0.x);
	    }
	}
    }
    f->size = d;
    return d;
}

static void resize_reclbl(field_t * f, point sz, int nojustify_p)
{
    int i, amt;
    double inc;
    point d, newsz;
    field_t *sf;

    /* adjust field */
    d.x = sz.x - f->size.x;
    d.y = sz.y - f->size.y;
    f->size = sz;

    /* adjust text */
    if (f->lp && !nojustify_p) {
	P2PF(d, f->lp->d);
    }

    /* adjust children */
    if (f->n_flds) {

	if (f->LR)
	    inc = (double) d.x / f->n_flds;
	else
	    inc = (double) d.y / f->n_flds;
	for (i = 0; i < f->n_flds; i++) {
	    sf = f->fld[i];
	    amt = ((int) ((i + 1) * inc)) - ((int) (i * inc));
	    if (f->LR)
		newsz = pointof(sf->size.x + amt, sz.y);
	    else
		newsz = pointof(sz.x, sf->size.y + amt);
	    resize_reclbl(sf, newsz, nojustify_p);
	}
    }
}

/* pos_reclbl:
 * Assign position info for each field. Also, set
 * the sides attribute, which indicates which sides of the
 * record are accessible to the field.
 */
static void pos_reclbl(field_t * f, point ul, int sides)
{
    int i, last, mask;

    f->sides = sides;
    f->b.LL = pointof(ul.x, ul.y - f->size.y);
    f->b.UR = pointof(ul.x + f->size.x, ul.y);
    last = f->n_flds - 1;
    for (i = 0; i <= last; i++) {
	if (sides) {
	    if (f->LR) {
		if (i == 0) {
		    if (i == last) mask = TOP | BOTTOM | RIGHT | LEFT;
		    else mask = TOP | BOTTOM | LEFT;
		}
		else if (i == last) mask = TOP | BOTTOM | RIGHT;
		else mask = TOP | BOTTOM;
	    }
	    else {
		if (i == 0) {
		    if (i == last) mask = TOP | BOTTOM | RIGHT | LEFT;
		    else mask = TOP | RIGHT | LEFT;
		}
		else if (i == last) mask = LEFT | BOTTOM | RIGHT;
		else mask = LEFT | RIGHT;
	    }
	}
	else mask = 0;
	pos_reclbl(f->fld[i], ul, sides & mask);
	if (f->LR)
	    ul.x = ul.x + f->fld[i]->size.x;
	else
	    ul.y = ul.y - f->fld[i]->size.y;
    }
}

#ifdef DEBUG
static void indent(int l)
{
    int i;
    for (i = 0; i < l; i++)
	fputs("  ", stderr);
}

static void prbox(box b)
{
    fprintf(stderr, "((%d,%d),(%d,%d))\n", b.LL.x, b.LL.y, b.UR.x, b.UR.y);
}

static void dumpL(field_t * info, int level)
{
    int i;

    indent(level);
    if (info->n_flds == 0) {
	fprintf(stderr, "Label \"%s\" ", info->lp->text);
	prbox(info->b);
    } else {
	fprintf(stderr, "Tbl ");
	prbox(info->b);
	for (i = 0; i < info->n_flds; i++) {
	    dumpL(info->fld[i], level + 1);
	}
    }
}
#endif

/* syntax of labels: foo|bar|baz or foo|(recursive|label)|baz */
static void record_init(node_t * n)
{
    field_t *info;
    point ul, sz;
    int len;
    char *textbuf;		/* temp buffer for storing labels */
    int sides = BOTTOM | RIGHT | TOP | LEFT; 

    reclblp = ND_label(n)->text;
    len = strlen(reclblp);
    textbuf = N_NEW(len + 1, char);
    if (!(info = parse_reclbl(n,NOT(GD_flip(n->graph)), TRUE, textbuf))) {
	agerr(AGERR, "bad label format %s\n", ND_label(n)->text);
	reclblp = "\\N";
	info = parse_reclbl(n, NOT(GD_flip(n->graph)), TRUE, textbuf);
    }
    free(textbuf);

    size_reclbl(n, info);
    sz.x = POINTS(ND_width(n));
    sz.y = POINTS(ND_height(n));
    if (mapbool(late_string(n, N_fixed, "false"))) {
	if ((sz.x < info->size.x) || (sz.y < info->size.y)) {
/* should check that the record really won't fit, e.g., there may be no text.
			agerr(AGWARN, "node '%s' size may be too small\n",
				n->name);
 */
	}
    } else {
	sz.x = MAX(info->size.x, sz.x);
	sz.y = MAX(info->size.y, sz.y);
    }
    resize_reclbl(info, sz, mapbool(late_string(n, N_nojustify, "false")));
    ul = pointof(-sz.x / 2, sz.y / 2);
    pos_reclbl(info, ul, sides);
    ND_width(n) = PS2INCH(info->size.x);
    ND_height(n) = PS2INCH(info->size.y);
    ND_shape_info(n) = (void *) info;
}

static void record_free(node_t * n)
{
    field_t *p = ND_shape_info(n);

    free(p);
}

static field_t *map_rec_port(field_t * f, char *str)
{
    field_t *rv;
    int sub;

    if (f->id && (strcmp(f->id, str) == 0))
	rv = f;
    else {
	rv = NULL;
	for (sub = 0; sub < f->n_flds; sub++)
	    if ((rv = map_rec_port(f->fld[sub], str)))
		break;
    }
    return rv;
}

static port record_port(node_t * n, char *portname, char *compass)
{
    field_t *f;
    field_t *subf;
    port rv;
    int  sides;    /* bitmap of which sides the port lies along */

    if (portname[0] == '\0')
	return Center;
    sides = BOTTOM | RIGHT | TOP | LEFT; 
    f = (field_t *) ND_shape_info(n);
    if ((subf = map_rec_port(f, portname))) {
	if (compassPort(n, &subf->b, &rv, compass, subf->sides, NULL)) {
	    agerr(AGWARN,
	      "node %s, port %s, unrecognized compass point '%s' - ignored\n",
	      n->name, portname, compass);
	}
    } else if (compassPort(n, &f->b, &rv, portname, sides, NULL)) {
	unrecognized(n, portname);
    }

    return rv;
}

/* record_inside:
 * Note that this does not handle Mrecords correctly. It assumes 
 * everything is a rectangle.
 */
static boolean
record_inside(inside_t * inside_context, pointf p)
{

    field_t *fld0;
    box *bp = inside_context->s.bp;
    node_t *n = inside_context->s.n;
    box bbox;

    /* convert point to node coordinate system */
    p = flip_ptf(p, GD_rankdir(n->graph));

    if (bp == NULL) {
	fld0 = (field_t *) ND_shape_info(n);
	bbox = fld0->b;
    }
    else bbox = *bp;

    return INSIDE(p, bbox);
}

/* record_path:
 * Generate box path from port to border.
 * See poly_path for constraints.
 */
static int record_path(node_t* n, port* prt, int side, box rv[], int *kptr)
{
    int i, ls, rs;
    point p;
    field_t *info;

    if (!prt->defined) return 0;
    p = prt->p;
    info = (field_t *) ND_shape_info(n);

    for (i = 0; i < info->n_flds; i++) {
	if (!GD_flip(n->graph)) {
	    ls = info->fld[i]->b.LL.x;
	    rs = info->fld[i]->b.UR.x;
	} else {
	    ls = info->fld[i]->b.LL.y;
	    rs = info->fld[i]->b.UR.y;
	}
	if (BETWEEN(ls, p.x, rs)) {
	    /* FIXME: I don't understand this code */
	    if (GD_flip(n->graph)) {
		rv[0] = flip_rec_box(info->fld[i]->b, ND_coord_i(n));
	    } else {
		rv[0].LL.x = ND_coord_i(n).x + ls;
		rv[0].LL.y = ND_coord_i(n).y - ND_ht_i(n) / 2;
		rv[0].UR.x = ND_coord_i(n).x + rs;
	    }
	    rv[0].UR.y = ND_coord_i(n).y + ND_ht_i(n) / 2;
	    *kptr = 1;
	    break;
	}
    }
    return side;
}

static void gen_fields(GVJ_t * job, node_t * n, field_t * f)
{
    int i;
    double cx, cy;
    point A[2];

    if (f->lp) {
	cx = (f->b.LL.x + f->b.UR.x) / 2.0 + ND_coord_i(n).x;
	cy = (f->b.LL.y + f->b.UR.y) / 2.0 + ND_coord_i(n).y;
	f->lp->p = pointof((int) cx, (int) cy);
	emit_label(job, f->lp, (void *) n);
    }

    for (i = 0; i < f->n_flds; i++) {
	if (i > 0) {
	    if (f->LR) {
		A[0] = f->fld[i]->b.LL;
		A[1].x = A[0].x;
		A[1].y = f->fld[i]->b.UR.y;
	    } else {
		A[1] = f->fld[i]->b.UR;
		A[0].x = f->fld[i]->b.LL.x;
		A[0].y = A[1].y;
	    }
	    A[0] = add_points(A[0], ND_coord_i(n));
	    A[1] = add_points(A[1], ND_coord_i(n));
	    gvrender_polyline(job, A, 2);
	}
	gen_fields(job, n, f->fld[i]);
    }
}

static void record_gencode(GVJ_t * job, node_t * n)
{
    point A[4];
    int i, style;
    field_t *f;
    extern int xdemitState;

    xdemitState = EMIT_DRAW;
    f = (field_t *) ND_shape_info(n);
    A[0] = f->b.LL;
    A[2] = f->b.UR;
    A[1].x = A[2].x;
    A[1].y = A[0].y;
    A[3].x = A[0].x;
    A[3].y = A[2].y;
    for (i = 0; i < 4; i++)
	A[i] = add_points(A[i], ND_coord_i(n));
    style = stylenode(job, n);
    pencolor(job, n);
    if (style & FILLED)
	fillcolor(job, n);	/* emit fill color */
    if (streq(ND_shape(n)->name, "Mrecord"))
	style |= ROUNDED;
    if (style & (ROUNDED | DIAGONALS))
	round_corners(job, n, A, 4, ROUNDED);
    else
	gvrender_polygon(job, A, 4, style & FILLED);
    xdemitState = EMIT_LABEL;
    gen_fields(job, n, f);
}

static shape_desc **UserShape;
static int N_UserShape;

shape_desc *find_user_shape(char *name)
{
    int i;
    if (UserShape) {
	for (i = 0; i < N_UserShape; i++) {
	    if (streq(UserShape[i]->name, name))
		return UserShape[i];
	}
    }
    return NULL;
}

static shape_desc *user_shape(char *name)
{
    int i;
    shape_desc *p;

    if ((p = find_user_shape(name)))
	return p;
    i = N_UserShape++;
    UserShape = ALLOC(N_UserShape, UserShape, shape_desc *);
    p = UserShape[i] = NEW(shape_desc);
    *p = Shapes[0];
    p->name = name;
    p->usershape = TRUE;
    if (Lib == NULL && strcmp(name, "custom"))
	agerr(AGWARN, "using %s for unknown shape %s\n", Shapes[0].name,
	      p->name);
    return p;
}

shape_desc *bind_shape(char *name, node_t * np)
{
    shape_desc *ptr, *rv = NULL;
    char *str;

    str = safefile(agget(np, "shapefile"));
    /* If shapefile is defined and not epsf, set shape = custom */
    if (str && strcmp(name, "epsf"))
	name = "custom";
    if (strcmp(name, "custom")) {
	for (ptr = Shapes; ptr->name; ptr++) {
	    if (!strcmp(ptr->name, name)) {
		rv = ptr;
		break;
	    }
	}
    }
    if (rv == NULL)
	rv = user_shape(name);
    return rv;
}

static boolean epsf_inside(inside_t * inside_context, pointf p)
{
    pointf P;
    double x2;
    node_t *n = inside_context->s.n;

    P = flip_ptf(p, GD_rankdir(n->graph));
    x2 = ND_ht_i(n) / 2;
    return ((P.y >= -x2) && (P.y <= x2) && (P.x >= -ND_lw_i(n))
	    && (P.x <= ND_rw_i(n)));
}

static void epsf_gencode(GVJ_t * job, node_t * n)
{
    epsf_t *desc;

    desc = (epsf_t *) (ND_shape_info(n));
    if (!desc)
	return;
    gvrender_begin_context(job);
    if (desc)
	fprintf(job->output_file,
		"%d %d translate newpath user_shape_%d\n",
		ND_coord_i(n).x + desc->offset.x,
		ND_coord_i(n).y + desc->offset.y, desc->macro_id);
    ND_label(n)->p = ND_coord_i(n);
    gvrender_end_context(job);
    emit_label(job, ND_label(n), (void *) n);
}
