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

/*
 *  graphics code generator
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "render.h"
#include "agxbuf.h"
#include "htmltable.h"

#define P2RECT(p, pr, sx, sy) (pr[0].x = p.x - sx, pr[0].y = p.y - sy, pr[1].x = p.x + sx, pr[1].y = p.y + sy)
#define FUZZ 3
#define EPSILON .0001

static char *defaultlinestyle[3] = { "solid\0", "setlinewidth\0001\0", 0 };

/* push empty graphic state for current object */
obj_state_t* push_obj_state(GVJ_t *job)
{
    obj_state_t *obj, *parent;

    if (! (obj = zmalloc(sizeof(obj_state_t))))
        agerr(AGERR, "no memory from zmalloc()\n");

    parent = obj->parent = job->obj;
    job->obj = obj;
    if (parent) {
        obj->pencolor = parent->pencolor;        /* default styles to parent's style */
        obj->fillcolor = parent->fillcolor;
        obj->pen = parent->pen;
        obj->fill = parent->fill;
        obj->penwidth = parent->penwidth;
    }
    else {
	/* obj->pencolor = NULL */
	/* obj->fillcolor = NULL */
	obj->pen = PEN_SOLID;
	obj->fill = FILL_NONE;
	obj->penwidth = PENWIDTH_NORMAL;
    }
    return obj;
}

/* pop graphic state of current object */
void pop_obj_state(GVJ_t *job)
{
    obj_state_t *obj = job->obj;

    assert(obj);

    free(obj->url);
    free(obj->labelurl);
    free(obj->tailurl);
    free(obj->headurl);
    free(obj->tooltip);
    free(obj->labeltooltip);
    free(obj->tailtooltip);
    free(obj->headtooltip);
    free(obj->target);
    free(obj->labeltarget);
    free(obj->tailtarget);
    free(obj->headtarget);
    free(obj->url_map_p);
    free(obj->url_bsplinemap_p);
    free(obj->url_bsplinemap_n);

    job->obj = obj->parent;
    free(obj);
}

/* initMapData:
 * Store image map data into job, substituting for node, edge, etc.
 * names.
 * Return 1 if an assignment was made for url or tooltip or target.
 */
int
initMapData (GVJ_t* job, char* lbl, char* url, char* tooltip, char* target,
  void* gobj)
{
    obj_state_t *obj = job->obj;
    int flags = job->flags;
    int assigned = 0;

    if ((flags & GVRENDER_DOES_LABELS) && lbl)
        obj->label = lbl;
    if ((flags & GVRENDER_DOES_MAPS) && url && url[0]) {
        obj->url = strdup_and_subst_obj(url, gobj);
	assigned = 1;
    }
    if (flags & GVRENDER_DOES_TOOLTIPS) {
        if (tooltip && tooltip[0]) {
            obj->tooltip = strdup_and_subst_obj(tooltip, gobj);
            obj->explicit_tooltip = TRUE;
	    assigned = 1;
        }
        else if (obj->label) {
            obj->tooltip = strdup(obj->label);
	    assigned = 1;
        }
    }
    if ((flags & GVRENDER_DOES_TARGETS) && target && target[0]) {
        obj->target = strdup_and_subst_obj(target, gobj);
	assigned = 1;
    }
    return assigned;
}

static void
initObjMapData (GVJ_t* job, textlabel_t *lab, void* gobj)
{
    char* lbl;
    char* url = agget(gobj, "href");
    char* tooltip = agget(gobj, "tooltip");
    char* target = agget(gobj, "target");

    if (lab) lbl = lab->text;
    else lbl = NULL;
    if (!url || !*url) url = agget(gobj, "URL");
    initMapData (job, lbl, url, tooltip, target, gobj);
}

static void map_point(GVJ_t *job, point P)
{
    obj_state_t *obj = job->obj;
    int flags = job->flags;
    pointf *p, pf;

    if (flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS)) {
	if (flags & GVRENDER_DOES_MAP_RECTANGLE) {
	    obj->url_map_shape = MAP_RECTANGLE;
	    obj->url_map_n = 2;
	}
	else {
	    obj->url_map_shape = MAP_POLYGON;
	    obj->url_map_n = 4;
	}
	free(obj->url_map_p);
	obj->url_map_p = p = N_NEW(obj->url_map_n, pointf);
	P2PF(P,pf);
	P2RECT(pf, p, FUZZ, FUZZ);
	if (! (flags & GVRENDER_DOES_TRANSFORM))
	    gvrender_ptf_A(job, p, p, 2);
	if (! (flags & GVRENDER_DOES_MAP_RECTANGLE))
	    rect2poly(p);
    }
}

void emit_map_rect(GVJ_t *job, point LL, point UR)
{
    obj_state_t *obj = job->obj;
    int flags = job->flags;
    pointf *p;

    if (flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS)) {
	if (flags & GVRENDER_DOES_MAP_RECTANGLE) {
	    obj->url_map_shape = MAP_RECTANGLE;
	    obj->url_map_n = 2;
	}
	else {
	    obj->url_map_shape = MAP_POLYGON;
	    obj->url_map_n = 4;
	}
	free(obj->url_map_p);
	obj->url_map_p = p = N_NEW(obj->url_map_n, pointf);
	P2PF(LL,p[0]);
	P2PF(UR,p[1]);
	if (! (flags & GVRENDER_DOES_TRANSFORM))
	    gvrender_ptf_A(job, p, p, 2);
	if (! (flags & GVRENDER_DOES_MAP_RECTANGLE))
	    rect2poly(p);
    }
}

static void map_label(GVJ_t *job, textlabel_t *lab)
{
    obj_state_t *obj = job->obj;
    int flags = job->flags;
    pointf *p;

    if (flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS)) {
	if (flags & GVRENDER_DOES_MAP_RECTANGLE) {
	    obj->url_map_shape = MAP_RECTANGLE;
	    obj->url_map_n = 2;
	}
	else {
	    obj->url_map_shape = MAP_POLYGON;
	    obj->url_map_n = 4;
	}
	free(obj->url_map_p);
	obj->url_map_p = p = N_NEW(obj->url_map_n, pointf);
	P2RECT(lab->p, p, lab->dimen.x / 2., lab->dimen.y / 2.);
	if (! (flags & GVRENDER_DOES_TRANSFORM))
	    gvrender_ptf_A(job, p, p, 2);
	if (! (flags & GVRENDER_DOES_MAP_RECTANGLE))
	    rect2poly(p);
    }
}

/* isRect:
 * isRect function returns true when polygon has
 * regular rectangular shape. Rectangle is regular when
 * it is not skewed and distorted and orientation is almost zero
 */
static boolean isRect(polygon_t * p)
{
    return (p->sides == 4 && (ROUND(p->orientation) % 90) == 0
            && p->distortion == 0.0 && p->skew == 0.0);
}

/*
 * isFilled function returns 1 if filled style has been set for node 'n'
 * otherwise returns 0. it accepts pointer to node_t as an argument
 */
static int ifFilled(node_t * n)
{
    char *style, *p, **pp;
    int r = 0;
    style = late_nnstring(n, N_style, "");
    if (style[0]) {
        pp = parse_style(style);
        while ((p = *pp)) {
            if (strcmp(p, "filled") == 0)
                r = 1;
            pp++;
        }
    }
    return r;
}

/* pEllipse:
 * pEllipse function returns 'np' points from the circumference
 * of ellipse described by radii 'a' and 'b'.
 * Assumes 'np' is greater than zero.
 * 'np' should be at least 4 to sample polygon from ellipse
 */
static pointf *pEllipse(double a, double b, int np)
{
    double theta = 0.0;
    double deltheta = 2 * M_PI / np;
    int i;
    pointf *ps;

    ps = N_NEW(np, pointf);
    for (i = 0; i < np; i++) {
        ps[i].x = a * cos(theta);
        ps[i].y = b * sin(theta);
        theta += deltheta;
    }
    return ps;
}

#define HW 2.0   /* maximum distance away from line, in points */

/* check_control_points:
 * check_control_points function checks the size of quadrilateral
 * formed by four control points
 * returns 1 if four points are in line (or close to line)
 * else return 0
 */
static int check_control_points(pointf *cp)
{
    double dis1 = ptToLine2 (cp[0], cp[3], cp[1]);
    double dis2 = ptToLine2 (cp[0], cp[3], cp[2]);
    if (dis1 < HW*HW && dis2 < HW*HW)
        return 1;
    else
        return 0;
}

#ifdef DEBUG
static void psmapOutput (point* ps, int n)
{
   int i;
   fprintf (stdout, "newpath %d %d moveto\n", ps[0].x, ps[0].y);
   for (i=1; i < n; i++)
        fprintf (stdout, "%d %d lineto\n", ps[i].x, ps[i].y);
   fprintf (stdout, "closepath stroke\n");
}
#endif

typedef struct segitem_s {
    pointf p;
    struct segitem_s* next;
} segitem_t;

#define MARK_FIRST_SEG(L) ((L)->next = (segitem_t*)1)
#define FIRST_SEG(L) ((L)->next == (segitem_t*)1)
#define INIT_SEG(P,L) {(L)->next = 0; (L)->p = P;} 

static segitem_t* appendSeg (pointf p, segitem_t* lp)
{
    segitem_t* s = GNEW(segitem_t);
    INIT_SEG (p, s);
    lp->next = s;
    return s;
}

/* map_bspline_poly:
 * Output the polygon determined by the n points in p1, followed
 * by the n points in p2 in reverse order. Assumes n <= 50.
 */
static void map_bspline_poly(pointf **pbs_p, int **pbs_n, int *pbs_poly_n, int n, pointf* p1, pointf* p2)
{
    int i = 0, nump = 0, last = 2*n-1;

    for ( ; i < *pbs_poly_n; i++)
        nump += (*pbs_n)[i];

    (*pbs_poly_n)++;
    *pbs_n = grealloc(*pbs_n, (*pbs_poly_n) * sizeof(int));
    (*pbs_n)[i] = 2*n;
    *pbs_p = grealloc(*pbs_p, (nump + 2*n) * sizeof(pointf));

    for (i = 0; i < n; i++) {
        (*pbs_p)[nump+i] = p1[i];
        (*pbs_p)[nump+last-i] = p2[i];
    }
#ifdef DEBUG
    psmapOutput (*pbs_p + nump, last+1);
#endif
}

/* approx_bezier:
 * Approximate Bezier by line segments. If the four points are
 * almost colinear, as determined by check_control_points, we store
 * the segment cp[0]-cp[3]. Otherwise we split the Bezier into 2 and recurse. 
 * Since 2 contiguous segments share an endpoint, we actually store
 * the segments as a list of points.
 * New points are appended to the list given by lp. The tail of the
 * list is returned.
 */
static segitem_t* approx_bezier (pointf *cp, segitem_t* lp)
{
    pointf sub_curves[8];

    if (check_control_points(cp)) {
        if (FIRST_SEG (lp)) INIT_SEG (cp[0], lp);
        lp = appendSeg (cp[3], lp);
    }
    else {
        Bezier (cp, 3, 0.5, sub_curves, sub_curves+4);
        lp = approx_bezier (sub_curves, lp);
        lp = approx_bezier (sub_curves+4, lp);
    }
    return lp;
}

/* bisect:
 * Return the angle of the bisector between the two rays
 * pp-cp and cp-np. The bisector returned is always to the
 * left of pp-cp-np.
 */
static double bisect (pointf pp, pointf cp, pointf np)
{
  double ang, theta, phi;
  theta = atan2(np.y - cp.y,np.x - cp.x);
  phi = atan2(pp.y - cp.y,pp.x - cp.x);
  ang = theta - phi;
  if (ang > 0) ang -= 2*M_PI;

  return (phi + ang/2.0);
}

/* mkSegPts:
 * Determine polygon points related to 2 segments prv-cur and cur-nxt.
 * The points lie on the bisector of the 2 segments, passing through cur,
 * and distance HW from cur. The points are stored in p1 and p2.
 * If p1 is NULL, we use the normal to cur-nxt.
 * If p2 is NULL, we use the normal to prv-cur.
 * Assume at least one of prv or nxt is non-NULL.
 */
static void mkSegPts (segitem_t* prv, segitem_t* cur, segitem_t* nxt,
        pointf* p1, pointf* p2)
{
    pointf cp, pp, np;
    double theta, delx, dely;
    pointf p;

    cp = cur->p;
    /* if prv or nxt are NULL, use the one given to create a collinear
     * prv or nxt. This could be more efficiently done with special case code, 
     * but this way is more uniform.
     */
    if (prv) {
        pp = prv->p;
        if (nxt)
            np = nxt->p;
        else {
            np.x = 2*cp.x - pp.x;
            np.y = 2*cp.y - pp.y;
        }
    }
    else {
        np = nxt->p;
        pp.x = 2*cp.x - np.x;
        pp.y = 2*cp.y - np.y;
    }
    theta = bisect(pp,cp,np);
    delx = HW*cos(theta);
    dely = HW*sin(theta);
    p.x = cp.x + delx;
    p.y = cp.y + dely;
    *p1 = p;
    p.x = cp.x - delx;
    p.y = cp.y - dely;
    *p2 = p;
}

/* map_output_bspline:
 * Construct and output a closed polygon approximating the input
 * B-spline bp. We do this by first approximating bp by a sequence
 * of line segments. We then use the sequence of segments to determine
 * the polygon.
 * In cmapx, polygons are limited to 100 points, so we output polygons
 * in chunks of 100.
 */
static void map_output_bspline (pointf **pbs, int **pbs_n, int *pbs_poly_n, bezier* bp)
{
    segitem_t* segl = GNEW(segitem_t);
    segitem_t* segp = segl;
    segitem_t* segprev;
    segitem_t* segnext;
    int nc, j, k, cnt;
    pointf pts[4];
    pointf pt1[50], pt2[50];

    MARK_FIRST_SEG(segl);
    nc = (bp->size - 1)/3; /* nc is number of bezier curves */
    for (j = 0; j < nc; j++) {
        for (k = 0; k < 4; k++) {
            pts[k].x = (double)bp->list[3*j + k].x;
            pts[k].y = (double)bp->list[3*j + k].y;
        }
        segp = approx_bezier (pts, segp);
    }

    segp = segl;
    segprev = 0;
    cnt = 0;
    while (segp) {
        segnext = segp->next;
        mkSegPts (segprev, segp, segnext, pt1+cnt, pt2+cnt);
        cnt++;
        if ((segnext == NULL) || (cnt == 50)) {
            map_bspline_poly (pbs, pbs_n, pbs_poly_n, cnt, pt1, pt2);
            pt1[0] = pt1[cnt-1];
            pt2[0] = pt2[cnt-1];
            cnt = 1;
        }
        segprev = segp;
        segp = segnext;
    }

    /* free segl */
    while (segl) {
        segp = segl->next;
        free (segl);
        segl = segp;
    }
}


/* parse_layers:
 * Split input string into tokens, with separators specified by
 * the layersep attribute. Store the values in the gvc->layerIDs array,
 * starting at index 1, and return the count.
 * Free previously stored list. Note that there is no mechanism
 * to free the memory before exit.
 */
static int parse_layers(GVC_t *gvc, graph_t * g, char *p)
{
    int ntok;
    char *tok;
    int sz;

    gvc->layerDelims = agget(g, "layersep");
    if (!gvc->layerDelims)
        gvc->layerDelims = DEFAULT_LAYERSEP;

    ntok = 0;
    sz = 0;
    gvc->layers = strdup(p);

    for (tok = strtok(gvc->layers, gvc->layerDelims); tok;
         tok = strtok(NULL, gvc->layerDelims)) {
        ntok++;
        if (ntok > sz) {
            sz += SMALLBUF;
            gvc->layerIDs = ALLOC(sz, gvc->layerIDs, char *);
        }
        gvc->layerIDs[ntok] = tok;
    }
    if (ntok) {
        gvc->layerIDs = RALLOC(ntok + 2, gvc->layerIDs, char *);        /* shrink to minimum size */
        gvc->layerIDs[0] = NULL;
        gvc->layerIDs[ntok + 1] = NULL;
    }

    return ntok;
}

/* chkOrder:
 * Determine order of output.
 * Output usually in breadth first graph walk order
 */
static int chkOrder(graph_t * g)
{
    char *p = agget(g, "outputorder");
    if (p) {
        char c = *p;
        if ((c == 'n') && !strcmp(p + 1, "odesfirst"))
            return EMIT_SORTED;
        if ((c == 'e') && !strcmp(p + 1, "dgesfirst"))
            return EMIT_EDGE_SORTED;
    }
    return 0;
}

static void init_job_flags(GVJ_t * job, graph_t * g)
{
    switch (job->output_lang) {
    case GVRENDER_PLUGIN:
	job->flags |= chkOrder(g)
		   | job->render.features->flags
		   | job->device.features->flags;
	break;
    case VTX:
        /* output sorted, i.e. all nodes then all edges */
        job->flags |= EMIT_SORTED;
        break;
    case DIA:
        /* output in preorder traversal of the graph */
        job->flags |= EMIT_PREORDER
		   | GVDEVICE_BINARY_FORMAT;
        break;
    default:
        job->flags |= chkOrder(g);
        break;
    }
}

static void init_layering(GVC_t * gvc, graph_t * g)
{
    char *str;

    /* free layer strings and pointers from previous graph */
    if (gvc->layers)
	free(gvc->layers);
    if (gvc->layerIDs)
	free(gvc->layerIDs);

    if ((str = agget(g, "layers")) != 0) {
	gvc->numLayers = parse_layers(gvc, g, str);
    } else {
	gvc->layerIDs = NULL;
	gvc->numLayers = 1;
    }
}

static void firstlayer(GVJ_t *job)
{
    job->numLayers = job->gvc->numLayers;
    if ((job->numLayers > 1)
		&& (! (gvrender_features(job) & GVDEVICE_DOES_LAYERS))) {
	agerr(AGWARN, "layers not supported in %s output\n",
		job->output_langname);
	job->numLayers = 1;
    }

    job->layerNum = 1;
}

static boolean validlayer(GVJ_t *job)
{
    return (job->layerNum <= job->numLayers);
}

static void nextlayer(GVJ_t *job)
{
    job->layerNum++;
}

static point pagecode(GVJ_t *job, char c)
{
    point rv;
    rv.x = rv.y = 0;
    switch (c) {
    case 'T':
	job->pagesArrayFirst.y = job->pagesArraySize.y - 1;
	rv.y = -1;
	break;
    case 'B':
	rv.y = 1;
	break;
    case 'L':
	rv.x = 1;
	break;
    case 'R':
	job->pagesArrayFirst.x = job->pagesArraySize.x - 1;
	rv.x = -1;
	break;
    }
    return rv;
}

static void init_job_pagination(GVJ_t * job, graph_t *g)
{
    GVC_t *gvc = job->gvc;
    pointf pageSize;	/* page size for the graph - points*/
    pointf imageSize;	/* image size on one page of the graph - points */
    pointf margin;	/* margin for a page of the graph - points */
    pointf centering = {0.0, 0.0}; /* centering offset - points */

    /* unpaginated image size - in points - in graph orientation */
    imageSize = job->view;

    /* rotate imageSize to page orientation */
    if (job->rotation)
	imageSize = exch_xyf(imageSize);

    /* margin - in points - in page orientation */
    margin = job->margin;

    /* determine pagination */
    if (gvc->graph_sets_pageSize) {
	/* page was set by user */

        /* determine size of page for image */
	pageSize.x = gvc->pageSize.x - 2 * margin.x;
	pageSize.y = gvc->pageSize.y - 2 * margin.y;

	if (pageSize.x < EPSILON)
	    job->pagesArraySize.x = 1;
	else {
	    job->pagesArraySize.x = (int)(imageSize.x / pageSize.x);
	    if ((imageSize.x - (job->pagesArraySize.x * pageSize.x)) > EPSILON)
		job->pagesArraySize.x++;
	}
	if (pageSize.y < EPSILON)
	    job->pagesArraySize.y = 1;
	else {
	    job->pagesArraySize.y = (int)(imageSize.y / pageSize.y);
	    if ((imageSize.y - (job->pagesArraySize.y * pageSize.y)) > EPSILON)
		job->pagesArraySize.y++;
	}
	job->numPages = job->pagesArraySize.x * job->pagesArraySize.y;

	/* find the drawable size in points */
	imageSize.x = MIN(imageSize.x, pageSize.x);
	imageSize.y = MIN(imageSize.y, pageSize.y);
    } else {
	/* page not set by user, use default from renderer */
	if (job->render.features) {
	    pageSize.x = job->device.features->default_pagesize.x - 2*margin.x;
	    if (pageSize.x < 0.)
		pageSize.x = 0.;
	    pageSize.y = job->device.features->default_pagesize.y - 2*margin.y;
	    if (pageSize.y < 0.)
		pageSize.y = 0.;
	}
	else
	    pageSize.x = pageSize.y = 0.;
	job->pagesArraySize.x = job->pagesArraySize.y = job->numPages = 1;
        
        if (pageSize.x < imageSize.x)
	    pageSize.x = imageSize.x;
        if (pageSize.y < imageSize.y)
	    pageSize.y = imageSize.y;
    }

    /* initial window size */
    job->width = ROUND((pageSize.x + 2*margin.x) * job->dpi.x / POINTS_PER_INCH);
    job->height = ROUND((pageSize.y + 2*margin.y) * job->dpi.y / POINTS_PER_INCH);

    /* set up pagedir */
    job->pagesArrayMajor.x = job->pagesArrayMajor.y 
		= job->pagesArrayMinor.x = job->pagesArrayMinor.y = 0;
    job->pagesArrayFirst.x = job->pagesArrayFirst.y = 0;
    job->pagesArrayMajor = pagecode(job, gvc->pagedir[0]);
    job->pagesArrayMinor = pagecode(job, gvc->pagedir[1]);
    if ((abs(job->pagesArrayMajor.x + job->pagesArrayMinor.x) != 1)
     || (abs(job->pagesArrayMajor.y + job->pagesArrayMinor.y) != 1)) {
	job->pagesArrayMajor = pagecode(job, 'B');
	job->pagesArrayMinor = pagecode(job, 'L');
	agerr(AGWARN, "pagedir=%s ignored\n", gvc->pagedir);
    }

    /* determine page box including centering */
    if (GD_drawing(g)->centered) {
	if (pageSize.x > imageSize.x)
	    centering.x = (pageSize.x - imageSize.x) / 2;
	if (pageSize.y > imageSize.y)
	    centering.y = (pageSize.y - imageSize.y) / 2;
    }

    /* rotate back into graph orientation */
    if (job->rotation) {
	imageSize = exch_xyf(imageSize);
	pageSize = exch_xyf(pageSize);
	margin = exch_xyf(margin);
	centering = exch_xyf(centering);
    }

    /* canvas area, centered if necessary */
    job->canvasBox.LL.x = margin.x + centering.x;
    job->canvasBox.LL.y = margin.y + centering.y;
    job->canvasBox.UR.x = margin.x + centering.x + imageSize.x;
    job->canvasBox.UR.y = margin.y + centering.y + imageSize.y;

    /* size of one page in graph units */
    job->pageSize.x = imageSize.x / job->zoom;
    job->pageSize.y = imageSize.y / job->zoom;
}

static void firstpage(GVJ_t *job)
{
    job->pagesArrayElem = job->pagesArrayFirst;
}

static boolean validpage(GVJ_t *job)
{
    return ((job->pagesArrayElem.x >= 0)
	 && (job->pagesArrayElem.x < job->pagesArraySize.x)
	 && (job->pagesArrayElem.y >= 0)
	 && (job->pagesArrayElem.y < job->pagesArraySize.y));
}

static void nextpage(GVJ_t *job)
{
    job->pagesArrayElem = add_points(job->pagesArrayElem, job->pagesArrayMinor);
    if (validpage(job) == FALSE) {
	if (job->pagesArrayMajor.y)
	    job->pagesArrayElem.x = job->pagesArrayFirst.x;
	else
	    job->pagesArrayElem.y = job->pagesArrayFirst.y;
	job->pagesArrayElem = add_points(job->pagesArrayElem, job->pagesArrayMajor);
    }
}

static boolean write_edge_test(Agraph_t * g, Agedge_t * e)
{
    Agraph_t *sg;
    int c;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	sg = GD_clust(g)[c];
	if (agcontains(sg, e))
	    return FALSE;
    }
    return TRUE;
}

static boolean write_node_test(Agraph_t * g, Agnode_t * n)
{
    Agraph_t *sg;
    int c;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	sg = GD_clust(g)[c];
	if (agcontains(sg, n))
	    return FALSE;
    }
    return TRUE;
}

void emit_background(GVJ_t * job, graph_t *g)
{
    char *str;

    if (! ((str = agget(g, "bgcolor")) && str[0])) {
	if (gvrender_features(job) & GVRENDER_NO_BG)
	    str = "transparent";
	else
	    str = "white";
    }
    gvrender_set_fillcolor(job, str);
    gvrender_set_pencolor(job, str);
    gvrender_box(job, job->pageBox, TRUE);	/* filled */
}

static void setup_page(GVJ_t * job, graph_t * g)
{
    obj_state_t *obj = job->obj;
    int nump = 0, flags = job->flags;
    pointf sz, *p = NULL;
    box cbb;

    /* establish current box in graph units */
    job->pageBox.LL.x = job->pagesArrayElem.x * job->pageSize.x - job->pad.x;
    job->pageBox.LL.y = job->pagesArrayElem.y * job->pageSize.y - job->pad.y;
    job->pageBox.UR.x = job->pageBox.LL.x + job->pageSize.x;
    job->pageBox.UR.y = job->pageBox.LL.y + job->pageSize.y;

    /* establish pageOffset from graph origin, in graph units */
    if (job->rotation) {
	job->pageOffset.x = -job->pad.x + job->pageSize.y * (job->pagesArrayElem.y +1);
	job->pageOffset.y =  job->pad.y - job->pageSize.x * job->pagesArrayElem.x;
    }
    else {
	job->pageOffset.x =  job->pad.x - job->pageSize.x * job->pagesArrayElem.x;
	job->pageOffset.y =  job->pad.y - job->pageSize.y * job->pagesArrayElem.y;
    }

    /* calculate clip region in graph units using width/height since window
     * might have been resized since view was calculated */
    sz.x = job->width / (job->scale.x * 2.);
    sz.y = job->height / (job->scale.y * 2.);
    if (job->rotation)
        sz = exch_xyf(sz);

    job->clip.UR.x = job->focus.x + sz.x;
    job->clip.UR.y = job->focus.y + sz.y;
    job->clip.LL.x = job->focus.x - sz.x;
    job->clip.LL.y = job->focus.y - sz.y;

    /* clib box for this page in graph units */
    job->clip.UR.x = MAX(job->clip.UR.x, job->pageBox.UR.x);
    job->clip.UR.y = MAX(job->clip.UR.y, job->pageBox.UR.y);
    job->clip.LL.x = MIN(job->clip.LL.x, job->pageBox.LL.x);
    job->clip.LL.y = MIN(job->clip.LL.y, job->pageBox.LL.y);

    /* canvasBox in device units in graph orientation */
    cbb.LL.x = ROUND(job->canvasBox.LL.x * job->dpi.x / POINTS_PER_INCH);
    cbb.LL.y = ROUND(job->canvasBox.LL.y * job->dpi.y / POINTS_PER_INCH);
    cbb.UR.x = ROUND(job->canvasBox.UR.x * job->dpi.x / POINTS_PER_INCH);
    cbb.UR.y = ROUND(job->canvasBox.UR.y * job->dpi.y / POINTS_PER_INCH);

    /* pageBoundingBox in device units and page orientation */
    job->pageBoundingBox = cbb;
    if (job->rotation) {
	job->pageBoundingBox.LL = exch_xy(job->pageBoundingBox.LL);
	job->pageBoundingBox.UR = exch_xy(job->pageBoundingBox.UR);
    }
	
    /* maximum boundingBox in device units and page orientation */
    if (job->common->viewNum == 0)
        job->boundingBox = job->pageBoundingBox;
    else
        EXPANDBB(job->boundingBox, job->pageBoundingBox);

    /* CAUTION - This block was difficult to get right. */
    /* Test with and without assymetric margins, e.g: -Gmargin="1,0" */
    if (job->rotation) {
	job->translation.y = -job->pageBox.UR.y - cbb.LL.y / job->scale.y;
	if ((job->flags & GVRENDER_Y_GOES_DOWN) || (Y_invert)) {
	    /* test with: -Glandscape -Tgif -Tsvg -Tpng */
	    job->translation.x = -job->pageBox.UR.x - cbb.LL.x / job->scale.x;
	}
	else {
	    /* test with: -Glandscape -Tps */
	    job->translation.x = -job->pageBox.LL.x + cbb.LL.x / job->scale.x;
	}
    }
    else {
	job->translation.x = -job->pageBox.LL.x + cbb.LL.x / job->scale.x;
	if ((job->flags & GVRENDER_Y_GOES_DOWN) || (Y_invert)) {
	    /* test with: -Tgif -Tsvg -Tpng */
	    job->translation.y = -job->pageBox.UR.y - cbb.LL.y / job->scale.y;
	}
	else {
	    /* test with: -Tps */
	    job->translation.y = -job->pageBox.LL.y + cbb.LL.y / job->scale.y;
	}
    }

    job->translation.x -= job->focus.x + job->pad.x - job->view.x / (job->zoom * 2.0);
    job->translation.y -= job->focus.y + job->pad.y - job->view.y / (job->zoom * 2.0);

    if ((flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS))
            && (obj->url || obj->explicit_tooltip)) {
        if (flags & (GVRENDER_DOES_MAP_RECTANGLE | GVRENDER_DOES_MAP_POLYGON)) {
            if (flags & GVRENDER_DOES_MAP_RECTANGLE) {
                obj->url_map_shape = MAP_RECTANGLE;
                nump = 2;
            }
            else {
                obj->url_map_shape = MAP_POLYGON;
                nump = 4;
            }
            p = N_NEW(nump, pointf);
            p[0] = job->pageBox.LL;
            p[1] = job->pageBox.UR;
            if (! (flags & (GVRENDER_DOES_MAP_RECTANGLE)))
                rect2poly(p);
        }
        if (! (flags & GVRENDER_DOES_TRANSFORM))
            gvrender_ptf_A(job, p, p, nump);
        obj->url_map_p = p;
        obj->url_map_n = nump;
    }
}

static boolean is_natural_number(char *sstr)
{
    unsigned char *str = (unsigned char *) sstr;

    while (*str)
	if (NOT(isdigit(*str++)))
	    return FALSE;
    return TRUE;
}

static int layer_index(GVC_t *gvc, char *str, int all)
{
    GVJ_t *job = gvc->job;
    int i;

    if (streq(str, "all"))
	return all;
    if (is_natural_number(str))
	return atoi(str);
    if (gvc->layerIDs)
	for (i = 1; i <= job->numLayers; i++)
	    if (streq(str, gvc->layerIDs[i]))
		return i;
    return -1;
}

static boolean selectedlayer(GVJ_t *job, char *spec)
{
    GVC_t *gvc = job->gvc;
    int n0, n1;
    unsigned char buf[SMALLBUF];
    char *w0, *w1;
    agxbuf xb;
    boolean rval = FALSE;

    agxbinit(&xb, SMALLBUF, buf);
    agxbput(&xb, spec);
    w1 = w0 = strtok(agxbuse(&xb), gvc->layerDelims);
    if (w0)
	w1 = strtok(NULL, gvc->layerDelims);
    switch ((w0 != NULL) + (w1 != NULL)) {
    case 0:
	rval = FALSE;
	break;
    case 1:
	n0 = layer_index(gvc, w0, job->layerNum);
	rval = (n0 == job->layerNum);
	break;
    case 2:
	n0 = layer_index(gvc, w0, 0);
	n1 = layer_index(gvc, w1, job->numLayers);
	if ((n0 < 0) || (n1 < 0))
	    rval = TRUE;
	else if (n0 > n1) {
	    int t = n0;
	    n0 = n1;
	    n1 = t;
	}
	rval = BETWEEN(n0, job->layerNum, n1);
	break;
    }
    agxbfree(&xb);
    return rval;
}

static boolean node_in_layer(GVJ_t *job, graph_t * g, node_t * n)
{
    char *pn, *pe;
    edge_t *e;

    if (job->numLayers <= 1)
	return TRUE;
    pn = late_string(n, N_layer, "");
    if (selectedlayer(job, pn))
	return TRUE;
    if (pn[0])
	return FALSE;		/* Only check edges if pn = "" */
    if ((e = agfstedge(g, n)) == NULL)
	return TRUE;
    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	pe = late_string(e, E_layer, "");
	if ((pe[0] == '\0') || selectedlayer(job, pe))
	    return TRUE;
    }
    return FALSE;
}

static boolean edge_in_layer(GVJ_t *job, graph_t * g, edge_t * e)
{
    char *pe, *pn;
    int cnt;

    if (job->numLayers <= 1)
	return TRUE;
    pe = late_string(e, E_layer, "");
    if (selectedlayer(job, pe))
	return TRUE;
    if (pe[0])
	return FALSE;
    for (cnt = 0; cnt < 2; cnt++) {
	pn = late_string(cnt < 1 ? e->tail : e->head, N_layer, "");
	if ((pn[0] == '\0') || selectedlayer(job, pn))
	    return TRUE;
    }
    return FALSE;
}

static boolean clust_in_layer(GVJ_t *job, graph_t * sg)
{
    char *pg;
    node_t *n;

    if (job->numLayers <= 1)
	return TRUE;
    pg = late_string(sg, agfindattr(sg, "layer"), "");
    if (selectedlayer(job, pg))
	return TRUE;
    if (pg[0])
	return FALSE;
    for (n = agfstnode(sg); n; n = agnxtnode(sg, n))
	if (node_in_layer(job, sg, n))
	    return TRUE;
    return FALSE;
}

static boolean node_in_box(node_t *n, boxf b)
{
    return boxf_overlap(ND_bb(n), b);
}

static void emit_begin_node(GVJ_t * job, node_t * n)
{
    obj_state_t *obj;
    int flags = job->flags;
    int sides, peripheries, i, j, filled = 0, rect = 0, shape, nump = 0;
    polygon_t *poly = NULL;
    pointf *vertices, ldimen, *p = NULL;
    point coord;
    char *s;

    obj = push_obj_state(job);
    obj->type = NODE_OBJTYPE;
    obj->u.n = n;
    obj->emit_state = EMIT_NDRAW;

    if (flags & GVRENDER_DOES_Z) {
        obj->z = late_double(n, N_z, 0.0, -MAXFLOAT);
    }
    initObjMapData (job, ND_label(n), n);
    if ((flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS))
           && (obj->url || obj->explicit_tooltip)) {

        /* checking shape of node */
        shape = shapeOf(n);
        /* node coordinate */
        coord = ND_coord_i(n);
        /* checking if filled style has been set for node */
        filled = ifFilled(n);

        if (shape == SH_POLY || shape == SH_POINT) {
            poly = (polygon_t *) ND_shape_info(n);

            /* checking if polygon is regular rectangle */
            if (isRect(poly) && (poly->peripheries || filled))
                rect = 1;
        }

        /* When node has polygon shape and requested output supports polygons
         * we use a polygon to map the clickable region that is a:
         * circle, ellipse, polygon with n side, or point.
         * For regular rectangular shape we have use node's bounding box to map clickable region
         */
        if (poly && !rect && (flags & GVRENDER_DOES_MAP_POLYGON)) {

            if (poly->sides < 3)
                sides = 1;
            else
                sides = poly->sides;

            if (poly->peripheries < 2)
                peripheries = 1;
            else
                peripheries = poly->peripheries;

            vertices = poly->vertices;

            if ((s = agget(n, "samplepoints")))
                nump = atoi(s);
            /* We want at least 4 points. For server-side maps, at most 100
             * points are allowed. To simplify things to fit with the 120 points
             * used for skewed ellipses, we set the bound at 60.
             */
            if ((nump < 4) || (nump > 60))
                nump = DFLT_SAMPLE;
            /* use bounding box of text label for mapping
             * when polygon has no peripheries and node is not filled
             */
            if (poly->peripheries == 0 && !filled) {
                obj->url_map_shape = MAP_RECTANGLE;
                nump = 2;
                p = N_NEW(nump, pointf);
                ldimen = ND_label(n)->dimen;
                P2RECT(coord, p, ldimen.x / 2.0, ldimen.y / 2.0);
            }
            /* circle or ellipse */
            else if (poly->sides < 3 && poly->skew == 0.0 && poly->distortion == 0.0) {
                if (poly->regular) {
                    obj->url_map_shape = MAP_CIRCLE;
                    nump = 2;              /* center of circle and top right corner of bb */
                    p = N_NEW(nump, pointf);
                    p[0].x = coord.x;
                    p[0].y = coord.y;
                    p[1].x = coord.x + vertices[peripheries - 1].x;
                    p[1].y = coord.y + vertices[peripheries - 1].y;
                }
                else { /* ellipse is treated as polygon */
                    obj->url_map_shape= MAP_POLYGON;
                    p = pEllipse((double)(vertices[peripheries - 1].x),
                                 (double)(vertices[peripheries - 1].y), nump);
                    for (i = 0; i < nump; i++) {
                        p[i].x += coord.x;
                        p[i].y += coord.y;
                    }
                }
            }
            /* all other polygonal shape */
            else {
                int offset = (peripheries - 1)*(poly->sides);
                obj->url_map_shape = MAP_POLYGON;
                /* distorted or skewed ellipses and circles are polygons with 120
                 * sides. For mapping we convert them into polygon with sample sides
                 */
                if (poly->sides >= nump) {
                    int delta = poly->sides / nump;
                    p = N_NEW(nump, pointf);
                    for (i = 0, j = 0; j < nump; i += delta, j++) {
                        p[j].x = coord.x + vertices[i + offset].x;
                        p[j].y = coord.y + vertices[i + offset].y;
                    }
                } else {
                    nump = sides;
                    p = N_NEW(nump, pointf);
                    for (i = 0; i < nump; i++) {
                        p[i].x = coord.x + vertices[i + offset].x;
                        p[i].y = coord.y + vertices[i + offset].y;
                    }
                }
            }
        }
        else {
            /* we have to use the node's bounding box to map clickable region
             * when requested output format is not capable of polygons.
             */
            obj->url_map_shape = MAP_RECTANGLE;
            nump = 2;
            p = N_NEW(nump, pointf);
            p[0].x = coord.x - ND_lw_i(n);
            p[0].y = coord.y - (ND_ht_i(n) / 2);
            p[1].x = coord.x + ND_rw_i(n);
            p[1].y = coord.y + (ND_ht_i(n) / 2);
        }
        if (! (flags & GVRENDER_DOES_TRANSFORM))
            gvrender_ptf_A(job, p, p, nump);
        obj->url_map_p = p;
        obj->url_map_n = nump;
    }

#ifdef WITH_CODEGENS
    Obj = NODE;
#endif
    setColorScheme (agget (n, "colorscheme"));
    gvrender_begin_context(job);
    gvrender_begin_node(job, n);
}

static void emit_end_node(GVJ_t * job)
{
    gvrender_end_node(job);
    gvrender_end_context(job);
#ifdef WITH_CODEGENS
    Obj = NONE;
#endif
    pop_obj_state(job);
}

static void emit_node(GVJ_t * job, node_t * n)
{
    GVC_t *gvc = job->gvc;
    char *s;

    if (ND_shape(n) 				     /* node has a shape */
	    && node_in_layer(job, n->graph, n)       /* and is in layer */
	    && node_in_box(n, job->clip)             /* and is in page/view */
	    && (ND_state(n) != gvc->common.viewNum)) /* and not already drawn */
    {
	ND_state(n) = gvc->common.viewNum; 	     /* mark node as drawn */

        gvrender_comment(job, n->name);
	s = late_string(n, N_comment, "");
	if (s[0])
	    gvrender_comment(job, s);
        
	emit_begin_node(job, n);
	ND_shape(n)->fns->codefn(job, n);
	emit_end_node(job);
    }
}

/* calculate an offset vector, length d, perpendicular to line p,q */
static pointf computeoffset_p(pointf p, pointf q, double d)
{
    pointf res;
    double x = p.x - q.x, y = p.y - q.y;

    /* keep d finite as line length approaches 0 */
    d /= sqrt(x * x + y * y + EPSILON);
    res.x = y * d;
    res.y = -x * d;
    return res;
}

/* calculate offset vector, length d, perpendicular to spline p,q,r,s at q&r */
static pointf computeoffset_qr(pointf p, pointf q, pointf r, pointf s,
			       double d)
{
    pointf res;
    double len;
    double x = q.x - r.x, y = q.y - r.y;

    len = sqrt(x * x + y * y);
    if (len < EPSILON) {
	/* control points are on top of each other
	   use slope between endpoints instead */
	x = p.x - s.x, y = p.y - s.y;
	/* keep d finite as line length approaches 0 */
	len = sqrt(x * x + y * y + EPSILON);
    }
    d /= len;
    res.x = y * d;
    res.y = -x * d;
    return res;
}

static void emit_attachment(GVJ_t * job, textlabel_t * lp, splines * spl)
{
    pointf sz, AF[3];
    point p;
    unsigned char *s;

    for (s = (unsigned char *) (lp->text); *s; s++) {
	if (isspace(*s) == FALSE)
	    break;
    }
    if (*s == 0)
	return;

    sz = lp->dimen;
    AF[0] = pointfof((double)(lp->p.x) + sz.x / 2., (double)(lp->p.y) - sz.y / 2.);
    AF[1] = pointfof(AF[0].x - sz.x, AF[0].y);
    p = dotneato_closest(spl, lp->p);
    P2PF(p,AF[2]);
    /* Don't use edge style to draw attachment */
    gvrender_set_style(job, job->gvc->defaultlinestyle);
    /* Use font color to draw attachment
       - need something unambiguous in case of multicolored parallel edges
       - defaults to black for html-like labels
     */
    gvrender_set_pencolor(job, lp->fontcolor);
    gvrender_polyline(job, AF, 3);
}

/* edges colors can be mutiple colors separated by ":"
 * so we commpute a default pencolor with the same number of colors. */
static char* default_pencolor(char *pencolor, char *deflt)
{
    static char *buf;
    static int bufsz;
    char *p;
    int len, ncol;

    ncol = 1;
    for (p = pencolor; *p; p++) {
	if (*p == ':')
	    ncol++;
    }
    len = ncol * (strlen(deflt) + 1);
    if (bufsz < len) {
	bufsz = len + 10;
	buf = realloc(buf, bufsz);
    }
    strcpy(buf, deflt);
    while(--ncol) {
	strcat(buf, ":");
	strcat(buf, deflt);
    }
    return buf;
}

static void emit_edge_graphics(GVJ_t * job, edge_t * e)
{
    int i, j, cnum, numc = 0;
    char *color, *pencolor, *fillcolor, *style;
    char *headcolor, *tailcolor, *lastcolor;
    char *colors = NULL;
    char **styles = 0;
    char **sp;
    bezier bz = { 0, 0, 0, 0 };
    bezierf bzf;
    splinesf offspl, tmpspl;
    pointf pf0, pf1, pf2 = { 0, 0 }, pf3, *offlist, *tmplist;
    double scale, numc2;
    double penwidth;
    char *p;

#define SEP 2.0

    style = late_string(e, E_style, "");
    /* We shortcircuit drawing an invisible edge because the arrowhead
     * code resets the style to solid, and most of the code generators
     * (except PostScript) won't honor a previous style of invis.
     */
    if (style[0]) {
	styles = parse_style(style);
	sp = styles;
	while ((p = *sp++)) {
	    if (streq(p, "invis"))
		return;
	}
    }
    setColorScheme (agget (e, "colorscheme"));
    if (ED_spl(e)) {
	scale = late_double(e, E_arrowsz, 1.0, 0.0);
	color = late_string(e, E_color, "");

	if (styles) gvrender_set_style(job, styles);

	/* need to know how many colors separated by ':' */
	for (p = color; *p; p++)
	    if (*p == ':')
		numc++;

	fillcolor = pencolor = color;
	if (ED_gui_state(e) & GUI_STATE_ACTIVE) {
	    pencolor = late_nnstring(e, E_activepencolor,
			default_pencolor(pencolor, DEFAULT_ACTIVEPENCOLOR));
	    fillcolor = late_nnstring(e, E_activefillcolor, DEFAULT_ACTIVEFILLCOLOR);
	}
	else if (ED_gui_state(e) & GUI_STATE_SELECTED) {
	    pencolor = late_nnstring(e, E_selectedpencolor,
			default_pencolor(pencolor, DEFAULT_SELECTEDPENCOLOR));
	    fillcolor = late_nnstring(e, E_selectedfillcolor, DEFAULT_SELECTEDFILLCOLOR);
	}
	else if (ED_gui_state(e) & GUI_STATE_DELETED) {
	    pencolor = late_nnstring(e, E_deletedpencolor,
			default_pencolor(pencolor, DEFAULT_DELETEDPENCOLOR));
	    fillcolor = late_nnstring(e, E_deletedfillcolor, DEFAULT_DELETEDFILLCOLOR);
	}
	else if (ED_gui_state(e) & GUI_STATE_VISITED) {
	    pencolor = late_nnstring(e, E_visitedpencolor,
			default_pencolor(pencolor, DEFAULT_VISITEDPENCOLOR));
	    fillcolor = late_nnstring(e, E_visitedfillcolor, DEFAULT_VISITEDFILLCOLOR);
	}
	if (E_penwidth && ((p=agxget(e,E_penwidth->index)) && p[0])) {
	    penwidth = late_double(e, E_penwidth, 1.0, 0.0);
	    gvrender_set_penwidth(job, penwidth);
	}
	if (pencolor != color)
    	    gvrender_set_pencolor(job, pencolor);
	if (fillcolor != color)
	    gvrender_set_fillcolor(job, fillcolor);
	color = pencolor;
	/* if more than one color - then generate parallel beziers, one per color */
	if (numc) {
	    /* calculate and save offset vector spline and initialize first offset spline */
	    tmpspl.size = offspl.size = ED_spl(e)->size;
	    offspl.list = malloc(sizeof(bezier) * offspl.size);
	    tmpspl.list = malloc(sizeof(bezier) * tmpspl.size);
	    numc2 = (2 + numc) / 2.0;
	    for (i = 0; i < offspl.size; i++) {
		bz = ED_spl(e)->list[i];
		tmpspl.list[i].size = offspl.list[i].size = bz.size;
		offlist = offspl.list[i].list =
		    malloc(sizeof(pointf) * bz.size);
		tmplist = tmpspl.list[i].list =
		    malloc(sizeof(pointf) * bz.size);
		P2PF(bz.list[0], pf3);
		for (j = 0; j < bz.size - 1; j += 3) {
		    pf0 = pf3;
		    P2PF(bz.list[j + 1], pf1);
		    /* calculate perpendicular vectors for each bezier point */
		    if (j == 0)	/* first segment, no previous pf2 */
			offlist[j] = computeoffset_p(pf0, pf1, SEP);
		    else	/* i.e. pf2 is available from previous segment */
			offlist[j] = computeoffset_p(pf2, pf1, SEP);

		    P2PF(bz.list[j + 2], pf2);
		    P2PF(bz.list[j + 3], pf3);
		    offlist[j + 1] = offlist[j + 2] =
			computeoffset_qr(pf0, pf1, pf2, pf3, SEP);
		    /* initialize tmpspl to outermost position */
		    tmplist[j].x = pf0.x - numc2 * offlist[j].x;
		    tmplist[j].y = pf0.y - numc2 * offlist[j].y;
		    tmplist[j + 1].x = pf1.x - numc2 * offlist[j + 1].x;
		    tmplist[j + 1].y = pf1.y - numc2 * offlist[j + 1].y;
		    tmplist[j + 2].x = pf2.x - numc2 * offlist[j + 2].x;
		    tmplist[j + 2].y = pf2.y - numc2 * offlist[j + 2].y;
		}
		/* last segment, no next pf1 */
		offlist[j] = computeoffset_p(pf2, pf3, SEP);
		tmplist[j].x = pf3.x - numc2 * offlist[j].x;
		tmplist[j].y = pf3.y - numc2 * offlist[j].y;
	    }
	    lastcolor = headcolor = tailcolor = color;
	    colors = strdup(color);
	    for (cnum = 0, color = strtok(colors, ":"); color;
		cnum++, color = strtok(0, ":")) {
		if (!color[0])
		    color = DEFAULT_COLOR;
		if (color != lastcolor) {
	            if (! (ED_gui_state(e) & (GUI_STATE_ACTIVE | GUI_STATE_SELECTED))) {
		        gvrender_set_pencolor(job, color);
		        gvrender_set_fillcolor(job, color);
		    }
		    lastcolor = color;
		}
		if (cnum == 0)
		    headcolor = tailcolor = color;
		if (cnum == 1)
		    tailcolor = color;
		for (i = 0; i < tmpspl.size; i++) {
		    tmplist = tmpspl.list[i].list;
		    offlist = offspl.list[i].list;
		    for (j = 0; j < tmpspl.list[i].size; j++) {
			tmplist[j].x += offlist[j].x;
			tmplist[j].y += offlist[j].y;
		    }
		    gvrender_beziercurve(job, tmplist, tmpspl.list[i].size,
					 FALSE, FALSE, FALSE);
		}
	    }
	    if (bz.sflag) {
		if (color != tailcolor) {
		    color = tailcolor;
	            if (! (ED_gui_state(e) & (GUI_STATE_ACTIVE | GUI_STATE_SELECTED))) {
		        gvrender_set_pencolor(job, color);
		        gvrender_set_fillcolor(job, color);
		    }
		}
		arrow_gen(job, EMIT_TDRAW, bz.sp, bz.list[0],
			scale, bz.sflag);
	    }
	    if (bz.eflag) {
		if (color != headcolor) {
		    color = headcolor;
	            if (! (ED_gui_state(e) & (GUI_STATE_ACTIVE | GUI_STATE_SELECTED))) {
		        gvrender_set_pencolor(job, color);
		        gvrender_set_fillcolor(job, color);
		    }
		}
		arrow_gen(job, EMIT_HDRAW, bz.ep, bz.list[bz.size - 1],
			scale, bz.eflag);
	    }
	    free(colors);
	    for (i = 0; i < offspl.size; i++) {
		free(offspl.list[i].list);
		free(tmpspl.list[i].list);
	    }
	    free(offspl.list);
	    free(tmpspl.list);
	} else {
	    if (! (ED_gui_state(e) & (GUI_STATE_ACTIVE | GUI_STATE_SELECTED))) {
	        if (color[0]) {
		    gvrender_set_pencolor(job, color);
		    gvrender_set_fillcolor(job, color);
	        } else {
		    gvrender_set_pencolor(job, DEFAULT_COLOR);
		    gvrender_set_fillcolor(job, DEFAULT_COLOR);
	        }
	    }
	    for (i = 0; i < ED_spl(e)->size; i++) {
		bz = ED_spl(e)->list[i];
		/* convert points to pointf for gvrender api */
		bzf.size = bz.size;
		bzf.list = malloc(sizeof(pointf) * bzf.size);
		for (j = 0; j < bz.size; j++)
		    P2PF(bz.list[j], bzf.list[j]);
		if (gvrender_features(job) & GVRENDER_DOES_ARROWS) {
		    gvrender_beziercurve(job, bzf.list, bz.size, bz.sflag,
					 bz.eflag, FALSE);
		} else {
		    gvrender_beziercurve(job, bzf.list, bz.size, FALSE,
					 FALSE, FALSE);
		    if (bz.sflag) {
			arrow_gen(job, EMIT_TDRAW, bz.sp, bz.list[0],
				scale, bz.sflag);
		    }
		    if (bz.eflag) {
			arrow_gen(job, EMIT_HDRAW, bz.ep, bz.list[bz.size - 1],
				scale, bz.eflag);
		    }
                    /* arrow_gen resets the job style 
                     * If we have more splines to do, restore the old one.
                     */
		    if ((ED_spl(e)->size>1) && (bz.sflag||bz.eflag) && styles) 
			gvrender_set_style(job, styles);
		}
		free(bzf.list);
	    }
	}
    }
}

static boolean edge_in_box(edge_t *e, boxf b)
{
    splines *spl;
    textlabel_t *lp;

    spl = ED_spl(e);
    if (spl && boxf_overlap(spl->bb, b))
        return TRUE;

    lp = ED_label(e);
    if (lp && overlap_label(lp, b))
        return TRUE;

    return FALSE;
}

void emit_begin_edge(GVJ_t * job, edge_t * e)
{
    obj_state_t *obj;
    int flags = job->flags;
    char *s;
    textlabel_t *lab = NULL, *tlab = NULL, *hlab = NULL;
    pointf *pbs = NULL;
    int	i, nump, *pbs_n = NULL, pbs_poly_n = 0;
    char* dflt_url = NULL;
    char* dflt_target = NULL;

    obj = push_obj_state(job);
    obj->type = EDGE_OBJTYPE;
    obj->u.e = e;
    obj->emit_state = EMIT_EDRAW;

    if (flags & GVRENDER_DOES_Z) {
        obj->tail_z= late_double(e->tail, N_z, 0.0, -1000.0);
        obj->head_z= late_double(e->head, N_z, 0.0, -MAXFLOAT);
    }

    if (flags & GVRENDER_DOES_LABELS) {
	if ((lab = ED_label(e)))
	    obj->label = lab->text;
	obj->taillabel = obj->headlabel = obj->label;
	if ((tlab = ED_tail_label(e)))
	    obj->taillabel = tlab->text;
	if ((hlab = ED_head_label(e)))
	    obj->headlabel = hlab->text;
    }

    if (flags & GVRENDER_DOES_MAPS) {
        if (((s = agget(e, "href")) && s[0]) || ((s = agget(e, "URL")) && s[0]))
            dflt_url = strdup_and_subst_obj(s, (void*)e);
	if (((s = agget(e, "edgehref")) && s[0]) || ((s = agget(e, "edgeURL")) && s[0]))
            obj->url = strdup_and_subst_obj(s, (void*)e);
	else if (dflt_url)
	    obj->url = strdup(dflt_url);
	if (((s = agget(e, "labelhref")) && s[0]) || ((s = agget(e, "labelURL")) && s[0]))
            obj->labelurl = strdup_and_subst_obj(s, (void*)e);
	else if (dflt_url)
	    obj->labelurl = strdup(dflt_url);
	if (((s = agget(e, "tailhref")) && s[0]) || ((s = agget(e, "tailURL")) && s[0])) {
            obj->tailurl = strdup_and_subst_obj(s, (void*)e);
            obj->explicit_tailurl = TRUE;
	}
	else if (dflt_url)
	    obj->tailurl = strdup(dflt_url);
	if (((s = agget(e, "headhref")) && s[0]) || ((s = agget(e, "headURL")) && s[0])) {
            obj->headurl = strdup_and_subst_obj(s, (void*)e);
            obj->explicit_headurl = TRUE;
	}
	else if (dflt_url)
	    obj->headurl = strdup(dflt_url);
    } 

    if (flags & GVRENDER_DOES_TARGETS) {
        if ((s = agget(e, "target")) && s[0])
            dflt_target = strdup_and_subst_obj(s, (void*)e);
        if ((s = agget(e, "edgetarget")) && s[0]) {
	    obj->explicit_edgetarget = TRUE;
            obj->target = strdup_and_subst_obj(s, (void*)e);
	}
	else if (dflt_target)
	    obj->target = strdup(dflt_target);
        if ((s = agget(e, "labeltarget")) && s[0])
            obj->labeltarget = strdup_and_subst_obj(s, (void*)e);
	else if (dflt_target)
	    obj->labeltarget = strdup(dflt_target);
        if ((s = agget(e, "tailtarget")) && s[0]) {
            obj->tailtarget = strdup_and_subst_obj(s, (void*)e);
	    obj->explicit_tailtarget = TRUE;
	}
	else if (dflt_target)
	    obj->tailtarget = strdup(dflt_target);
        if ((s = agget(e, "headtarget")) && s[0]) {
	    obj->explicit_headtarget = TRUE;
            obj->headtarget = strdup_and_subst_obj(s, (void*)e);
	}
	else if (dflt_target)
	    obj->headtarget = strdup(dflt_target);
    } 

    if (flags & GVRENDER_DOES_TOOLTIPS) {
        if (((s = agget(e, "tooltip")) && s[0]) ||
            ((s = agget(e, "edgetooltip")) && s[0])) {
            obj->tooltip = strdup_and_subst_obj(s, (void*)e);
	    obj->explicit_tooltip = TRUE;
	}
	else if (obj->label)
	    obj->tooltip = strdup(obj->label);

        if ((s = agget(e, "labeltooltip")) && s[0]) {
            obj->labeltooltip = strdup_and_subst_obj(s, (void*)e);
	    obj->explicit_labeltooltip = TRUE;
	}
	else if (obj->label)
	    obj->labeltooltip = strdup(obj->label);

        if ((s = agget(e, "tailtooltip")) && s[0]) {
            obj->tailtooltip = strdup_and_subst_obj(s, (void*)e);
	    obj->explicit_tailtooltip = TRUE;
	}
	else if (obj->taillabel)
	    obj->tailtooltip = strdup(obj->taillabel);

        if ((s = agget(e, "headtooltip")) && s[0]) {
            obj->headtooltip = strdup_and_subst_obj(s, (void*)e);
	    obj->explicit_headtooltip = TRUE;
	}
	else if (obj->headlabel)
	    obj->headtooltip = strdup(obj->headlabel);
    } 
    
    free (dflt_url);
    free (dflt_target);

    if (flags & (GVRENDER_DOES_MAPS | GVRENDER_DOES_TOOLTIPS)) {
	if (ED_spl(e) && (obj->url || obj->tooltip) && (flags & GVRENDER_DOES_MAP_POLYGON)) {
	    int ns;
	    splines *spl;

	    spl = ED_spl(e);
	    ns = spl->size; /* number of splines */
	    for (i = 0; i < ns; i++)
		map_output_bspline (&pbs, &pbs_n, &pbs_poly_n, spl->list+i);
	    obj->url_bsplinemap_poly_n = pbs_poly_n;
	    obj->url_bsplinemap_n = pbs_n;
	    if (! (flags & GVRENDER_DOES_TRANSFORM)) {
    		for ( nump = 0, i = 0; i < pbs_poly_n; i++)
        	    nump += pbs_n[i];
		gvrender_ptf_A(job, pbs, pbs, nump);		
	    }
	    obj->url_bsplinemap_p = pbs;
	    obj->url_map_shape = MAP_POLYGON;
	    obj->url_map_p = pbs;
	    obj->url_map_n = pbs_n[0];
	}
    }

#ifdef WITH_CODEGENS
    Obj = EDGE;
#endif
    gvrender_begin_context(job);
    gvrender_begin_edge(job, e);
    if (obj->url || obj->explicit_tooltip)
	gvrender_begin_anchor(job, obj->url, obj->tooltip, obj->target);
}

static void
emit_edge_label(GVJ_t* job, textlabel_t* lbl, int lkind, int explicit,
    char* url, char* tooltip, char* target, splines* spl)
{
    int flags = job->flags;
    if (lbl == NULL) return;
    if ((url || explicit) && !(flags & EMIT_CLUSTERS_LAST)) {
	map_label(job, lbl);
	gvrender_begin_anchor(job, url, tooltip, target);
    }
    emit_label(job, lkind, lbl);
    if (spl) emit_attachment(job, lbl, spl);
    if (url || explicit) {
	if (flags & EMIT_CLUSTERS_LAST) {
	    map_label(job, lbl);
	    gvrender_begin_anchor(job, url, tooltip, target);
	}
	gvrender_end_anchor(job);
    }
}

/* nodeIntersect:
 * Common logic for setting hot spots at the beginning and end of 
 * an edge.
 * If we are given a value (url, tooltip, target) explicitly set for
 * the head/tail, we use that. 
 * Otherwise, if we are given a value explicitly set for the edge,
 * we use that.
 * Otherwise, we use whatever the argument value is.
 * We also note whether or not the tooltip was explicitly set.
 * If the url is non-NULL or the tooltip was explicit, we set
 * a hot spot around point p.
 */
static void
nodeIntersect (GVJ_t * job, point p, 
    boolean explicit_iurl, char* iurl,
    boolean explicit_itooltip, char* itooltip,
    boolean explicit_itarget, char* itarget)
{
    obj_state_t *obj = job->obj;
    char* url;
    char* tooltip;
    char* target;
    boolean explicit;

    if (explicit_iurl) url = iurl;
    else url = obj->url;
    if (explicit_itooltip) {
	tooltip = itooltip;
	explicit = TRUE;
    }
    else if (obj->explicit_tooltip) {
	tooltip = obj->tooltip;
	explicit = TRUE;
    }
    else {
	explicit = FALSE;
	tooltip = itooltip;
    }
    if (explicit_itarget)
	target = itarget;
    else if (obj->explicit_edgetarget)
	target = obj->target;
    else
	target = itarget;

    if (url || explicit) {
	map_point(job, p);
	gvrender_begin_anchor(job, url, tooltip, target);
	gvrender_end_anchor(job);
    }
}

void emit_end_edge(GVJ_t * job)
{
    obj_state_t *obj = job->obj;
    edge_t *e = obj->u.e;
    int i, nump;

    if (obj->url || obj->explicit_tooltip) {
	gvrender_end_anchor(job);
	if (obj->url_bsplinemap_poly_n) {
	    for ( nump = obj->url_bsplinemap_n[0], i = 1; i < obj->url_bsplinemap_poly_n; i++) {
		/* additional polygon maps around remaining bezier pieces */
		obj->url_map_n = obj->url_bsplinemap_n[i];
		obj->url_map_p = &(obj->url_bsplinemap_p[nump]);
		gvrender_begin_anchor(job, obj->url, obj->tooltip, obj->target);
		gvrender_end_anchor(job);
		nump += obj->url_bsplinemap_n[i];
	    }
	}
    }
    obj->url_map_n = 0;       /* null out copy so that it doesn't get freed twice */
    obj->url_map_p = NULL;

    if (ED_spl(e)) {
	point p;
	bezier bz;

	/* process intersection with tail node */
	bz = ED_spl(e)->list[0];
	if (bz.sflag) /* Arrow at start of splines */
	    p = bz.sp;
	else /* No arrow at start of splines */
	    p = bz.list[0];
	nodeIntersect (job, p, obj->explicit_tailurl, obj->tailurl,
	    obj->explicit_tailtooltip, obj->tailtooltip, 
	    obj->explicit_tailtarget, obj->tailtarget); 
        
	/* process intersection with head node */
	bz = ED_spl(e)->list[ED_spl(e)->size - 1];
	if (bz.eflag) /* Arrow at end of splines */
	    p = bz.ep;
	else /* No arrow at end of splines */
	    p = bz.list[bz.size - 1];
	nodeIntersect (job, p, obj->explicit_headurl, obj->headurl,
	    obj->explicit_headtooltip, obj->headtooltip, 
	    obj->explicit_headtarget, obj->headtarget); 
    }

    emit_edge_label(job, ED_label(e), EMIT_ELABEL, obj->explicit_labeltooltip, 
	obj->labelurl, obj->labeltooltip, obj->labeltarget, 
	((mapbool(late_string(e, E_decorate, "false")) && ED_spl(e)) ? ED_spl(e) : 0));
    emit_edge_label(job, ED_head_label(e), EMIT_HLABEL, 
	obj->explicit_headtooltip, obj->headurl, obj->headtooltip, obj->headtarget, 0);
    emit_edge_label(job, ED_tail_label(e), EMIT_TLABEL, 
	obj->explicit_tailtooltip, obj->tailurl, obj->tailtooltip, obj->tailtarget, 0);

    gvrender_end_edge(job);
    gvrender_end_context(job);
#ifdef WITH_CODEGENS
    Obj = NONE;
#endif
    pop_obj_state(job);
}

static void emit_edge(GVJ_t * job, edge_t * e)
{
    char *s;

    if (edge_in_box(e, job->clip) && edge_in_layer(job, e->head->graph, e) ) {

	s = malloc(strlen(e->tail->name) + 2 + strlen(e->head->name) + 1);
	strcpy(s,e->tail->name);
	if (AG_IS_DIRECTED(e->tail->graph))
	    strcat(s,"->");
	else
	    strcat(s,"--");
	strcat(s,e->head->name);
	gvrender_comment(job, s);
	free(s);

	s = late_string(e, E_comment, "");
	if (s[0])
	    gvrender_comment(job, s);

	emit_begin_edge(job, e);
	emit_edge_graphics (job, e);
	emit_end_edge(job);
    }
}

static void init_gvc(GVC_t * gvc, graph_t * g)
{
    double xf, yf;
    char *p;
    int i;

    gvc->g = g;

    /* margins */
    gvc->graph_sets_margin = FALSE;
    if ((p = agget(g, "margin"))) {
        i = sscanf(p, "%lf,%lf", &xf, &yf);
        if (i > 0) {
            gvc->margin.x = gvc->margin.y = xf * POINTS_PER_INCH;
            if (i > 1)
                gvc->margin.y = yf * POINTS_PER_INCH;
            gvc->graph_sets_margin = TRUE;
        }
    }

    /* pad */
    gvc->graph_sets_pad = FALSE;
    if ((p = agget(g, "pad"))) {
        i = sscanf(p, "%lf,%lf", &xf, &yf);
        if (i > 0) {
            gvc->pad.x = gvc->pad.y = xf * POINTS_PER_INCH;
            if (i > 1)
                gvc->pad.y = yf * POINTS_PER_INCH;
            gvc->graph_sets_pad = TRUE;
        }
    }

    /* pagesize */
    gvc->graph_sets_pageSize = FALSE;
    P2PF(GD_drawing(g)->page, gvc->pageSize);
    if ((GD_drawing(g)->page.x > 0) && (GD_drawing(g)->page.y > 0))
        gvc->graph_sets_pageSize = TRUE;

    /* rotation */
    if (GD_drawing(g)->landscape)
	gvc->rotation = 90;
    else 
	gvc->rotation = 0;

    /* pagedir */
    gvc->pagedir = "BL";
    if ((p = agget(g, "pagedir")) && p[0])
            gvc->pagedir = p;

    /* bounding box */
    B2BF(GD_bb(g),gvc->bb);

    /* clusters have peripheries */
    G_peripheries = agfindattr(g, "peripheries");

    /* default font */
    gvc->defaultfontname = late_nnstring(g->proto->n,
                N_fontname, DEFAULT_FONTNAME);
    gvc->defaultfontsize = late_double(g->proto->n,
                N_fontsize, DEFAULT_FONTSIZE, MIN_FONTSIZE);

    /* default line style */
    gvc->defaultlinestyle = defaultlinestyle;

    gvc->graphname = g->name;
}

static void init_job_pad(GVJ_t *job)
{
    GVC_t *gvc = job->gvc;
    
    if (gvc->graph_sets_pad) {
	job->pad = gvc->pad;
    }
    else {
	switch (job->output_lang) {
	case GVRENDER_PLUGIN:
	    job->pad.x = job->pad.y = job->render.features->default_pad;
	    break;
	default:
	    job->pad.x = job->pad.y = DEFAULT_GRAPH_PAD;
	    break;
	}
    }
}

static void init_job_margin(GVJ_t *job)
{
    GVC_t *gvc = job->gvc;
    
    if (gvc->graph_sets_margin) {
	job->margin = gvc->margin;
    }
    else {
        /* set default margins depending on format */
        switch (job->output_lang) {
        case GVRENDER_PLUGIN:
            job->margin = job->device.features->default_margin;
            break;
        case HPGL: case PCL: case MIF: case METAPOST: case VTX: case QPDF:
            job->margin.x = job->margin.y = DEFAULT_PRINT_MARGIN;
            break;
        default:
            job->margin.x = job->margin.y = DEFAULT_EMBED_MARGIN;
            break;
        }
    }

}

static void init_job_dpi(GVJ_t *job, graph_t *g)
{
    GVJ_t *firstjob = job->gvc->active_jobs;

    if (GD_drawing(g)->dpi != 0) {
        job->dpi.x = job->dpi.y = (double)(GD_drawing(g)->dpi);
    }
    else if (firstjob->device_sets_dpi) {
        job->dpi = firstjob->device_dpi;   /* some devices set dpi in initialize() */
    }
    else {
        /* set default margins depending on format */
        switch (job->output_lang) {
        case GVRENDER_PLUGIN:
            job->dpi = job->device.features->default_dpi;
            break;
        default:
            job->dpi.x = job->dpi.y = (double)(DEFAULT_DPI);
            break;
        }
    }
}

static void init_job_viewport(GVJ_t * job, graph_t * g)
{
    GVC_t *gvc = job->gvc;
    pointf UR, size, sz;
    char *str;
    double X, Y, Z, x, y;
    int rv;

    assert((gvc->bb.LL.x == 0) && (gvc->bb.LL.y == 0));
    P2PF(gvc->bb.UR, UR);

    job->bb.LL.x = -job->pad.x;           /* job->bb is bb of graph and padding - graph units */
    job->bb.LL.y = -job->pad.y;
    job->bb.UR.x = UR.x + job->pad.x;
    job->bb.UR.y = UR.y + job->pad.y;
    sz.x = job->bb.UR.x - job->bb.LL.x;   /* size, including padding - graph units */
    sz.y = job->bb.UR.y - job->bb.LL.y;

    /* determine final drawing size and scale to apply. */
    /* N.B. size given by user is not rotated by landscape mode */
    /* start with "natural" size of layout */

    Z = 1.0;
    if (GD_drawing(g)->size.x > 0) {	/* graph size was given by user... */
	P2PF(GD_drawing(g)->size, size);
	if ((size.x < sz.x) || (size.y < sz.y) /* drawing is too big... */
	    || ((GD_drawing(g)->filled) /* or ratio=filled requested and ... */
		&& (size.x > sz.x) && (size.y > sz.y))) /* drawing is too small... */
	    Z = MIN(size.x/sz.x, size.y/sz.y);
    }
    
    /* default focus, in graph units = center of bb */
    x = UR.x / 2.;
    y = UR.y / 2.;

    /* rotate and scale bb to give default absolute size in points*/
    job->rotation = job->gvc->rotation;
    X = sz.x * Z;
    Y = sz.y * Z;

    /* user can override */
    if ((str = agget(g, "viewport")))
	rv = sscanf(str, "%lf,%lf,%lf,%lf,%lf", &X, &Y, &Z, &x, &y);
    /* rv is ignored since args retain previous values if not scanned */

 	/* job->view gives port size in graph units, unscaled or rotated
 	 * job->zoom gives scaling factor.
 	 * job->focus gives the position in the graph of the center of the port
	 */
    job->view.x = X;
    job->view.y = Y;
    job->zoom = Z;              /* scaling factor */
    job->focus.x = x;
    job->focus.y = y;
}

static void emit_colors(GVJ_t * job, graph_t * g)
{
    graph_t *sg;
    node_t *n;
    edge_t *e;
    int c;
    char *str, *colors;

    gvrender_set_fillcolor(job, DEFAULT_FILL);
    if (((str = agget(g, "bgcolor")) != 0) && str[0])
	gvrender_set_fillcolor(job, str);
    if (((str = agget(g, "fontcolor")) != 0) && str[0])
	gvrender_set_pencolor(job, str);
    for (c = 1; c <= GD_n_cluster(g); c++) {
	sg = GD_clust(g)[c];
	if (((str = agget(sg, "color")) != 0) && str[0])
	    gvrender_set_pencolor(job, str);
	if (((str = agget(sg, "fillcolor")) != 0) && str[0])
	    gvrender_set_fillcolor(job, str);
	if (((str = agget(sg, "fontcolor")) != 0) && str[0])
	    gvrender_set_pencolor(job, str);
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (((str = agget(n, "color")) != 0) && str[0])
	    gvrender_set_pencolor(job, str);
	if (((str = agget(n, "fillcolor")) != 0) && str[0])
	    gvrender_set_fillcolor(job, str);
	if (((str = agget(n, "fontcolor")) != 0) && str[0])
	    gvrender_set_pencolor(job, str);
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (((str = agget(e, "color")) != 0) && str[0]) {
		if (strchr(str, ':')) {
		    colors = strdup(str);
		    for (str = strtok(colors, ":"); str;
			str = strtok(0, ":")) {
			if (str[0])
			    gvrender_set_pencolor(job, str);
		    }
		    free(colors);
		} else
		    gvrender_set_pencolor(job, str);
	    }
	    if (((str = agget(e, "fontcolor")) != 0) && str[0])
		gvrender_set_pencolor(job, str);
	}
    }
}

static void emit_view(GVJ_t * job, graph_t * g, int flags)
{
    GVC_t * gvc = job->gvc;
    node_t *n;
    edge_t *e;

    gvc->common.viewNum++;
    /* when drawing, lay clusters down before nodes and edges */
    if (!(flags & EMIT_CLUSTERS_LAST))
	emit_clusters(job, g, flags);
    if (flags & EMIT_SORTED) {
	/* output all nodes, then all edges */
	gvrender_begin_nodes(job);
	for (n = agfstnode(g); n; n = agnxtnode(g, n))
	    emit_node(job, n);
	gvrender_end_nodes(job);
	gvrender_begin_edges(job);
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    for (e = agfstout(g, n); e; e = agnxtout(g, e))
		emit_edge(job, e);
	}
	gvrender_end_edges(job);
    } else if (flags & EMIT_EDGE_SORTED) {
	/* output all edges, then all nodes */
	gvrender_begin_edges(job);
	for (n = agfstnode(g); n; n = agnxtnode(g, n))
	    for (e = agfstout(g, n); e; e = agnxtout(g, e))
		emit_edge(job, e);
	gvrender_end_edges(job);
	gvrender_begin_nodes(job);
	for (n = agfstnode(g); n; n = agnxtnode(g, n))
	    emit_node(job, n);
	gvrender_end_nodes(job);
    } else if (flags & EMIT_PREORDER) {
	gvrender_begin_nodes(job);
	for (n = agfstnode(g); n; n = agnxtnode(g, n))
	    if (write_node_test(g, n))
		emit_node(job, n);
	gvrender_end_nodes(job);
	gvrender_begin_edges(job);

	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		if (write_edge_test(g, e))
		    emit_edge(job, e);
	    }
	}
	gvrender_end_edges(job);
    } else {
	/* output in breadth first graph walk order */
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    emit_node(job, n);
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		emit_node(job, e->head);
		emit_edge(job, e);
	    }
	}
    }
    /* when mapping, detect events on clusters after nodes and edges */
    if (flags & EMIT_CLUSTERS_LAST)
	emit_clusters(job, g, flags);
}

static void emit_begin_graph(GVJ_t * job, graph_t * g)
{
    obj_state_t *obj;

    obj = push_obj_state(job);
    obj->type = ROOTGRAPH_OBJTYPE;
    obj->u.g = g;
    obj->emit_state = EMIT_GDRAW;

    initObjMapData (job, GD_label(g), g);

#ifdef WITH_CODEGENS
    Obj = NONE;
#endif
    gvrender_begin_graph(job, g);
}

static void emit_end_graph(GVJ_t * job, graph_t * g)
{
    gvrender_end_graph(job);
#ifdef WITH_CODEGENS
    Obj = NONE;
#endif
    pop_obj_state(job);
}

void emit_graph(GVJ_t * job, graph_t * g)
{
    obj_state_t *obj;
    node_t *n;
    char *s;
    int flags = job->flags;
    GVC_t *gvc = job->gvc;
    textlabel_t *lab;
    point p1, p2;

    /* device dpi is now known */
    job->scale.x = job->zoom * job->dpi.x / POINTS_PER_INCH;
    job->scale.y = job->zoom * job->dpi.y / POINTS_PER_INCH;

    job->devscale.x = job->dpi.x / POINTS_PER_INCH;
    job->devscale.y = job->dpi.y / POINTS_PER_INCH;
    if ((job->flags & GVRENDER_Y_GOES_DOWN) || (Y_invert))
	job->devscale.y *= -1;

    s = late_string(g, agfindattr(g, "comment"), "");
    gvrender_comment(job, s);

    emit_begin_graph(job, g);
    obj = job->obj;

    if (flags & EMIT_COLORS)
	emit_colors(job,g);

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	ND_state(n) = 0;
    /* iterate layers */
    for (firstlayer(job); validlayer(job); nextlayer(job)) {
	if (job->numLayers > 1)
	    gvrender_begin_layer(job);

	/* iterate pages */
	for (firstpage(job); validpage(job); nextpage(job)) {
	    setColorScheme (agget (g, "colorscheme"));
    	    setup_page(job, g);
	    gvrender_begin_page(job);
	    gvdevice_prepare(job);
	    gvrender_set_pencolor(job, DEFAULT_COLOR);
	    gvrender_set_fillcolor(job, DEFAULT_FILL);
	    gvrender_set_font(job, gvc->defaultfontname, gvc->defaultfontsize);
	    if ((flags & GVRENDER_DOES_LABELS) && ((lab = GD_label(g))))
		/* do graph label on every page and rely on clipping to show it on the right one(s) */
		obj->label = lab->text;
		/* If EMIT_CLUSTERS_LAST is set, we assume any URL or tooltip
		 * attached to the root graph is emitted either in begin_page
		 * or end_page of renderer.
		 */
            if (!(flags & EMIT_CLUSTERS_LAST) && 
                  (obj->url || obj->explicit_tooltip)) {
		PF2P(job->clip.LL, p1);
		PF2P(job->clip.UR, p2);
		emit_map_rect(job, p1, p2);
		gvrender_begin_anchor(job, obj->url, obj->tooltip, obj->target);
	    }
	    if (job->numLayers == 1)
		emit_background(job, g);
	    if (GD_label(g))
		emit_label(job, EMIT_GLABEL, GD_label(g));
            if (!(flags & EMIT_CLUSTERS_LAST) && 
                  (obj->url || obj->explicit_tooltip))
		gvrender_end_anchor(job);
//	    if (boxf_overlap(job->clip, job->pageBox))
	        emit_view(job,g,flags);
	    gvdevice_format(job);
	    gvrender_end_page(job);
	} 

	if (job->numLayers > 1)
	    gvrender_end_layer(job);
    } 
    emit_end_graph(job, g);
}

/* support for stderr_once */
static void free_string_entry(Dict_t * dict, char *key, Dtdisc_t * disc)
{
    agstrfree(key);
}

static Dict_t *strings;
static Dtdisc_t stringdict = {
    0,				/* key  - the object itself */
    0,				/* size - null-terminated string */
    -1,				/* link - allocate separate holder objects  */
    NIL(Dtmake_f),
    (Dtfree_f) free_string_entry,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

int emit_once(char *str)
{
    if (strings == 0)
	strings = dtopen(&stringdict, Dtoset);
    if (!dtsearch(strings, str)) {
	dtinsert(strings, agstrdup(str));
	return TRUE;
    }
    return FALSE;
}

void emit_once_reset(void)
{
    if (strings) {
	dtclose(strings);
	strings = 0;
    }
}

static char **checkClusterStyle(graph_t* sg, int *flagp)
{
    char *style;
    char **pstyle = 0;
    int istyle = 0;

    if (((style = agget(sg, "style")) != 0) && style[0]) {
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
	    } else pp++;
	}
    }

    *flagp = istyle;
    return pstyle;
}

static void emit_begin_cluster(GVJ_t * job, Agraph_t * sg)
{
    obj_state_t *obj;

    obj = push_obj_state(job);
    obj->type = CLUSTER_OBJTYPE;
    obj->u.sg = sg;
    obj->emit_state = EMIT_CDRAW;

    initObjMapData (job, GD_label(sg), sg);

#ifdef WITH_CODEGENS
    Obj = CLST;
#endif
    gvrender_begin_cluster(job, sg);
}

static void emit_end_cluster(GVJ_t * job, Agraph_t * g)
{
    gvrender_end_cluster(job, g);
#ifdef WITH_CODEGENS
    Obj = NONE;
#endif
    pop_obj_state(job);
}

void emit_clusters(GVJ_t * job, Agraph_t * g, int flags)
{
    int c, istyle, filled;
    boxf BF;
    pointf AF[4];
    char *color, *fillcolor, *pencolor, **style;
    graph_t *sg;
    node_t *n;
    edge_t *e;
    obj_state_t *obj;
    textlabel_t *lab;
    int doAnchor;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	sg = GD_clust(g)[c];
	if (clust_in_layer(job, sg) == FALSE)
	    continue;
	/* when mapping, detect events on clusters after sub_clusters */
	if (flags & EMIT_CLUSTERS_LAST)
	    emit_clusters(job, sg, flags);
	emit_begin_cluster(job, sg);
	obj = job->obj;
	doAnchor = (obj->url || obj->explicit_tooltip);
	setColorScheme (agget (sg, "colorscheme"));
	gvrender_begin_context(job);
	if (doAnchor && !(flags & EMIT_CLUSTERS_LAST)) {
	    emit_map_rect(job, GD_bb(sg).LL, GD_bb(sg).UR);
	    gvrender_begin_anchor(job, obj->url, obj->tooltip, obj->target);
	}
	filled = FALSE;
	istyle = 0;
	if ((style = checkClusterStyle(sg, &istyle))) {
	    gvrender_set_style(job, style);
	    if (istyle & FILLED)
		filled = TRUE;
	}
	fillcolor = pencolor = 0;
	if (GD_gui_state(sg) & GUI_STATE_ACTIVE) {
	    pencolor = late_nnstring(sg, G_activepencolor, DEFAULT_ACTIVEPENCOLOR);
	    fillcolor = late_nnstring(sg, G_activefillcolor, DEFAULT_ACTIVEFILLCOLOR);
	    filled = TRUE;
	}
	else if (GD_gui_state(sg) & GUI_STATE_SELECTED) {
	    pencolor = late_nnstring(sg, G_activepencolor, DEFAULT_SELECTEDPENCOLOR);
	    fillcolor = late_nnstring(sg, G_activefillcolor, DEFAULT_SELECTEDFILLCOLOR);
	    filled = TRUE;
	}
	else if (GD_gui_state(sg) & GUI_STATE_DELETED) {
	    pencolor = late_nnstring(sg, G_deletedpencolor, DEFAULT_DELETEDPENCOLOR);
	    fillcolor = late_nnstring(sg, G_deletedfillcolor, DEFAULT_DELETEDFILLCOLOR);
	    filled = TRUE;
	}
	else if (GD_gui_state(sg) & GUI_STATE_VISITED) {
	    pencolor = late_nnstring(sg, G_visitedpencolor, DEFAULT_VISITEDPENCOLOR);
	    fillcolor = late_nnstring(sg, G_visitedfillcolor, DEFAULT_VISITEDFILLCOLOR);
	    filled = TRUE;
	}
	else {
	    if (((color = agget(sg, "pencolor")) != 0) && color[0])
		pencolor = color;
	    else if (((color = agget(sg, "color")) != 0) && color[0])
		fillcolor = pencolor = color;
	    /* bgcolor is supported for backward compatability */
	    else if (((color = agget(sg, "bgcolor")) != 0) && color[0]) {
		fillcolor = color;
	        filled = TRUE;
            }
	    if (((color = agget(sg, "fillcolor")) != 0) && color[0])
		fillcolor = color;
	}
	if (!pencolor) pencolor = DEFAULT_COLOR;
	if (!fillcolor) fillcolor = DEFAULT_FILL;
        B2BF(GD_bb(sg), BF);
	if (istyle & ROUNDED) {
	    if (late_int(sg, G_peripheries, 1, 0) || filled) {
		AF[0] = BF.LL;
		AF[2] = BF.UR;
		AF[1].x = AF[2].x;
		AF[1].y = AF[0].y;
		AF[3].x = AF[0].x;
		AF[3].y = AF[2].y;
		round_corners(job, fillcolor, pencolor, AF, 4, istyle);
	    }
	}
	else {
    	    gvrender_set_pencolor(job, pencolor);
	    gvrender_set_fillcolor(job, fillcolor);
	    if (late_int(sg, G_peripheries, 1, 0))
		gvrender_box(job, BF, filled);
	    else if (filled) { 
		if (fillcolor && fillcolor != pencolor)
		    gvrender_set_pencolor(job, fillcolor);
		gvrender_box(job, BF, filled);
	    }
	}
	if ((lab = GD_label(sg)))
	    emit_label(job, EMIT_CLABEL, lab);

	if (doAnchor) {
	    if (flags & EMIT_CLUSTERS_LAST) {
		emit_map_rect(job, GD_bb(sg).LL, GD_bb(sg).UR);
		gvrender_begin_anchor(job, obj->url, obj->tooltip, obj->target);
	    }
	    gvrender_end_anchor(job);
	}

	if (flags & EMIT_PREORDER) {
	    for (n = agfstnode(sg); n; n = agnxtnode(sg, n)) {
		emit_node(job, n);
		for (e = agfstout(sg, n); e; e = agnxtout(sg, e))
		    emit_edge(job, e);
	    }
	}
	gvrender_end_context(job);
	emit_end_cluster(job, g);
	/* when drawing, lay down clusters before sub_clusters */
	if (!(flags & EMIT_CLUSTERS_LAST))
	    emit_clusters(job, sg, flags);
    }
}

static boolean is_style_delim(int c)
{
    switch (c) {
    case '(':
    case ')':
    case ',':
    case '\0':
	return TRUE;
    default:
	return FALSE;
    }
}

#define SID 1

static int style_token(char **s, agxbuf * xb)
{
    char *p = *s;
    int token, rc;
    char c;

    while (*p && (isspace(*p) || (*p == ',')))
	p++;
    switch (*p) {
    case '\0':
	token = 0;
	break;
    case '(':
    case ')':
	token = *p++;
	break;
    default:
	token = SID;
	while (!is_style_delim(c = *p)) {
	    rc = agxbputc(xb, c);
	    p++;
	}
    }
    *s = p;
    return token;
}

#define FUNLIMIT 64
static unsigned char outbuf[SMALLBUF];
static agxbuf ps_xb;

static void cleanup(void)
{
    agxbfree(&ps_xb);
}

/* parse_style:
 * This is one of the worse internal designs in graphviz.
 * The use of '\0' characters within strings seems cute but it
 * makes all of the standard functions useless if not dangerous.
 * Plus the function uses static memory for both the array and
 * the character buffer. One hopes all of the values are used
 * before the function is called again.
 */
char **parse_style(char *s)
{
    static char *parse[FUNLIMIT];
    static boolean is_first = TRUE;
    int fun = 0;
    boolean in_parens = FALSE;
    unsigned char buf[SMALLBUF];
    char *p;
    int c;
    agxbuf xb;

    if (is_first) {
	agxbinit(&ps_xb, SMALLBUF, outbuf);
	atexit(cleanup);
	is_first = FALSE;
    }

    agxbinit(&xb, SMALLBUF, buf);
    p = s;
    while ((c = style_token(&p, &xb)) != 0) {
	switch (c) {
	case '(':
	    if (in_parens) {
		agerr(AGERR, "nesting not allowed in style: %s\n", s);
		parse[0] = (char *) 0;
		agxbfree(&xb);
		return parse;
	    }
	    in_parens = TRUE;
	    break;

	case ')':
	    if (in_parens == FALSE) {
		agerr(AGERR, "unmatched ')' in style: %s\n", s);
		parse[0] = (char *) 0;
		agxbfree(&xb);
		return parse;
	    }
	    in_parens = FALSE;
	    break;

	default:
	    if (in_parens == FALSE) {
		if (fun == FUNLIMIT - 1) {
		    agerr(AGWARN, "truncating style '%s'\n", s);
		    parse[fun] = (char *) 0;
		    agxbfree(&xb);
		    return parse;
		}
		agxbputc(&ps_xb, '\0');	/* terminate previous */
		parse[fun++] = agxbnext(&ps_xb);
	    }
	    agxbput(&ps_xb, agxbuse(&xb));
	    agxbputc(&ps_xb, '\0');
	}
    }

    if (in_parens) {
	agerr(AGERR, "unmatched '(' in style: %s\n", s);
	parse[0] = (char *) 0;
	agxbfree(&xb);
	return parse;
    }
    parse[fun] = (char *) 0;
    agxbfree(&xb);
    (void)agxbuse(&ps_xb);		/* adds final '\0' to buffer */
    return parse;
}

static boxf bezier_bb(bezier bz)
{
    int i;
    point p;
    box bb;
    boxf bbf;

    assert(bz.size > 0);
    bb.LL = bb.UR = bz.list[0];
    for (i = 1; i < bz.size; i++) {
	p=bz.list[i];
        EXPANDBP(bb,p);
    }
    B2BF(bb, bbf);
    return bbf;
}

static void init_splines_bb(splines *spl)
{
    int i;
    bezier bz;
    boxf bb, b;
    pointf p, u;

    assert(spl->size > 0);
    bz = spl->list[0];
    bb = bezier_bb(bz);
    for (i = 0; i < spl->size; i++) {
        if (i > 0) {
            bz = spl->list[i];
            b = bezier_bb(bz);
            EXPANDBB(bb, b);
        }
        if (bz.sflag) {
            P2PF(bz.sp, p);
            P2PF(bz.list[0], u);
            b = arrow_bb(p, u, 1, bz.sflag);
            EXPANDBB(bb, b);
        }
        if (bz.eflag) {
            P2PF(bz.ep, p);
            P2PF(bz.list[bz.size - 1], u);
            b = arrow_bb(p, u, 1, bz.eflag);
            EXPANDBB(bb, b);
        }
    }
    spl->bb = bb;
}

static void init_bb_edge(edge_t *e)
{
    splines *spl;

    spl = ED_spl(e);
    if (spl)
        init_splines_bb(spl);

//    lp = ED_label(e);
//    if (lp)
//        {}
}

static void init_bb_node(graph_t *g, node_t *n)
{
    edge_t *e;

    ND_bb(n).LL.x = ND_coord_i(n).x - ND_lw_i(n);
    ND_bb(n).LL.y = ND_coord_i(n).y - ND_ht_i(n) / 2.;
    ND_bb(n).UR.x = ND_coord_i(n).x + ND_rw_i(n);
    ND_bb(n).UR.y = ND_coord_i(n).y + ND_ht_i(n) / 2.;

    for (e = agfstout(g, n); e; e = agnxtout(g, e))
        init_bb_edge(e);

    /* IDEA - could also save in the node the bb of the node and
    all of its outedges, then the scan time would be proportional
    to just the number of nodes for many graphs.
    Wouldn't work so well if the edges are sprawling all over the place
    because then the boxes would overlap a lot and require more tests,
    but perhaps that wouldn't add much to the cost before trying individual
    nodes and edges. */
}

static void init_bb(graph_t *g)
{
        node_t *n;

	    for (n = agfstnode(g); n; n = agnxtnode(g, n))
		        init_bb_node(g, n);
}

static void auto_output_filename(GVJ_t *job)
{
    static char *buf;
    static int bufsz;
    char gidx[100];  /* large enough for '.' plus any integer */
    char *fn, *p;
    int len;

    if (job->graph_index)
	sprintf(gidx, ".%d", job->graph_index + 1);
    else
	gidx[0] = '\0';
    if (!(fn = job->input_filename))
	fn = "noname.dot";
    len = strlen(fn)			/* typically "something.dot" */
	+ strlen(gidx) 			/* "", ".2", ".3", ".4", ... */
	+ 1 				/* "." */
	+ strlen(job->output_langname)  /* e.g. "png" */
	+ 1; 				/* null terminaor */
    if (bufsz < len) {
	    bufsz = len + 10;
	    buf = realloc(buf, bufsz * sizeof(char));
    }
    strcpy(buf, fn);
    strcat(buf, gidx);
    strcat(buf, ".");
    if ((p = strchr(job->output_langname, ':'))) {
        strcat(buf, p+1);
	strcat(buf, ".");
	strncat(buf, job->output_langname, (p - job->output_langname));
    }
    else {
        strcat(buf, job->output_langname);
    }

    job->output_filename = buf;
}

extern gvevent_key_binding_t gvevent_key_binding[];
extern int gvevent_key_binding_size;
extern gvdevice_callbacks_t gvdevice_callbacks;

int gvRenderJobs (GVC_t * gvc, graph_t * g)
{
    static GVJ_t *prevjob;
    GVJ_t *job, *firstjob;

    if (!GD_drawing(g)) {
        agerr (AGERR, "Layout was not done.  Missing layout plugins? \n");
        return -1;
    }

    init_bb(g);
    init_gvc(gvc, g);
    init_layering(gvc, g);

    gvc->keybindings = gvevent_key_binding;
    gvc->numkeys = gvevent_key_binding_size;
    for (job = gvjobs_first(gvc); job; job = gvjobs_next(gvc)) {
	if (gvc->gvg) {
	    job->input_filename = gvc->gvg->input_filename;
	    job->graph_index = gvc->gvg->graph_index;
	}
	else {
	    job->input_filename = NULL;
	    job->graph_index = 0;
	}
	job->common = &(gvc->common);
	job->layout_type = gvc->layout.type;
	if (!GD_drawing(g)) {
	    agerr (AGERR, "layout was not done\n");
	    return -1;
	}

        job->output_lang = gvrender_select(job, job->output_langname);
        if (job->output_lang == NO_SUPPORT) {
            agerr (AGERR, "renderer for %s is unavailable\n", job->output_langname);
            return -1;
        }
#ifdef WITH_CODEGENS
	Output_lang = job->output_lang;
#endif

	/* if we already have an active job list and the device doesn't support mutiple output files, or we are about to write to a different output device */
        firstjob = gvc->active_jobs;
        if (firstjob
	    && (!(firstjob->flags & GVDEVICE_DOES_PAGES)
	      || (strcmp(job->output_langname,firstjob->output_langname)))) {

	    gvrender_end_job(firstjob);
            gvdevice_finalize(firstjob); /* finalize previous jobs */
	    
            gvc->active_jobs = NULL; /* clear active list */
	    gvc->common.viewNum = 0;
	    prevjob = NULL;
        }

	if (prevjob) {
            prevjob->next_active = job;  /* insert job in active list */
	}
	else {
	    gvc->active_jobs = job;   /* first job of new list */
	    gvdevice_initialize(job);    /* FIXME? - semantics are unclear */

            if (!job->output_file) {        /* if not yet opened */
	        if (gvc->common.auto_outfile_names)
		    auto_output_filename(job);
                if (job->output_filename) {
		    job->output_file = fopen(job->output_filename, "w");
		    if (job->output_file == NULL) {
		        perror(job->output_filename);
		        exit(1);
		    }
	        }
                else
                    job->output_file = stdout;
#ifdef WITH_CODEGENS
	        Output_file = job->output_file;
#endif

#ifdef HAVE_SETMODE
#ifdef O_BINARY
	        if (job->flags & GVDEVICE_BINARY_FORMAT)
		    setmode(fileno(job->output_file), O_BINARY);
#endif
#endif
            }
	    gvrender_begin_job(job);
	}
	job->next_active = NULL;      /* terminate active list */
	job->callbacks = &gvdevice_callbacks;

	init_job_flags(job, g);
	init_job_pad(job);
	init_job_margin(job);
	init_job_dpi(job, g);
	init_job_viewport(job, g);
	init_job_pagination(job, g);

	if (! (job->flags & GVDEVICE_EVENTS)) {
    		/* Show_boxes is not defined, if at all, 
                 * until splines are generated in dot 
                 */
#ifdef DEBUG
	    job->common->show_boxes = Show_boxes; 
#endif
	    emit_graph(job, g); /* FIXME? - should this be a special case of finalize() ? */

	    /* Flush is necessary because we may be writing to a pipe. */
	    if (job->output_file && ! job->external_context && job->output_lang != TK)
	        fflush(job->output_file);
	}

        /* the last job, after all input graphs are processed,
         *      is finalized from gvFreeContext()
         */
	prevjob = job;
    }
    return 0;
}
