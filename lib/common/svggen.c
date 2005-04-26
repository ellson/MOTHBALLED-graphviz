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


/* this is a rough start at a working SVG driver.
you can get image nodes by setting a node's
 [shape=webimage,shapefile="http://www.your.site.com/path/image.png"]
(of course can also be a file: reference if you run locally.
which causes a warning (FIX that) but the back end turns this into
a custom shape.  you can also set
[ href = "http://some.place.com/whatever" ] 
to get clickable nodes (and edges for that matter).

some major areas needing work:
0. fonts including embedded font support.  is SVG finished in this area?
1. styles, including dotted/dashed lines, also "style function"
passthrough in SVG similar to what we have in postscript.
2. look at what happens in landscape mode, pagination? etc.
3. allow arbitrary user transforms via graph "style" attribute.
4. javascript hooks.  particularly, look at this in the context
of SVG animation for dynadag output (n.b. dynadag only in alpha release)
5. image node improvement, e.g. set clip path to node boundary, and
support scaling with fixed aspect ratio (this feature seems to be
broken in the current Adobe SVG plugin for Windows).  
6. can we support arbitrary HTML for node contents?
7. encode abstract graph as interleaved XML in some sort of graph XML dialect?
8. accessibility features
9. embed directions about getting plugin, if not present in browser

Stephen North
north@research.att.com
*/

#include	"render.h"
#include	"utils.h"
#ifdef HAVE_LIBZ
#include	"zlib.h"
#endif
#ifdef MSWIN32
#include	<io.h>
#include	<stdarg.h>
#endif


/* SVG font modifiers */
#define REGULAR 0
#define BOLD	1
#define ITALIC	2

/* SVG patterns */
#define P_SOLID	0
#define P_NONE  15
#define P_DOTTED 4		/* i wasn't sure about this */
#define P_DASHED 11		/* or this */

/* SVG bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 3

#ifdef TESTFAILED
/* sodipodi doesn't understand style sheets */
#define DEFAULT_STYLE\
"<style type='text/css'>\n" \
"<![CDATA[\n" \
".node ellipse {fill:none;filter:URL(#MyFilter)}\n" \
".node polygon {fill:none;filter:URL(#MyFilter)}\n" \
".cluster polygon {fill:none;filter:URL(#MyFilter)}\n" \
".edge path {fill:none;stroke:black;stroke-width:1;}\n" \
"text {font-family:Times;stroke:black;stroke-width:.4;font-size:12px}\n" \
"]]>\n" \
"</style>"

#define DEFAULT_FILTER \
"<filter id=\"MyFilter\" x=\"-20%\" y=\"-20%\" width=\"160%\" height=\"160%\">\n" \
"<feGaussianBlur in=\"SourceAlpha\" stdDeviation=\"4\" result=\"blur\"/>\n" \
"<feOffset in=\"blur\" dx=\"4\" dy=\"4\" result=\"offsetBlur\"/>\n" \
"<feSpecularLighting in=\"blur\" surfaceScale=\"5\" specularConstant=\"1\"\n" \
"specularExponent=\"10\" style=\"lighting-color:white\" result=\"specOut\">\n" \
"<fePointLight x=\"-5000\" y=\"-10000\" z=\"20000\"/>\n" \
"</feSpecularLighting>\n" \
"<feComposite in=\"specOut\" in2=\"SourceAlpha\" operator=\"in\" result=\"specOut\"/>\n" \
"<feComposite in=\"SourceGraphic\" in2=\"specOut\" operator=\"arithmetic\"\n" \
"k1=\"0\" k2=\"1\" k3=\"1\" k4=\"0\" result=\"litPaint\"/>\n" \
"<feMerge>\n" \
"<feMergeNode in=\"offsetBlur\"/>\n" \
"<feMergeNode in=\"litPaint\"/>\n" \
"</feMerge>\n" \
"</filter>"
#endif

#define SCALE 1

/* SVG dash array */
static char *sdarray = "5,2";
/* SVG dot array */
static char *sdotarray = "1,5";

static int N_pages;
/* static 	point	Pages; */
static double Scale;
static pointf Offset;
static int Rot;
static box PB;
static int onetime = TRUE;

static node_t *Curnode;

typedef struct context_t {
    char *pencolor, *fillcolor, *fontfam, fontopt, font_was_set;
    char pen, fill, penwidth, style_was_set;
    double fontsz;
} context_t;

#define MAXNEST 4
static context_t cstk[MAXNEST];
static int SP;

static char *op[] = { "graph", "node", "edge", "graph" };

#ifdef HAVE_LIBZ
static gzFile Zfile;
#endif

static void svg_fputs(char *s)
{
    int len;

    len = strlen(s);
    switch (Output_lang) {
    case SVGZ:
#ifdef HAVE_LIBZ
	gzwrite(Zfile, s, (unsigned) len);
	break;
#else
	agerr(AGERR, "No libz support.\n");
	exit(1);
#endif
    case SVG:
	fwrite(s, sizeof(char), (unsigned) len, Output_file);
	break;
    }
}

/* svg_printf:
 * Note that this function is unsafe due to the fixed buffer size.
 * It should only be used when the caller is sure the input will not
 * overflow the buffer. In particular, it should be avoided for
 * input coming from users. Also, if vsnprintf is available, the
 * code should check for return values to use it safely.
 */
static void svg_printf(const char *format, ...)
{
    char buf[BUFSIZ];
    va_list argp;

    va_start(argp, format);
#ifdef HAVE_VSNPRINTF
    (void) vsnprintf(buf, sizeof(buf), format, argp);
#else
    (void) vsprintf(buf, format, argp);
#endif
    va_end(argp);

    svg_fputs(buf);
}

static void svg_reset(void)
{
    onetime = TRUE;
}

static void init_svg(void)
{
    SP = 0;
    cstk[0].pencolor = DEFAULT_COLOR;	/* SVG pencolor */
    cstk[0].fillcolor = "";	/* SVG fillcolor */
    cstk[0].fontfam = DEFAULT_FONTNAME;	/* font family name */
    cstk[0].fontsz = DEFAULT_FONTSIZE;	/* font size */
    cstk[0].fontopt = REGULAR;	/* modifier: REGULAR, BOLD or ITALIC */
    cstk[0].pen = P_SOLID;	/* pen pattern style, default is solid */
    cstk[0].fill = P_NONE;
    cstk[0].penwidth = WIDTH_NORMAL;
}

/* can't hack a transform directly in SVG preamble, alas */
static point svgpt(point p)
{
    point rv;

    if (Rot == 0) {
	rv.x = PB.LL.x / Scale + p.x + Offset.x;
	rv.y = PB.UR.y / Scale - 1 - p.y - Offset.y;
    } else {
	rv.x = PB.UR.x / Scale - 1 - p.y - Offset.x;
	rv.y = PB.UR.y / Scale - 1 - p.x - Offset.y;
    }
    return rv;
}

static void svgbzptarray(point * A, int n)
{
    int i;
    point p;
    char *c;

    c = "M";			/* first point */
    for (i = 0; i < n; i++) {
	p.x = A[i].x;
	p.y = A[i].y;
	p = svgpt(p);
	svg_printf("%s%d,%d", c, p.x, p.y);
	if (i == 0)
	    c = "C";		/* second point */
	else
	    c = " ";		/* remaining points */
    }
}

static int svg_comparestr(const void *s1, const void *s2)
{
    return strcmp(*(char **) s1, *(char **) s2);
}

char *svg_resolve_color(char *name, int useKnown)
{
/* color names from http://www.w3.org/TR/SVG/types.html */
/* NB.  List must be LANG_C sorted */
    static char *svg_known_colors[] = {
	"aliceblue", "antiquewhite", "aqua", "aquamarine", "azure",
	"beige", "bisque", "black", "blanchedalmond", "blue",
	"blueviolet", "brown", "burlywood",
	"cadetblue", "chartreuse", "chocolate", "coral",
	"cornflowerblue", "cornsilk", "crimson", "cyan",
	"darkblue", "darkcyan", "darkgoldenrod", "darkgray",
	"darkgreen", "darkgrey", "darkkhaki", "darkmagenta",
	"darkolivegreen", "darkorange", "darkorchid", "darkred",
	"darksalmon", "darkseagreen", "darkslateblue", "darkslategray",
	"darkslategrey", "darkturquoise", "darkviolet", "deeppink",
	"deepskyblue", "dimgray", "dimgrey", "dodgerblue",
	"firebrick", "floralwhite", "forestgreen", "fuchsia",
	"gainsboro", "ghostwhite", "gold", "goldenrod", "gray",
	"green", "greenyellow", "grey",
	"honeydew", "hotpink", "indianred",
	"indigo", "ivory", "khaki",
	"lavender", "lavenderblush", "lawngreen", "lemonchiffon",
	"lightblue", "lightcoral", "lightcyan", "lightgoldenrodyellow",
	"lightgray", "lightgreen", "lightgrey", "lightpink",
	"lightsalmon", "lightseagreen", "lightskyblue",
	"lightslategray", "lightslategrey", "lightsteelblue",
	"lightyellow", "lime", "limegreen", "linen",
	"magenta", "maroon", "mediumaquamarine", "mediumblue",
	"mediumorchid", "mediumpurple", "mediumseagreen",
	"mediumslateblue", "mediumspringgreen", "mediumturquoise",
	"mediumvioletred", "midnightblue", "mintcream",
	"mistyrose", "moccasin",
	"navajowhite", "navy", "oldlace",
	"olive", "olivedrab", "orange", "orangered", "orchid",
	"palegoldenrod", "palegreen", "paleturquoise",
	"palevioletred", "papayawhip", "peachpuff", "peru", "pink",
	"plum", "powderblue", "purple",
	"red", "rosybrown", "royalblue",
	"saddlebrown", "salmon", "sandybrown", "seagreen", "seashell",
	"sienna", "silver", "skyblue", "slateblue", "slategray",
	"slategrey", "snow", "springgreen", "steelblue",
	"tan", "teal", "thistle", "tomato", "turquoise",
	"violet",
	"wheat", "white", "whitesmoke",
	"yellow", "yellowgreen",
    };

    static char buf[SMALLBUF];
    char *tok;
    color_t color;

    tok = canontoken(name);
    if (!useKnown || (bsearch(&tok, svg_known_colors,
			      sizeof(svg_known_colors) / sizeof(char *),
			      sizeof(char *), svg_comparestr) == NULL)) {
	/* if tok was not found in known_colors */
	if (streq(tok, "transparent")) {
	    tok = "none";
	} else {
	    colorxlate(name, &color, RGBA_BYTE);
	    sprintf(buf, "#%02x%02x%02x",
		    color.u.rgba[0], color.u.rgba[1], color.u.rgba[2]);
	    tok = buf;
	}
    }
    return tok;
}

static void svg_font(context_t * cp)
{
    char *color, buf[BUFSIZ];
    int needstyle = 0;

    strcpy(buf, " style=\"");
    if (strcasecmp(cp->fontfam, DEFAULT_FONTNAME)) {
	sprintf(buf + strlen(buf), "font-family:%s;", cp->fontfam);
	needstyle++;
    }
    if (cp->fontsz != DEFAULT_FONTSIZE) {
	sprintf(buf + strlen(buf), "font-size:%.2f;", (cp->fontsz));
	needstyle++;
    }
    color = svg_resolve_color(cp->pencolor, 1);
    if ((strcasecmp(color, "black"))) {
	sprintf(buf + strlen(buf), "fill:%s;", color);
	needstyle++;
    }
    if (needstyle) {
	strcat(buf, "\"");
	svg_fputs(buf);
    }
}


static void svg_grstyle(context_t * cp, int filled)
{
    svg_fputs(" style=\"");
    if (filled)
	svg_printf("fill:%s;", svg_resolve_color(cp->fillcolor, 1));
    else
	svg_fputs("fill:none;");
    svg_printf("stroke:%s;", svg_resolve_color(cp->pencolor, 1));
    if (cp->penwidth != WIDTH_NORMAL)
	svg_printf("stroke-width:%d;", cp->penwidth);
    if (cp->pen == P_DASHED) {
	svg_printf("stroke-dasharray:%s;", sdarray);
    } else if (cp->pen == P_DOTTED) {
	svg_printf("stroke-dasharray:%s;", sdotarray);
    }
    svg_fputs("\"");
}

static void svg_comment(char *str)
{
    svg_fputs("<!-- ");
    svg_fputs(xml_string(str));
    svg_fputs(" -->\n");
}

static void
svg_begin_job(FILE * ofp, graph_t * g, char **lib, char *user,
	      char *info[], point pages)
{
    char *s;
#if HAVE_LIBZ
    int fd;
#endif

    switch (Output_lang) {
    case SVGZ:
#if HAVE_LIBZ
	fd = dup(fileno(Output_file));	/* open dup so can gzclose 
					   independent of FILE close */
#ifdef HAVE_SETMODE
#ifdef O_BINARY
	/*
	 * Windows will do \n -> \r\n  translations on
	 * stdout unless told otherwise.
	 */
	setmode(fd, O_BINARY);
#endif
#endif

	Zfile = gzdopen(fd, "wb");
	if (!Zfile) {
	    agerr(AGERR, "Error opening compressed output file\n");
	    exit(1);
	}
	break;
#else
	agerr(AGERR, "No libz support.\n");
	exit(1);
#endif
    case SVG:
	break;
    }

/*	Pages = pages; */
    N_pages = pages.x * pages.y;
    svg_fputs
	("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    if ((s = agget(g, "stylesheet")) && s[0]) {
	svg_fputs("<?xml-stylesheet href=\"");
	svg_fputs(s);
	svg_fputs("\" type=\"text/css\"?>\n");
    }
    svg_fputs("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\"\n");
    svg_fputs
	(" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\"");
#if 1
    /* This is to work around a bug in the SVG 1.0 DTD */
    if ((agfindattr(g, "href")
	 || agfindattr(g->proto->n, "href")
	 || agfindattr(g->proto->e, "href")
	 || agfindattr(g, "URL")
	 || agfindattr(g->proto->n, "URL")
	 || agfindattr(g->proto->e, "URL"))) {
	svg_fputs
	    (" [\n <!ATTLIST svg xmlns:xlink CDATA #FIXED \"http://www.w3.org/1999/xlink\">\n]");
    }
#endif
    svg_fputs(">\n<!-- Generated by ");
    svg_fputs(xml_string(info[0]));
    svg_fputs(" version ");
    svg_fputs(xml_string(info[1]));
    svg_fputs(" (");
    svg_fputs(xml_string(info[2]));
    svg_fputs(")\n     For user: ");
    svg_fputs(xml_string(user));
    svg_fputs("   Title: ");
    svg_fputs(xml_string(g->name));
    svg_printf("    Pages: %d -->\n", N_pages);
}

static void svg_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    double dpi = GD_drawing(g)->dpi;

    PB.LL.x = PB.LL.y = 0;
    PB.UR.x = (bb.UR.x - bb.LL.x + 2 * GD_drawing(g)->margin.x) * SCALE;
    PB.UR.y = (bb.UR.y - bb.LL.y + 2 * GD_drawing(g)->margin.y) * SCALE;
    Offset.x = GD_drawing(g)->margin.x * SCALE;
    Offset.y = GD_drawing(g)->margin.y * SCALE;
    if (onetime) {
#if 0
	fprintf(stderr, "LL %d %d UR %d %d\n", PB.LL.x, PB.LL.y, PB.UR.x,
		PB.UR.y);
#endif
	init_svg();
	onetime = FALSE;
    }
    if (dpi < 1.0)
	svg_printf("<svg width=\"%dpt\" height=\"%dpt\"\n",
		   PB.UR.x - PB.LL.x + 2, PB.UR.y - PB.LL.y + 2);
    else
	svg_printf("<svg width=\"%dpx\" height=\"%dpx\"\n",
		   ROUND(dpi / POINTS_PER_INCH) * (PB.UR.x - PB.LL.x + 2),
		   ROUND(dpi / POINTS_PER_INCH) * (PB.UR.y - PB.LL.y + 2));
    /* establish absolute units in points */
    svg_printf(" viewBox = \"%d %d %d %d\"\n", PB.LL.x - 1, PB.LL.y - 1,
	       PB.UR.x + 1, PB.UR.y + 1);
    /* namespace of svg */
    svg_fputs(" xmlns=\"http://www.w3.org/2000/svg\"");
    /* namespace of xlink  if needed */
    if ((agfindattr(g, "href")
	 || agfindattr(g->proto->n, "href")
	 || agfindattr(g->proto->e, "href")
	 || agfindattr(g, "URL")
	 || agfindattr(g->proto->n, "URL")
	 || agfindattr(g->proto->e, "URL"))) {
	svg_fputs(" xmlns:xlink=\"http://www.w3.org/1999/xlink\"");
    }
    svg_fputs(">\n");
}

static void svg_end_graph(void)
{
    svg_fputs("</svg>\n");
    switch (Output_lang) {
    case SVGZ:
#ifdef HAVE_LIBZ
	gzclose(Zfile);
	break;
#else
	agerr(AGERR, "No libz support\n");
	exit(1);
#endif
    case SVG:
	break;
    }
}

static void
svg_begin_page(graph_t * g, point page, double scale, int rot,
	       point offset)
{
/*	int		page_number; */
    /* point        sz; */

    Scale = scale;
    Rot = rot;
/*	page_number =  page.x + page.y * Pages.x + 1; */
    /* sz = sub_points(PB.UR,PB.LL); */

    /* its really just a page of the graph, but its still a graph,
     * and it is the entire graph if we're not currently paging */
    svg_printf("<g id=\"%s0\" class=\"graph\"", op[Obj]);
    if (scale != 1.0)
	svg_printf(" transform = \"scale(%f)\"\n", scale);
    /* default style */
    svg_fputs(" style=\"font-family:");
    svg_fputs(cstk[0].fontfam);
    svg_printf(";font-size:%.2f;\">\n", cstk[0].fontsz);
    svg_fputs("<title>");
    svg_fputs(xml_string(g->name));
    svg_fputs("</title>\n");
}

static void svg_end_page(void)
{
    svg_fputs("</g>\n");
}

static void svg_begin_layer(char *layerName, int n, int nLayers)
{
    /* svg_printf("<g id=\"%s\" class=\"layer\">\n", xml_string(layerName)); */
    svg_fputs("<g id=\"");
    svg_fputs(xml_string(layerName));
    svg_fputs("\" class=\"layer\">\n");
    Obj = NONE;
}

static void svg_end_layer(void)
{
    svg_fputs("</g>\n");
    Obj = NONE;
}

static void svg_begin_cluster(graph_t * g)
{
    svg_printf("<g id=\"%s%d\" class=\"cluster\">", op[Obj],
	       g->meta_node->id);
    svg_fputs("<title>");
    svg_fputs(xml_string(g->name));
    svg_fputs("</title>\n");
}

static void svg_end_cluster(void)
{
    svg_fputs("</g>\n");
}

static void svg_begin_node(node_t * n)
{
    Curnode = n;
    svg_printf("<g id=\"%s%d\" class=\"node\">", op[Obj], n->id);
    svg_fputs("<title>");
    svg_fputs(xml_string(n->name));
    svg_fputs("</title>\n");
}

static void svg_end_node(void)
{
    svg_fputs("</g>\n");
}

static void svg_begin_edge(edge_t * e)
{
    char *edgeop;

    svg_printf("<g id=\"%s%d\" class=\"edge\">", op[Obj], e->id);
    if (e->tail->graph->root->kind & AGFLAG_DIRECTED)
	edgeop = "&#45;&gt;";
    else
	edgeop = "&#45;&#45;";
    svg_fputs("<title>");
    svg_fputs(xml_string(e->tail->name));
    svg_fputs(edgeop);
    /* can't do this in single svg_printf because
     * xml_string's buffer gets reused. */
    svg_fputs(xml_string(e->head->name));
    svg_fputs("</title>\n");
}

static void svg_end_edge(void)
{
    svg_fputs("</g>\n");
}

static void svg_begin_context(void)
{
    assert(SP + 1 < MAXNEST);
    cstk[SP + 1] = cstk[SP];
    SP++;
}

static void svg_begin_anchor(char *href, char *tooltip, char *target)
{
    svg_fputs("<a xlink:href=\"");
    svg_fputs(xml_string(href));
    if (tooltip && tooltip[0]) {
	svg_fputs("\" xlink:title=\"");
	svg_fputs(xml_string(tooltip));
    }
    if (target && target[0]) {
	svg_fputs("\" target=\"");
	svg_fputs(xml_string(target));
    }
    svg_fputs("\">\n");
}

static void svg_end_anchor(void)
{
    svg_fputs("</a>\n");
}

static void svg_end_context(void)
{
    int psp = SP - 1;
    assert(SP > 0);
    /*free(cstk[psp].fontfam); */
    SP = psp;
}

static void svg_set_font(char *name, double size)
{
    char *p;
#if 0
/* see below */
    char *q;
#endif
    context_t *cp;

    cp = &(cstk[SP]);
    cp->font_was_set = TRUE;
    cp->fontsz = size;
    p = strdup(name);
#if 0
/*
 * this doesn't work as originally intended 
 * fontnames can be things like: "Times-Roman"
 * where "Roman" is not an ITALIC or BOLD indicator.
 */
    if ((q = strchr(p, '-'))) {
	*q++ = 0;
	if (strcasecmp(q, "italic") == 0)
	    cp->fontopt = ITALIC;
	else if (strcasecmp(q, "bold") == 0)
	    cp->fontopt = BOLD;
    }
    cp->fontfam = p;
#else
    cp->fontfam = p;
#endif
}

static void svg_set_pencolor(char *name)
{
    cstk[SP].pencolor = name;
}

static void svg_set_fillcolor(char *name)
{
    cstk[SP].fillcolor = name;
}

static void svg_set_style(char **s)
{
    char *line, *p;
    context_t *cp;

    cp = &(cstk[SP]);
    while ((p = line = *s++)) {
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
	else if (streq(line, "setlinewidth")) {
	    while (*p)
		p++;
	    p++;
	    cp->penwidth = atol(p);
	} else if (streq(line, "filled"))
	    cp->fill = P_SOLID;
	else if (streq(line, "unfilled"))
	    cp->fill = P_NONE;
	else {
	    agerr(AGERR,
		  "svg_set_style: unsupported style %s - ignoring\n",
		  line);
	}
	cp->style_was_set = TRUE;
    }
    /* if (cp->style_was_set) svg_style(cp); */
}

static void svg_textline(point p, textline_t * line)
{
    char *anchor, *string;
    point mp;
    context_t *cp;

    string = xml_string(line->str);
    if (!string[0])
	return;

    cp = &(cstk[SP]);
    if (cp->pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
#if 0
/* adobe svg doesn't like the dx propert */
    if (line->xshow) {
	anchor = "start";
	switch (line->just) {
	case 'l':
	    break;
	case 'r':
	    p.x -= line->width;
	    break;
	default:
	case 'n':
	    p.x -= line->width / 2;
	    break;
	}
    } else {
#endif
	switch (line->just) {
	case 'l':
	    anchor = "start";
	    break;
	case 'r':
	    anchor = "end";
	    break;
	default:
	case 'n':
	    anchor = "middle";
	    break;
	}
#if 0
/* adobe svg doesn't like the dx propert */
    }
#endif
    mp = svgpt(p);

    svg_printf("<text text-anchor=\"%s\"", anchor);
    if (Rot) {
	svg_printf(" transform=\"rotate(-90 %d %d)\"", mp.x, mp.y);
    }
    svg_font(cp);
    svg_printf(" x=\"%d\" y=\"%d", mp.x, mp.y);
#if 0
/* adobe svg doesn't like the dx propert */
    if (line->xshow) {
	svg_fputs("\" dx=\"");
	svg_fputs(line->xshow);
    }
#endif
    svg_fputs("\">");
    svg_fputs(string);
    svg_fputs("</text>\n");
}

static void svg_ellipse(point p, int rx, int ry, int filled)
{
    point mp;

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    mp.x = p.x;
    mp.y = p.y;
    mp = svgpt(mp);
    svg_printf("<ellipse cx=\"%d\" cy=\"%d\"", mp.x, mp.y);
    if (Rot) {
	int t;
	t = rx;
	rx = ry;
	ry = t;
    }
    mp.x = rx;
    mp.y = ry;
    svg_printf(" rx=\"%d\" ry=\"%d\"", mp.x, mp.y);
    svg_grstyle(&cstk[SP], filled);
    svg_fputs("/>\n");
}

static void
svg_bezier(point * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    svg_fputs("<path");
    svg_grstyle(&cstk[SP], filled);
    svg_fputs(" d=\"");
    svgbzptarray(A, n);
    svg_fputs("\"/>\n");
}

static void svg_polygon(point * A, int n, int filled)
{
    int i;
    point p;

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    svg_fputs("<polygon");
    svg_grstyle(&cstk[SP], filled);
    svg_fputs(" points=\"");
    for (i = 0; i < n; i++) {
	p = svgpt(A[i]);
	svg_printf("%d,%d ", p.x, p.y);
    }
    /* because Adobe SVG is broken */
    p = svgpt(A[0]);
    svg_printf("%d,%d", p.x, p.y);
    svg_fputs("\"/>\n");
}

static void svg_polyline(point * A, int n)
{
    int i;
    point p;

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    svg_fputs("<polyline");
    svg_grstyle(&cstk[SP], 0);
    svg_fputs(" points=\"");
    for (i = 0; i < n; i++) {
	p = svgpt(A[i]);
	svg_printf("%d,%d ", p.x, p.y);
    }
    svg_fputs("\"/>\n");
}

static void svg_user_shape(char *name, point * A, int n, int filled)
{
    int i;
    point p;
    point sz;
    char *imagefile;
    int minx, miny;

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    if (streq(name, "custom"))
	imagefile = agget(Curnode, "shapefile");
    else
	imagefile = name;
    if (imagefile == 0) {
	svg_polygon(A, n, filled);
	return;
    }
    p = ND_coord_i(Curnode);
    p.x -= ND_lw_i(Curnode);
    p.y += ND_ht_i(Curnode) / 2;
    p = svgpt(p);
    sz.x = ROUND((ND_lw_i(Curnode) + ND_rw_i(Curnode)));
    sz.y = ROUND(ND_ht_i(Curnode));

    svg_fputs("<clipPath id=\"mypath");
    svg_fputs(name);
    svg_fputs(Curnode->name);
    svg_fputs("\">\n<polygon points=\"");
    minx = svgpt(A[0]).x;
    miny = svgpt(A[0]).y;
    for (i = 0; i < n; i++) {
	p = svgpt(A[i]);

	if (p.x < minx)
	    minx = p.x;
	if (p.y < miny)
	    miny = p.y;

	svg_printf("%d,%d ", p.x, p.y);
    }
    /* because Adobe SVG is broken (?) */
    p = svgpt(A[0]);
    svg_printf("%d,%d ", p.x, p.y);
    svg_fputs("\"/>\n</clipPath>\n<image xlink:href=\"");
    svg_fputs(imagefile);
    svg_printf
	("\" width=\"%dpx\" height=\"%dpx\" preserveAspectRatio=\"xMidYMid meet\" x=\"%d\" y=\"%d\" clip-path=\"url(#mypath",
	 sz.x, sz.y, minx, miny);
    svg_fputs(name);
    svg_fputs(Curnode->name);
    svg_fputs(")\"/>\n");
}

codegen_t SVG_CodeGen = {
    svg_reset,
    svg_begin_job, 0,		/* svg_end_job */
    svg_begin_graph, svg_end_graph,
    svg_begin_page, svg_end_page,
    svg_begin_layer, svg_end_layer,
    svg_begin_cluster, svg_end_cluster,
    0, /* svg_begin_nodes */ 0,	/* svg_end_nodes */
    0, /* svg_begin_edges */ 0,	/* svg_end_edges */
    svg_begin_node, svg_end_node,
    svg_begin_edge, svg_end_edge,
    svg_begin_context, svg_end_context,
    svg_begin_anchor, svg_end_anchor,
    svg_set_font, svg_textline,
    svg_set_pencolor, svg_set_fillcolor, svg_set_style,
    svg_ellipse, svg_polygon,
    svg_bezier, svg_polyline,
    0,				/* bezier_has_arrows */
    svg_comment,
    0,				/* svg_textsize */
    svg_user_shape,
    0				/* svg_usershape_size */
};
