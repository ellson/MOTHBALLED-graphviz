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

#define NEWANCHORS 0

#include "render.h"
#include "htmltable.h"

/* IMAP font modifiers */
#define REGULAR 0
#define BOLD            1
#define ITALIC          2

/* IMAP patterns */
#define P_SOLID         0
#define P_NONE  15
#define P_DOTTED 4		/* i wasn't sure about this */
#define P_DASHED 11		/* or this */

/* IMAP bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 3

/* static int        N_pages; */
/* static point    Pages; */
static int onetime = TRUE;
static int isLatin1;

typedef struct context_t {
    char color_ix, *fontfam, fontopt, font_was_set;
    char pen, fill, penwidth, style_was_set;
    double fontsz;
} context_t;

#ifdef OLD
#define MAXNEST 4
static context_t cstk[MAXNEST];
static int SP;
#endif

static double Dpi;
static double DevScale;
static double CompScale;
static int Rot;

static point Viewport;
static pointf GraphFocus;
static double Zoom;

/* isRect:
 * isRect function returns true when polygon has 
 * regular rectangular shape. Rectangle is regular when
 * it is not skewed and distorted and orientation is almost zero     
 */
bool isRect(polygon_t * p)
{
    return (p->sides == 4 && (ROUND(p->orientation) % 90) == 0
	    && p->distortion == 0.0 && p->skew == 0.0);
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

static pointf mapptf(pointf p)
{
    pointf rv;

    if (Rot == 0) {
	rv.x = (p.x - GraphFocus.x) * CompScale + Viewport.x / 2.;
	rv.y = -(p.y - GraphFocus.y) * CompScale + Viewport.y / 2.;
    } else {
	rv.x = -(p.y - GraphFocus.y) * CompScale + Viewport.x / 2.;
	rv.y = -(p.x - GraphFocus.x) * CompScale + Viewport.y / 2.;
    }
    /* apply clipping */
    if (rv.x < 0)
	rv.x = 0;
    if (rv.x > (double) Viewport.x)
	rv.x = (double) (Viewport.x - 1);
    if (rv.y < 0)
	rv.y = 0;
    if (rv.y > (double) Viewport.y)
	rv.y = (double) (Viewport.y - 1);
    return rv;
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

#if ! NEWANCHORS
static void mapOutput (char* shapename, point* pp, int nump,
		char* url, char *target, char *label, char *tooltip)
{
    int i;

    if (Output_lang == IMAP && url && url[0]) {
	fprintf(Output_file, "%s %s ", shapename, url);
	if (strcmp(shapename, "circle") == 0)
	    fprintf(Output_file, "%d,%d %d,%d", 
		pp[0].x, pp[0].y, (pp[1].x+pp[0].x), pp[0].y);
	else {
	    for (i = 0; i < nump; i++)
		fprintf(Output_file, "%d,%d ", pp[i].x, pp[i].y);
	}
	fprintf(Output_file, "\n");

    } else if (Output_lang == ISMAP && url && url[0]) {
	fprintf(Output_file, "rectangle (%d,%d) (%d,%d) %s %s\n",
		pp[0].x, pp[0].y, pp[1].x, pp[1].y, url, label);

    } else if (Output_lang == CMAP || Output_lang == CMAPX) {
	fprintf(Output_file, "<area shape=\"%s\"", shapename);
	if (url && url[0])
	    fprintf(Output_file, " href=\"%s\"", xml_string(url));
	if (target && target[0])
	    fprintf(Output_file, " target=\"%s\"", xml_string(target));
	if (tooltip && tooltip[0])
	    fprintf(Output_file, " title=\"%s\"", xml_string(tooltip));
	/*
	 * alt text is intended for the visually impaired, but such
	 * folk are not likely to be clicking around on a graph anyway.
	 * IE on the PC platform (but not on Macs) incorrectly
	 * uses (non-empty) alt strings instead of title strings for tooltips.
	 * To make tooltips work and avoid this IE issue,
	 * while still satisfying usability guidelines
	 * that require that there is always an alt string,
	 * we generate just an empty alt string.
	 */
	fprintf(Output_file, " alt=\"\"");
	fprintf(Output_file, " coords=\"");

	if (strcmp(shapename, "circle") == 0)
	    fprintf(Output_file, "%d,%d,%d\"", pp[0].x, pp[0].y, pp[1].x);
	else {
	    for (i = 0; i < nump; i++)
		fprintf(Output_file, "%d,%d ", pp[i].x, pp[i].y);
	    fprintf(Output_file, "\"");
	}

	if (Output_lang == CMAPX)
	    fprintf(Output_file, " /");
	fprintf(Output_file, ">\n");

    } else if (Output_lang == POSTSCRIPT || Output_lang == PDF) {
	if (url && url[0]) {
	    fprintf(Output_file, "[ /Rect [ %d %d %d %d ]\n"
		"  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		pp[0].x, pp[0].y, pp[1].x, pp[1].y, ps_string(url, isLatin1));
	}
    }
}
#endif

/*
 * map_output_poly function generates the map file of requested format 
 * currently imap, cmapx, ismap, cmap, postscipt and pdf formats are supported
 * This function maps the node. 
 */
static void
map_output_poly(node_t * n, char *url,
		char *target, char *label, char *tooltip, int sample)
{
    int sides, peri, nump, i, j, filled = 0, rect = 0, nshape;
    pointf *ppf, *vertices, ldimen;
    point *pp, coord;
    char *shapename;
    polygon_t *poly = NULL;

    /* checking shape of node */
    nshape = shapeOf(n);
    /* node coordinate */
    coord = ND_coord_i(n);
    /* checking if filled style has been set for node */
    filled = ifFilled(n);

    if (nshape == SH_POLY || nshape == SH_POINT) {
	poly = (polygon_t *) ND_shape_info(n);

	/* checking if polygon is regular rectangle */
	if (isRect(poly) && (poly->peripheries || filled))
	    rect = 1;
    }

    /* When node has polygon shape and requested output format is imap or cmapx,
     * we have used shape of polygon to map clickable region that is circle, 
     * ellipse, polygon with n side, or point. For regular rectangular shape 
     * we have use node's bounding box to map clickable region 
     */
    if (poly && !rect && (Output_lang == IMAP || Output_lang == CMAPX)) {

	if (poly->sides < 3)
	    sides = 1;
	else
	    sides = poly->sides;

	if (poly->peripheries < 2)
	    peri = 1;
	else
	    peri = poly->peripheries;

	vertices = poly->vertices;

	/* use bounding box of text label for mapping   
	 * when polygon has no peripheries and node is not filled 
	 */
	if (poly->peripheries == 0 && !filled) {
	    shapename = "rect";
	    nump = 2;
	    ppf = N_NEW(nump, pointf);
	    pp = N_NEW(nump, point);
	    ldimen = ND_label(n)->dimen;
	    ppf[0].x = coord.x - (ldimen.x) / 2.0;
	    ppf[0].y = coord.y + (ldimen.y) / 2.0;
	    ppf[1].x = coord.x + (ldimen.x) / 2.0;
	    ppf[1].y = coord.y - (ldimen.y) / 2.0;

	} 
 	/* circle or ellipse */
	else if (poly->sides < 3 && poly->skew == 0.0
		    && poly->distortion == 0.0) {
	    if (poly->regular) {
		shapename = "circle";	/* circle */
		nump = 2;		/* center of circle and radius */
		ppf = N_NEW(nump, pointf);
		pp = N_NEW(nump, point);
		ppf[0].x = coord.x;
		ppf[0].y = coord.y;
		ppf[1].x = vertices[peri - 1].x;
		ppf[1].y = vertices[peri - 1].y;
	    }
	    else { /* ellipse is treated as polygon */
		double a, b;
		shapename = "poly";	/* ellipse */
		a = vertices[peri - 1].x;
		b = vertices[peri - 1].y;
		nump = sample;
		ppf = pEllipse(a, b, nump);
		pp = N_NEW(nump, point);
		for (i = 0; i < nump; i++) {
		    ppf[i].x += coord.x;
		    ppf[i].y += coord.y;
		}
	    }
	    /* all other polygonal shape */
	} else {
	    int offset = (peri - 1)*(poly->sides);
	    shapename = "poly";
	    /* distorted or skewed ellipses and circles are polygons with 120 
             * sides. For mapping we convert them into polygon with sample sides
	     */
	    if (poly->sides == 120) {
		int delta = 120/sample;
		nump = sample;
		ppf = N_NEW(nump, pointf);
		pp = N_NEW(nump, point);
		for (i = 0, j = 0; j < nump; i += delta, j++) {
		    ppf[j].x = coord.x + vertices[i + offset].x;
		    ppf[j].y = coord.y + vertices[i + offset].y;
		}
	    } else {
		nump = sides;
		ppf = N_NEW(nump, pointf);
		pp = N_NEW(nump, point);
		for (i = 0; i < nump; i++) {
		    ppf[i].x = coord.x + vertices[i + offset].x;
		    ppf[i].y = coord.y + vertices[i + offset].y;
		}
	    }
	}
    } else {
	/* we have used node's bounding box to map clickable region  
	 * when requested output format is neither imap nor cmapx and for all 
	 * node shapes other than polygon ( except regular ractangle polygon ) 
	 */
	shapename = "rect";
	nump = 2;
	ppf = N_NEW(nump, pointf);
	pp = N_NEW(nump, point);
	ppf[0].x = ND_coord_i(n).x - ND_lw_i(n);
	ppf[0].y = ND_coord_i(n).y + (ND_ht_i(n) / 2);
	ppf[1].x = ND_coord_i(n).x + ND_rw_i(n);
	ppf[1].y = ND_coord_i(n).y - (ND_ht_i(n) / 2);
    }

    /* apply scaling and translation if necessary */
    if (Output_lang == ISMAP
	|| Output_lang == IMAP
	|| Output_lang == CMAP || Output_lang == CMAPX) {
	for (i = 0; i < nump; i++) {
	    ppf[i] = mapptf(ppf[i]);
	}
    }

    for (i = 0; i < nump; i++) {
	pp[i].x = ROUND(ppf[i].x);
	pp[i].y = ROUND(ppf[i].y);
    }

#if ! NEWANCHORS
    mapOutput (shapename, pp, nump, url, target, label, tooltip);
#endif
    free(ppf);
    free(pp);
}

static void
map_output_rect(pointf p1, pointf p2, char *url, char *target, char *label,
		char *tooltip)
{
    pointf ppf1, ppf2;
    point pp[2];
    double t;

    if (!(url && url[0]) && !(tooltip && tooltip[0]))
	return;

    /* apply scaling and translation if necessary */
    if (Output_lang == ISMAP
	|| Output_lang == IMAP
	|| Output_lang == CMAP || Output_lang == CMAPX) {
	ppf1 = mapptf(p1);
	ppf2 = mapptf(p2);
    } else {
	ppf1 = p1;
	ppf2 = p2;
    }
    pp[0].x = ROUND(ppf1.x);
    pp[0].y = ROUND(ppf1.y);
    pp[1].x = ROUND(ppf2.x);
    pp[1].y = ROUND(ppf2.y);

    /* suppress maps that are totally clipped in either x or y */
    if (pp[0].x == pp[1].x || pp[0].y == pp[1].y)
	return;

    /* fix up coordinate order */
    if (pp[1].x < pp[0].x) {
	t = pp[1].x;
	pp[1].x = pp[0].x;
	pp[0].x = t;
    }
    if (pp[1].y < pp[0].y) {
	t = pp[1].y;
	pp[1].y = pp[0].y;
	pp[0].y = t;
    }
#if ! NEWANCHORS
    mapOutput ("rect", pp, 2, url, target, label, tooltip);
#endif
}

/* radius of mouse-sensitive region around a point */
#define FUZZ 3
static void
map_output_fuzzy_point(pointf p, char *url, char *target, char *label,
		       char *tooltip)
{
    pointf p1, p2;

    p1.x = p.x - FUZZ;
    p1.y = p.y - FUZZ;
    p2.x = p.x + FUZZ;
    p2.y = p.y + FUZZ;

    map_output_rect(p1, p2, url, target, label, tooltip);
}

static void map_reset(void)
{
    onetime = TRUE;
}

static void init_imap(void)
{
#ifdef OLD
    SP = 0;
    cstk[0].color_ix = 0;	/* IMAP color index 0-7 */
    cstk[0].fontfam = "Times";	/* font family name */
    cstk[0].fontopt = REGULAR;	/* modifier: REGULAR, BOLD or ITALIC */
    cstk[0].pen = P_SOLID;	/* pen pattern style, default is sold */
    cstk[0].fill = P_NONE;
    cstk[0].penwidth = WIDTH_NORMAL;
#endif
}

static void doHTMLdata(htmldata_t * dp, point p, void *obj)
{
    char *url = NULL, *target = NULL, *title = NULL;
    pointf p1, p2;
    int havetitle = 0;

    if ((url = dp->href) && url[0]) {
	switch (agobjkind(obj)) {
	case AGGRAPH:
	    url = strdup_and_subst_graph(url, (graph_t *) obj);
	    break;
	case AGNODE:
	    url = strdup_and_subst_node(url, (node_t *) obj);
	    break;
	case AGEDGE:
	    url = strdup_and_subst_edge(url, (edge_t *) obj);
	    break;
	}
    }
    target = dp->target;
    if ((title = dp->title) && title[0]) {
	havetitle++;
	switch (agobjkind(obj)) {
	case AGGRAPH:
	    title = strdup_and_subst_graph(title, (graph_t *) obj);
	    break;
	case AGNODE:
	    title = strdup_and_subst_node(title, (node_t *) obj);
	    break;
	case AGEDGE:
	    title = strdup_and_subst_edge(title, (edge_t *) obj);
	    break;
	}
    }
    if (url || title) {
	p1.x = p.x + dp->box.LL.x;
	p1.y = p.y + dp->box.LL.y;
	p2.x = p.x + dp->box.UR.x;
	p2.y = p.y + dp->box.UR.y;
	map_output_rect(p1, p2, url, target, "", title);
    }
    free(url);
    free(title);
}

/* forward declaration */
static void doHTMLcell(htmlcell_t * cp, point p, void *obj);

static void doHTMLtbl(htmltbl_t * tbl, point p, void *obj)
{
    htmlcell_t **cells = tbl->u.n.cells;
    htmlcell_t *cp;

    while ((cp = *cells++))
	doHTMLcell(cp, p, obj);
    if (tbl->data.href)
	doHTMLdata(&tbl->data, p, obj);
}

static void doHTMLcell(htmlcell_t * cp, point p, void *obj)
{
    if (cp->child.kind == HTML_TBL)
	doHTMLtbl(cp->child.u.tbl, p, obj);
    if (cp->data.href)
	doHTMLdata(&cp->data, p, obj);
}

static void doHTMLlabel(htmllabel_t * lbl, point p, void *obj)
{
    if (lbl->kind == HTML_TBL) {
	doHTMLtbl(lbl->u.tbl, p, obj);
    }
}

static void
map_begin_job(FILE * ofp, graph_t * g, char **lib, char *user,
	      char *info[], point pages)
{
    /* Pages = pages; */
    /* N_pages = pages.x * pages.y; */
    if (Output_lang == IMAP) {
	fprintf(Output_file, "base referer\n");
    } else if (Output_lang == ISMAP) {
/*		fprintf(Output_file,"base referer\n"); */
    } else if (Output_lang == CMAP || Output_lang == CMAPX) {
/*		fprintf(Output_file,"base referer\n"); */
    } else if (Output_lang == POSTSCRIPT || Output_lang == PDF) {
/*		fprintf(Output_file,"base referer\n"); */
    }
    isLatin1 = (GD_charset(g) == CHAR_LATIN1);
}

static void map_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    Dpi = GD_drawing(g)->dpi;
    if (Dpi < 1.0)
	Dpi = DEFAULT_DPI;
    DevScale = Dpi / POINTS_PER_INCH;

    Viewport.x = gvc->job->width;
    Viewport.y = gvc->job->height;
    if (Viewport.x) {
	Zoom = gvc->job->zoom;
	GraphFocus = gvc->job->focus;
    } else {
	Viewport.x =
	    (bb.UR.x - bb.LL.x + 2 * GD_drawing(g)->margin.x) * DevScale +
	    2;
	Viewport.y =
	    (bb.UR.y - bb.LL.y + 2 * GD_drawing(g)->margin.y) * DevScale +
	    2;
	GraphFocus.x = (GD_bb(g).UR.x - GD_bb(g).LL.x) / 2.;
	GraphFocus.y = (GD_bb(g).UR.y - GD_bb(g).LL.y) / 2.;
	Zoom = 1.0;
    }
    CompScale = Zoom * DevScale;

    if (onetime) {
	init_imap();
	onetime = FALSE;
    }
}

static graph_t *Root_Graph;
static char *Default_URL;

static void
map_begin_page(graph_t * g, point page, double scale, int rot,
	       point offset)
{
    char *url, *name;

    Rot = rot;
    name = xml_string(g->name);

    if (Output_lang == CMAPX) {
	fprintf(Output_file, "<map id=\"%s\" name=\"%s\">\n", name, name);
    }
    Root_Graph = g;
    Default_URL = NULL;
    if (((url = agget(g, "href")) && url[0])
	|| ((url = agget(g, "URL")) && url[0])) {
	if (Output_lang == IMAP) {
	    fprintf(Output_file, "default %s\n", url);
	} else if (Output_lang == ISMAP) {
	    fprintf(Output_file, "default %s %s\n", url, g->name);
	} else if (Output_lang == CMAP || Output_lang == CMAPX) {
	    Default_URL = url;
	}
    }

}

static void map_end_page(void)
{
    pointf Default_p1, Default_p2;
    char *Default_label;
    graph_t *g = Root_Graph;

    if (GD_label(g) && GD_label(g)->html)
	doHTMLlabel(GD_label(g)->u.html, GD_label(g)->p, (void *) g);
    if (Default_URL) {
	Default_URL = strdup_and_subst_graph(Default_URL, g);
	Default_label = g->name;
	Default_p1.x = GD_bb(g).LL.x;
	Default_p1.y = GD_bb(g).LL.y;
	Default_p2.x = GD_bb(g).UR.x;
	Default_p2.y = GD_bb(g).UR.y;
	map_output_rect(Default_p1, Default_p2, Default_URL, "",
			Default_label, "");
	free(Default_URL);
    }
    if (Output_lang == CMAPX) {
	fprintf(Output_file, "</map>\n");
    }
}

void map_begin_cluster(graph_t * g)
{
    char *s, *url = NULL, *target = NULL, *title = NULL, *tooltip = NULL,
	*m_url = NULL, *m_target = NULL, *m_tooltip = NULL;
    pointf p1, p2;

    if (GD_label(g)) {
	if (GD_label(g)->html)
	    doHTMLlabel(GD_label(g)->u.html, GD_label(g)->p, (void *) g);
	title = GD_label(g)->text;
    }
    if (((s = agget(g, "href")) && s[0])
	|| ((s = agget(g, "URL")) && s[0]))
	m_url = url = strdup_and_subst_graph(s, g);
    if ((s = agget(g, "target")) && s[0])
	m_target = target = strdup_and_subst_graph(s, g);
    if ((s = agget(g, "tooltip")) && s[0])
	m_tooltip = tooltip = strdup_and_subst_graph(s, g);
    else
	tooltip = title;
    if (url || m_tooltip) {
	p1.x = GD_bb(g).LL.x;
	p1.y = GD_bb(g).LL.y;
	p2.x = GD_bb(g).UR.x;
	p2.y = GD_bb(g).UR.y;
	map_output_rect(p1, p2, url, target, title, tooltip);
    }
    free(m_target);
    free(m_tooltip);
    free(m_url);
}

void map_begin_node(node_t * n)
{
    char *s, *url = NULL, *target = NULL, *tooltip = NULL,
	*m_url = NULL, *m_target = NULL, *m_tooltip = NULL;
    
    if (ND_label(n)->html)
	doHTMLlabel(ND_label(n)->u.html, ND_coord_i(n), (void *) n);
    if (((s = agget(n, "href")) && s[0]) || ((s = agget(n, "URL")) && s[0]))
	m_url = url = strdup_and_subst_node(s, n);
    if ((s = agget(n, "target")) && s[0])
	m_target = target = strdup_and_subst_node(s, n);
    if ((s = agget(n, "tooltip")) && s[0])
	m_tooltip = tooltip = strdup_and_subst_node(s, n);
    else
	tooltip = ND_label(n)->text;

    if (url || m_tooltip) {
	int sample;
	char *p = agget(n, "samplepoints");
	
	if (p)
	    sample = atoi(p);
	/* We want at least 4 points. For server-side maps, at most 100
         * points are allowed. To simplify things to fit with the 120 points
         * used for skewed ellipses, we set the bound at 60.
         */
	if ((sample < 4) || (sample > 60))
	    sample = DFLT_SAMPLE;
	map_output_poly(n, url, target, ND_label(n)->text, tooltip, sample);
    }

    free(m_url);
    free(m_target);
    free(m_tooltip);
}

void map_begin_edge(edge_t * e)
{
    /* strings */
    char *s, *label, *taillabel, *headlabel, *url, *tailurl, *headurl,
	*target, *tailtarget, *headtarget, *tooltip, *tailtooltip, *headtooltip,
	*m_url = NULL, *m_tailurl = NULL, *m_headurl = NULL,
	*m_target = NULL, *m_tailtarget = NULL, *m_headtarget = NULL,
	*m_tooltip = NULL, *m_tailtooltip = NULL, *m_headtooltip = NULL;

    textlabel_t *lab = NULL, *tlab = NULL, *hlab = NULL;
    pointf p, p1, p2;
    bezier bz;

    /*  establish correct text for main edge label, URL, tooltip */
    label = NULL;
    if ((lab = ED_label(e))) {
	if (lab->html)
	    doHTMLlabel(lab->u.html, lab->p, (void *) e);
	label = lab->text;
    }
    url = NULL;
    if (((s = agget(e, "href")) && s[0]) || ((s = agget(e, "URL")) && s[0]))
	m_url = url = strdup_and_subst_edge(s, e);
    tooltip = label;
    if ((s = agget(e, "tooltip")) && s[0])
	m_tooltip = tooltip = strdup_and_subst_edge(s, e);
    target = NULL;
    if ((s = agget(e, "target")) && s[0])
	m_target = target = strdup_and_subst_edge(s, e);

    /*  establish correct text for tail label, URL, tooltip */
    taillabel = label;
    if ((tlab = ED_tail_label(e))) {
	if (tlab->html)
	    doHTMLlabel(tlab->u.html, tlab->p, (void *) e);
	taillabel = tlab->text;
    }
    tailurl = url;
    if (((s = agget(e, "tailhref")) && s[0]) || ((s = agget(e, "tailURL")) && s[0]))
	m_tailurl = tailurl = strdup_and_subst_edge(s, e);
    tailtooltip = taillabel;
    if ((s = agget(e, "tailtooltip")) && s[0])
	m_tailtooltip = tailtooltip = strdup_and_subst_edge(s, e);
    tailtarget = target;
    if ((s = agget(e, "tailtarget")) && s[0])
	m_tailtarget = tailtarget = strdup_and_subst_edge(s, e);

    /*  establish correct text for head label, URL, tooltip */
    headlabel = label;
    if ((hlab = ED_head_label(e))) {
	if (hlab->html)
	    doHTMLlabel(hlab->u.html, hlab->p, (void *) e);
	headlabel = hlab->text;
    }
    headurl = url;
    if (((s = agget(e, "headhref")) && s[0]) || ((s = agget(e, "headURL")) && s[0]))
	m_headurl = headurl = strdup_and_subst_edge(s, e);
    headtooltip = headlabel;
    if ((s = agget(e, "headtooltip")) && s[0])
	m_headtooltip = headtooltip = strdup_and_subst_edge(s, e);
    headtarget = target;
    if ((s = agget(e, "headtarget")) && s[0])
	m_headtarget = headtarget = strdup_and_subst_edge(s, e);

    /* strings are now set  - next we map the three labels */

    if (lab && (url || m_tooltip)) {
	/* map a rectangle around the edge label */
	p1.x = lab->p.x - lab->dimen.x / 2;
	p1.y = lab->p.y - lab->dimen.y / 2;
	p2.x = lab->p.x + lab->dimen.x / 2;
	p2.y = lab->p.y + lab->dimen.y / 2;
	map_output_rect(p1, p2, url, target, label, tooltip);
    }

    if (tlab && (tailurl || m_tailtooltip)) {
	/* map a rectangle around the edge taillabel */
	p1.x = tlab->p.x - tlab->dimen.x / 2;
	p1.y = tlab->p.y - tlab->dimen.y / 2;
	p2.x = tlab->p.x + tlab->dimen.x / 2;
	p2.y = tlab->p.y + tlab->dimen.y / 2;
	map_output_rect(p1, p2, tailurl, tailtarget, taillabel, tailtooltip);
    }

    if (hlab && (headurl || m_headtooltip)) {
	/* map a rectangle around the edge headlabel */
	p1.x = hlab->p.x - hlab->dimen.x / 2;
	p1.y = hlab->p.y - hlab->dimen.y / 2;
	p2.x = hlab->p.x + hlab->dimen.x / 2;
	p2.y = hlab->p.y + hlab->dimen.y / 2;
	map_output_rect(p1, p2, headurl, headtarget, headlabel, headtooltip);
    }

    /* finally we map the two ends of the edge where they touch nodes */

    /* process intersecion with tail node */
    if (ED_spl(e) && (tailurl || m_tailtooltip)) {
	bz = ED_spl(e)->list[0];
	if (bz.sflag) {
	    /* Arrow at start of splines */
	    p.x = bz.sp.x;
	    p.y = bz.sp.y;
	} else {
	    /* No arrow at start of splines */
	    p.x = bz.list[0].x;
	    p.y = bz.list[0].y;
	}
	map_output_fuzzy_point(p, tailurl, tailtarget, taillabel, tailtooltip);
    }

    /* process intersection with head node */
    if (ED_spl(e) && (headurl || m_headtooltip)) {
	bz = ED_spl(e)->list[ED_spl(e)->size - 1];
	if (bz.eflag) {
	    /* Arrow at end of splines */
	    p.x = bz.ep.x;
	    p.y = bz.ep.y;
	} else {
	    /* No arrow at end of splines */
	    p.x = bz.list[bz.size - 1].x;
	    p.y = bz.list[bz.size - 1].y;
	}
	map_output_fuzzy_point(p, headurl, headtarget, headlabel, headtooltip);
    }

    free(m_url);
    free(m_target);
    free(m_tailtarget);
    free(m_headtarget);
    free(m_tailurl);
    free(m_headurl);
    free(m_tooltip);
    free(m_tailtooltip);
    free(m_headtooltip);
}

static void map_begin_anchor(char *href, char *tooltip, char *target)
{
#if NEWANCHORS
    fprintf(Output_file, "<area");
    if (href && href[0])
	fprintf(Output_file, " href=\"%s\"", xml_string(href));
    if (tooltip && tooltip[0])
	fprintf(Output_file, " title=\"%s\"", xml_string(tooltip));
    if (target && target[0])
	fprintf(Output_file, " target=\"%s\"", xml_string(target));
#endif
}

static void map_end_anchor(void)
{
#if NEWANCHORS
    fprintf(Output_file, "/>\n");
#endif
}

static void map_polygon(point * A, int n, int filled)
{
#if NEWANCHORS
    int i;
    point p;

    fprintf(Output_file, " shape=\"polygon\" coords=\"");
    for (i = 0; i < n; i++) {
	p = mappt(A[i]);
	fprintf(Output_file, "%d,%d,", p.x, p.y);
    }
    p = mappt(A[0]);
    fprintf(Output_file, "%d,%d\"", p.x, p.y);
#endif
}

static void map_ellipse(point p, int rx, int ry, int filled)
{
#if NEWANCHORS
    fprintf(Output_file, " shape=\"ellipse\"");
#endif
}


codegen_t MAP_CodeGen = {
    map_reset,
    map_begin_job, 0,		/* map_end_job */
    map_begin_graph, 0,		/* map_end_graph */
    map_begin_page, map_end_page,
    0, /* map_begin_layer */ 0,	/* map_end_layer */
    map_begin_cluster, 0,	/* map_end_cluster */
    0, /* map_begin_nodes */ 0,	/* map_end_nodes */
    0, /* map_begin_edges */ 0,	/* map_end_edges */
    map_begin_node, 0,		/* map_end_node */
    map_begin_edge, 0,		/* map_end_edge */
    0, /* map_begin_context */ 0,	/* map_end_context */
    map_begin_anchor, map_end_anchor,
    0, /* map_set_font */ 0,	/* map_textline */
    0, /* map_set_color */ 0, /* map_set_color */ 0,	/* map_set_style */
    map_ellipse, map_polygon,
    0, /* map_bezier */ 0,	/* map_polyline */
    0,				/* bezier_has_arrows */
    0,				/* map_comment */
    0,				/* map_textsize */
    0,				/* map_user_shape */
    0				/* map_usershapesize */
};
