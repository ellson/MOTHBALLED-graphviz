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

#ifdef HAVE_LIBGD
#include "gd.h"

#ifdef HAVE_GD_PNG

/* for N_GNEW() */
#include "memory.h"

/* for gvcolor_t */
#include "color.h"

/* for ? */
#include "graph.h"

/* for late_double() */
#include "agxbuf.h"
#include "utils.h"

/* for wind() */
#include "pathutil.h"

extern shape_kind shapeOf(node_t *);

extern pointf Bezier(pointf * V, int degree, double t, pointf * Left, pointf * Right);

typedef enum { FORMAT_VRML, } format_type;

#ifndef MAXFLOAT
#define MAXFLOAT 10000000.
#endif

#define BEZIERSUBDIVISION 10

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

static pointf vrml_node_point(GVJ_t *job, node_t *n, pointf p)
{
    pointf rv;

    /* make mp relative to PNG canvas */
    if (job->rotation) {
	rv.x = ( p.y - ND_coord_i(n).y + ND_lw_i(n)     ) * Scale;
	rv.y = (-p.x + ND_coord_i(n).x + ND_ht_i(n) / 2.) * Scale;
    } else {
	rv.x = ( p.x - ND_coord_i(n).x + ND_lw_i(n)     ) * Scale;
	rv.y = (-p.y + ND_coord_i(n).y + ND_ht_i(n) / 2.) * Scale;
    }
    return rv;
}

static int color_index(gdImagePtr im, gvcolor_t color)
{
    /* seems gd alpha is "transparency" rather than the usual "opacity" */
    return (gdImageColorResolveAlpha(im,
		color.u.rgba[0],
		color.u.rgba[1],
		color.u.rgba[2],
		(255 - color.u.rgba[3]) * gdAlphaMax / 255));
}

static int set_penstyle(GVJ_t * job, gdImagePtr im, gdImagePtr brush)
{
    gvstyle_t *style = job->style;
    int i, pen, pencolor, transparent, width, dashstyle[40];

    pen = pencolor = color_index(im, style->pencolor);
    transparent = gdImageGetTransparent(im);
    if (style->pen == PEN_DASHED) {
        for (i = 0; i < 20; i++)
            dashstyle[i] = pencolor;
        for (; i < 40; i++)
            dashstyle[i] = transparent;
        gdImageSetStyle(im, dashstyle, 20);
        pen = gdStyled;
    } else if (style->pen == PEN_DOTTED) {
        for (i = 0; i < 2; i++)
            dashstyle[i] = pencolor;
        for (; i < 24; i++)
            dashstyle[i] = transparent;
        gdImageSetStyle(im, dashstyle, 24);
        pen = gdStyled;
    }
    width = style->penwidth * job->scale.x;
    if (width < PENWIDTH_NORMAL)
        width = PENWIDTH_NORMAL;  /* gd can't do thin lines */
    gdImageSetThickness(im, width);
    /* use brush instead of Thickness to improve end butts */
    if (width != PENWIDTH_NORMAL) {
        brush = gdImageCreate(width, width);
        gdImagePaletteCopy(brush, im);
        gdImageFilledRectangle(brush, 0, 0, width - 1, width - 1, pencolor);
        gdImageSetBrush(im, brush);
        if (pen == gdStyled)
            pen = gdStyledBrushed;
        else
            pen = gdBrushed;
    }
    return pen;
}

/* warmed over VRML code starts here */

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
	    Scale * (bb.UR.x + bb.LL.x) / 72.,
	    Scale * (bb.UR.y + bb.LL.y) / 72.,
	    Scale * 2 * z / 72.);
    fprintf(out, "] }\n");
}

static void vrml_begin_node(GVJ_t *job)
{
    FILE *out = job->output_file;
    obj_state_t *obj = job->obj;
    node_t *n = obj->u.n;
    double z = obj->z;
    int width, height;
    int transparent;

    fprintf(out, "# node %s\n", n->name);
    if (z < MinZ) MinZ = z;
    if (shapeOf(n) != SH_POINT) {
	PNGfile = nodefile(job->output_filename, n);

	width = (ND_lw_i(n) + ND_rw_i(n)) * Scale + 3;
	height = (ND_ht_i(n)) * Scale + 3;
	im = gdImageCreate(width, height);

	/* make backround transparent */
	transparent = gdImageColorResolve(im, 255, 255, 254);
	gdImageColorTransparent(im, transparent);
    }
}

static void vrml_end_node(GVJ_t *job)
{
    if (im) {
	gdImagePng(im, PNGfile);
	gdImageDestroy(im);
	im = NULL;
	fclose(PNGfile);
    }
}

static void vrml_begin_edge(GVJ_t *job)
{
    FILE *out = job->output_file;
    obj_state_t *obj = job->obj;
    edge_t *e = obj->u.e;

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
    fprintf(out, "      center 0 %.3f 0\n", y0);
    fprintf(out, "      rotation %.3f 0 %.3f %.3f\n", -z, x, -theta);
    fprintf(out, "      translation %.3f %.3f %.3f\n", o_x, o_y - y0, o_z);
    fprintf(out, "    }\n");
}

static void vrml_end_edge(GVJ_t *job)
{
    if (IsSegment)
	finishSegment(job->output_file, job->obj->u.e);
    fprintf(job->output_file, "] }\n");
}

static void vrml_textpara(GVJ_t *job, pointf p, textpara_t * para)
{
    obj_state_t *obj = job->obj;
    char *err;
    pointf mp;
    int brect[8];
    extern gdFontPtr gdFontSmall;

    if (! obj->u.n)
	return;

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

    mp = vrml_node_point(job, obj->u.n, p);

    err = gdImageStringFT(im, brect,
	    color_index(im, job->style->pencolor),
	    para->fontname, para->fontsize, job->rotation ? PI/2 : 0,
	    ROUND(mp.x), ROUND(mp.y), (char*)para->str);
    if (err) {
	/* revert to builtin fonts */
	gdImageString(im,
	       	gdFontSmall, ROUND(mp.x), ROUND(mp.y),
		(unsigned char *) para->str,
		color_index(im, job->style->pencolor));
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
interpolate_zcoord(GVJ_t *job, pointf p1, pointf fst, double fstz, pointf snd, double sndz)
{
    obj_state_t *obj = job->obj;
    edge_t *e = obj->u.e;
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
collinear (pointf * A)
{
    double w;

    w = wind(A[0],A[1],A[2]);
    return (fabs(w) <= 1);
}

/* straight:
 * Return true if bezier points are collinear
 * At present, just check with 4 points, the common case.
 */
static int
straight (pointf * A, int n)
{
    if (n != 4) return 0;
    return (collinear(A) && collinear(A+1));
}

static void
doSegment (GVJ_t *job, pointf* A, point p0, double z0, point p1, double z1)
{
    FILE *out = job->output_file;
    gvstyle_t *style = job->style;
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
    fprintf(out, "        height %.3f radius %.3f }\n", CylHt, style->penwidth);
    fprintf(out, "      appearance Appearance {\n");
    fprintf(out, "        material Material {\n");
    fprintf(out, "          ambientIntensity 0.33\n");
    fprintf(out, "          diffuseColor %.3f %.3f %.3f\n",
	    style->pencolor.u.rgba[0] / 255.,
	    style->pencolor.u.rgba[1] / 255.,
	    style->pencolor.u.rgba[2] / 255.);
    fprintf(out, "        }\n");
    fprintf(out, "      }\n");
    fprintf(out, "    }\n");
}

static void
vrml_bezier(GVJ_t *job, pointf * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    FILE *out = job->output_file;
    gvstyle_t *style = job->style;
    obj_state_t *obj = job->obj;
    edge_t *e = obj->u.e;
    double fstz = obj->tail_z, sndz = obj->head_z;
    pointf p1, V[4];
    int i, j, step;

    assert(e);

    if (straight(A,n)) {
	doSegment (job, A, ND_coord_i(e->tail),Fstz,ND_coord_i(e->head),Sndz);
	return;
    }

    fprintf(out, "Shape { geometry Extrusion  {\n");
    fprintf(out, "  spine [");
    V[3] = A[0];
    for (i = 0; i + 3 < n; i += 3) {
	V[0] = V[3];
	for (j = 1; j <= 3; j++)
	    V[j] = A[i + j];
	for (step = 0; step <= BEZIERSUBDIVISION; step++) {
	    p1 = Bezier(V, 3, (double) step / BEZIERSUBDIVISION, NULL, NULL);
	    fprintf(out, " %.3f %.3f %.3f", p1.x, p1.y,
		    interpolate_zcoord(job, p1, A[0], fstz, A[n - 1], sndz));
	}
    }
    fprintf(out, " ]\n");
    fprintf(out, "  crossSection [ %.3f %.3f, %.3f %.3f, %.3f %.3f, %.3f %.3f ]\n",
	    (style->penwidth), (style->penwidth), -(style->penwidth),
	    (style->penwidth), -(style->penwidth), -(style->penwidth),
	    (style->penwidth), -(style->penwidth));
    fprintf(out, "}\n");
    fprintf(out, " appearance DEF E%d Appearance {\n", e->id);
    fprintf(out, "   material Material {\n");
    fprintf(out, "   ambientIntensity 0.33\n");
    fprintf(out, "   diffuseColor %.3f %.3f %.3f\n",
	    style->pencolor.u.rgba[0] / 255.,
	    style->pencolor.u.rgba[1] / 255.,
	    style->pencolor.u.rgba[2] / 255.);
    fprintf(out, "   }\n");
    fprintf(out, " }\n");
    fprintf(out, "}\n");
}

/* doArrowhead:
 * If edge is straight, we attach a cone to the edge as a group.
 */
static void doArrowhead (GVJ_t *job, pointf * A)
{
    FILE *out = job->output_file;
    gvstyle_t *style = job->style;
    obj_state_t *obj = job->obj;
    edge_t *e = obj->u.e;
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
	fprintf(out, "  translation 0 %.3f 0\n", -y);
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
    fprintf(out, "          diffuseColor %.3f %.3f %.3f\n",
	    style->pencolor.u.rgba[0] / 255.,
	    style->pencolor.u.rgba[1] / 255.,
	    style->pencolor.u.rgba[2] / 255.);
    fprintf(out, "        }\n");
    fprintf(out, "      }\n");
    fprintf(out, "    }\n");
    fprintf(out, "  ]\n");
    fprintf(out, "}\n");
}

static void vrml_polygon(GVJ_t *job, pointf * A, int np, int filled)
{
    FILE *out = job->output_file;
    gvstyle_t *style = job->style;
    obj_state_t *obj = job->obj;
    node_t *n;
    edge_t *e;
    double z = obj->z;
    pointf p, mp;
    gdPoint *points;
    int i, pen;
    gdImagePtr brush = NULL;
    double theta;

    switch (obj->type) {
    case ROOTGRAPH_OBJTYPE:
	fprintf(out, " Background { skyColor %.3f %.3f %.3f }\n",
	    style->fillcolor.u.rgba[0] / 255.,
	    style->fillcolor.u.rgba[1] / 255.,
	    style->fillcolor.u.rgba[2] / 255.);
	Saw_skycolor = TRUE;
	break;
    case CLUSTER_OBJTYPE:
	break;
    case NODE_OBJTYPE:
	n = obj->u.n;
	pen = set_penstyle(job, im, brush);
	points = N_GNEW(np, gdPoint);
	for (i = 0; i < np; i++) {
	    mp = vrml_node_point(job, n, A[i]);
	    points[i].x = ROUND(mp.x);
	    points[i].y = ROUND(mp.y);
	}
	if (filled)
	    gdImageFilledPolygon(im, points, np, color_index(im, job->style->fillcolor));
	gdImagePolygon(im, points, np, pen);
	free(points);
	if (brush)
	    gdImageDestroy(brush);

	fprintf(out, "Shape {\n");
	fprintf(out, "  appearance Appearance {\n");
	fprintf(out, "    material Material {\n");
	fprintf(out, "      ambientIntensity 0.33\n");
	fprintf(out, "        diffuseColor 1 1 1\n");
	fprintf(out, "    }\n");
	fprintf(out, "    texture ImageTexture { url \"node%d.png\" }\n", n->id);
	fprintf(out, "  }\n");
	fprintf(out, "  geometry Extrusion {\n");
	fprintf(out, "    crossSection [");
	for (i = 0; i < np; i++) {
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
	break;
    case EDGE_OBJTYPE:
	e = obj->u.e;
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
	fprintf(out, "          geometry Cone {bottomRadius %.3f height %.3f }\n",
		style->penwidth * 2.5, style->penwidth * 10.0);
	fprintf(out, "          appearance USE E%d\n", e->id);
	fprintf(out, "        }\n");
	fprintf(out, "      ]\n");
	fprintf(out, "    }\n");
	fprintf(out, "  ]\n");
	fprintf(out, "}\n");
	break;
    }
}

/* doSphere:
 * Output sphere in VRML for point nodes.
 */
static void 
doSphere (GVJ_t *job, node_t *n, pointf p, double z, double rx, double ry)
{
    FILE *out = job->output_file;
    gvstyle_t *style = job->style;

//    if (!(strcmp(cstk[SP].fillcolor, "transparent"))) {
//	return;
//    }
 
    fprintf(out, "Transform {\n");
    fprintf(out, "  translation %.3f %.3f %.3f\n", p.x, p.y, z);
    fprintf(out, "  scale %.3f %.3f %.3f\n", rx, rx, rx);
    fprintf(out, "  children [\n");
    fprintf(out, "    Transform {\n");
    fprintf(out, "      children [\n");
    fprintf(out, "        Shape {\n");
    fprintf(out, "          geometry Sphere { radius 1.0 }\n");
    fprintf(out, "          appearance Appearance {\n");
    fprintf(out, "            material Material {\n");
    fprintf(out, "              ambientIntensity 0.33\n");
    fprintf(out, "              diffuseColor %.3f %.3f %.3f\n", 
	    style->pencolor.u.rgba[0] / 255.,
	    style->pencolor.u.rgba[1] / 255.,
	    style->pencolor.u.rgba[2] / 255.);
    fprintf(out, "            }\n");
    fprintf(out, "          }\n");
    fprintf(out, "        }\n");
    fprintf(out, "      ]\n");
    fprintf(out, "    }\n");
    fprintf(out, "  ]\n");
    fprintf(out, "}\n");
}

static void vrml_ellipse(GVJ_t * job, pointf * A, int filled)
{
    FILE *out = job->output_file;
    gvstyle_t *style = job->style;
    obj_state_t *obj = job->obj;
    node_t *n;
    edge_t *e;
    double z = obj->z;
    double rx, ry;
    int dx, dy;
    pointf npf, nqf, mp;
    point np;
    int pen;
    gdImagePtr brush = NULL;

    rx = A[1].x - A[0].x;
    ry = A[1].y - A[0].y;

    switch (obj->type) {
    case ROOTGRAPH_OBJTYPE:
    case CLUSTER_OBJTYPE:
	break;
    case NODE_OBJTYPE:
	n = obj->u.n;
        P2PF(ND_coord_i(n), mp);

	if (shapeOf(n) == SH_POINT) {
	    doSphere (job, n, A[0], z, rx, ry);
	    return;
	}
	pen = set_penstyle(job, im, brush);

	npf = vrml_node_point(job, n, A[0]);
	nqf = vrml_node_point(job, n, A[1]);

	dx = ROUND(2 * (nqf.x - npf.x));
	dy = ROUND(2 * (nqf.y - npf.y));

	PF2P(npf, np);

	if (filled)
	    gdImageFilledEllipse(im, np.x, np.y, dx, dy, color_index(im, style->fillcolor));
	gdImageArc(im, np.x, np.y, dx, dy, 0, 360, pen);

	if (brush)
	    gdImageDestroy(brush);

	fprintf(out, "Transform {\n");
	fprintf(out, "  translation %.3f %.3f %.3f\n", mp.x, mp.y, z);
	fprintf(out, "  scale %.3f %.3f 1\n", rx, ry);
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
	break;
    case EDGE_OBJTYPE:
	e = obj->u.e;
	/* this is gruesome, but how else can we get z coord */
	if (DIST2(A[0], ND_coord_i(e->tail)) < DIST2(A[0], ND_coord_i(e->head)))
	    z = obj->tail_z;
	else
	    z = obj->head_z;

	fprintf(out, "Transform {\n");
	fprintf(out, "  translation %.3f %.3f %.3f\n", A[0].x, A[0].y, z);
	fprintf(out, "  children [\n");
	fprintf(out, "    Shape {\n");
	fprintf(out, "      geometry Sphere {radius %.3f }\n", (double) rx);
	fprintf(out, "      appearance USE E%d\n", e->id);
	fprintf(out, "    }\n");
	fprintf(out, "  ]\n");
	fprintf(out, "}\n");
    }
}

static void vrml_polyline(GVJ_t *job, pointf * A, int n)
{
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
    0,				/* vrml_resolve_color */
    vrml_ellipse,
    vrml_polygon,
    vrml_bezier,
    vrml_polyline,
    0,                          /* vrml_comment */
};

static gvrender_features_t vrml_features = {
    GVRENDER_DOES_Z, 		/* flags */
    0,                          /* default margin - points */
    {72.,72.},                  /* default dpi */
    NULL,                       /* knowncolors */
    0,                          /* sizeof knowncolors */
    RGBA_BYTE,                  /* color_type */
    NULL,                       /* device */
    "vrml",                     /* gvloadimage target for usershapes */
};
#endif				/* HAVE_GD_PNG */
#endif				/* HAVE_LIBGD */

gvplugin_installed_t gvrender_vrml_types[] = {
#ifdef HAVE_LIBGD
#ifdef HAVE_GD_PNG
    {FORMAT_VRML, "vrml", 1, &vrml_engine, &vrml_features},
#endif
#endif
    {0, NULL, 0, NULL, NULL}
};
