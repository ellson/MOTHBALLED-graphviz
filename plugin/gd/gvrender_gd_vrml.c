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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>

#include "gvplugin_render.h"

#ifdef HAVE_GD_PNG
#include "gd.h"
#include "graph.h"

/* for late_double() */
#include "agxbuf.h"
#include "utils.h"

/* for wind() */
#include "pathutil.h"

extern shape_kind shapeOf(node_t *);

typedef enum { FORMAT_VRML, } format_type;

#ifndef MAXFLOAT
#define MAXFLOAT 10000000.
#endif

#define		NONE		0
#define		NODE		1
#define		EDGE		2
#define		CLST		3

#define BEZIERSUBDIVISION 10

/* font modifiers */
#define REGULAR 0
#define BOLD		1
#define ITALIC		2

/* patterns */
#define P_SOLID		0
#define P_NONE  15
#define P_DOTTED 4		/* i wasn't sure about this */
#define P_DASHED 11		/* or this */

/* bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 3

typedef struct {
    unsigned char r, g, b;
} Color;


/* static int	N_pages; */
/* static point	Pages; */
static double Scale;
static double MinZ;
/* static int	onetime = TRUE; */
static int Saw_skycolor;

static gdImagePtr im;
static FILE *PNGfile;
static int    IsSegment;   /* set true if edge is line segment */
static double CylHt;       /* height of cylinder part of edge */
static double EdgeLen;     /* length between centers of endpoints */
static double HeadHt, TailHt;  /* height of arrows */
static double Fstz, Sndz;  /* z values of tail and head points */

typedef struct context_t {
    unsigned char pencolor_ix, fillcolor_ix;
    char *pencolor, *fillcolor;
    char *fontfam, fontopt, font_was_set;
    double r, g, b;     /* fill color values */
    char pen, fill, penwidth;
    double fontsz;
} context_t;

#define MAXNEST 4
static context_t cstk[MAXNEST];
static int SP;

/* gdirname:
 * Returns directory pathname prefix
 * Code adapted from dgk
 */
static char *gdirname(char *pathname)
{
    char *last;

    /* go to end of path */
    for (last = pathname; *last; last++);
    /* back over trailing '/' */
    while (last > pathname && *--last == '/');
    /* back over non-slash chars */
    for (; last > pathname && *last != '/'; last--);
    if (last == pathname) {
	/* all '/' or "" */
	if (*pathname != '/')
	    *last = '.';
	/* preserve // */
	else if (pathname[1] == '/')
	    last++;
    } else {
	/* back over trailing '/' */
	for (; *last == '/' && last > pathname; last--);
	/* preserve // */
	if (last == pathname && *pathname == '/' && pathname[1] == '/')
	    last++;
    }
    last++;
    *last = '\0';

    return pathname;
}

static char *nodefilename(char *filename, node_t * n, char *buf)
{
    static char *dir;
    static char disposable[1024];

    if (dir == 0) {
	if (filename)
	    dir = gdirname(strcpy(disposable, filename));
	else
	    dir = ".";
    }
    sprintf(buf, "%s/node%d.png", dir, n->id);
    return buf;
}

static FILE *nodefile(char *filename, node_t * n)
{
    FILE *rv;
    char buf[1024];

    rv = fopen(nodefilename(filename, n, buf), "wb");
    return rv;
}

static void init_png(gdImagePtr im)
{
    int transparent;

    if ((transparent = gdImageGetTransparent(im)) == -1) {
	transparent = gdImageColorResolve(im, 255, 255, 254);
	gdImageColorTransparent(im, transparent);
    }
}

static pointf vrml_node_point(GVJ_t *job, node_t *n, point p)
{
    pointf rv;

    /* make mp relative to PNG canvas */
    if (job->rotation) {
	rv.x = (p.y - ND_coord_i(n).y + ND_lw_i(n)) * Scale;
	rv.y = (ND_coord_i(n).x - p.x + ND_ht_i(n) / 2) * Scale;
    } else {
	rv.x = (p.x - ND_coord_i(n).x + ND_lw_i(n)) * Scale;
	rv.y = (ND_coord_i(n).y - p.y + ND_ht_i(n) / 2) * Scale;
    }
    return rv;
}

static void vrml_font(context_t * cp)
{
/* FIX
	char		   *fw, *fa;

	fw = fa = "Regular";
	switch (cp->fontopt) {
	case BOLD:
		fw = "Bold";
		break;
	case ITALIC:
		fa = "Italic";
		break;
	}
*/
}

/* warmed over VRML code starts here */

static void vrml_resolve_color(GVJ_t * job, gvcolor_t * color)
{
    gdImagePtr im = (gdImagePtr) job->surface;

    if (!im)
        return;

    /* seems gd alpha is "transparency" rather than the usual "opacity" */
    color->u.index = gdImageColorResolveAlpha(im,
                          color->u.rgba[0],
                          color->u.rgba[1],
                          color->u.rgba[2],
                          (255 - color->u.rgba[3]) * gdAlphaMax / 255);
    color->type = COLOR_INDEX;
}

static int white, black, transparent, basecolor;

static void vrml_begin_page(GVJ_t *job)
{
    FILE *out = job->output_file;

    Scale = (double) DEFAULT_DPI / POINTS_PER_INCH;
    fprintf(out, "#VRML V2.0 utf8\n");

    Saw_skycolor = FALSE;
    MinZ = MAXDOUBLE;
    fprintf(out, "Group { children [\n");
    fprintf(out, "  Transform {\n");
    fprintf(out, "    scale %.3f %.3f %.3f\n", .0278, .0278, .0278);
    fprintf(out, "    children [\n");

    SP = 0;
    cstk[0].fillcolor = "white";
    cstk[0].fontfam = "times";	/* font family name */
    cstk[0].fontopt = REGULAR;	/* modifier: REGULAR, BOLD or ITALIC */
    cstk[0].pen = P_SOLID;	/* pen pattern style, default is solid */
    cstk[0].fill = P_NONE;
    cstk[0].penwidth = WIDTH_NORMAL;
}

static void vrml_end_page(GVJ_t *job)
{
    FILE *out = job->output_file;
    double d, z;
    box bb = job->boundingBox;

    d = MAX(bb.UR.x - bb.LL.x,bb.UR.y - bb.LL.y);
    /* Roughly fill 3/4 view assuming FOV angle of PI/4.
     * Small graphs and non-square aspect ratios will upset this.
     */
    z = (0.6667*d)/tan(PI/8.0) + MinZ;  /* fill 3/4 of view */

    if (!Saw_skycolor)
	fprintf(out, " Background { skyColor 1 1 1 }\n");
    fprintf(out, "  ] }\n");
    fprintf(out, "  Viewpoint {position %.3f %.3f %.3f}\n",
	    .0278 * (bb.UR.x + bb.LL.x) / 2.0,
	    .0278 * (bb.UR.y + bb.LL.y) / 2.0, .0278 * z);
    fprintf(out, "] }\n");
}

static void vrml_begin_node(GVJ_t *job)
{
    FILE *out = job->output_file;
    obj_state_t *obj = job->obj;
    node_t *n = obj->n;
    double z = obj->z;
    int width, height;

    fprintf(out, "# node %s\n", n->name);
    if (z < MinZ) MinZ = z;
    if (shapeOf(n) != SH_POINT) {
	PNGfile = nodefile(job->output_filename, n);
	width = (ND_lw_i(n) + ND_rw_i(n)) * Scale + 3;
	height = (ND_ht_i(n)) * Scale + 3;
	im = gdImageCreate(width, height);
	init_png(im);
    }
}

static void vrml_end_node(GVJ_t *job)
{
    obj_state_t *obj = job->obj;
    node_t *n = obj->n;

    if (shapeOf(n) != SH_POINT) {
	gdImagePng(im, PNGfile);
	gdImageDestroy(im);
	im = 0;
	fclose(PNGfile);
    }
}

static void vrml_begin_edge(GVJ_t *job)
{
    FILE *out = job->output_file;
    edge_t *e = job->obj->e;

    IsSegment = 0;
    fprintf(out, "# edge %s -> %s\n", e->tail->name, e->head->name);
    fprintf(out, " Group { children [\n");
}

static void
finishSegment (FILE *out, edge_t *e)
{
    point p0 = ND_coord_i(e->tail);
    point p1 = ND_coord_i(e->head);
    double o_x, o_y, o_z;
    double x, y, y0, z, theta;

    o_x = ((double)(p0.x + p1.x))/2;
    o_y = ((double)(p0.y + p1.y))/2;
    o_z = (Fstz + Sndz)/2;
    /* Compute rotation */
    /* Pick end point with highest y */
    if (p0.y > p1.y) {
	x = p0.x;
	y = p0.y;
        z = Fstz;
    }
    else {
	x = p1.x;
	y = p1.y;
        z = Sndz;
    }
    /* Translate center to the origin */
    x -= o_x;
    y -= o_y;
    z -= o_z;
    if (p0.y > p1.y)
	theta = acos(2*y/EdgeLen) + PI;
    else
	theta = acos(2*y/EdgeLen);
    if (!x && !z)   /* parallel  to y-axis */
	x = 1;

    y0 = (HeadHt-TailHt)/2.0;
    fprintf(out, "      ]\n");
    fprintf(out, "      center 0 %f 0\n", y0);
    fprintf(out, "      rotation %f 0 %f   %f\n", -z, x, -theta);
    fprintf(out, "      translation %.3f %.3f %.3f\n", o_x, o_y - y0, o_z);
    fprintf(out, "    }\n");
}

static void vrml_end_edge(GVJ_t *job)
{
    if (IsSegment)
	finishSegment(job->output_file, job->obj->e);
    fprintf(job->output_file, "] }\n");
}

static void vrml_begin_context(void)
{
    assert(SP + 1 < MAXNEST);
    cstk[SP + 1] = cstk[SP];
    SP++;
}

static void vrml_end_context(void)
{
    int psp = SP - 1;
    assert(SP > 0);
    if (cstk[SP].font_was_set)
	vrml_font(&(cstk[psp]));
    /* free(cstk[psp].fontfam); */
    SP = psp;
}

static void vrml_set_font(char *name, double size)
{
    char *p, *q;
    context_t *cp;

    cp = &(cstk[SP]);
    cp->font_was_set = TRUE;
    cp->fontsz = size;
    p = strdup(name);
    if ((q = strchr(p, '-'))) {
	*q++ = 0;
	if (strcasecmp(q, "italic") == 0)
	    cp->fontopt = ITALIC;
	else if (strcasecmp(q, "bold") == 0)
	    cp->fontopt = BOLD;
    }
    cp->fontfam = p;
    vrml_font(&cstk[SP]);
}

static void vrml_textpara(GVJ_t *job, pointf pf, textpara_t * para)
{
    obj_state_t *obj = job->obj;
    char *err;
    point p;
    pointf mp;
    int brect[8];
    extern gdFontPtr gdFontSmall;

    if (! obj->n)
	return;

    PF2P(pf, p);
    switch (para->just) {
    case 'l':
	break;
    case 'r':
	p.x -= para->width;
	break;
    default:
    case 'n':
	p.x -= para->width / 2;
	break;
    }
/*	p.y += cstk[SP].fontsz*2/3; */

    mp = vrml_node_point(job, obj->n, p);

    err = gdImageStringFT(im, brect, job->style->pencolor.u.index, para->fontname,
	    		  para->fontsize, job->rotation ? PI/2 : 0,
			  ROUND(mp.x), ROUND(mp.y), (char*)para->str);
    if (err) {
	/* revert to builtin fonts */
	gdImageString(im, gdFontSmall, ROUND(mp.x), ROUND(mp.y),
		      (unsigned char *) para->str, job->style->pencolor.u.index);
    }
}

/* interpolate_zcoord:
 * Given 2 points in 3D p = (fst.x,fst.y,fstz) and q = (snd.x, snd.y, sndz),
 * and a point p1 in the xy plane lying on the line segment connecting 
 * the projections of the p and q, find the z coordinate of p1 when it
 * is projected up onto the segment (p,q) in 3-space. 
 *
 * Why the special case for ranks? Is the arithmetic really correct?
 */
static double 
interpolate_zcoord(GVJ_t *job, pointf p1, point fst, double fstz, point snd, double sndz)
{
    obj_state_t *obj = job->obj;
    edge_t *e = obj->e;
    double len, d, rv;

    if (fstz == sndz)
	return fstz;
    if (ND_rank(e->tail) != ND_rank(e->head)) {
	if (snd.y == fst.y)
	    rv = (fstz + sndz) / 2.0;
	else
	    rv = fstz + (sndz - fstz) * (p1.y - fst.y) / (snd.y - fst.y);
    } 
    else {
	len = DIST(fst, snd);
	d = DIST(p1, fst)/len;
	rv = fstz + d*(sndz - fstz);
    }
    return rv;
}

/* collinear:
 * Return true if the 3 points starting at A are collinear.
 */
static int
collinear (point * A)
{
    Ppoint_t a, b, c;
    double w;

    a.x = A->x;
    a.y = A->y;
    A++;
    b.x = A->x;
    b.y = A->y;
    A++;
    c.x = A->x;
    c.y = A->y;

    w = wind(a,b,c);
    return (fabs(w) <= 1);
}

/* straight:
 * Return true if bezier points are collinear
 * At present, just check with 4 points, the common case.
 */
static int
straight (point * A, int n)
{
    if (n != 4) return 0;
    return (collinear(A) && collinear(A+1));
}

static void
doSegment (FILE *out, point* A, point p0, double z0, point p1, double z1)
{
    double d1, d0;
    double delx, dely, delz;

    delx = p0.x - p1.x;
    dely = p0.y - p1.y;
    delz = z0 - z1;
    EdgeLen = sqrt(delx*delx + dely*dely + delz*delz);
    d0 = DIST(A[0],p0);
    d1 = DIST(A[3],p1);
    CylHt = EdgeLen - d0 - d1;
    TailHt = HeadHt = 0;

    IsSegment = 1;
    fprintf(out, "Transform {\n");
    fprintf(out, "  children [\n");
    fprintf(out, "    Shape {\n");
    fprintf(out, "      geometry Cylinder {\n"); 
    fprintf(out, "        bottom FALSE top FALSE\n"); 
    fprintf(out, "        height %f radius %d }\n", CylHt, cstk[SP].penwidth);
    fprintf(out, "      appearance Appearance {\n");
    fprintf(out, "        material Material {\n");
    fprintf(out, "          ambientIntensity 0.33\n");
    fprintf(out, "          diffuseColor %f %f %f\n", 
	cstk[SP].r,cstk[SP].g,cstk[SP].b);
    fprintf(out, "        }\n");
    fprintf(out, "      }\n");
    fprintf(out, "    }\n");
}

static void
vrml_bezier(GVJ_t *job, point * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    FILE *out = job->output_file;
    obj_state_t *obj = job->obj;
    edge_t *e = obj->e;
    double fstz = obj->tail_z, sndz = obj->head_z;
    pointf p1, V[4];
    int i, j, step;
    context_t *cp;

    assert(obj->e);

    cp = &(cstk[SP]);
    if (cp->pen == P_NONE)
	return;
    if (straight(A,n)) {
	doSegment (out, A, ND_coord_i(e->tail),Fstz,ND_coord_i(e->head),Sndz);
	return;
    }

    fprintf(out, "Shape { geometry Extrusion  {\n");
    fprintf(out, "  spine [");
    V[3].x = A[0].x;
    V[3].y = A[0].y;
    for (i = 0; i + 3 < n; i += 3) {
	V[0] = V[3];
	for (j = 1; j <= 3; j++) {
	    V[j].x = A[i + j].x;
	    V[j].y = A[i + j].y;
	}
	for (step = 0; step <= BEZIERSUBDIVISION; step++) {
	    p1 = Bezier(V, 3, (double) step / BEZIERSUBDIVISION, NULL,
			NULL);
	    fprintf(out, " %.3f %.3f %.3f", p1.x, p1.y,
		    interpolate_zcoord(job, p1, A[0], fstz, A[n - 1], sndz));
	}
    }
    fprintf(out, " ]\n");
    fprintf(out, "  crossSection [ %d %d, %d %d, %d %d, %d %d ]\n",
	    (cp->penwidth), (cp->penwidth), -(cp->penwidth),
	    (cp->penwidth), -(cp->penwidth), -(cp->penwidth),
	    (cp->penwidth), -(cp->penwidth));
    fprintf(out, "}\n");
    fprintf(out, " appearance DEF E%d Appearance {\n", e->id);
    fprintf(out, "   material Material {\n");
    fprintf(out, "   ambientIntensity 0.33\n");
    fprintf(out, "   diffuseColor %.3f %.3f %.3f\n",
	    cstk[SP].r, cstk[SP].g, cstk[SP].b);
    fprintf(out, "   }\n");
    fprintf(out, " }\n");
    fprintf(out, "}\n");
}

/* doArrowhead:
 * If edge is straight, we attach a cone to the edge as a group.
 */
static void
doArrowhead (GVJ_t *job, point* A)
{
    FILE *out = job->output_file;
    obj_state_t *obj = job->obj;
    edge_t *e = obj->e;
    double rad, ht, y;
    pointf p0;      /* center of triangle base */
    point  tp,hp;

    p0.x = (A[0].x + A[2].x)/2.0;
    p0.y = (A[0].y + A[2].y)/2.0;
    rad = DIST(A[0],A[2])/2.0;
    ht = DIST(p0,A[1]);

    y = (CylHt + ht)/2.0;

    tp = ND_coord_i(e->tail);
    hp = ND_coord_i(e->head);
    fprintf(out, "Transform {\n");
    if (DIST2(A[1], tp) < DIST2(A[1], hp)) {
	TailHt = ht;
	fprintf(out, "  translation 0 -%.3f 0\n", y);
	fprintf(out, "  rotation 0 0 1 %.3f\n", PI);
    }
    else {
	HeadHt = ht;
	fprintf(out, "  translation 0 %.3f 0\n", y);
    }
    fprintf(out, "  children [\n");
    fprintf(out, "    Shape {\n");
    fprintf(out, "      geometry Cone {bottomRadius %.3f height %.3f }\n",
	rad, ht);
    fprintf(out, "      appearance Appearance {\n");
    fprintf(out, "        material Material {\n");
    fprintf(out, "          ambientIntensity 0.33\n");
    fprintf(out, "          diffuseColor %f %f %f\n", cstk[SP].r,cstk[SP].g,cstk[SP].b);
    fprintf(out, "        }\n");
    fprintf(out, "      }\n");
    fprintf(out, "    }\n");
    fprintf(out, "  ]\n");
    fprintf(out, "}\n");
}

static void vrml_polygon(GVJ_t *job, point * A, int np, int filled)
{
    FILE *out = job->output_file;
    obj_state_t *obj = job->obj;
    graph_t *g = obj->g;
    node_t *n = obj->n;
    edge_t *e = obj->e;
    double z = obj->z;
    pointf p, mp;
    int i;
    gdPoint *points;
    int style[20];
    int pen, width;
    gdImagePtr brush = NULL;
    double theta;

    if (g) {
	fprintf(out, " Background { skyColor %.3f %.3f %.3f }\n",
		cstk[SP].r, cstk[SP].g, cstk[SP].b);
	Saw_skycolor = TRUE;
    }
    else if (n) {

	if (cstk[SP].pen != P_NONE) {
	    cstk[SP].pencolor_ix = vrml_resolve_color(cstk[SP].pencolor);
	    cstk[SP].fillcolor_ix = vrml_resolve_color(cstk[SP].fillcolor);
	    if (cstk[SP].pen == P_DASHED) {
		for (i = 0; i < 10; i++)
		    style[i] = cstk[SP].pencolor_ix;
		for (; i < 20; i++)
		    style[i] = gdTransparent;
		gdImageSetStyle(im, style, 20);
		pen = gdStyled;
	    } else if (cstk[SP].pen == P_DOTTED) {
		for (i = 0; i < 2; i++)
		    style[i] = cstk[SP].pencolor_ix;
		for (; i < 12; i++)
		    style[i] = gdTransparent;
		gdImageSetStyle(im, style, 12);
		pen = gdStyled;
	    } else {
		pen = cstk[SP].pencolor_ix;
	    }
	    if (cstk[SP].penwidth != WIDTH_NORMAL) {
		width = cstk[SP].penwidth;
		brush = gdImageCreate(width, width);
		gdImagePaletteCopy(brush, im);
		gdImageFilledRectangle(brush,
				       0, 0, width - 1, width - 1,
				       cstk[SP].pencolor_ix);
		gdImageSetBrush(im, brush);
		if (pen == gdStyled)
		    pen = gdStyledBrushed;
		else
		    pen = gdBrushed;
	    }
	    points = N_GNEW(np, gdPoint);
	    for (i = 0; i < np; i++) {
		mp = vrml_node_point(job, n, A[i]);
		points[i].x = ROUND(mp.x);
		points[i].y = ROUND(mp.y);
	    }
	    if (filled)
		gdImageFilledPolygon(im, points, np, cstk[SP].fillcolor_ix);
	    gdImagePolygon(im, points, np, pen);
	    free(points);
	    if (brush)
		gdImageDestroy(brush);
	}

	fprintf(out, "Shape {\n");
	fprintf(out, "  appearance Appearance {\n");
	fprintf(out, "    material Material {\n");
	fprintf(out, "      ambientIntensity 0.33\n");
	fprintf(out, "        diffuseColor 1 1 1\n");
	fprintf(out, "    }\n");
	fprintf(out, "    texture ImageTexture { url \"node%d.png\" }\n",
		n->id);
	fprintf(out, "  }\n");
	fprintf(out, "  geometry Extrusion {\n");
	fprintf(out, "    crossSection [");
	for (i = 0; i < n; i++) {
	    p.x = A[i].x - ND_coord_i(n).x;
	    p.y = A[i].y - ND_coord_i(n).y;
	    fprintf(out, " %.3f %.3f,", p.x, p.y);
	}
	p.x = A[0].x - ND_coord_i(n).x;
	p.y = A[0].y - ND_coord_i(n).y;
	fprintf(out, " %.3f %.3f ]\n", p.x, p.y);
	fprintf(out, "    spine [ %d %d %.3f, %d %d %.3f ]\n",
		ND_coord_i(n).x, ND_coord_i(n).y, z - .01,
		ND_coord_i(n).x, ND_coord_i(n).y, z + .01);
	fprintf(out, "  }\n");
	fprintf(out, "}\n");

    }
    else if (e) {
	if (cstk[SP].pen == P_NONE)
	    return;
	if (np != 3) {
	    static int flag;
	    if (!flag) {
		flag++;
		agerr(AGWARN,
		  "vrml_polygon: non-triangle arrowheads not supported - ignoring\n");
	    }
	}
	if (IsSegment) {
	    doArrowhead (job, A);
	    return;
	}
	p.x = p.y = 0.0;
	for (i = 0; i < np; i++) {
	    p.x += A[i].x;
	    p.y += A[i].y;
	}
	p.x = p.x / np;
	p.y = p.y / np;

	/* it is bad to know that A[1] is the aiming point, but we do */
	theta =
	    atan2((A[0].y + A[2].y) / 2.0 - A[1].y,
		  (A[0].x + A[2].x) / 2.0 - A[1].x) + PI / 2.0;


	/* this is gruesome, but how else can we get z coord */
	if (DIST2(p, ND_coord_i(e->tail)) < DIST2(p, ND_coord_i(e->head)))
	    z = obj->tail_z;
	else
	    z = obj->head_z;

	/* FIXME: arrow vector ought to follow z coord of bezier */
	fprintf(out, "Transform {\n");
	fprintf(out, "  translation %.3f %.3f %.3f\n", p.x, p.y, z);
	fprintf(out, "  children [\n");
	fprintf(out, "    Transform {\n");
	fprintf(out, "      rotation 0 0 1 %.3f\n", theta);
	fprintf(out, "      children [\n");
	fprintf(out, "        Shape {\n");
	fprintf(out,
		"          geometry Cone {bottomRadius %.3f height %.3f }\n",
		cstk[SP].penwidth * 2.5, cstk[SP].penwidth * 10.0);
	fprintf(out, "          appearance USE E%d\n", e->id);
	fprintf(out, "        }\n");
	fprintf(out, "      ]\n");
	fprintf(out, "    }\n");
	fprintf(out, "  ]\n");
	fprintf(out, "}\n");
    }
}

/* doSphere:
 * Output sphere in VRML for point nodes.
 */
static void 
doSphere (FILE *out, node_t *n, point p, double z, int rx, int ry)
{
    pointf  mp;

    if (!(strcmp(cstk[SP].fillcolor, "transparent"))) {
	return;
    }
 
    mp.x = ND_coord_i(n).x;
    mp.y = ND_coord_i(n).y;

    fprintf(out, "Transform {\n");
    fprintf(out, "  translation %.3f %.3f %.3f\n", mp.x, mp.y, z);
    fprintf(out, "  scale %d %d %d\n", rx, rx, rx);
    fprintf(out, "  children [\n");
    fprintf(out, "    Transform {\n");
    fprintf(out, "      children [\n");
    fprintf(out, "        Shape {\n");
    fprintf(out, "          geometry Sphere { radius 1.0 }\n");
    fprintf(out, "          appearance Appearance {\n");
    fprintf(out, "            material Material {\n");
    fprintf(out, "              ambientIntensity 0.33\n");
    fprintf(out, "              diffuseColor %f %f %f\n", 
	cstk[SP].r,cstk[SP].g,cstk[SP].b);
    fprintf(out, "            }\n");
    fprintf(out, "          }\n");
    fprintf(out, "        }\n");
    fprintf(out, "      ]\n");
    fprintf(out, "    }\n");
    fprintf(out, "  ]\n");
    fprintf(out, "}\n");
}

static void vrml_ellipse(GVJ_t *job, point p, int rx, int ry, int filled)
{
    FILE *out = job->output_file;
    obj_state_t *obj = job->obj;
    node_t *n = obj->n;
    edge_t *e = obj->e;
    double z = obj->z;
    pointf mp;
    int i;
    int style[40];		/* need 2* size for arcs, I don't know why */
    int pen, width;
    gdImagePtr brush = NULL;

    if (n) {
	if (shapeOf(n) == SH_POINT) {
	    doSphere (out, n, p, z, rx, ry);
	    return;
	}
	cstk[SP].pencolor_ix = vrml_resolve_color(cstk[SP].pencolor);
	cstk[SP].fillcolor_ix = vrml_resolve_color(cstk[SP].fillcolor);
	if (cstk[SP].pen != P_NONE) {
	    if (cstk[SP].pen == P_DASHED) {
		for (i = 0; i < 20; i++)
		    style[i] = cstk[SP].pencolor_ix;
		for (; i < 40; i++)
		    style[i] = gdTransparent;
		gdImageSetStyle(im, style, 40);
		pen = gdStyled;
	    } else if (cstk[SP].pen == P_DOTTED) {
		for (i = 0; i < 2; i++)
		    style[i] = cstk[SP].pencolor_ix;
		for (; i < 24; i++)
		    style[i] = gdTransparent;
		gdImageSetStyle(im, style, 24);
		pen = gdStyled;
	    } else {
		pen = cstk[SP].pencolor_ix;
	    }
	    if (cstk[SP].penwidth != WIDTH_NORMAL) {
		width = cstk[SP].penwidth;
		brush = gdImageCreate(width, width);
		gdImagePaletteCopy(brush, im);
		gdImageFilledRectangle(brush,
				       0, 0, width - 1, width - 1,
				       cstk[SP].pencolor_ix);
		gdImageSetBrush(im, brush);
		if (pen == gdStyled)
		    pen = gdStyledBrushed;
		else
		    pen = gdBrushed;
	    }
	    mp = vrml_node_point(job, n, p);

	    if (filled) {
		gdImageFilledEllipse(im, ROUND(mp.x), ROUND(mp.y),
				     ROUND(Scale * (rx + rx)),
				     ROUND(Scale * (ry + ry)),
				     cstk[SP].fillcolor_ix);
	    }
	    gdImageArc(im, ROUND(mp.x), ROUND(mp.y),
		       ROUND(Scale * (rx + rx)), ROUND(Scale * (ry + ry)),
		       0, 360, pen);
	    if (brush)
		gdImageDestroy(brush);
	}

	mp.x = ND_coord_i(n).x;
	mp.y = ND_coord_i(n).y;

	fprintf(out, "Transform {\n");
	fprintf(out, "  translation %.3f %.3f %.3f\n", mp.x, mp.y, z);
	fprintf(out, "  scale %d %d 1\n", rx, ry);
	fprintf(out, "  children [\n");
	fprintf(out, "    Transform {\n");
	fprintf(out, "      rotation 1 0 0   1.57\n");
	fprintf(out, "      children [\n");
	fprintf(out, "        Shape {\n");
	fprintf(out, "          geometry Cylinder { side FALSE }\n");
	fprintf(out, "          appearance Appearance {\n");
	fprintf(out, "            material Material {\n");
	fprintf(out, "              ambientIntensity 0.33\n");
	fprintf(out, "              diffuseColor 1 1 1\n");
	fprintf(out, "            }\n");
	fprintf(out, "            texture ImageTexture { url \"node%d.png\" }\n", n->id);
	fprintf(out, "          }\n");
	fprintf(out, "        }\n");
	fprintf(out, "      ]\n");
	fprintf(out, "    }\n");
	fprintf(out, "  ]\n");
	fprintf(out, "}\n");

    }
    else if (e) {
	if (cstk[SP].pen == P_NONE)
	    return;
	mp.x = (double) p.x;
	mp.y = (double) p.y;
	/* this is gruesome, but how else can we get z coord */
	if (DIST2(mp, ND_coord_i(e->tail)) < DIST2(mp, ND_coord_i(e->head)))
	    z = obj->tail_z;
	else
	    z = obj->head_z;

	fprintf(out, "Transform {\n");
	fprintf(out, "  translation %.3f %.3f %.3f\n", mp.x, mp.y, z);
	fprintf(out, "  children [\n");
	fprintf(out, "    Shape {\n");
	fprintf(out, "      geometry Sphere {radius %.3f }\n", (double) rx);
	fprintf(out, "      appearance USE E%d\n", e->id);
	fprintf(out, "    }\n");
	fprintf(out, "  ]\n");
	fprintf(out, "}\n");
    }
}

static void vrml_polyline(point * A, int n)
{
/*
	pointf		  p, p1;
	int			 i;

	if (cstk[SP].pen != P_NONE) {
		p.x = A[0].x;
		p.y = A[0].y;
		for (i = 1; i < n; i++) {
			p1.x = A[i].x;
			p1.y = A[i].y;
#ifdef NONEOFTHISEITHER
			if (cstk[SP].pen == P_DASHED) {
				gdImageDashedLine(im, ROUND(p.x), ROUND(p.y),
					ROUND(p1.x), ROUND(p1.y), cstk[SP].color_ix);
			} else {
				gdImageLine(im, ROUND(p.x), ROUND(p.y),
					ROUND(p1.x), ROUND(p1.y), cstk[SP].color_ix);
			}
#endif
			p.x = p1.x;
			p.y = p1.y;
		}
	}
*/
}

static gvrender_engine_t vrml_engine = {
    0,                          /* vrml_begin_job */
    0,                          /* vrml_end_job */
    0,                          /* vrml_begin_graph */
    0,                          /* vrml_end_graph */
    0,                          /* vrml_begin_layer */
    0,                          /* vrml_end_layer */
    vrml_begin_page,
    vrml_end_page,
    0,                          /* vrml_begin_cluster */
    0,                          /* vrml_end_cluster */
    0,                          /* vrml_begin_nodes */
    0,                          /* vrml_end_nodes */
    0,                          /* vrml_begin_edges */
    0,                          /* vrml_end_edges */
    vrml_begin_node,
    vrml_end_node,
    vrml_begin_edge,
    vrml_end_edge,
    0,                          /* vrml_begin_anchor */
    0,                          /* vrml_end_anchor */
    vrml_textpara,
    vrml_resolve_color,
    vrml_ellipse,
    vrml_polygon,
    vrml_bezier,
    vrml_polyline,
    0,                          /* vrml_comment */
};

static gvrender_features_t vrml_features = {
    GVRENDER_DOES_Z 
        | GVRENDER_Y_GOES_DOWN, /* flags */
    0,                          /* default margin - points */
    {96.,96.},                  /* default dpi */
    NULL,                       /* knowncolors */
    0,                          /* sizeof knowncolors */
    RGBA_BYTE,                  /* color_type */
    NULL,                       /* device */
    NULL,                       /* gvloadimage target for usershapes */
};
#endif				/* HAVE_GD_PNG */

gvplugin_installed_t gvrender_gd_types[] = {
#ifdef HAVE_GD_PNG
    {FORMAT_VRML, "vrml", 1, &vrml_engine, &vrml_features},
#endif
    {0, NULL, 0, NULL, NULL}
};
