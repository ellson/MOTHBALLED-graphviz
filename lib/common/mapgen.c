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

#include                "render.h"
#include                "htmltable.h"


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

static void
map_output_rect(pointf p1, pointf p2, char *url, char *target, char *label,
		char *tooltip)
{
    pointf ppf1, ppf2;
    point pp1, pp2;
    double t;

    if (!(url && url[0]))
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
    pp1.x = ROUND(ppf1.x);
    pp1.y = ROUND(ppf1.y);
    pp2.x = ROUND(ppf2.x);
    pp2.y = ROUND(ppf2.y);

    /* suppress maps that are totally clipped in either x or y */
    if (pp1.x == pp2.x || pp1.y == pp2.y)
	return;

    /* fix up coordinate order */
    if (pp2.x < pp1.x) {
	t = pp2.x;
	pp2.x = pp1.x;
	pp1.x = t;
    }
    if (pp2.y < pp1.y) {
	t = pp2.y;
	pp2.y = pp1.y;
	pp1.y = t;
    }
#if ! NEWANCHORS

    if (Output_lang == IMAP) {
	fprintf(Output_file, "rect %s %d,%d %d,%d\n",
		url, pp1.x, pp1.y, pp2.x, pp2.y);
    } else if (Output_lang == ISMAP) {
	fprintf(Output_file, "rectangle (%d,%d) (%d,%d) %s %s\n",
		pp1.x, pp1.y, pp2.x, pp2.y, url, label);
    } else if (Output_lang == CMAP || Output_lang == CMAPX) {
	fprintf(Output_file, "<area shape=\"rect\" href=\"%s\"",
		xml_string(url));
	if (target && target[0]) {
	    fprintf(Output_file, " target=\"%s\"", xml_string(target));
	}
	if (tooltip && tooltip[0]) {
	    fprintf(Output_file, " title=\"%s\"", xml_string(tooltip));
	}
#if 0
	/* If the node has a non-empty label, use that for the alt string */
	if (label && label[0]) {
	    fprintf(Output_file, " alt=\"%s\"", xml_string(label));
	} else {
	    fprintf(Output_file, " alt=\"\"");
	}
#else
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
#endif
	fprintf(Output_file, " coords=\"%d,%d,%d,%d\"",
		pp1.x, pp1.y, pp2.x, pp2.y);
	if (Output_lang == CMAPX)
	    fprintf(Output_file, " /");
	fprintf(Output_file, ">\n");

    } else if (Output_lang == POSTSCRIPT || Output_lang == PDF) {
	fprintf(Output_file, "[ /Rect [ %d %d %d %d ]\n"
		"  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		pp1.x, pp1.y, pp2.x, pp2.y, ps_string(url));
    }
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
    char *url, *target;
    pointf p1, p2;

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
	if (!(target = dp->target) || !target[0]) {
	    target = "";
	}

	p1.x = p.x + dp->box.LL.x;
	p1.y = p.y + dp->box.LL.y;
	p2.x = p.x + dp->box.UR.x;
	p2.y = p.y + dp->box.UR.y;
	map_output_rect(p1, p2, url, target, "", 0);
	free(url);
    }
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
    char *url;

    Rot = rot;

    if (Output_lang == CMAPX) {
	fprintf(Output_file, "<map id=\"%s\" name=\"%s\">\n", g->name,
		g->name);
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
    char *url, *target, *title = "", *tooltip = "",
	*m_target = NULL, *m_tooltip = NULL;
    pointf p1, p2;

    if (GD_label(g) && GD_label(g)->html)
	doHTMLlabel(GD_label(g)->u.html, GD_label(g)->p, (void *) g);
    if (((url = agget(g, "href")) && url[0])
	|| ((url = agget(g, "URL")) && url[0])) {
	if (GD_label(g) != NULL)
	    title = GD_label(g)->text;
	if ((target = agget(g, "target")) && target[0]) {
	    m_target = target = strdup_and_subst_graph(target, g);
	}
	p1.x = GD_bb(g).LL.x;
	p1.y = GD_bb(g).LL.y;
	p2.x = GD_bb(g).UR.x;
	p2.y = GD_bb(g).UR.y;
	url = strdup_and_subst_graph(url, g);
	if ((tooltip = agget(g, "tooltip")) && tooltip[0]) {
	    m_tooltip = tooltip = strdup_and_subst_graph(tooltip, g);
	} else {
	    tooltip = title;
	}
	map_output_rect(p1, p2, url, target, title, tooltip);
	if (m_target)
	    free(m_target);
	if (m_tooltip)
	    free(m_tooltip);
	free(url);
    }
}

void map_begin_node(node_t * n)
{
    char *url, *target, *tooltip, *m_target = NULL, *m_tooltip = NULL;
    pointf p1, p2;

    if (ND_label(n)->html)
	doHTMLlabel(ND_label(n)->u.html, ND_coord_i(n), (void *) n);
    if (((url = agget(n, "href")) && url[0])
	|| ((url = agget(n, "URL")) && url[0])) {
	if ((target = agget(n, "target")) && target[0]) {
	    m_target = target = strdup_and_subst_node(target, n);
	}
	p1.x = ND_coord_i(n).x - ND_lw_i(n);
	p1.y = ND_coord_i(n).y - (ND_ht_i(n) / 2);
	p2.x = ND_coord_i(n).x + ND_rw_i(n);
	p2.y = ND_coord_i(n).y + (ND_ht_i(n) / 2);
	url = strdup_and_subst_node(url, n);
	if ((tooltip = agget(n, "tooltip")) && tooltip[0]) {
	    m_tooltip = tooltip = strdup_and_subst_node(tooltip, n);
	} else {
	    tooltip = ND_label(n)->text;
	}
	map_output_rect(p1, p2, url, target, ND_label(n)->text, tooltip);
	if (m_target)
	    free(m_target);
	if (m_tooltip)
	    free(m_tooltip);
	free(url);
    }
}

void map_begin_edge(edge_t * e)
{
    /* strings */
    char *label, *taillabel, *headlabel;
    char *url, *headurl, *tailurl, *tooltip, *target;
    char *tailtooltip = NULL, *headtooltip = NULL, *tailtarget =
	NULL, *headtarget = NULL;

    /* malloc flags for strings */
    char *m_url = NULL, *m_headurl = NULL, *m_tailurl = NULL;
    char *m_target = NULL, *m_tailtarget = NULL, *m_headtarget = NULL;
    char *m_tooltip = NULL, *m_tailtooltip = NULL, *m_headtooltip = NULL;

    textlabel_t *lab = NULL, *tlab = NULL, *hlab = NULL;
    pointf p, p1, p2;
    bezier bz;

    /*  establish correct text for main edge label, URL, tooltip */
    if ((lab = ED_label(e))) {
	if (lab->html)
	    doHTMLlabel(lab->u.html, lab->p, (void *) e);
	label = lab->text;
    } else {
	label = "";
    }
    if (((url = agget(e, "href")) && url[0])
	|| ((url = agget(e, "URL")) && url[0])) {
	m_url = url = strdup_and_subst_edge(url, e);
	if ((tooltip = agget(e, "tooltip")) && tooltip[0]) {
	    m_tooltip = tooltip = strdup_and_subst_edge(tooltip, e);
	} else {
	    tooltip = label;
	}
	if ((target = agget(e, "target")) && target[0]) {
	    m_target = target = strdup_and_subst_edge(target, e);
	} else {
	    target = "";
	}
    } else {
	tooltip = "";
	target = "";
    }

    /*  establish correct text for tail label, URL, tooltip */
    if ((tlab = ED_tail_label(e))) {
	if (tlab->html)
	    doHTMLlabel(tlab->u.html, tlab->p, (void *) e);
	taillabel = tlab->text;
    } else {
	taillabel = label;
    }
    if (((tailurl = agget(e, "tailhref")) && tailurl[0])
	|| ((tailurl = agget(e, "tailURL")) && tailurl[0])) {
	m_tailurl = tailurl = strdup_and_subst_edge(tailurl, e);
	if ((tailtooltip = agget(e, "tailtooltip")) && tailtooltip[0]) {
	    m_tailtooltip = tailtooltip =
		strdup_and_subst_edge(tailtooltip, e);
	} else {
	    tailtooltip = taillabel;
	}
	if ((tailtarget = agget(e, "tailtarget")) && tailtarget[0]) {
	    m_tailtarget = tailtarget =
		strdup_and_subst_edge(tailtarget, e);
	} else {
	    tailtarget = target;
	}
    } else if (url) {
	tailurl = url;
	tailtooltip = tooltip;
	tailtarget = target;
    }

    /*  establish correct text for head label, URL, tooltip */
    if ((hlab = ED_head_label(e))) {
	if (hlab->html)
	    doHTMLlabel(hlab->u.html, hlab->p, (void *) e);
	headlabel = hlab->text;
    } else {
	headlabel = label;
    }
    if (((headurl = agget(e, "headhref")) && headurl[0])
	|| ((headurl = agget(e, "headURL")) && headurl[0])) {
	m_headurl = headurl = strdup_and_subst_edge(headurl, e);
	if ((headtooltip = agget(e, "headtooltip")) && headtooltip[0]) {
	    m_headtooltip = headtooltip =
		strdup_and_subst_edge(headtooltip, e);
	} else {
	    headtooltip = headlabel;
	}
	if ((headtarget = agget(e, "headtarget")) && headtarget[0]) {
	    m_headtarget = headtarget =
		strdup_and_subst_edge(headtarget, e);
	} else {
	    headtarget = target;
	}
    } else if (url) {
	headurl = url;
	headtooltip = tooltip;
	headtarget = target;
    }

    /* strings are now set  - next we map the three labels */

    if (lab && url) {
	/* map a rectangle around the edge label */
	p1.x = lab->p.x - lab->dimen.x / 2;
	p1.y = lab->p.y - lab->dimen.y / 2;
	p2.x = lab->p.x + lab->dimen.x / 2;
	p2.y = lab->p.y + lab->dimen.y / 2;
	map_output_rect(p1, p2, url, target, label, tooltip);
    }

    if (tlab && (url || tailurl)) {
	/* map a rectangle around the edge taillabel */
	p1.x = tlab->p.x - tlab->dimen.x / 2;
	p1.y = tlab->p.y - tlab->dimen.y / 2;
	p2.x = tlab->p.x + tlab->dimen.x / 2;
	p2.y = tlab->p.y + tlab->dimen.y / 2;
	map_output_rect(p1, p2, tailurl, tailtarget, taillabel,
			tailtooltip);
    }

    if (hlab && (url || headurl)) {
	/* map a rectangle around the edge headlabel */
	p1.x = hlab->p.x - hlab->dimen.x / 2;
	p1.y = hlab->p.y - hlab->dimen.y / 2;
	p2.x = hlab->p.x + hlab->dimen.x / 2;
	p2.y = hlab->p.y + hlab->dimen.y / 2;
	map_output_rect(p1, p2, headurl, headtarget, headlabel,
			headtooltip);
    }
#if 0
    /* FIXME - what is this supposed to do?  Perhaps map spline control points? */
    if (ED_spl(e) && url) {
	int i, j;

	for (i = 0; i < ED_spl(e)->size; i++) {
	    bz = ED_spl(e)->list[i];
	    for (j = 0; j < bz.size; j += 3) {
		if (((i == 0) && (j == 0))	/* origin */
		    ||((i == (ED_spl(e)->size - 1))
		       && (j == (bz.size - 1)))) {
		    continue;
		}
		p.x = bz.list[j].x;
		p.y = bz.list[j].y;
		map_output_fuzzy_point(p, url, ED_label(e)->text);
	    }
	}
    }
#endif

    /* finally we map the two ends of the edge where they touch nodes */

    /* process intersecion with tail node */
    if (ED_spl(e) && (url || tailurl)) {
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
	map_output_fuzzy_point(p, tailurl, tailtarget, taillabel,
			       tailtooltip);
    }

    /* process intersection with head node */
    if (ED_spl(e) && (url || headurl)) {
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
	map_output_fuzzy_point(p, headurl, headtarget, headlabel,
			       headtooltip);
    }

    if (m_url)
	free(m_url);
    if (m_target)
	free(m_target);
    if (m_tailtarget)
	free(m_tailtarget);
    if (m_headtarget)
	free(m_headtarget);
    if (m_tailurl)
	free(m_tailurl);
    if (m_headurl)
	free(m_headurl);
    if (m_tooltip)
	free(m_tooltip);
    if (m_tailtooltip)
	free(m_tailtooltip);
    if (m_headtooltip)
	free(m_headtooltip);
}

static void map_begin_anchor(char *href, char *tooltip, char *target)
{
#if NEWANCHORS
    fprintf(Output_file, "<area href=\"%s\"", xml_string(href));
    if (tooltip && tooltip[0]) {
	fprintf(Output_file, " title=\"%s\"", xml_string(tooltip));
    }
    if (target && target[0]) {
	fprintf(Output_file, " target=\"%s\"", xml_string(target));
    }
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
