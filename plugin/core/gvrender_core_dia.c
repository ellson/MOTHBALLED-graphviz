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

/* FIXME - incomplete replacement for codegen */

#include "config.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"
#include "const.h"

#include "gvplugin_render.h"
#include "gvplugin_device.h"
#include "gvio.h"
#include "gvcint.h"

typedef enum { FORMAT_DIA, } format_type;

extern char *xml_string(char *str);

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

static double Scale;
static pointf Offset;
static int Rot;
static box PB;
static int onetime = TRUE;

typedef struct context_t {
    char *pencolor, *fillcolor, *fontfam, fontopt, font_was_set;
    char pen, fill, penwidth, style_was_set;
    double fontsz;
} context_t;

#define MAXNEST 4
static context_t cstk[MAXNEST];
static int SP;

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

//    tok = canontoken(name);
tok = name;
    if (!SVG_COLORS_P || (bsearch(&tok, svg_known_colors,
                              sizeof(svg_known_colors) / sizeof(char *),
                              sizeof(char *), dia_comparestr) == NULL)) {
        /* if tok was not found in known_colors */
        if (streq(tok, "transparent")) {
            tok = "none";
        } else {
//            colorxlate(name, &color, RGBA_BYTE);
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

static pointf dia_pt(pointf p)
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

static void dia_style(GVJ_t * job, context_t * cp)
{
    if (cp->pencolor != DEFAULT_COLOR) {
	gvputs(job, "      <dia:attribute name=\"border_color\">\n");
	gvprintf(job, "        <dia:color val=\"%s\"/>\n",
		   dia_resolve_color(cp->pencolor));
	gvputs(job, "      </dia:attribute>\n");
    }
    if (cp->penwidth != WIDTH_NORMAL) {
	gvputs(job, "      <dia:attribute name=\"line_width\">\n");
	gvprintf(job, "        <dia:real val=\"%g\"/>\n",
		   Scale * (cp->penwidth));
	gvputs(job, "      </dia:attribute>\n");
    }
    if (cp->pen == P_DASHED) {
	gvputs(job, "      <dia:attribute name=\"line_style\">\n");
	gvprintf(job, "        <dia:real val=\"%d\"/>\n", 1);
	gvputs(job, "      </dia:attribute>\n");
#if 0
    } else if (cp->pen == P_DOTTED) {
	gvprintf(job, "stroke-dasharray:%s;", sdotarray);
#endif
    }
}

static void dia_stylefill(GVJ_t * job, context_t * cp, int filled)
{
    if (filled) {
	gvputs(job, "      <dia:attribute name=\"inner_color\">\n");
	gvprintf(job, "        <dia:color val=\"%s\"/>\n",
		   dia_resolve_color(cp->fillcolor));
	gvputs(job, "      </dia:attribute>\n");
    } else {
	gvputs(job, "      <dia:attribute name=\"show_background\">\n");
	gvprintf(job, "        <dia:boolean val=\"%s\"/>\n", "true");
	gvputs(job, "      </dia:attribute>\n");
    }
}

static void dia_comment(GVJ_t * job, char *str)
{
    gvputs(job, "<!-- ");
    gvputs(job, xml_string(str));
    gvputs(job, " -->\n");
}

static void
dia_begin_job(GVJ_t *job)
{
    gvprintf(job, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
}

static void dia_end_job(GVJ_t *job)
{
}

static void dia_begin_graph(GVJ_t * job)
{
    gvprintf(job, "<dia:diagram xmlns:dia=\"http://www.lysator.liu.se/~alla/dia/\">\n");
#if 0
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
#endif
    gvputs(job, "  <dia:diagramdata>\n");

    gvputs(job, "    <dia:attribute name=\"background\">\n");
    gvputs(job, "      <dia:color val=\"#ffffff\"/>\n");
    gvputs(job, "    </dia:attribute>\n");

    gvputs(job, "    <dia:attribute name=\"paper\">\n");
    gvputs(job, "      <dia:composite type=\"paper\">\n");
    gvputs(job, "        <dia:attribute name=\"name\">\n");
    gvputs(job, "          <dia:string>#A4#</dia:string>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"tmargin\">\n");
    gvputs(job, "          <dia:real val=\"2.8222\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"bmargin\">\n");
    gvputs(job, "          <dia:real val=\"2.8222\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"lmargin\">\n");
    gvputs(job, "          <dia:real val=\"2.8222\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"rmargin\">\n");
    gvputs(job, "          <dia:real val=\"2.8222\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"is_portrait\">\n");
    gvputs(job, "          <dia:boolean val=\"true\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"scaling\">\n");
    gvputs(job, "          <dia:real val=\"1\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"fitto\">\n");
    gvputs(job, "          <dia:boolean val=\"false\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "      </dia:composite>\n");
    gvputs(job, "    </dia:attribute>\n");

    gvputs(job, "    <dia:attribute name=\"grid\">\n");
    gvputs(job, "      <dia:composite type=\"grid\">\n");
    gvputs(job, "        <dia:attribute name=\"width_x\">\n");
    gvputs(job, "          <dia:real val=\"1\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"width_y\">\n");
    gvputs(job, "          <dia:real val=\"1\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"visible_x\">\n");
    gvputs(job, "          <dia:int val=\"1\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "        <dia:attribute name=\"visible_y\">\n");
    gvputs(job, "          <dia:int val=\"1\"/>\n");
    gvputs(job, "        </dia:attribute>\n");
    gvputs(job, "      </dia:composite>\n");
    gvputs(job, "    </dia:attribute>\n");

    gvputs(job, "    <dia:attribute name=\"guides\">\n");
    gvputs(job, "      <dia:composite type=\"guides\">\n");
    gvputs(job, "        <dia:attribute name=\"hguides\"/>\n");
    gvputs(job, "        <dia:attribute name=\"vguides\"/>\n");
    gvputs(job, "      </dia:composite>\n");
    gvputs(job, "    </dia:attribute>\n");

    gvputs(job, "  </dia:diagramdata>\n");
}

static void dia_end_graph(GVJ_t * job)
{
    gvprintf(job, "</dia:diagram>\n");
}

static void
dia_begin_page(GVJ_t * job)
{
    gvprintf(job, "  <dia:layer name=\"Background\" visible=\"true\">\n");
}

static void dia_end_page(GVJ_t * job)
{
    gvputs(job, "  </dia:layer>\n");
}

static void dia_begin_cluster(GVJ_t * job)
{
//    obj_state_t *obj = job->obj;

    gvprintf(job, "    <dia:group>\n");
}

static void dia_end_cluster(GVJ_t * job)
{
    gvprintf(job, "    </dia:group>\n");
}

static void dia_begin_node(GVJ_t * job)
{
    gvprintf(job, "    <dia:group>\n");
}

static void dia_end_node(GVJ_t * job)
{
    gvprintf(job, "    </dia:group>\n");
}

static void dia_begin_edge(GVJ_t * job)
{
    gvprintf(job, "    <dia:group>\n");
}

static void dia_end_edge(GVJ_t * job)
{
    gvprintf(job, "    </dia:group>\n");
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
#if 0
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
	    agwarn("dia_set_style: unsupported style %s - ignoring\n",
		  line);
	}
	cp->style_was_set = TRUE;
    }
    /* if (cp->style_was_set) dia_style(cp); */
#endif
}

static void dia_textspan(GVJ_t * job, pointf p, textspan_t * span)
{
    obj_state_t *obj = job->obj;
    PostscriptAlias *pA;
    int anchor;
    double size;
    char *family=NULL, *weight=NULL, *stretch=NULL, *style=NULL;

    switch (span->just) {
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
    p.y += span->yoffset_centerline;

    pA = span->font->postscript_alias;
    if (pA) {
        switch(GD_fontnames(job->gvc->g)) {
                case PSFONTS:
                    family = pA->name;
                    weight = pA->weight;
                    style = pA->style;
                    break;
                case SVGFONTS:
                    family = pA->svg_font_family;
                    weight = pA->svg_font_weight;
                    style = pA->svg_font_style;
                    break;
                default:
                case NATIVEFONTS:
                    family = pA->family;
                    weight = pA->weight;
                    style = pA->style;
                    break;
        }
        stretch = pA->stretch;
#if 0
        gvprintf(job, " font-family=\"%s", family);
        if (pA->svg_font_family) gvprintf(job, ",%s", pA->svg_font_family);
        gvputs(job, "\"");
        if (weight) gvprintf(job, " font-weight=\"%s\"", weight);
        if (stretch) gvprintf(job, " font-stretch=\"%s\"", stretch);
        if (style) gvprintf(job, " font-style=\"%s\"", style);
#endif
    }
    else
	family = span->fontname;
    size = span->fontsize;

#if 0
    switch (obj->pencolor.type) {
    case COLOR_STRING:
        if (strcasecmp(obj->pencolor.u.string, "black"))
            gvprintf(job, " fill=\"%s\"", obj->pencolor.u.string);
        break;
    case RGBA_BYTE:
        gvprintf(job, " fill=\"#%02x%02x%02x\"",
                obj->pencolor.u.rgba[0], obj->pencolor.u.rgba[1], obj->pencolor.u.rgba[2]);
        break;
    default:
        assert(0);              /* internal error */
    }
#endif

    gvprintf(job,
	"    <dia:object type=\"Standard - Text\" version=\"0\" id=\"%s\">\n",
	 "0");
    gvputs(job, "      <dia:attribute name=\"text\">\n");
    gvputs(job, "        <dia:composite type=\"text\">\n");
    gvputs(job, "          <dia:attribute name=\"string\">\n");
    gvputs(job, "            <dia:string>#");
    gvputs(job, xml_string(span->str));
    gvputs(job, "#</dia:string>\n");
    gvputs(job, "          </dia:attribute>\n");
    gvputs(job, "          <dia:attribute name=\"font\">\n");
    gvprintf(job, "            <dia:font name=\"%s\"/>\n", family);
    gvputs(job, "          </dia:attribute>\n");
    gvputs(job, "          <dia:attribute name=\"height\">\n");
    gvprintf(job, "            <dia:real val=\"%g\"/>\n", size);
    gvputs(job, "          </dia:attribute>\n");
    gvputs(job, "          <dia:attribute name=\"pos\">\n");
    gvprintf(job, "            <dia:point val=\"%g,%g\"/>\n", p.x, p.y);
    gvputs(job, "          </dia:attribute>\n");
    gvputs(job, "          <dia:attribute name=\"color\">\n");
#if 0
    gvprintf(job, "            <dia:color val=\"%s\"/>\n",
	       dia_resolve_color(cp->pencolor));
#else
    gvprintf(job, "            <dia:color val=\"%s\"/>\n",
	       "black");
#endif
    gvputs(job, "          </dia:attribute>\n");
    gvputs(job, "          <dia:attribute name=\"alignment\">\n");
    gvprintf(job, "            <dia:enum val=\"%d\"/>\n", anchor);
    gvputs(job, "          </dia:attribute>\n");
    gvputs(job, "        </dia:composite>\n");
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"obj_pos\">\n");
    gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", p.x, p.y);
    gvputs(job, "      </dia:attribute>\n");
#if 0
    gvputs(job, "      <dia:attribute name=\"obj_bb\">\n");
    gvprintf(job, "        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       p.x - (Scale * (span->size.x) / 2.), p.y - 0.4,
	       p.x + (Scale * (span->size.x) / 2.), p.y + 0.4);
    gvputs(job, "      </dia:attribute>\n");
#endif
    gvputs(job, "    </dia:object>\n");
}

static void dia_ellipse(GVJ_t * job, pointf *A, int filled)
{
#if 0
    pointf cp, rp;
    int nodeId;

    switch (Obj) {
    case NODE:
	nodeId = Curnode->id;
	break;
    default:
	nodeId = -1;
	break;
    }

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    cp = dia_pt(p);

    if (Rot) {
	int t;
	t = rx;
	rx = ry;
	ry = t;
    }
    rp.x = Scale * rx;
    rp.y = Scale * ry;

    gvprintf(job,
	"    <dia:object type=\"Standard - Ellipse\" version=\"0\" id=\"%d\">\n",
	 nodeId);
    gvputs(job, "      <dia:attribute name=\"elem_corner\">\n");
    gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", cp.x - rp.x,
	       cp.y - rp.y);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"elem_width\">\n");
    gvprintf(job, "        <dia:real val=\"%g\"/>\n", rp.x + rp.x);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"elem_height\">\n");
    gvprintf(job, "        <dia:real val=\"%g\"/>\n", rp.y + rp.y);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"obj_pos\">\n");
    gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", cp.x - rp.x,
	       cp.y - rp.y);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"obj_bb\">\n");
    gvprintf(job, "        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       cp.x - rp.x - .11, cp.y - rp.y - .11, cp.x + rp.x + .11,
	       cp.y + rp.y + .11);
    gvputs(job, "      </dia:attribute>\n");
    dia_style(job, &cstk[SP]);
    dia_stylefill(job, &cstk[SP], filled);
    gvputs(job, "    </dia:object>\n");
#endif
}


int ellipse_connection(pointf cp, pointf p)
{
    int conn = 0;
#if 0
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
#endif
    return conn;
}


int box_connection(node_t * n, pointf p)
{
    int conn = 0;
#if 0
    int i = 0, j, sides, conn = 0, peripheries, z;
    double xsize, ysize, mindist2 = 0.0, dist2;
    polygon_t *poly;
    pointf P, *vertices;
    static point *A;
    static int A_size;

    poly = (polygon_t *) ND_shape_info(n);
    vertices = poly->vertices;
    sides = poly->sides;
    peripheries = poly->peripheries;

    if (A_size < sides) {
	A_size = sides + 5;
	A = malloc(A_size*sizeof(pointf));
    }

    xsize = ((ND_lw(n) + ND_rw(n)) / POINTS(ND_width(n))) * 16.0;
    ysize = ((ND_ht(n)) / POINTS(ND_height(n))) * 16.0;

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
    }

    z = 0;
    while (z < i) {
	dist2 = DIST2(p, dia_pt(A[z]));
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
	P.x = (dia_pt(A[z]).x + dia_pt(A[z + 1]).x) / 2;
	P.y = (dia_pt(A[z]).y + dia_pt(A[z + 1]).y) / 2;
	dist2 = DIST2(p, P);
	if (dist2 < mindist2) {
	    mindist2 = dist2;
	    conn = 2 * z + 1;
	}
	z++;
    }
#endif
    return conn;
}


static void
dia_bezier(GVJ_t *job, pointf * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
#if 0
    int i, conn_h, conn_t;
    pointf p, firstp = { 0, 0 }, llp = {
    0, 0}, urp = {
    0, 0};
    node_t *head, *tail;
    char *shape_t;
    pointf cp_h, cp_t;

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }

    gvprintf(job,
	"    <dia:object type=\"Standard - BezierLine\" version=\"0\" id=\"%s\">\n",
	 "00");
    gvputs(job, "       <dia:attribute name=\"bez_points\">\n");
    for (i = 0; i < n; i++) {
	p = dia_pt(A[i]);
	if (!i)
	    llp = urp = firstp = p;
	if (p.x < llp.x || p.y < llp.y)
	    llp = p;
	if (p.x > urp.x || p.y > urp.y)
	    urp = p;
	gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", p.x, p.y);
    }
    gvputs(job, "      </dia:attribute>\n");
    dia_style(job, &cstk[SP]);
    gvputs(job, "      <dia:attribute name=\"obj_pos\">\n");
    gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", firstp.x, firstp.y);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"obj_bb\">\n");
    gvprintf(job, "        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       llp.x - .11, llp.y - .11, urp.x + .11, urp.y + .11);
    gvputs(job, "      </dia:attribute>\n");
    if (!Curedge) return;

    conn_h = conn_t = -1;

    head = Curedge->head;
    tail = Curedge->tail;

    shape_t = ND_shape(tail)->name;

    /* arrowheads */
    if (arrow_at_start) {
	gvputs(job, "      <dia:attribute name=\"start_arrow\">\n");
	gvputs(job, "          <dia:enum val=\"3\"/>\n");
	gvputs(job, "      </dia:attribute>\n");
	gvputs(job, "      <dia:attribute name=\"start_arrow_length\">\n");
	gvputs(job, "      	<dia:real val=\"0.8\"/>\n");
	gvputs(job, "      </dia:attribute>\n");
	dia_fputs
	    ("		 <dia:attribute name=\"start_arrow_width\">\n");
	gvputs(job, "			<dia:real val=\"0.8\"/>\n");
	gvputs(job, "      </dia:attribute>\n");
    }
    if (arrow_at_end) {
	gvputs(job, "      <dia:attribute name=\"end_arrow\">\n");
	gvputs(job, "          <dia:enum val=\"3\"/>\n");
	gvputs(job, "      </dia:attribute>\n");
	gvputs(job, "      <dia:attribute name=\"end_arrow_length\">\n");
	gvputs(job, "      	<dia:real val=\"0.8\"/>\n");
	gvputs(job, "      </dia:attribute>\n");
	dia_fputs
	    ("		 <dia:attribute name=\"end_arrow_width\">\n");
	gvputs(job, "			<dia:real val=\"0.8\"/>\n");
	gvputs(job, "      </dia:attribute>\n");
    }

    gvputs(job, "      <dia:attribute name=\"conn_endpoints\">\n");
    gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", dia_pt(A[0]).x,
	       dia_pt(A[0]).y);
    gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", dia_pt(A[n - 1]).x,
	       dia_pt(A[n - 1]).y);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:connections>\n");

    if ((strcmp(shape_t, "ellipse") == 0)
	|| (strcmp(shape_t, "circle") == 0)
	|| (strcmp(shape_t, "doublecircle") == 0)) {
	cp_h = dia_pt(ND_coord_i(head));
	if (AG_IS_DIRECTED(Rootgraph))
	    conn_h = ellipse_connection(cp_h, dia_pt(A[n - 1]));
	else
	    conn_h = ellipse_connection(cp_h, dia_pt(A[0]));
    } else {
	if (AG_IS_DIRECTED(Rootgraph))
	    conn_h = box_connection(head, dia_pt(A[n - 1]));
	else
	    conn_h = box_connection(head, dia_pt(A[0]));
    }

    if ((strcmp(shape_t, "ellipse") == 0)
	|| (strcmp(shape_t, "circle") == 0)
	|| (strcmp(shape_t, "doublecircle") == 0)) {
	cp_t = dia_pt(ND_coord_i(tail));
	if (AG_IS_DIRECTED(Rootgraph))
	    conn_t = ellipse_connection(cp_t, dia_pt(A[0]));
	else
	    conn_t = ellipse_connection(cp_t, dia_pt(A[n - 1]));
    } else {
	if (AG_IS_DIRECTED(Rootgraph))
	    conn_t = box_connection(tail, dia_pt(A[0]));
	else
	    conn_t = box_connection(tail, dia_pt(A[n - 1]));
    }

    if (arrow_at_start) {
	gvprintf(job,
	    "        <dia:connection handle=\"0\" to=\"%d\" connection=\"%d\"/>\n",
	     head->id, conn_h);
	gvprintf(job,
	    "        <dia:connection handle=\"%d\" to=\"%d\" connection=\"%d\"/>\n",
	     (n - 1), tail->id, conn_t);
    } else {
	gvprintf(job,
	    "        <dia:connection handle=\"0\" to=\"%d\" connection=\"%d\"/>\n",
	     tail->id, conn_t);
	gvprintf(job,
	    "        <dia:connection handle=\"%d\" to=\"%d\" connection=\"%d\"/>\n",
	     (n - 1), head->id, conn_h);
    }

    gvputs(job, "      </dia:connections>\n");
    gvputs(job, "    </dia:object>\n");
#endif
}



static void dia_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
#if 0
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
	gvprintf(job,
	    "    <dia:object type=\"Standard - Polygon\" version=\"0\" id=\"%d\">\n",
	     Curnode->id);
	break;
    case EDGE:
	return;
	break;
    case CLST:
	gvprintf(job,
	    "    <dia:object type=\"Standard - Polygon\" version=\"0\" id=\"%s\">\n",
	     Curgraph->name);
	break;
    default:
	gvprintf(job,
	    "    <dia:object type=\"Standard - Polygon\" version=\"0\" id=\"%s\">\n",
	     "polygon");
	break;
    }
    gvputs(job, "       <dia:attribute name=\"poly_points\">\n");
    for (i = 0; i < n; i++) {
	p = dia_pt(A[i]);
	if (!i)
	    llp = urp = firstp = p;
	if (p.x < llp.x || p.y < llp.y)
	    llp = p;
	if (p.x > urp.x || p.y > urp.y)
	    urp = p;
	gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", p.x, p.y);
    }
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"obj_pos\">\n");
    gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", firstp.x, firstp.y);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"obj_bb\">\n");
    gvprintf(job, "        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       llp.x - .11, llp.y - .11, urp.x + .11, urp.y + .11);
    gvputs(job, "      </dia:attribute>\n");
    dia_style(job, &cstk[SP]);
    dia_stylefill(job, &cstk[SP], filled);
    gvputs(job, "    </dia:object>\n");
#endif
}

static void dia_polyline(GVJ_t * job, pointf * A, int n)
{
#if 0
    int i;
    pointf p, firstp = { 0, 0 }, llp = {
    0, 0}, urp = {
    0, 0};

    if (cstk[SP].pen == P_NONE) {
	/* its invisible, don't draw */
	return;
    }
    gvprintf(job,
	"    <dia:object type=\"Standard - PolyLine\" version=\"0\" id=\"%s\">\n",
	 "0");
    gvputs(job, "      <dia:attribute name=\"poly_points\">\n");
    for (i = 0; i < n; i++) {
	p = dia_pt(A[i]);
	if (!i)
	    llp = urp = firstp = p;
	if (p.x < llp.x || p.y < llp.y)
	    llp = p;
	if (p.x > urp.x || p.y > urp.y)
	    urp = p;
	gvprintf(job, "<dia:point val=\"%g,%g\"/>\n", p.x, p.y);
    }
    gvputs(job, "      </dia:attribute>\n");
    dia_style(job, &cstk[SP]);
    gvputs(job, "      <dia:attribute name=\"obj_pos\">\n");
    gvprintf(job, "        <dia:point val=\"%g,%g\"/>\n", firstp.x, firstp.y);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "      <dia:attribute name=\"obj_bb\">\n");
    gvprintf(job, "        <dia:rectangle val=\"%g,%g;%g,%g\"/>\n",
	       llp.x - .11, llp.y - .11, urp.x + .11, urp.y + .11);
    gvputs(job, "      </dia:attribute>\n");
    gvputs(job, "    </dia:object>\n");
#endif
}

gvrender_engine_t dia_engine = {
    dia_begin_job, dia_end_job,
    dia_begin_graph, dia_end_graph,
    0, /* dia_begin_layer */ 0,	/* dia_end_layer */
    dia_begin_page, dia_end_page,
    dia_begin_cluster, dia_end_cluster,
    0, /* dia_begin_nodes */ 0,	/* dia_end_nodes */
    0, /* dia_begin_edges */ 0,	/* dia_end_edges */
    dia_begin_node, dia_end_node,
    dia_begin_edge, dia_end_edge,
    0, /* dia_begin_anchor */ 0, /* dia_end_anchor */
    0, /* dia_begin_label */ 0, /* dia_end_label */
    dia_textspan, dia_resolve_color,
    dia_ellipse, dia_polygon,
    dia_bezier, dia_polyline,
    dia_comment,
    0, 			/* dia_library_shape */
};

static gvrender_features_t render_features_dia = {
    0,                          /* flags */
    4.,                         /* default pad - graph units */
    NULL,                       /* knowncolors */
    0,                          /* sizeof knowncolors */
    HSVA_DOUBLE,                /* color_type */
};

static gvdevice_features_t device_features_dia = {
    GVDEVICE_BINARY_FORMAT
      | GVDEVICE_COMPRESSED_FORMAT, /* flags */
    {0.,0.},                    /* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {72.,72.},                  /* default dpi */
};

gvplugin_installed_t gvrender_dia_types[] = {
    {FORMAT_DIA, "dia", -1, &dia_engine, &render_features_dia},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_dia_types[] = {
    {FORMAT_DIA, "dia:dia", -1, NULL, &device_features_dia},
    {0, NULL, 0, NULL, NULL}
};

