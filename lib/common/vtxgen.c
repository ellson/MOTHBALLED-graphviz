/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/


/*
 * vtxgen.c generates graph diagrams in the format for
 *  Confluents's Visual Thought
 */

/*
 * If this time code is a pain to port, then just comment out the
 * next line.  It only provides an optional information field
 * in the (header...) block 
 */
#define SUPPORT_WRITEDATE

#include "render.h"
#ifdef SUPPORT_WRITEDATE
#include <time.h>
#endif


/* VTX font modifiers */
#define REGULAR 0
#define BOLD	1
#define ITALIC	2
#define UNDERSORE 4
#define STRIKE 8

/* VTX patterns */
#define P_NONE  0
#define P_SOLID	1
#define P_DOTTED 2
#define P_DASHED 3

/* VTX bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 3

/* VTX shape mappings */
typedef struct shapemap_s {
    char *shape;
    char *vtxshape;
} shapemap_t;

static shapemap_t shapemap[] = {
    {"box", "\"Rectangle\""},
    {"ellipse", "\"Ellipse\""},
    {"circle", "\"Ellipse\""},
    {"triangle", "\"Triangle\""},
    {"diamond", "\"Diamond\""},
    {"trapezium", "\"Trapezoid\""},
    {"parallelogram", "\"Parallelogram\""},
    {"hexagon", "\"Hexagon\""},
    {NULL, "\"Ellipse\""}	/* default */
};


static point Pages;
static double Scale;
static int Rot;
/* static	box		PB; */
static int onetime = TRUE;

typedef struct context_t {
    int color_r, color_g, color_b;
    char *fontfam, fontopt, font_was_set;
    char pen, fill, penwidth, style_was_set;
    double fontsz;
} context_t;

#define MAXNEST 4
static context_t cstk[MAXNEST];
static int SP;

static void vtx_reset(void)
{
    onetime = TRUE;
}


static void init_vtx(void)
{
    SP = 0;
    cstk[0].color_r = cstk[0].color_g = cstk[0].color_b = 0;
    cstk[0].fontfam = "Times";	/* font family name */
    cstk[0].fontopt = REGULAR;	/* modifier: REGULAR, BOLD or ITALIC */
    cstk[0].pen = P_SOLID;	/* pen pattern style, default is solid */
    cstk[0].fill = P_NONE;
    cstk[0].penwidth = WIDTH_NORMAL;
}

static pointf vtx_pt(pointf p)
{
    pointf rv;

    if (Rot == 0) {
	rv.x = p.x;
	rv.y = p.y;
    } else {
	rv.x = p.y;
	rv.y = p.x;
    }
    return rv;
}

static void vtx_ptarray(point * A, int n)
{
    int i;
    pointf p;

    fprintf(Output_file, "    (points\n");
    for (i = 0; i < n; i++) {
	p.x = (double) A[i].x;
	p.y = (double) A[i].y;
	p = vtx_pt(p);
	fprintf(Output_file, "      (%g %g)\n", p.x, p.y);
    }
    fprintf(Output_file, "    )\n");
}

static void vtx_bzptarray(point * A, int start, int end)
{
    pointf p;
    int qx = 0, qy = 0;
    int i, j, incr = (start > end) ? -1 : 1;

    fprintf(Output_file, "    (points\n");
    for (i = start, j = 1; i != end; i += incr, j++) {
	switch (j % 3) {
	case 0:
	    p.x = (double) A[i].x;
	    p.y = (double) A[i].y;
	    p = vtx_pt(p);
	    fprintf(Output_file, "      (%g %g)\n", p.x, p.y);
	    break;
	case 1:
#if 1
	    qx = A[i].x;
	    qy = A[i].y;
#else
	    p.x = (double) A[i].x;
	    p.y = (double) A[i].y;
	    p = vtx_pt(p);
	    fprintf(Output_file, "      (%g %g)\n", p.x, p.y);
#endif
	    break;
	case 2:
#if 1
	    /* undo EK's strange coding of straight segments */
	    if (A[i].x == qx && A[i].y == qy) {
		if ((A[i - 2].x == qx && A[i - 2].y == qy)
		    || (A[i + 1].x == qx && A[i + 1].y == qy)) {
		    p.x = (A[i + 1].x + A[i - 2].x) / 2.0;
		    p.y = (A[i + 1].y + A[i - 2].y) / 2.0;
		} else {
		    p.x = (double) qx;
		    p.y = (double) qy;
		}
	    } else {
		p.x = (A[i].x + qx) / 2.0;
		p.y = (A[i].y + qy) / 2.0;
	    }
#else
	    p.x = (double) A[i].x;
	    p.y = (double) A[i].y;
#endif
	    p = vtx_pt(p);
	    fprintf(Output_file, "      (%g %g)\n", p.x, p.y);
	    break;
	}
    }
    fprintf(Output_file, "    )\n");
}

static void vtx_font(context_t * cp)
{
/* FIX
	char	*fw,*fa;

	fw = fa = "Regular";
	switch (cp->fontopt) {
		case BOLD: fw = "Bold"; break;
		case ITALIC: fa = "Italic"; break;
	}
*/
}

static void vtx_comment(char *str)
{
    fprintf(Output_file, "; %s\n", str);
}

static void
vtx_begin_job(FILE * ofp, graph_t * g, const char **lib, char *info[], point pages)
{
    char *date = "";
#ifdef SUPPORT_WRITEDATE
    time_t when;
    struct tm *tm;
    size_t date_length = 200;

    time(&when);
    tm = localtime(&when);
    date = N_GNEW(date_length, char);
    strftime(date, date_length, "%a %b %e %H:%M:%S %Z %Y", tm);
#endif

    Pages = pages;
    /* N_pages = pages.x * pages.y; */

    fprintf(Output_file, "; Visual Thought 1.0\n"
	    "\n"
	    "(header\n"
	    "  (program \"%s\")\n"
	    "  (version \"%s\")\n"
	    "  (buildDate \"%s\")\n"
	    "  (writeDate \"%s\")\n"
	    "  (documentPath \"\")\n"
	    ")\n" "\n", info[0], info[1], info[2], date);

    free(date);
}

static void vtx_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    /* PB = bb; */
    if (onetime) {
	init_vtx();
	onetime = FALSE;
    }
}

static void
vtx_begin_page(graph_t * g, point page, double scale, int rot,
	       point offset)
{
    int page_number;
    /* point        sz; */

    Scale = scale;
    Rot = rot;
    page_number = page.x + page.y * Pages.x + 1;
    /* sz = sub_points(PB.UR,PB.LL); */

    fprintf(Output_file, "(document\n"
	    "  (palette F)\n"
	    "  (layout\n"
	    "    (page \"Letter\")\n"
	    "    (units \"Inches\")\n"
	    "    (orientation \"portrait\")\n"
	    "    (numberOfPages %d %d)\n"
	    "    (scale %g)\n"
	    "    (margins 18 18 18 18)\n"
	    "  )\n"
	    ")\n"
	    "\n"
	    "(views\n"
	    "  (view\n"
	    "    (location 269 49)\n"
	    "    (size 632 723)\n"
	    "    (zoom %g)\n"
	    "    (documentLocation 0 119)\n"
	    "    (gridSnap T)\n"
	    "    (gridVisibility F)\n"
	    "    (gridSpacing 9)\n"
	    "    (pageBreaks T)\n"
	    "    (toolVisibility T)\n"
	    "    (rulerVisibility T)\n"
	    "  )\n"
	    ")\n"
	    "\n", page_number, Pages.x * Pages.y, scale * 100, scale);
}

static void vtx_begin_nodes(void)
{
    fprintf(Output_file, "(shapes\n");
}

static void vtx_end_nodes(void)
{
    fprintf(Output_file, ")\n" "\n");
}

static void vtx_begin_edges(void)
{
    fprintf(Output_file, "(connections\n");
}

static void vtx_end_edges(void)
{
    fprintf(Output_file, ")\n" "\n" "(groups\n" ")\n");
}

static void vtx_begin_node(node_t * n)
{
    shapemap_t *p;

    for (p = shapemap; p->shape; p++) {
	if (streq(ND_shape(n)->name, p->shape)) {
	    break;
	}
    }
    fprintf(Output_file, "  (shape\n"
	    "    (id %ld)\n"
	    "    (layer %ld)\n"
	    "    (type %s)\n", AGID(n) + 1, AGID(n), p->vtxshape);
}

static void vtx_end_node(void)
{
    fprintf(Output_file, "  )\n");
}

static void vtx_begin_edge(edge_t * e)
{
    fprintf(Output_file, "  (connection\n"
	    "    (id %ld)\n"
	    "    (layer %ld)\n"
	    "    (rotation 0)\n"
	    "    (textRotation 0)\n"
	    "    (locked F)\n"
	    "    (start %ld)\n"
	    "    (end %ld)\n",
	    AGID(e) + 1, AGID(e), AGID(agtail(e)) + 1, AGID(aghead(e)) + 1);
}

static void vtx_end_edge(void)
{
    fprintf(Output_file, "  )\n");
}

static void vtx_begin_context(void)
{
    assert(SP + 1 < MAXNEST);
    cstk[SP + 1] = cstk[SP];
    SP++;
}

static void vtx_end_context(void)
{
    int psp = SP - 1;

    assert(SP > 0);
    SP = psp;
}

static void vtx_set_font(char *name, double size)
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
    vtx_font(&cstk[SP]);
}

static void vtx_style(void)
{
    context_t *cp;

    cp = &(cstk[SP]);
    fprintf(Output_file, "    (style\n"
	    "      (filled %s)\n"
	    "      (fillColor %d %d %d)\n"
	    "      (stroked T)\n"
	    "      (strokeColor %d %d %d)\n"
	    "      (lineWidth %d)\n"
	    "      (shadowed F)\n"
	    "      (shadowColor 39321 39321 39321)\n"
	    "    )\n",
	    cp->fill ? "T" : "F",
	    cp->color_r, cp->color_g, cp->color_b,
	    cp->color_r, cp->color_g, cp->color_b, cp->penwidth);
}

static void vtx_node_style(void)
{
    fprintf(Output_file, "    (rotation 0)\n" "    (locked F)\n");
    vtx_style();
    fprintf(Output_file, "    (flipHorizontal F)\n"
	    "    (flipVertical F)\n");
}

static void vtx_set_color(char *name)
{
    gvcolor_t color;
    context_t *cp;

    cp = &(cstk[SP]);
    colorxlate(name, &color, RGBA_WORD);
    cp->color_r = color.u.rrggbbaa[0];
    cp->color_g = color.u.rrggbbaa[1];
    cp->color_b = color.u.rrggbbaa[2];
}

static void vtx_set_style(char **s)
{
    char *line;
    context_t *cp;

    cp = &(cstk[SP]);
    while ((line = *s++)) {
	if (streq(line, "solid"))
	    cp->pen = P_SOLID;
	else if (streq(line, "dashed"))
	    cp->pen = P_DASHED;
	else if (streq(line, "dotted"))
	    cp->pen = P_DOTTED;
	else if (streq(line, "invis"))
	    cp->pen = P_NONE;
	else if (streq(line, "bold"))
	    cp->penwidth = WIDTH_BOLD;
	else if (streq(line, "filled"))
	    cp->fill = P_SOLID;
	else if (streq(line, "unfilled"))
	    cp->fill = P_NONE;
	else {
	    agerr(AGERR,
		  "vtx_set_style: unsupported style %s - ignoring\n",
		  line);
	}
	cp->style_was_set = TRUE;
    }
}

static char *vtx_string(char *s)
{
    static char *buf = NULL;
    static int bufsize = 0;
    int pos = 0;
    char *p, esc;

    if (!buf) {
	bufsize = 64;
	buf = N_GNEW(bufsize, char);
    }

    p = buf;
    while (*s) {
	if (pos > (bufsize - 8)) {
	    bufsize *= 2;
	    buf = grealloc(buf, bufsize);
	    p = buf + pos;
	}
	esc = 0;
	switch (*s) {
	case '\t':
	    esc = 't';
	    break;
	case '{':
	case '}':
	case '\\':
	    esc = *s;
	    break;
	}
	if (esc) {
	    *p++ = '\\';
	    *p++ = esc;
	    pos += 2;
	} else {
	    *p++ = *s;
	    pos++;
	}
	s++;
    }
    *p = '\0';
    return buf;
}

static void vtx_textpara(point p, textpara_t * para)
{
    pointf mp;
    double fontsz = Scale * cstk[SP].fontsz;

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }

    mp.x = (double) p.x;
    mp.y = (double) (p.y - fontsz / 2 + 2);
    mp = vtx_pt(mp);
    if (Obj == EDGE) {
	fprintf(Output_file, "    (showText T)\n"
		"    (textDistancePercentage 0.5)\n"
		"    (textWidth 72)\n"
		"    (textOffset 0)\n"
		"    (rtfText{\\rtf1\\ansi\\deff0\n"
		"{\\fonttbl{\\f0\\fnil helvetica medium;}}\n"
		"{\\colortbl\\red0\\green0\\blue0;}\n"
		"\\cf0\\plain\\pard {\\fs%d %s}})\n",
		(int) ((fontsz * 2) - 8), vtx_string(para->str));
    } else {
	fprintf(Output_file, "    (showText T)\n"
		"    (textVerticalAlignment \"left\")\n"
		"    (rtfText{\\rtf1\\ansi\\deff0\n"
		"{\\fonttbl{\\f0\\fnil helvetica medium;}}\n"
		"{\\colortbl\\red0\\green0\\blue0;}\n"
		"\\cf0\\plain\\pard {\\fs%d %s}})\n",
		(int) ((fontsz * 2) - 8), vtx_string(para->str));
    }
}

static void vtx_bezier(point * A, int n, int arrow_at_start,
		       int arrow_at_end, int filled)
{
    if (arrow_at_start) {
	vtx_bzptarray(A, n - 2, 0);
	fprintf(Output_file, "    (curved T)\n");
	vtx_style();
	fprintf(Output_file, "    (drawStartArrowhead %s)\n"
		"    (drawEndArrowhead %s)\n"
		"    (startArrowhead \"StandardArrow\")\n"
		"    (endArrowhead \"StandardArrow\")\n",
		arrow_at_end ? "T" : "F", arrow_at_start ? "T" : "F");
    } else {
	vtx_bzptarray(A, 1, n - 1);
	fprintf(Output_file, "    (curved T)\n");
	vtx_style();
	fprintf(Output_file, "    (drawStartArrowhead %s)\n"
		"    (drawEndArrowhead %s)\n"
		"    (startArrowhead \"StandardArrow\")\n"
		"    (endArrowhead \"StandardArrow\")\n",
		arrow_at_start ? "T" : "F", arrow_at_end ? "T" : "F");
    }
}

static void vtx_polygon(point * A, int n, int filled)
{
    int i;
    pointf mp, max, min;

    mp.x = 0;
    mp.y = 0;
    max.x = min.x = (double) A[0].x;
    max.y = min.y = (double) A[0].y;
    for (i = 0; i < n; i++) {
	mp.x += (double) A[i].x;
	mp.y += (double) A[i].y;
	max.x = MAX(max.x, (double) A[i].x);
	max.y = MAX(max.y, (double) A[i].y);
	min.x = MIN(min.x, (double) A[i].x);
	min.y = MIN(min.y, (double) A[i].y);
    }
    mp.x /= n;
    mp.y /= n;
    mp = vtx_pt(mp);
    max = vtx_pt(max);
    min = vtx_pt(min);
    fprintf(Output_file, "    (location %g %g)\n"
	    "    (size %g %g)\n",
	    mp.x, mp.y, max.x - min.x, max.y - min.y);
    vtx_node_style();
}

static void vtx_ellipse(point p, int rx, int ry, int filled)
{
    pointf mp;

    mp.x = (double) p.x;
    mp.y = (double) p.y;
    mp = vtx_pt(mp);
    fprintf(Output_file, "    (location %g %g)\n"
	    "    (size %g %g)\n",
	    mp.x, mp.y, (double) (rx + rx), (double) (ry + ry));
    vtx_node_style();
}

static void vtx_polyline(point * A, int n)
{
    vtx_ptarray(A, n);
    fprintf(Output_file, "    (curved F)\n");
    vtx_style();
}

static void vtx_usershape(usershape_t *us, boxf b, point *A, int n, boolean filled)
{
/* FIXME */
    int i;
    pointf mp, max, min;

    mp.x = 0;
    mp.y = 0;
    max.x = min.x = (double) A[0].x;
    max.y = min.y = (double) A[0].y;
    for (i = 0; i < n; i++) {
	mp.x += (double) A[i].x;
	mp.y += (double) A[i].y;
	max.x = MAX(max.x, (double) A[i].x);
	max.y = MAX(max.y, (double) A[i].y);
	min.x = MIN(min.x, (double) A[i].x);
	min.y = MIN(min.y, (double) A[i].y);
    }
    mp.x /= n;
    mp.y /= n;
    mp = vtx_pt(mp);
    max = vtx_pt(max);
    min = vtx_pt(min);
    fprintf(Output_file, "    (location %g %g)\n"
	    "    (size %g %g)\n",
	    mp.x, mp.y, max.x - min.x, max.y - min.y);
    vtx_node_style();
}

codegen_t VTX_CodeGen = {
    vtx_reset,
    vtx_begin_job, 0,		/* vtx_end_job */
    vtx_begin_graph, 0,		/* vtx_end_graph */
    vtx_begin_page, 0,		/* vtx_end_page */
    0, /* vtx_begin_layer */ 0,	/* vtx_end_layer */
    0, /* vtx_begin_cluster */ 0,	/* vtx_end_cluster */
    vtx_begin_nodes, vtx_end_nodes,
    vtx_begin_edges, vtx_end_edges,
    vtx_begin_node, vtx_end_node,
    vtx_begin_edge, vtx_end_edge,
    vtx_begin_context, vtx_end_context,
    0, /* vtx_begin_anchor */ 0,	/* vtx_end_anchor */
    vtx_set_font, vtx_textpara,
    vtx_set_color, vtx_set_color, vtx_set_style,
    vtx_ellipse, vtx_polygon,
    vtx_bezier, vtx_polyline,
    1,				/* bezier_has_arrows */
    vtx_comment,
    vtx_usershape
};
