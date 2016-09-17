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

#include "config.h"

#include "render.h"

#include <stdarg.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

/* DIA font modifiers */
#define REGULAR 0
#define BOLD	1
#define ITALIC	2

/* DIA patterns */
#define P_SOLID	0
#define P_NONE  15
#define P_DOTTED 4		/* i wasn't sure about this */
#define P_DASHED 11		/* or this */

/* DIA bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 3

#define DIA_RESOLUTION 1.0
#define SCALE (DIA_RESOLUTION/15.0)

#define		NODE		1
#define		EDGE		2
#define		CLST		3


/* FIXME - these are not used currently - probably something missing */
#if 0
/* DIA dash array */
static char *sdarray = "5,2";
/* DIA dot array */
static char *sdotarray = "1,5";
static int GraphURL, ClusterURL, NodeURL, EdgeURL;
static char *op[] = { "graph", "node", "edge", "graph" };
#endif

/* static	int		N_pages; */
/* static 	point	Pages; */
static double Scale;
static pointf Offset;
static int Rot;
static box PB;
static int onetime = TRUE;

static node_t *Curnode;
static edge_t *Curedge;
static graph_t *Curgraph, *Rootgraph;

typedef struct context_t {
    char *pencolor, *fillcolor, *fontfam, fontopt, font_was_set;
    char pen, fill, penwidth, style_was_set;
    double fontsz;
} context_t;

#define MAXNEST 4
static context_t cstk[MAXNEST];
static int SP;

#ifdef HAVE_LIBZ
static gzFile Zfile;
#endif

static int dia_fputs(char *s)
{
    int len;

    len = strlen(s);

#ifdef HAVE_LIBZ
    return gzwrite(Zfile, s, (unsigned) len);
#else
    return 0;
#endif
}


/* dia_printf:
 * Note that this function is unsafe due to the fixed buffer size.
 * It should only be used when the caller is sure the input will not
 * overflow the buffer. In particular, it should be avoided for
 * input coming from users. Also, if vsnprintf is available, the
 * code should check for return values to use it safely.
 */
static int dia_printf(const char *format, ...)
{
    char buf[BUFSIZ];
    va_list argp;
    int len;

    va_start(argp, format);
#ifdef HAVE_VSNPRINTF
    (void) vsnprintf(buf, sizeof(buf), format, argp);
#else
    (void) vsprintf(buf, format, argp);
#endif
    va_end(argp);
    len = strlen(buf);

    /* some *sprintf (e.g C99 std)
       don't return the number of
       bytes actually written */

#ifdef HAVE_LIBZ
    return gzwrite(Zfile, buf, (unsigned) len);
#else
    return 0;
#endif
}

#define SVG_COLORS_P 0

static int dia_comparestr(const void *s1, const void *s2)
{
        return strcmp(*(char **) s1, *(char **) s2);
}

static char *dia_resolve_color(char *name)
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
    gvcolor_t color;

    tok = canontoken(name);
    if (!SVG_COLORS_P || (bsearch(&tok, svg_known_colors,
                              sizeof(svg_known_colors) / sizeof(char *),
                              sizeof(char *), dia_comparestr) == NULL)) {
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


static void dia_reset(void)
{
    onetime = TRUE;
}


static void init_dia(void)
{
    SP = 0;
    cstk[0].pencolor = DEFAULT_COLOR;	/* DIA pencolor */
    cstk[0].fillcolor = "";	/* DIA fillcolor */
    cstk[0].fontfam = DEFAULT_FONTNAME;	/* font family name */
    cstk[0].fontsz = DEFAULT_FONTSIZE;	/* font size */
    cstk[0].fontopt = REGULAR;	/* modifier: REGULAR, BOLD or ITALIC */
    cstk[0].pen = P_SOLID;	/* pen pattern style, default is solid */
    cstk[0].fill = P_NONE;
    cstk[0].penwidth = WIDTH_NORMAL;
}

static pointf diapt(point p)
{
    pointf rv;

    if (Rot == 0) {
	rv.x = PB.LL.x + p.x * Scale + Offset.x;
	rv.y = PB.UR.y - 1 - p.y * Scale - Offset.y;
    } else {
	rv.x = PB.UR.x - 1 - p.y * Scale - Offset.x;
	rv.y = PB.UR.y - 1 - p.x * Scale - Offset.y;
    }
    return rv;
}

static pointf diaptf(pointf p)
{
    pointf rv;

    if (Rot == 0) {
	rv.x = PB.LL.x + p.x * Scale + Offset.x;
	rv.y = PB.UR.y - 1 - p.y * Scale - Offset.y;
    } else {
	rv.x = PB.UR.x - 1 - p.y * Scale - Offset.x;
	rv.y = PB.UR.y - 1 - p.x * Scale - Offset.y;
    }
    return rv;
}

static void dia_grstyle(context_t * cp)
{
    if (strcmp(cp->pencolor, DEFAULT_COLOR)) {
	dia_fputs("      <dia:attribute name=\"border_color\">\n");
	dia_printf("        <dia:color val=\"%s\"/>\n",
		   dia_resolve_color(cp->pencolor));
	dia_fputs("      </dia:attribute>\n");
    }
    if (cp->penwidth != WIDTH_NORMAL) {
	dia_fputs("      <dia:attribute name=\"line_width\">\n");
	dia_printf("        <dia:real val=\"%g\"/>\n",
		   Scale * (cp->penwidth));
	dia_fputs("      </dia:attribute>\n");
    }
    if (cp->pen == P_DASHED) {
	dia_fputs("      <dia:attribute name=\"line_style\">\n");
	dia_printf("        <dia:real val=\"%d\"/>\n", 1);
	dia_fputs("      </dia:attribute>\n");
#if 0
    } else if (cp->pen == P_DOTTED) {
	dia_printf("stroke-dasharray:%s;", sdotarray);
#endif
    }
}

static void dia_grstylefill(context_t * cp, int filled)
{
    if (filled) {
	dia_fputs("      <dia:attribute name=\"inner_color\">\n");
	dia_printf("        <dia:color val=\"%s\"/>\n",
		   dia_resolve_color(cp->fillcolor));
	dia_fputs("      </dia:attribute>\n");
    } else {
	dia_fputs("      <dia:attribute name=\"show_background\">\n");
	dia_printf("        <dia:boolean val=\"%s\"/>\n", "true");
	dia_fputs("      </dia:attribute>\n");
    }
}

static void dia_comment(char *str)
{
    dia_fputs("<!-- ");
    dia_fputs(xml_string(str));
    dia_fputs(" -->\n");
}

static void
dia_begin_job(FILE * ofp, graph_t * g, const char **lib, char *info[], point pages)
{
#if HAVE_LIBZ
    int fd;

    fd = dup(fileno(Output_file));	/* open dup so can gzclose 
					   independent of FILE close */
    Zfile = gzdopen(fd, "wb");
    if (!Zfile) {
	agerr(AGERR, "Error opening compressed output file\n");
	exit(1);
    }
#else
    agerr(AGERR,
	  "No support for compressed output. Not compiled with zlib.\n");
    exit(1);
#endif

/*	Pages = pages; */
/*	N_pages = pages.x * pages.y; */
    dia_printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

}

static void dia_end_job(void)
{
}

static void dia_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    Rootgraph = g;
    PB.LL.x = PB.LL.y = 0;
    PB.UR.x = (bb.UR.x - bb.LL.x + 2 * GD_drawing(g)->margin.x) * SCALE;
    PB.UR.y = (bb.UR.y - bb.LL.y + 2 * GD_drawing(g)->margin.y) * SCALE;
    Offset.x = GD_drawing(g)->margin.x * SCALE;
    Offset.y = GD_drawing(g)->margin.y * SCALE;
    if (onetime) {
	init_dia();
	onetime = FALSE;
    }
    dia_fputs
	("<dia:diagram xmlns:dia=\"http://www.lysator.liu.se/~alla/dia/\">\n");
    dia_fputs("  <dia:diagramdata>\n");
    dia_fputs("    <dia:attribute name=\"background\">\n");
    dia_fputs("      <dia:color val=\"#ffffff\"/>\n");
    dia_fputs("    </dia:attribute>\n");
    dia_fputs("    <dia:attribute name=\"paper\">\n");
    dia_fputs("      <dia:composite type=\"paper\">\n");
    dia_fputs("        <dia:attribute name=\"name\">\n");
    dia_fputs("          <dia:string>#A4#</dia:string>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"tmargin\">\n");
    dia_fputs("          <dia:real val=\"2.8222\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"bmargin\">\n");
    dia_fputs("          <dia:real val=\"2.8222\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"lmargin\">\n");
    dia_fputs("          <dia:real val=\"2.8222\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"rmargin\">\n");
    dia_fputs("          <dia:real val=\"2.8222\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"is_portrait\">\n");
    dia_fputs("          <dia:boolean val=\"true\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"scaling\">\n");
    dia_fputs("          <dia:real val=\"1\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"fitto\">\n");
    dia_fputs("          <dia:boolean val=\"false\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("      </dia:composite>\n");
    dia_fputs("    </dia:attribute>\n");
    dia_fputs("    <dia:attribute name=\"grid\">\n");
    dia_fputs("      <dia:composite type=\"grid\">\n");
    dia_fputs("        <dia:attribute name=\"width_x\">\n");
    dia_fputs("          <dia:real val=\"1\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"width_y\">\n");
    dia_fputs("          <dia:real val=\"1\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"visible_x\">\n");
    dia_fputs("          <dia:int val=\"1\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("        <dia:attribute name=\"visible_y\">\n");
    dia_fputs("          <dia:int val=\"1\"/>\n");
    dia_fputs("        </dia:attribute>\n");
    dia_fputs("      </dia:composite>\n");
    dia_fputs("    </dia:attribute>\n");
    dia_fputs("    <dia:attribute name=\"guides\">\n");
    dia_fputs("      <dia:composite type=\"guides\">\n");
    dia_fputs("        <dia:attribute name=\"hguides\"/>\n");
    dia_fputs("        <dia:attribute name=\"vguides\"/>\n");
    dia_fputs("      </dia:composite>\n");
    dia_fputs("    </dia:attribute>\n");
    dia_fputs("  </dia:diagramdata>\n");
}

static void dia_end_graph(void)
{
    dia_printf("</dia:diagram>\n");
#ifdef HAVE_LIBZ
    gzclose(Zfile);
#endif
}

static void
dia_begin_page(graph_t * g, point page, double scale, int rot,
	       point offset)
{
    /* int          page_number; */
    /* point        sz; */

    Scale = scale * SCALE;
    Rot = rot;
    /* page_number =  page.x + page.y * Pages.x + 1; */
    /* sz = sub_points(PB.UR,PB.LL); */

    dia_printf("  <dia:layer name=\"Background\" visible=\"true\">\n");
}

static void dia_end_page(void)
{
    dia_fputs("  </dia:layer>\n");
}

static void dia_begin_cluster(graph_t * g)
{
    dia_printf("<dia:group>\n");
    Curgraph = g;
}

static void dia_end_cluster(void)
{
    dia_printf("</dia:group>\n");
}

static void dia_begin_node(node_t * n)
{
    dia_printf("<dia:group>\n");
    Curnode = n;
}

static void dia_end_node(void)
{
    dia_printf("</dia:group>\n");
}

static void dia_begin_edge(edge_t * e)
{
    Curedge = e;
}

static void dia_end_edge(void)
{
    Curedge = NULL;
}

static void dia_begin_context(void)
{
    assert(SP + 1 < MAXNEST);
    cstk[SP + 1] = cstk[SP];
    SP++;
}

static void dia_end_context(void)
{
    int psp = SP - 1;
    assert(SP > 0);
    /*free(cstk[psp].fontfam); */
    SP = psp;
}

static void dia_set_font(char *name, double size)
{
    char *p;
    context_t *cp;

    cp = &(cstk[SP]);
    cp->font_was_set = TRUE;
    cp->fontsz = size;
    p = strdup(name);
    cp->fontfam = p;
}

static void dia_set_pencolor(char *name)
{
    cstk[SP].pencolor = name;
}

static void dia_set_fillcolor(char *name)
{
    cstk[SP].fillcolor = name;
}

static void dia_set_style(char **s)
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
	    agerr(AGWARN,
		  "dia_set_style: unsupported style %s - ignoring\n",
		  line);
	}
	cp->style_was_set = TRUE;
    }
    /* if (cp->style_was_set) dia_style(cp); */
}

static void dia_textpara(point p, textpara_t * para)
{
    int anchor;
    pointf mp;
    context_t *cp;

    cp = &(cstk[SP]);
    switch (para->just) {
    case 'l':
	anchor = 0;
	break;
    case 'r':
	anchor = 2;
	break;
    default:
    case 'n':
	anchor = 1;
	break;
    }

    mp = diapt(p);
    dia_printf
	("    <dia:object type=\"Standard - Text\" version=\"0\" id=\"%s\">\n",
	 "0");
    dia_fputs("      <dia:attribute name=\"text\">\n");
    dia_fputs("        <dia:composite type=\"text\">\n");
    dia_fputs("          <dia:attribute name=\"string\">\n");
    dia_fputs("            <dia:string>#");
    dia_fputs(xml_string(para->str));
    dia_fputs("#</dia:string>\n");
    dia_fputs("          </dia:attribute>\n");
    dia_fputs("          <dia:attribute name=\"font\">\n");
    dia_printf("            <dia:font name=\"%s\"/>\n", cp->fontfam);
    dia_fputs("          </dia:attribute>\n");
    dia_fputs("          <dia:attribute name=\"height\">\n");
    dia_printf("            <dia:real val=\"%g\"/>\n",
	       Scale * (cp->fontsz));
    dia_fputs("          </dia:attribute>\n");
    dia_fputs("          <dia:attribute name=\"pos\">\n");
    dia_printf("            <dia:point val=\"%g,%g\"/>\n", mp.x, mp.y);
    dia_fputs("          </dia:attribute>\n");
    dia_fputs("          <dia:attribute name=\"color\">\n");
    dia_printf("            <dia:color val=\"%s\"/>\n",
	       dia_resolve_color(cp->pencolor));
    dia_fputs("          </dia:attribute>\n");
    dia_fputs("          <dia:attribute name=\"alignment\">\n");
    dia_printf("            <dia:enum val=\"%d\"/>\n", anchor);
    dia_fputs("          </dia:attribute>\n");
    dia_fputs("        </dia:composite>\n");
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"obj_pos\">\n");
    dia_printf("        <dia:point val=\"%g,%g\"/>\n", mp.x, mp.y);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"obj_bb\">\n");
    dia_printf("        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       mp.x - (Scale * (para->width) / 2.), mp.y - 0.4,
	       mp.x + (Scale * (para->width) / 2.), mp.y + 0.4);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("    </dia:object>\n");
}

static void dia_ellipse(point p, int rx, int ry, int filled)
{
    pointf cp, rp;
    int nodeId;

    switch (Obj) {
    case NODE:
	nodeId = AGID(Curnode);
	break;
    default:
	nodeId = -1;
	break;
    }

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    cp = diapt(p);

    if (Rot) {
	int t;
	t = rx;
	rx = ry;
	ry = t;
    }
    rp.x = Scale * rx;
    rp.y = Scale * ry;

    dia_printf
	("    <dia:object type=\"Standard - Ellipse\" version=\"0\" id=\"%d\">\n",
	 nodeId);
    dia_fputs("      <dia:attribute name=\"elem_corner\">\n");
    dia_printf("        <dia:point val=\"%g,%g\"/>\n", cp.x - rp.x,
	       cp.y - rp.y);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"elem_width\">\n");
    dia_printf("        <dia:real val=\"%g\"/>\n", rp.x + rp.x);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"elem_height\">\n");
    dia_printf("        <dia:real val=\"%g\"/>\n", rp.y + rp.y);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"obj_pos\">\n");
    dia_printf("        <dia:point val=\"%g,%g\"/>\n", cp.x - rp.x,
	       cp.y - rp.y);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"obj_bb\">\n");
    dia_printf("        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       cp.x - rp.x - .11, cp.y - rp.y - .11, cp.x + rp.x + .11,
	       cp.y + rp.y + .11);
    dia_fputs("      </dia:attribute>\n");
    dia_grstyle(&cstk[SP]);
    dia_grstylefill(&cstk[SP], filled);
    dia_fputs("    </dia:object>\n");
}


int ellipse_connection(pointf cp, pointf p)
{
    int conn = 0;

    if (cp.x == p.x) {
	if (cp.y > p.y)
	    conn = 1;
	else
	    conn = 6;
    } else if (cp.y == p.y) {
	if (cp.x > p.x)
	    conn = 3;
	else
	    conn = 4;
    } else if (cp.x < p.x) {
	if (cp.y < p.y)
	    conn = 7;
	else
	    conn = 2;
    } else if (cp.x > p.x) {
	if (cp.y < p.y)
	    conn = 5;
	else
	    conn = 0;
    }

    return conn;
}


int box_connection(node_t * n, pointf p)
{
    int i = 0, j, sides, conn = 0, peripheries, z;
    double xsize, ysize, mindist2 = 0.0, dist2;
    polygon_t *poly;
    pointf P, *vertices;
    static pointf *A;
    static int A_size;

    poly = (polygon_t *) ND_shape_info(n);
    vertices = poly->vertices;
    sides = poly->sides;
    peripheries = poly->peripheries;

    if (A_size < sides) {
	A_size = sides + 5;
	A = ALLOC(A_size, A, pointf);
    }

    xsize = (ND_lw(n) + ND_rw(n)) / POINTS(ND_width(n));
    ysize = (ND_ht(n)) / POINTS(ND_height(n));

    for (j = 0; j < peripheries; j++) {
	for (i = 0; i < sides; i++) {
	    P = vertices[i + j * sides];
	    A[i].x = P.x * xsize;
	    A[i].y = P.y * ysize;
	    if (sides > 2) {
		A[i].x += ND_coord(n).x;
		A[i].y += ND_coord(n).y;
	    }
	}
    }

    z = 0;
    while (z < i) {
	dist2 = DIST2(p, diaptf(A[z]));
	if (z == 0) {
	    mindist2 = dist2;
	    conn = 0;
	}
	if (dist2 < mindist2) {
	    mindist2 = dist2;
	    conn = 2 * z;
	}
	z++;
    }

    z = 0;
    while (z < i) {
	P.x = (diaptf(A[z]).x + diaptf(A[z + 1]).x) / 2;
	P.y = (diaptf(A[z]).y + diaptf(A[z + 1]).y) / 2;
	dist2 = DIST2(p, P);
	if (dist2 < mindist2) {
	    mindist2 = dist2;
	    conn = 2 * z + 1;
	}
	z++;
    }

    return conn;
}


static void
dia_bezier(point * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    int i, conn_h, conn_t;
    pointf p, firstp = { 0, 0 }, llp = { 0, 0}, urp = { 0, 0};
    node_t *head, *tail;
    char *shape_t;
    pointf cp_h, cp_t;

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }

    dia_printf
	("    <dia:object type=\"Standard - BezierLine\" version=\"0\" id=\"%s\">\n",
	 "00");
    dia_fputs("       <dia:attribute name=\"bez_points\">\n");
    for (i = 0; i < n; i++) {
	p = diapt(A[i]);
	if (!i)
	    llp = urp = firstp = p;
	if (p.x < llp.x || p.y < llp.y)
	    llp = p;
	if (p.x > urp.x || p.y > urp.y)
	    urp = p;
	dia_printf("        <dia:point val=\"%g,%g\"/>\n", p.x, p.y);
    }
    dia_fputs("      </dia:attribute>\n");
    dia_grstyle(&cstk[SP]);
    dia_fputs("      <dia:attribute name=\"obj_pos\">\n");
    dia_printf("        <dia:point val=\"%g,%g\"/>\n", firstp.x, firstp.y);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"obj_bb\">\n");
    dia_printf("        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       llp.x - .11, llp.y - .11, urp.x + .11, urp.y + .11);
    dia_fputs("      </dia:attribute>\n");

    if (Curedge) {
        conn_h = conn_t = -1;

        head = aghead(Curedge);
        tail = agtail(Curedge);

        shape_t = ND_shape(tail)->name;

        /* arrowheads */
        if (arrow_at_start) {
	    dia_fputs("      <dia:attribute name=\"start_arrow\">\n");
	    dia_fputs("          <dia:enum val=\"3\"/>\n");
	    dia_fputs("      </dia:attribute>\n");
	    dia_fputs("      <dia:attribute name=\"start_arrow_length\">\n");
	    dia_fputs("      	<dia:real val=\"0.8\"/>\n");
	    dia_fputs("      </dia:attribute>\n");
	    dia_fputs
	        ("		 <dia:attribute name=\"start_arrow_width\">\n");
	    dia_fputs("			<dia:real val=\"0.8\"/>\n");
	    dia_fputs("      </dia:attribute>\n");
        }
        if (arrow_at_end) {
	    dia_fputs("      <dia:attribute name=\"end_arrow\">\n");
	    dia_fputs("          <dia:enum val=\"3\"/>\n");
	    dia_fputs("      </dia:attribute>\n");
	    dia_fputs("      <dia:attribute name=\"end_arrow_length\">\n");
	    dia_fputs("      	<dia:real val=\"0.8\"/>\n");
	    dia_fputs("      </dia:attribute>\n");
	    dia_fputs
	        ("		 <dia:attribute name=\"end_arrow_width\">\n");
	    dia_fputs("			<dia:real val=\"0.8\"/>\n");
	    dia_fputs("      </dia:attribute>\n");
        }
    
        dia_fputs("      <dia:attribute name=\"conn_endpoints\">\n");
        dia_printf("        <dia:point val=\"%g,%g\"/>\n",
		diapt(A[0]).x, diapt(A[0]).y);
        dia_printf("        <dia:point val=\"%g,%g\"/>\n",
		diapt(A[n - 1]).x, diapt(A[n - 1]).y);
        dia_fputs("      </dia:attribute>\n");
        dia_fputs("      <dia:connections>\n");
    
/* FIXME !!! - What is this crap!   It should just user the arrow vector. */
        if ((strcmp(shape_t, "ellipse") == 0)
	    || (strcmp(shape_t, "circle") == 0)
	    || (strcmp(shape_t, "doublecircle") == 0)) {
	    cp_h = diaptf(ND_coord(head));
	    if (agisdirected(Rootgraph))

	        conn_h = ellipse_connection(cp_h, diapt(A[n - 1]));
	    else
	        conn_h = ellipse_connection(cp_h, diapt(A[0]));
        } else if (strcmp(shape_t, "record") == 0) { 
        } else {
	    if (agisdirected(Rootgraph))
	        conn_h = box_connection(head, diapt(A[n - 1]));
	    else
	        conn_h = box_connection(head, diapt(A[0]));
        }
    
        if ((strcmp(shape_t, "ellipse") == 0)
	    || (strcmp(shape_t, "circle") == 0)
	    || (strcmp(shape_t, "doublecircle") == 0)) {
	    cp_t = diaptf(ND_coord(tail));
	    if (agisdirected(Rootgraph))
	        conn_t = ellipse_connection(cp_t, diapt(A[0]));
	    else
	        conn_t = ellipse_connection(cp_t, diapt(A[n - 1]));
        } else if (strcmp(shape_t, "record") == 0) {
        } else {
	    if (agisdirected(Rootgraph))
	        conn_t = box_connection(tail, diapt(A[0]));
	    else
	        conn_t = box_connection(tail, diapt(A[n - 1]));
        }

        if (arrow_at_start) {
	    dia_printf
	        ("        <dia:connection handle=\"0\" to=\"%d\" connection=\"%d\"/>\n",
	         AGID(head), conn_h);
	    dia_printf
	        ("        <dia:connection handle=\"%d\" to=\"%d\" connection=\"%d\"/>\n",
	         (n - 1), AGID(tail), conn_t);
        } else {
	    dia_printf
	        ("        <dia:connection handle=\"0\" to=\"%d\" connection=\"%d\"/>\n",
	         AGID(tail), conn_t);
	    dia_printf
	        ("        <dia:connection handle=\"%d\" to=\"%d\" connection=\"%d\"/>\n",
	         (n - 1), AGID(head), conn_h);
        }
    
        dia_fputs("      </dia:connections>\n");
    }
    dia_fputs("    </dia:object>\n");
}



static void dia_polygon(point * A, int n, int filled)
{
    int i;
    pointf p, firstp = { 0, 0 }, llp = {
    0, 0}, urp = {
    0, 0};

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }

    switch (Obj) {
    case NODE:
	dia_printf
	    ("    <dia:object type=\"Standard - Polygon\" version=\"0\" id=\"%d\">\n",
	     AGID(Curnode));
	break;
    case EDGE:
	return;
	break;
    case CLST:
	dia_printf
	    ("    <dia:object type=\"Standard - Polygon\" version=\"0\" id=\"%s\">\n",
	     agnameof(Curgraph));
	break;
    default:
	dia_printf
	    ("    <dia:object type=\"Standard - Polygon\" version=\"0\" id=\"%s\">\n",
	     "polygon");
	break;
    }
    dia_fputs("       <dia:attribute name=\"poly_points\">\n");
    for (i = 0; i < n; i++) {
	p = diapt(A[i]);
	if (!i)
	    llp = urp = firstp = p;
	if (p.x < llp.x || p.y < llp.y)
	    llp = p;
	if (p.x > urp.x || p.y > urp.y)
	    urp = p;
	dia_printf("        <dia:point val=\"%g,%g\"/>\n", p.x, p.y);
    }
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"obj_pos\">\n");
    dia_printf("        <dia:point val=\"%g,%g\"/>\n", firstp.x, firstp.y);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"obj_bb\">\n");
    dia_printf("        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       llp.x - .11, llp.y - .11, urp.x + .11, urp.y + .11);
    dia_fputs("      </dia:attribute>\n");
    dia_grstyle(&cstk[SP]);
    dia_grstylefill(&cstk[SP], filled);
    dia_fputs("    </dia:object>\n");
}

static void dia_polyline(point * A, int n)
{
    int i;
    pointf p, firstp = { 0, 0 }, llp = {
    0, 0}, urp = {
    0, 0};

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    dia_printf
	("    <dia:object type=\"Standard - PolyLine\" version=\"0\" id=\"%s\">\n",
	 "0");
    dia_fputs("      <dia:attribute name=\"poly_points\">\n");
    for (i = 0; i < n; i++) {
	p = diapt(A[i]);
	if (!i)
	    llp = urp = firstp = p;
	if (p.x < llp.x || p.y < llp.y)
	    llp = p;
	if (p.x > urp.x || p.y > urp.y)
	    urp = p;
	dia_printf("<dia:point val=\"%g,%g\"/>\n", p.x, p.y);
    }
    dia_fputs("      </dia:attribute>\n");
    dia_grstyle(&cstk[SP]);
    dia_fputs("      <dia:attribute name=\"obj_pos\">\n");
    dia_printf("        <dia:point val=\"%g,%g\"/>\n", firstp.x, firstp.y);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("      <dia:attribute name=\"obj_bb\">\n");
    dia_printf("        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       llp.x - .11, llp.y - .11, urp.x + .11, urp.y + .11);
    dia_fputs("      </dia:attribute>\n");
    dia_fputs("    </dia:object>\n");
}

static void dia_usershape(usershape_t *us, boxf b, point *A, int n, boolean filled)
{
    char *imagefile;

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }

#if 0
/* FIXME */
    imagefile = agget(Curnode, "shapefile");
#else
    imagefile = NULL;
#endif

    if (! imagefile) {
	dia_polygon(A, n, filled);
	return;
    }
}

codegen_t DIA_CodeGen = {
    dia_reset,
    dia_begin_job, dia_end_job,
    dia_begin_graph, dia_end_graph,
    dia_begin_page, dia_end_page,
    0, /* dia_begin_layer */ 0,	/* dia_end_layer */
    dia_begin_cluster, dia_end_cluster,
    0, /* dia_begin_nodes */ 0,	/* dia_end_nodes */
    0, /* dia_begin_edges */ 0,	/* dia_end_edges */
    dia_begin_node, dia_end_node,
    dia_begin_edge, dia_end_edge,
    dia_begin_context, dia_end_context,
    0, /* dia_begin_anchor */ 0,	/* dia_end_anchor */
    dia_set_font, dia_textpara,
    dia_set_pencolor, dia_set_fillcolor, dia_set_style,
    dia_ellipse, dia_polygon,
    dia_bezier, dia_polyline,
    1,				/* bezier_has_arrows */
    dia_comment,
    dia_usershape
};
