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


#include "render.h"

#define PIC_COORDS_PER_LINE (16)	/* to avoid stdio BUF overflow */

static box BB;
static int BezierSubdivision = 10;
static int Rot;
static int onetime = TRUE;
static double Scale;
static double Fontscale;

/* static char	**U_lib,*User,*Vers; */
static const char *EscComment = ".\\\" ";	/* troff comment */

typedef struct grcontext_t {
    char *color, *font;
    double size;
} grcontext_t;

#define STACKSIZE 8
static grcontext_t S[STACKSIZE];
static int SP = 0;

static char picgen_msghdr[] = "dot picgen: ";
static void unsupported(char *s)
{
    agerr(AGWARN, "%s%s unsupported\n", picgen_msghdr, s);
}
static void warn(char *s)
{
    agerr(AGWARN, "%s%s\n", picgen_msghdr, s);
}

/* There are a couple of ways to generate output: 
    1. generate for whatever size is given by the bounding box
       - the drawing at its "natural" size might not fit on a physical page
         ~ dot size specification can be used to scale the drawing
         ~ and it's not difficult for user to scale the pic output to fit (multiply 4 (3 distinct) numbers on 3 lines by a scale factor)
       - some troff implementations may clip large graphs
         ~ handle by scaling to manageable size
       - give explicit width and height as parameters to .PS
       - pic scale variable is reset to 1.0
       - fonts are printed as size specified by caller, modified by user scaling
    2. scale to fit on a physical page
       - requires an assumption of page size (GNU pic assumes 8.5x11.0 inches)
         ~ any assumption is bound to be wrong more often than right
       - requires separate scaling of font point sizes since pic's scale variable doesn't affect text
         ~ possible, as above
       - likewise for line thickness
       - GNU pic does this (except for fonts) if .PS is used without explicit width or height; DWB pic does not
         ~ pic variants likely to cause trouble
  The first approach is used here.
*/

static pointf cvt2ptf(point p)
{
    pointf r;

    r.x = PS2INCH(p.x);
    r.y = PS2INCH(p.y);
    return r;
}


/* troff font mapping */
typedef struct {
    char trname[3], *psname;
} fontinfo;

static fontinfo fonttab[] = {
    {"AB", "AvantGarde-Demi"},
    {"AI", "AvantGarde-BookOblique"},
    {"AR", "AvantGarde-Book"},
    {"AX", "AvantGarde-DemiOblique"},
    {"B ", "Times-Bold"},
    {"BI", "Times-BoldItalic"},
    {"CB", "Courier-Bold"},
    {"CO", "Courier"},
    {"CX", "Courier-BoldOblique"},
    {"H ", "Helvetica"},
    {"HB", "Helvetica-Bold"},
    {"HI", "Helvetica-Oblique"},
    {"HX", "Helvetica-BoldOblique"},
    {"Hb", "Helvetica-Narrow-Bold"},
    {"Hi", "Helvetica-Narrow-Oblique"},
    {"Hr", "Helvetica-Narrow"},
    {"Hx", "Helvetica-Narrow-BoldOblique"},
    {"I ", "Times-Italic"},
    {"KB", "Bookman-Demi"},
    {"KI", "Bookman-LightItalic"},
    {"KR", "Bookman-Light"},
    {"KX", "Bookman-DemiItalic"},
    {"NB", "NewCenturySchlbk-Bold"},
    {"NI", "NewCenturySchlbk-Italic"},
    {"NR", "NewCenturySchlbk-Roman"},
    {"NX", "NewCenturySchlbk-BoldItalic"},
    {"PA", "Palatino-Roman"},
    {"PB", "Palatino-Bold"},
    {"PI", "Palatino-Italic"},
    {"PX", "Palatino-BoldItalic"},
    {"R ", "Times-Roman"},
    {"S ", "Symbol"},
    {"ZD", "ZapfDingbats"},
    {"\000\000", (char *) 0}
};

static char *picfontname(char *psname)
{
    char *rv;
    fontinfo *p;

    for (p = fonttab; p->psname; p++)
	if (streq(p->psname, psname))
	    break;
    if (p->psname)
	rv = p->trname;
    else {
	agerr(AGERR, "%s%s is not a troff font\n", picgen_msghdr, psname);
	/* try base font names, e.g. Helvetica-Outline-Oblique -> Helvetica-Outline -> Helvetica */
	if ((rv = strrchr(psname, '-'))) {
	    *rv = '\0';		/* psname is not specified as const ... */
	    rv = picfontname(psname);
	} else
	    rv = "R";
    }
    return rv;
}

static char *pic_fcoord(char *buf, pointf pf)
{
    sprintf(buf, "(%.5f,%.5f)", Scale * pf.x, Scale * pf.y);
    return buf;
}

static char *pic_coord(char *buf, point p)
{
    return pic_fcoord(buf, cvt2ptf(p));
}

static void pic_reset(void)
{
    onetime = TRUE;
}

static void pic_begin_job(FILE * ofp, graph_t * g, const char **lib, char *info[], point pages)
{
    /* U_lib = lib; */
    if (onetime && (pages.x * pages.y > 1)) {
	unsupported("pagination");
	onetime = FALSE;
    }
    fprintf(Output_file, "%s Creator: %s version %s (%s)\n",
	    EscComment, info[0], info[1], info[2]);
    fprintf(Output_file, "%s Title: %s\n", EscComment, agnameof(g));
}

static void pic_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    BB = bb;

    fprintf(Output_file,
	    "%s save point size and font\n.nr .S \\n(.s\n.nr DF \\n(.f\n",
	    EscComment);
}

static void pic_end_graph(void)
{
    fprintf(Output_file,
	    "%s restore point size and font\n.ps \\n(.S\n.ft \\n(DF\n",
	    EscComment);
}

static void pic_begin_page(graph_t * g, point page, double scale, int rot,
			   point offset)
{
    double height, width;

    if (onetime && rot && (rot != 90)) {
	unsupported("rotation");
	onetime = FALSE;
    }
    Rot = rot;
    height = PS2INCH((double) (BB.UR.y) - (double) (BB.LL.y));
    width = PS2INCH((double) (BB.UR.x) - (double) (BB.LL.x));
    Scale = scale;
    if (Rot == 90) {
	double temp = width;
	width = height;
	height = temp;
    }
    fprintf(Output_file, ".PS %.5f %.5f\n", width, height);
    EscComment = "#";		/* pic comment */
    fprintf(Output_file,
	    "%s to change drawing size, multiply the width and height on the .PS line above and the number on the two lines below (rounded to the nearest integer) by a scale factor\n",
	    EscComment);
    if (width > 0.0) {
	Fontscale = log10(width);
	Fontscale += 3.0 - (int) Fontscale;	/* between 3.0 and 4.0 */
    } else
	Fontscale = 3.0;
    Fontscale = pow(10.0, Fontscale);	/* a power of 10 times width, between 1000 and 10000 */
    fprintf(Output_file, ".nr SF %.0f\nscalethickness = %.0f\n", Fontscale,
	    Fontscale);
    fprintf(Output_file,
	    "%s don't change anything below this line in this drawing\n",
	    EscComment);
    fprintf(Output_file,
	    "%s non-fatal run-time pic version determination, version 2\n",
	    EscComment);
    fprintf(Output_file,
	    "boxrad=2.0 %s will be reset to 0.0 by gpic only\n",
	    EscComment);
    fprintf(Output_file, "scale=1.0 %s required for comparisons\n",
	    EscComment);
    fprintf(Output_file,
	    "%s boxrad is now 0.0 in gpic, else it remains 2.0\n",
	    EscComment);
    fprintf(Output_file,
	    "%s dashwid is 0.1 in 10th Edition, 0.05 in DWB 2 and in gpic\n",
	    EscComment);
    fprintf(Output_file,
	    "%s fillval is 0.3 in 10th Edition (fill 0 means black), 0.5 in gpic (fill 0 means white), undefined in DWB 2\n",
	    EscComment);
    fprintf(Output_file,
	    "%s fill has no meaning in DWB 2, gpic can use fill or filled, 10th Edition uses fill only\n",
	    EscComment);
    fprintf(Output_file,
	    "%s DWB 2 doesn't use fill and doesn't define fillval\n",
	    EscComment);
    fprintf(Output_file,
	    "%s reset works in gpic and 10th edition, but isn't defined in DWB 2\n",
	    EscComment);
    fprintf(Output_file, "%s DWB 2 compatibility definitions\n",
	    EscComment);
    fprintf(Output_file,
	    "if boxrad > 1.0 && dashwid < 0.075 then X\n\tfillval = 1;\n\tdefine fill Y Y;\n\tdefine solid Y Y;\n\tdefine reset Y scale=1.0 Y;\nX\n");
    fprintf(Output_file, "reset %s set to known state\n", EscComment);
    fprintf(Output_file, "%s GNU pic vs. 10th Edition d\\(e'tente\n",
	    EscComment);
    fprintf(Output_file,
	    "if fillval > 0.4 then X\n\tdefine setfillval Y fillval = 1 - Y;\n\tdefine bold Y thickness 2 Y;\n");
    fprintf(Output_file,
	    "\t%s if you use gpic and it barfs on encountering \"solid\",\n",
	    EscComment);
    fprintf(Output_file,
	    "\t%s\tinstall a more recent version of gpic or switch to DWB or 10th Edition pic;\n",
	    EscComment);
    fprintf(Output_file,
	    "\t%s\tsorry, the groff folks changed gpic; send any complaint to them;\n",
	    EscComment);
    fprintf(Output_file,
	    "X else Z\n\tdefine setfillval Y fillval = Y;\n\tdefine bold Y Y;\n\tdefine filled Y fill Y;\nZ\n");
    fprintf(Output_file,
	    "%s arrowhead has no meaning in DWB 2, arrowhead = 7 makes filled arrowheads in gpic and in 10th Edition\n",
	    EscComment);
    fprintf(Output_file,
	    "%s arrowhead is undefined in DWB 2, initially 1 in gpic, 2 in 10th Edition\n",
	    EscComment);
    fprintf(Output_file, "arrowhead = 7 %s not used by graphviz\n",
	    EscComment);
    fprintf(Output_file,
	    "%s GNU pic supports a boxrad variable to draw boxes with rounded corners; DWB and 10th Ed. do not\n",
	    EscComment);
    fprintf(Output_file, "boxrad = 0 %s no rounded corners in graphviz\n",
	    EscComment);
    fprintf(Output_file,
	    "%s GNU pic supports a linethick variable to set line thickness; DWB and 10th Ed. do not\n",
	    EscComment);
    fprintf(Output_file, "linethick = 0; oldlinethick = linethick\n");
    fprintf(Output_file,
	    "%s .PS w/o args causes GNU pic to scale drawing to fit 8.5x11 paper; DWB does not\n",
	    EscComment);
    fprintf(Output_file,
	    "%s maxpsht and maxpswid have no meaning in DWB 2.0, set page boundaries in gpic and in 10th Edition\n",
	    EscComment);
    fprintf(Output_file,
	    "%s maxpsht and maxpswid are predefined to 11.0 and 8.5 in gpic\n",
	    EscComment);
    fprintf(Output_file, "maxpsht = %f\nmaxpswid = %f\n", height, width);
    fprintf(Output_file, "Dot: [\n");
    fprintf(Output_file,
	    "define attrs0 %% %%; define unfilled %% %%; define rounded %% %%; define diagonals %% %%\n");
}

static void pic_end_page(void)
{
    fprintf(Output_file, "]\n.PE\n");
    EscComment = ".\\\" ";	/* troff comment */
    assert(SP == 0);
}

static void pic_begin_node(node_t * n)
{
    fprintf(Output_file, "%s\t%s\n", EscComment, agnameof(n));
}

static void pic_begin_edge(edge_t * e)
{
    fprintf(Output_file, "%s\t%s -> %s\n", EscComment,
	    agnameof(agtail(e)), agnameof(aghead(e)));
}

static void pic_begin_context(void)
{
    fprintf(Output_file, "{\n");
    if (SP == STACKSIZE - 1)
	warn("stk ovfl");
    else {
	SP++;
	S[SP] = S[SP - 1];
	fprintf(Output_file, "define attrs%d %% %%\n", SP);	/* ensure plain (no attributes) style at start of context */
    }
}

static void pic_end_context(void)
{
    if (SP == 0)
	warn("stk undfl");
    else {
	SP--;
	fprintf(Output_file, "}\n");	/* end context group */
	/* restore correct font and size for context */
	if (S[SP + 1].font
	    && (!(S[SP].font) || strcmp(S[SP + 1].font, S[SP].font)))
	    fprintf(Output_file, ".ft %s\n", picfontname(S[SP].font));
	if (S[SP + 1].size != S[SP].size) {
	    int sz;

	    if ((sz = (int) (S[SP].size * Scale)) < 1)
		sz = 1;
	    fprintf(Output_file, ".ps %d*\\n(SFu/%.0fu\n", sz, Fontscale);
	}
	fprintf(Output_file, "linethick = oldlinethick\n");
    }
}

static void pic_set_font(char *name, double size)
{
    if (name && (!(S[SP].font) || strcmp(S[SP].font, name))) {
	S[SP].font = name;
	fprintf(Output_file, ".ft %s\n", picfontname(name));
    }
    if (size != S[SP].size) {
	int sz;

	S[SP].size = size;
	if ((sz = (int) (size * Scale)) < 1)
	    sz = 1;
	fprintf(Output_file, ".ps %d*\\n(SFu/%.0fu\n", sz, Fontscale);
    }
}

static char *pic_string(char *s)
{
    static char *buf = NULL;
    static int bufsize = 0;
    int pos = 0;
    char *p;

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
	if (*s == '\015') {	/* GACK, PTUI! Fire up the teletype, boys;
				   somebody's sending an old-fashioned mechanical
				   "carriage return" control character. */
	    s++;
	    continue;
	}
	if (*s == '\\') {
	    strcpy(p, "\\(rs");	/* e.g. man 5 groff_char from mkssoftware.com */
	    p += 4;
	    pos += 4;
	    s++;
	    continue;
	}
	*p++ = *s++;
	pos++;
    }
    *p = '\0';
    return buf;
}

static void pic_textpara(point p, textpara_t * para)
{
    pointf pf;
    short flag = 0;
    double fontsz = S[SP].size;

    switch (para->just) {
    case 'l':
	p.x = p.x;
	break;
    case 'r':
	p.x = p.x - para->width;
	break;
    default:
    case 'n':
	p.x = p.x - para->width / 2;
	break;
    }
    pf = cvt2ptf(p);
    /* Why on earth would we do this. But it works. SCN 2/26/2002 */
    /* adjust text baseline for quirks of the renderer */
    pf.y += fontsz / (3.0 * POINTS_PER_INCH);
    pf.x += para->width / (2.0 * POINTS_PER_INCH);
    if (!(S[SP].size)) {	/* size was never set in this or hierarchically higher context */
	pic_set_font(S[SP].font, fontsz);	/* primarily to output font and/or size directives */
	for (flag = SP; ((S[flag].size = fontsz), flag); flag--)	/* set size in contexts */
	    ;			/* flag is zero again at loop termination */
    }
    if (fontsz != S[SP].size) {	/* size already set in context,
				   but different from request; start new context */
	flag = 1;
	pic_begin_context();
	pic_set_font(S[SP - 1].font, fontsz);
    }
    fprintf(Output_file, "\"%s\" at (%.5f,%.5f);\n",
	    pic_string(para->str), Scale * pf.x, Scale * pf.y);
    if (flag)
	pic_end_context();
}

static void pic_set_color(char *name)
{
    gvcolor_t color;

    S[SP].color = name;
    colorxlate(name, &color, HSVA_DOUBLE);
    /* just v used to set grayscale value */
    fprintf(Output_file, "setfillval %f\n", color.u.HSVA[2]);
}

static void pic_set_style(char **s)
{
    const char *line, *p;
    char skip = 0;
    char buf[BUFSIZ];

    buf[0] = '\0';
    fprintf(Output_file, "define attrs%d %%", SP);
    while ((p = line = *s++)) {
	while (*p)
	    p++;
	p++;
	while (*p) {
	    if (!strcmp(line, "setlinewidth")) {	/* a hack to handle the user-defined (PS) style spec in proc3d.dot */
		long n = atol(p);

		sprintf(buf,
			"oldlinethick = linethick;linethick = %ld * scalethickness / %.0f\n",
			n, Fontscale / Scale);
		skip = 1;
	    } else
		fprintf(Output_file, " %s", p);
	    while (*p)
		p++;
	    p++;
	}
	if (!skip)
	    fprintf(Output_file, " %s", line);
	skip = 0;
    }
    fprintf(Output_file, " %%\n");
    fprintf(Output_file, "%s", buf);
}

static void pic_ellipse(point p, int rx, int ry, int filled)
{
    pointf pf;

    pf = cvt2ptf(p);
    fprintf(Output_file,
	    "ellipse attrs%d %swid %.5f ht %.5f at (%.5f,%.5f);\n", SP,
	    filled ? "fill " : "", Scale * PS2INCH(2 * rx),
	    Scale * PS2INCH(2 * ry), Scale * pf.x, Scale * pf.y);
}

static void point_list_out(point * A, int n, int close)
{
    int j;
    char buf[SMALLBUF];

    for (j = 0; j < n; j++)
	fprintf(Output_file, "P%d: %s\n", j, pic_coord(buf, A[j]));
    for (j = 0; j + 1 < n; j++)
	fprintf(Output_file, "move to P%d; line attrs%d to P%d\n", j, SP,
		j + 1);
    if (close)
	fprintf(Output_file, "move to P%d; line attrs%d to P0\n", n - 1,
		SP);
}

static void pic_polygon(point * A, int n, int filled)
{
    /* test for special case: rectangle oriented with page */
    if ((n == 4) && (((A[0].x == A[1].x) && (A[0].y == A[3].y)
		      && (A[1].y == A[2].y) && (A[2].x == A[3].x))
		     || ((A[0].y == A[1].y) && (A[0].x == A[3].x)
			 && (A[1].x == A[2].x) && (A[2].y == A[3].y))
	)) {
	pointf pf1, pf2;

	pf1 = cvt2ptf(A[0]);	/* opposite */
	pf2 = cvt2ptf(A[2]);	/* corners  */
	if (filled) {
	    gvcolor_t color;

	    colorxlate(S[SP].color, &color, HSVA_DOUBLE);
	    fprintf(Output_file, "setfillval %f\n", color.u.HSVA[2]);
	}
	fprintf(Output_file, "box attrs%d %swid %.5f ht %.5f at (%.5f,%.5f);\n", SP, filled ? "fill " : "", Scale * fabs(pf1.x - pf2.x), Scale * fabs(pf1.y - pf2.y),	/* width, height */
		Scale * (pf1.x + pf2.x) / 2.0, Scale * (pf1.y + pf2.y) / 2.0);	/* center coordinates */
	return;
    }
    if (onetime && filled) {
	unsupported("shape fill");
	onetime = FALSE;
    }
    point_list_out(A, n, TRUE);
}

static void pic_polyline(point * A, int n)
{
    point_list_out(A, n, FALSE);
}

static void pic_usershape(usershape_t *us, boxf b, point *A, int n, boolean filled)
{
/* FIXME */
    /* it's not at all clear what xxx_user_shape is supposed to do; in most xxxgen.c files it emits a message */
    /* this defines the shape as a macro and then invokes the macro */
    fprintf(Output_file, "define %s {\n", us->name);
    fprintf(Output_file, "}\n%s\n", us->name);
}

static void pic_bezier(point * A, int n, int arrow_at_start,
		       int arrow_at_end, int filled)
{
    pointf V[4], p;
    int i, j, m, step;
    char buf[SMALLBUF];

    if (arrow_at_start || arrow_at_end)
	warn("not supposed to be making arrows here!");
    V[3] = cvt2ptf(A[0]);	/* initial cond */
    for (i = m = 0; i + 3 < n; i += 3) {
	V[0] = V[3];
	for (j = 1; j <= 3; j++)
	    V[j] = cvt2ptf(A[i + j]);
	p = Bezier(V, 3, 0.0, NULL, NULL);
	if (!i)
	    fprintf(Output_file, "P0: %s\n", pic_fcoord(buf, p));
	for (step = 1; step <= BezierSubdivision; step++) {
	    p = Bezier(V, 3, (double) step / BezierSubdivision, NULL,
		       NULL);
	    ++m;
	    fprintf(Output_file, "P%d: %s\n", m, pic_fcoord(buf, p));
	}
    }
    for (i = 0; i + 2 <= m; i += 2)	/* DWB 2 pic suffers from severe roundoff errors if too many steps are plotted at once */
	fprintf(Output_file, "move to P%d; line attrs%d to P%d then to P%d\n", i, SP, i + 1, i + 2);	/* use line, as splines can't be dotted or dashed */
}

static void pic_comment(char *str)
{
    fprintf(Output_file, "'\\\" %s\n", str);
}

codegen_t PIC_CodeGen = {
    pic_reset,
    pic_begin_job, 0,		/* pic_end_job */
    pic_begin_graph, pic_end_graph,
    pic_begin_page, pic_end_page,
    0, /* pic_begin_layer */ 0,	/* pic_end_layer */
    0, /* pic_begin_cluster */ 0,	/* pic_end_cluster */
    0, /* pic_begin_nodes */ 0,	/* pic_end_nodes */
    0, /* pic_begin_edges */ 0,	/* pic_end_edges */
    pic_begin_node, 0,		/* pic_end_node */
    pic_begin_edge, 0,		/* pic_end_edge */
    pic_begin_context, pic_end_context,
    0, /* pic_begin_anchor */ 0,	/* pic_end_anchor */
    pic_set_font, pic_textpara,
    pic_set_color, pic_set_color, pic_set_style,
    pic_ellipse, pic_polygon,
    pic_bezier, pic_polyline,
    0,				/* bezier_has_arrows */
    pic_comment,
    pic_usershape
};
