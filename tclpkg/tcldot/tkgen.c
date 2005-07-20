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
 * tkgen.c generate canvas commands to display a graph
 */

#include "tcldot.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/* font modifiers */
#define REGULAR 0
#define BOLD	1
#define ITALIC  2

/* patterns */
#define P_SOLID 0
#define P_NONE  15
#define P_DOTTED 4		/* i wasn't sure about this */
#define P_DASHED 11		/* or this */

/* bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 2

/* highlight flag */
#define HIGHLIGHT 1
#define NOHIGHLIGHT 0

#define ANONCHAR		'_'
#define ISEMPTYSTR(s)	(((s) == NULL) || (*(s) == '\0'))
#define NULL_FN(t)		(t(*)())0

/* static int		N_pages; */
static unsigned long ObjHandle;
/* static point	Pages; */
static double Scale;
static double DevScale;
static double CompScale;
static int Rot;

static point Viewport;
static pointf GraphFocus;
static double Zoom;

static int onetime = TRUE;
static char buffer[256];
static char fontname[SMALLBUF] = "Times 14 normal";
static Tcl_DString script;

typedef struct context_t {
    char pencolor[SMALLBUF], fillcolor[SMALLBUF], *fontfam, fontopt,
	font_was_set;
    char pen, fill, penwidth;
    double fontsz;
} context_t;

#define MAXNEST 4
static context_t cstk[MAXNEST];
static int SP;

static char *op[] = { "graph", "node", "edge", "graph" };

static tkgendata_t *Tkgendata;

static void tkgen_append_string(char *string)
{
    Tcl_DStringAppend(&script, string, strlen(string));
}

static void tkgen_start_item(char *item)
{
    tkgen_append_string(Tkgendata->canvas);
    tkgen_append_string(" create ");
    tkgen_append_string(item);
}

static void tkgen_end_item(void)
{
    tkgen_append_string("\n");
}

static void tkgen_append_point(pointf rv)
{
    sprintf(buffer, " %d %d", ROUND(rv.x), ROUND(rv.y));
    Tcl_DStringAppend(&script, buffer, strlen(buffer));
}

static void tkgen_append_attribute(char *attribute, char *value)
{
    Tcl_DStringAppend(&script, " ", 1);
    Tcl_DStringAppend(&script, attribute, strlen(attribute));
    /* value part may need to be quoted */
    Tcl_DStringAppendElement(&script, value);
}

static void tkgen_append_tag(int tag)
{
    sprintf(buffer, " -tags %d%s%ld", tag, op[Obj], ObjHandle);
    Tcl_DStringAppend(&script, buffer, strlen(buffer));
}

static void init_tk(void)
{
    SP = 0;
    cstk[0].pencolor[0] = '\0';
    cstk[0].fillcolor[0] = '\0';
    cstk[0].fontfam = "Times";
    cstk[0].fontopt = 0;
    cstk[0].penwidth = WIDTH_NORMAL;
}

static void tkpt(point p)
{
    pointf rv;


    if (Rot == 0) {
	rv.x = (p.x - GraphFocus.x) * CompScale + Viewport.x / 2.;
	rv.y = -(p.y - GraphFocus.y) * CompScale + Viewport.y / 2.;
    } else {
	rv.x = -(p.y - GraphFocus.y) * CompScale + Viewport.x / 2.;
	rv.y = -(p.x - GraphFocus.x) * CompScale + Viewport.y / 2.;
    }
    tkgen_append_point(rv);
}

static void tkptarray(point * A, int n)
{
    int i;

    for (i = 0; i < n; i++)
	tkpt(A[i]);
}

static void tk_font(context_t * cp)
{

    char *style;

    style = "normal";
    switch (cp->fontopt) {
    case BOLD:
	style = "bold";
	break;
    case ITALIC:
	style = "italic";
	break;
    }
    sprintf(fontname, "%s %d %s", cp->fontfam, ROUND(cp->fontsz), style);
}

static void tk_reset(void)
{
    onetime = TRUE;
}

static void
tk_begin_job(FILE * ofp, graph_t * g, char **lib, char *user, char *info[],
	     point pages)
{
    Output_file = ofp;
    /* Pages = pages; */
    /* N_pages = pages.x * pages.y; */
}

static void tk_end_job(void)
{
}

static void tk_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    double dpi = GD_drawing(g)->dpi;

    Tkgendata = (tkgendata_t *)gvc->job->surface;

    if (dpi < 1.0)
	dpi = DEFAULT_DPI;
    DevScale = dpi / POINTS_PER_INCH;

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

#if 0
    sprintf(buffer, "%d %d %d %d", 0, 0, PB.UR.x - PB.LL.x,
	    PB.UR.y - PB.LL.y);
    if (!(a = agfindattr(g, "bb"))) {
	a = agraphattr(g, "bb", "");
    }
    agxset(g, a->index, buffer);
#endif

    Tcl_DStringInit(&script);
    tkgen_append_string("global __tkgen_smooth_type\n"
			"if {![info exists __tkgen_smooth_type]} {\n"
			"    if {[catch {package require Tkspline}]} {\n"
			"        puts stderr {Warning: Tkspline package not available. Edges will not be optimally smoothed.}\n"
			"        set __tkgen_smooth_type true\n"
			"    } {\n"
			"        set __tkgen_smooth_type spline\n"
			"    }\n" "}\n");

    if (onetime) {
	init_tk();
	onetime = FALSE;
    }
}

static void tk_end_graph(void)
{
    Tcl_DStringResult(Tkgendata->interp, &script);
}

static void tk_begin_page(graph_t * g, point page, double scale, int rot,
			  point offset)
{
/*      int             page_number; */
/*      point           sz; */

    Scale = scale;
    CompScale = Zoom * Scale * DevScale;
    Rot = rot;
}

static void tk_begin_cluster(graph_t * g)
{
    ObjHandle = g->handle;
}

static void tk_begin_node(node_t * n)
{
    ObjHandle = n->handle;
}

static void tk_begin_edge(edge_t * e)
{
    ObjHandle = e->handle;
}

static void tk_begin_context(void)
{
    /*assert(SP + 1 < MAXNEST); *//* unknown problem with SGI cc */
    cstk[SP + 1] = cstk[SP];
    SP++;
}

static void tk_end_context(void)
{
    int psp = SP - 1;
    /* assert(SP > 0); *//* same as above */
    if (cstk[SP].font_was_set)
	tk_font(&(cstk[psp]));
    SP = psp;
}

static void tk_set_font(char *name, double size)
{
    char *p, *q;
    context_t *cp;

    cp = &(cstk[SP]);
    cp->font_was_set = TRUE;
    cp->fontsz = size * Zoom * Scale;
    p = strdup(name);
    if ((q = strchr(p, '-'))) {
	*q++ = 0;
	if (strcasecmp(q, "italic") == 0) {
	    cp->fontopt = ITALIC;
	} else if (strcasecmp(q, "bold") == 0) {
	    cp->fontopt = BOLD;
	}
    }
    cp->fontfam = p;
    tk_font(&cstk[SP]);
}

#if 0
static void tk_arrowhead(point p, double theta, double scale, int flag)
{
    point A[2];
    double v;
    char buf[20];

    v = cos(RADIANS(theta)) * 10.0;
    A[0].x = ROUND(v) + p.x;
    v = sin(RADIANS(theta)) * 10.0;
    A[0].y = ROUND(v) + p.y;
    A[1] = p;
    if (cstk[SP].pen != P_NONE) {
	tkgen_start_item("line");
	tkptarray(A, 2);
	if (cstk[SP].color[0])
	    tkgen_append_attribute("-fill", cstk[SP].color);
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    sprintf(buf, "%d", cstk[SP].penwidth);
	    tkgen_append_attribute("-width", buf);
	}
	if (cstk[SP].pen == P_DOTTED)
	    tkgen_append_attribute("-dash", "2");
	if (cstk[SP].pen == P_DASHED)
	    tkgen_append_attribute("-dash", "5");
	tkgen_append_attribute("-arrow", "last");
	tkgen_append_tag(HIGHLIGHT);
	tkgen_end_item();
    }
}
#endif

static void tk_set_pencolor(char *name)
{
    char *tok;

    tok = canontoken(name);
    strcpy(cstk[SP].pencolor, tok);
}

static void tk_set_fillcolor(char *name)
{
    char *tok;

    tok = canontoken(name);
    strcpy(cstk[SP].fillcolor, tok);
}

static void tk_set_style(char **s)
{
    char *line, *p;
    context_t *cp;

    cp = &(cstk[SP]);
    while ((p = line = *s++)) {
	if (streq(line, "solid")) {
	} /* no-op */
	else if (streq(line, "dashed"))
	    cp->pen = P_DASHED;
	else if (streq(line, "dotted"))
	    cp->pen = P_DOTTED;
	else if (streq(line, "invis"))
	    cp->pen = P_NONE;
	else if (streq(line, "bold"))
	    cp->penwidth = WIDTH_BOLD;
	else if (streq(line, "setlinewidth")) {
	    while (*p)
		p++;
	    p++;
	    cp->penwidth = atol(p);
	} else if (streq(line, "filled"))
	    cp->fill = P_SOLID;
	else if (streq(line, "unfilled")) {
	} /* no-op */
	else
	    fprintf(stderr,
		    "tk_set_style: unsupported style %s - ignoring\n",
		    line);
    }
}

static void tk_textline(point p, textline_t * line)
{
    char *str = line->str;
    double fontsz = cstk[SP].fontsz;

    tkgen_start_item("text");
    p.y += fontsz * 0.4;	/* cl correction */
    tkpt(p);
    tkgen_append_attribute("-text", str);
    if (cstk[SP].pencolor[0])
	tkgen_append_attribute("-fill", cstk[SP].pencolor);
    tkgen_append_attribute("-font", fontname);
    switch (line->just) {
    case 'l':
	tkgen_append_attribute("-anchor", "w");
	break;
    case 'r':
	tkgen_append_attribute("-anchor", "e");
	break;
    default:
    case 'n':
	break;
    }
    tkgen_append_attribute("-state", "disabled");
    tkgen_append_tag(NOHIGHLIGHT);
    tkgen_end_item();
}

static void
tk_bezier(point * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    char buf[20];

    if (cstk[SP].pen != P_NONE) {
	tkgen_start_item("line");
	tkptarray(A, n);
	if (cstk[SP].pencolor[0])
	    tkgen_append_attribute("-fill", cstk[SP].pencolor);
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    sprintf(buf, "%d", cstk[SP].penwidth);
	    tkgen_append_attribute("-width", buf);
	}
	if (cstk[SP].pen == P_DOTTED)
	    tkgen_append_attribute("-dash", "2");
	if (cstk[SP].pen == P_DASHED)
	    tkgen_append_attribute("-dash", "5");
	tkgen_append_string(" -smooth $__tkgen_smooth_type");
	tkgen_append_tag(HIGHLIGHT);
	tkgen_end_item();
    }
}

static void tk_polygon(point * A, int n, int filled)
{
    char buf[20];

    if (cstk[SP].pen != P_NONE) {
	tkgen_start_item("polygon");
	tkptarray(A, n);
	if (filled && cstk[SP].fillcolor[0])
	    tkgen_append_attribute("-fill", cstk[SP].fillcolor);
	else			/* tk polygons default to black fill, some fill
				   is necessary else "canvas find overlapping" doesn't
				   work as expected, use white instead */
	    tkgen_append_attribute("-fill", "white");
	if (cstk[SP].pencolor[0])
	    tkgen_append_attribute("-outline", cstk[SP].pencolor);
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    sprintf(buf, "%d", cstk[SP].penwidth);
	    tkgen_append_attribute("-width", buf);
	}
	if (cstk[SP].pen == P_DOTTED)
	    tkgen_append_attribute("-dash", "2");
	if (cstk[SP].pen == P_DASHED)
	    tkgen_append_attribute("-dash", "5");
	tkgen_append_tag(HIGHLIGHT);
	tkgen_end_item();
    }
}

static void tk_ellipse(point p, int rx, int ry, int filled)
{
    point A[2];
    char buf[20];

    if (cstk[SP].pen != P_NONE) {
	A[0].x = p.x - rx;
	A[0].y = p.y - ry;
	A[1].x = p.x + rx;
	A[1].y = p.y + ry;
	tkgen_start_item("oval");
	tkptarray(A, 2);
	if (filled && cstk[SP].fillcolor[0])
	    tkgen_append_attribute("-fill", cstk[SP].fillcolor);
	else			/* tk ovals default to no fill, some fill
				   is necessary else "canvas find overlapping" doesn't
				   work as expected, use white instead */
	    tkgen_append_attribute("-fill", "white");
	if (cstk[SP].pencolor[0])
	    tkgen_append_attribute("-outline", cstk[SP].pencolor);
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    sprintf(buf, "%d", cstk[SP].penwidth);
	    tkgen_append_attribute("-width", buf);
	}
	if (cstk[SP].pen == P_DOTTED)
	    tkgen_append_attribute("-dash", "2");
	if (cstk[SP].pen == P_DASHED)
	    tkgen_append_attribute("-dash", "5");
	tkgen_append_tag(HIGHLIGHT);
	tkgen_end_item();
    }
}

static void tk_polyline(point * A, int n)
{
    char buf[20];

    if (cstk[SP].pen != P_NONE) {
	tkgen_start_item("line");
	tkptarray(A, n);
	if (cstk[SP].pencolor[0])
	    tkgen_append_attribute("-fill", cstk[SP].pencolor);
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    sprintf(buf, "%d", cstk[SP].penwidth);
	    tkgen_append_attribute("-width", buf);
	}
	if (cstk[SP].pen == P_DOTTED)
	    tkgen_append_attribute("-dash", "2");
	if (cstk[SP].pen == P_DASHED)
	    tkgen_append_attribute("-dash", "5");
	tkgen_append_attribute("-state", "disabled");
	tkgen_append_tag(NOHIGHLIGHT);
	tkgen_end_item();
    }
}

static void tk_user_shape(char *name, point * A, int n, int filled)
{
    static boolean onetime = TRUE;
    if (onetime) {
	fprintf(stderr, "custom shapes not available with this driver\n");
	onetime = FALSE;
    }
    tk_polygon(A, n, filled);
}

codegen_t TK_CodeGen = {
    tk_reset,
    tk_begin_job, tk_end_job,
    tk_begin_graph, tk_end_graph,
    tk_begin_page, 0,		/* tk_end_page */
    0, /* tk_begin_layer */ 0,	/* tk_end_layer */
    tk_begin_cluster, 0,	/* tk_end_cluster */
    0, /* tk_begin_nodes */ 0,	/* tk_end_nodes */
    0, /* tk_begin_edges */ 0,	/* tk_end_edges */
    tk_begin_node, 0,		/* tk_end_node */
    tk_begin_edge, 0,		/* tk_end_edge */
    tk_begin_context, tk_end_context,
    0, /* tk_begin_anchor */ 0,	/* tk_end_anchor */
    tk_set_font, tk_textline,
    tk_set_pencolor, tk_set_fillcolor, tk_set_style,
    tk_ellipse, tk_polygon,
    tk_bezier, tk_polyline,
    0,				/* tk_arrows */
    0,				/* tk_comment */
    0,				/* tk_textsize */
    tk_user_shape,
    0				/* tk_usershapesize */
};
