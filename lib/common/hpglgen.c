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


/* TODO:
 *  Use encoded form for polyline and polygon
 */
#include <ctype.h>
#include "render.h"

#define SOLID  0
#define DOTTED 1
#define DASHED 2
#define INVIS  3
  /* Convert point (1/72 inch) to hpgl units (0.025 mm) */
#define PT2UNIT(p)  ((p)*(double)14.111)
#define PENW 0.0138
#define NPENS  32
#define CX(_x)   ((int)(_x))
#define CY(_y)   ((int)(_y))

    /* Origin of HP plotter from lower left corner, in points
     * This varies from plotter to plotter. We assume 1/4" and 
     * hope for the best.
     */
#define HP_OX     18
#define HP_OY     18

static char *raw_prefix = "";
static char *raw_suffix = "";
#ifdef NOTUSED_ERG_MUST_KNOW_WHY
static char *clr_prefix = "\033%-12345X@PJL ENTER LANGUAGE = HPGL2\n";
static char *clr_suffix = "\033%-12345X\n";
#endif
static char *pcl_prefix = "\033E\n\033%%0B\n";
static char *pcl_suffix = "\033%%0A\n";

static int N_pages;
/* static 	point	Pages; */
static double Scale;
static point Origin;
static box PB;
static int CurrentPen;
static int ColorsUsed;
static char *Sep = ";";
static int PageWidth;		/* Width of page, in points. */
static char *prefix;		/* Machine-dependent prefix and suffix */
static char *suffix;
/* static boolean	onetime = TRUE; */

#define MAXLINELEN   80
static int bufcnt;		/* Number of characters output on current line */
static char *text_hdr = "LB";
static void output(char *str)
{
    char *ptr = str;
    int len;

    while (*ptr != '\0')
	ptr++;
    len = ptr - str;
    if (bufcnt + len > MAXLINELEN) {
	fputs("\n", Output_file);
	bufcnt = 0;
    }
    fputs(str, Output_file);
    if ((len > 0) && (*(ptr - 1) == '\n'))
	bufcnt = 0;
    else
	bufcnt += len;
}

static void output_text(char *str)
{
    char *ptr = str;
    int len;
    char text_tail[32];

    sprintf(text_tail, "\03%s\n", Sep);
    while (*ptr != '\0')
	ptr++;
    len = (ptr - str) + strlen(text_tail) + strlen(text_hdr);
    if (bufcnt + len > MAXLINELEN) {
	fputs("\n", Output_file);
    }
    fputs(text_hdr, Output_file);
    fputs(str, Output_file);
    fputs(text_tail, Output_file);
    bufcnt = 0;
}

#ifdef SMILE
void doSmile(void)
{
    fprintf(Output_file,
	    "SP1SD1,341,2,1,4,14,5,0,6,0,7,5SSLO7PA%d,0LB\001\003IN\n",
	    (int) PT2UNIT(PageWid - 2 * HP_OX));

}
#endif

static void setPen(int p)
{
    char buffer[32];
    sprintf(buffer, "SP%d%s", p, Sep);
    output(buffer);
#ifdef HPDEBUG
    fprintf(stderr, "set pen %d\n", p);
#endif
    CurrentPen = p;
}

typedef struct {
    int symbol;
    int spacing;
    int face;
    int bold;
    int italic;
    double size;
} FontInfo;
static FontInfo dfltFont = { 277, 1, 5, 0, 0, 14.0 };
static FontInfo coordFont = { 277, 1, 5, 0, 0, 8.0 };
static FontInfo nullFont = { 0, 0, 0, 0, 0, 0.0 };

  /* Font 0 is the stdfont; font 1 is the alt font */
typedef struct {
    FontInfo fonts[2];
    int curfont;
} FontState;
static FontState fontState;

static int eqFontInfo(FontInfo * fi1, FontInfo * fi2)
{
    return ((fi1->face == fi2->face) &&
	    (fi1->spacing == fi2->spacing) &&
	    (fi1->bold == fi2->bold) &&
	    (fi1->italic == fi2->italic) &&
	    (fi1->size == fi2->size) && (fi1->symbol == fi2->symbol));
}

typedef struct {
    unsigned char r, g, b;
} Color;

static int eqColor(Color * c1, Color * c2)
{
    return ((c1->r == c2->r) && (c1->g == c2->g) && (c1->b == c2->b));
}

static Color black = { 0, 0, 0 };
static Color white = { 255, 255, 255 };
static Color *colorlist;

typedef struct GC_s {
    int bold;
    int style;
    Color color;
    FontInfo font;
    struct GC_s *prev;
} GC_t;
static GC_t *curGC;

static void set_color(Color * cp)
{
    int i;
    char buffer[32];

    if (eqColor(cp, &curGC->color))
	return;
    for (i = 0; i < ColorsUsed; i++) {
	if (eqColor(cp, &colorlist[i]))
	    break;
    }
    if (i == ColorsUsed) {
	if (ColorsUsed == NPENS)
	    i--;
	else
	    ColorsUsed++;
	sprintf(buffer, "PC%d,%d,%d,%d%s", i, cp->r, cp->g, cp->b, Sep);
	colorlist[i] = *cp;
	output(buffer);
#ifdef HPDEBUG
	fprintf(stderr, "set pen %d color %d %d %d\n", i, cp->r, cp->g,
		cp->b);
#endif
    }
    setPen(i);
    curGC->color = *cp;
}

static void initColors(void)
{
    colorlist = N_GNEW(NPENS, Color);
    colorlist[0] = white;
    colorlist[1] = black;
    ColorsUsed = 2;
}

static void destroyColors(void)
{
    free(colorlist);
    ColorsUsed = 0;
}

static void setFont(FontInfo * fi)
{
    int otherfont;
    char buffer[128];

    if (eqFontInfo(fi, &fontState.fonts[fontState.curfont]))
	return;
    otherfont = (fontState.curfont ? 0 : 1);

    if (!eqFontInfo(fi, &fontState.fonts[otherfont])) {
	if (fi->spacing)
	    sprintf(buffer, "%s1,%d,2,1,4,%.1f,5,%d,6,%d,7,%d%s",
		    (otherfont ? "AD" : "SD"), fi->symbol,
		    Scale * (fi->size), fi->italic, fi->bold, fi->face,
		    Sep);
	else
	    sprintf(buffer, "%s1,%d,2,0,3,%.1f,5,%d,6,%d,7,%d%s",
		    (otherfont ? "AD" : "SD"), fi->symbol,
		    (fi->size) / Scale, fi->italic, fi->bold, fi->face,
		    Sep);
	output(buffer);
    }
    sprintf(buffer, "%s%s\n", (otherfont ? "SA" : "SS"), Sep);
    output(buffer);
    fontState.curfont = otherfont;
    fontState.fonts[otherfont] = *fi;
    curGC->font = *fi;
}

static void set_line_bold(int on)
{
    char buffer[32];

    if (on) {
	sprintf(buffer, "PW%.3f%s\n", 2 * PENW, Sep);
	curGC->bold = TRUE;
    } else {
	sprintf(buffer, "PW%.3f%s\n", PENW, Sep);
	curGC->bold = FALSE;
    }
    output(buffer);
}

static void set_line_style(int sty)
{
    char buffer[8];
    char *opt = NULL;

    curGC->style = sty;
    switch (sty) {
    case SOLID:
	opt = "LT";
	break;
    case DOTTED:
	opt = "LT1";
	break;
    case DASHED:
	opt = "LT2";
	break;
    case INVIS:
    default:
	return;
    }
    sprintf(buffer, "%s%s", opt, Sep);
    output(buffer);
}

static GC_t *makeGC(GC_t * old)
{
    GC_t *newGC;
    newGC = GNEW(GC_t);
    if (old)
	*newGC = *old;
    else {
	newGC->bold = FALSE, newGC->style = SOLID, newGC->color = black;
	newGC->font = dfltFont;
    }
    newGC->prev = 0;
    return newGC;
}

static void initGC(void)
{
    char buffer[32];

    curGC = makeGC(0);
    /* Pick pen 1; set default pen width; set colors
     */
    sprintf(buffer, "SP1%sPW%.3f%s\n", Sep, PENW, Sep);
    output(buffer);
    fontState.curfont = 1;
    setFont(&dfltFont);
    CurrentPen = 1;
    initColors();
}

static void destroyGC(void)
{
    GC_t *gc, *gc1;
    for (gc = curGC; gc; gc = gc1) {
	gc1 = gc->prev;
	free(gc);
    }
    curGC = 0;
    fontState.fonts[0] = nullFont;
    fontState.fonts[1] = nullFont;
    fontState.curfont = 1;
    destroyColors();
}

static void saveGC(void)
{
    GC_t *newGC;
    newGC = makeGC(curGC);
    newGC->prev = curGC;
    curGC = newGC;
}

static void restoreGC(void)
{
    GC_t *gc, *newGC;
    gc = curGC;
    newGC = gc->prev;
    if (gc->bold != newGC->bold)
	set_line_bold(newGC->bold);
    if (gc->style != newGC->style)
	set_line_style(newGC->style);
    if (!eqColor(&gc->color, &newGC->color)) {
#ifdef HPDEBUG
	fprintf(stderr, "restore color\n");
#endif
	set_color(&newGC->color);
    }
    if (!eqFontInfo(&gc->font, &newGC->font))
	setFont(&newGC->font);
    free(gc);
    curGC = newGC;
}

static int isInvis(void)
{
    return (curGC->style == INVIS);
}

#if 0				/* not used */
static double _Xalign;
#define getTextAlign() (_Xalign)
static void initTextAlign(void)
{
    char buffer[20];
    _Xalign = -0.5;
    sprintf(buffer, "LO4%s", Sep);
    output(buffer);
}

static int setTextAlign(double al)
{
    char buffer[20];
    char opt;

    if (al == 0.0)
	opt = '1';
    else if (al == -1.0)
	opt = '7';
    else if (al == -0.5)
	opt = '4';
    else
	return 0;

    sprintf(buffer, "LO%c%s", opt, Sep);
    output(buffer);
    _Xalign = al;
    return 1;
}
#endif

static void hpgl_reset(void)
{
    /* onetime = TRUE; */
}

static void
hpgl_begin_job(FILE * ofp, graph_t * g, const char **lib, char *info[], point pages)
{
    /* Pages = pages; */
    N_pages = pages.x * pages.y;
}

static void hpgl_begin_graph(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    PB = bb;
    PageWidth = pb.x;
    if (Output_lang == PCL) {
	prefix = pcl_prefix;
	suffix = pcl_suffix;
    } else {
	prefix = raw_prefix;
	suffix = raw_suffix;
    }
}

static void hpgl_set_scale(double scx, double scy)
{
    char buffer[64];
    sprintf(buffer, "SC%.4f,%.4f,%.4f,%.4f,2%s\n",
	    -Origin.x / scx, PT2UNIT(scx), -Origin.y / scy, PT2UNIT(scy),
	    Sep);
    output(buffer);
}

static void hpgl_begin_page(graph_t * g, point page, double scale, int rot,
			    point offset)
{
    char buffer[64];
    box clipWin;

    bufcnt = 0;
    Scale = scale;

    /* Initialize output */
    output(prefix);
    sprintf(buffer, "BP%sIN%s", Sep, Sep);
    output(buffer);
#ifdef SMILE
    doSmile();
#endif
#if 0				/* not used */
    initTextAlign();
#endif
    initGC();

    if (N_pages > 1) {
	saveGC();
	setFont(&coordFont);
	if (rot == 90) {
	    sprintf(buffer, "RO90IP%s", Sep);
	    output(buffer);
	}
	sprintf(buffer, "PA0,0%sLB(%d,%d)\03%s\n", Sep, page.x, page.y,
		Sep);
	output(buffer);
	if (rot == 90) {
	    sprintf(buffer, "ROIP%s", Sep);
	    output(buffer);
	}
	restoreGC();
    }

    if (rot == 90) {
	/* Rotate layout. HPGL/2 automatically shifts
	 * origin to bottom right corner, so we have to
	 * use the page width to set the new origin.
	 */
	sprintf(buffer, "RO90IP%s", Sep);
	output(buffer);

	clipWin.LL.x = PB.LL.y - HP_OY - 1;
	clipWin.LL.y = PageWidth - PB.UR.x - HP_OX - 1;
	clipWin.UR.x = PB.UR.y - HP_OY + 1;
	clipWin.UR.y = PageWidth - PB.LL.x - HP_OX + 1;
	Origin.x = PB.LL.y + scale * offset.y - HP_OY;
	Origin.y = PageWidth - PB.LL.x - scale * offset.x - HP_OX;
    } else {
	clipWin.LL.x = PB.LL.x - HP_OX - 1;
	clipWin.LL.y = PB.LL.y - HP_OY - 1;
	clipWin.UR.x = PB.UR.x - HP_OX + 1;
	clipWin.UR.y = PB.UR.y - HP_OY + 1;
	Origin.x = PB.LL.x + scale * offset.x - HP_OX;
	Origin.y = PB.LL.y + scale * offset.y - HP_OY;
    }
    /* Set clipping window */
    sprintf(buffer, "IW%d,%d,%d,%d%s\n",
	    (int) PT2UNIT(clipWin.LL.x), (int) PT2UNIT(clipWin.LL.y),
	    (int) PT2UNIT(clipWin.UR.x), (int) PT2UNIT(clipWin.UR.y), Sep);
    /* output(buffer); *//* Turn off clipping. */
    hpgl_set_scale(scale, scale);

}

static void hpgl_end_page(void)
{
    char buffer[32];

    sprintf(buffer, "PU%sSP0%sPG;\n", Sep, Sep);	/* pen up; advance page */
    output(buffer);
    output(suffix);
    destroyGC();
}

static void hpgl_begin_context(void)
{
    saveGC();
}

static void hpgl_end_context(void)
{
    restoreGC();
}

static void mkFontCanon(unsigned char *old, unsigned char *new)
{
    unsigned char c;
    while ((c = *old++)) {
	if (isalnum(c) == FALSE)
	    continue;
	if (isupper(c))
	    c = tolower(c);
	*new++ = c;
    }
    *new = c;
}

  /* factors for turning font size, in points,
   * to pitches, in chars per inch, for fixed pitch fonts.
   */
static double courierPitch = 110.76923;
static double stickPitch = 102.85714;

typedef struct {
    char *name;
    int symbol;
    double *spacing;
    int face;
    int italic;
    int bold;
} FontIndex;
static FontIndex fontIndex[] = {
    {"timesroman", 277, 0, 5, 0, 0},
    {"timesbold", 277, 0, 5, 0, 3},
    {"timesitalic", 277, 0, 5, 1, 0},
    {"timesbolditalic", 277, 0, 5, 1, 3},
    {"helvetica", 277, 0, 4, 0, 0},
    {"helveticabold", 277, 0, 4, 0, 3},
    {"helveticaoblique", 277, 0, 4, 1, 0},
    {"helveticaboldoblique", 277, 0, 4, 1, 3},
    {"courier", 277, &courierPitch, 3, 0, 0},
    {"courierbold", 277, &courierPitch, 3, 0, 3},
    {"courieroblique", 277, &courierPitch, 3, 1, 0},
    {"courierboldoblique", 277, &courierPitch, 3, 1, 3},
    {"palatinoroman", 277, 0, 15, 0, 0},
    {"palatinobold", 277, 0, 15, 0, 3},
    {"palatinoitalic", 277, 0, 15, 1, 0},
    {"palatinobolditalic", 277, 0, 15, 1, 3},
    {"stickcw", 277, &stickPitch, 48, 0, 0},
    {"stick", 277, 0, 48, 0, 0},
    {"zapfdingbats", 332, 0, 45, 0, 0},
    {"symbol", 173, 0, 5, 0, 0}
};

static void mkFontInfo(char *name, double size, FontInfo * fip)
{
    int i;
    char buf[128];
    FontIndex *fi;

    mkFontCanon((unsigned char *) name, (unsigned char *) buf);
    fi = fontIndex;
    for (i = 0; i < sizeof(fontIndex) / sizeof(FontIndex) - 1; i++) {
	if (streq(buf, fi->name))
	    break;
	fi++;
    }
    fip->symbol = fi->symbol;
    fip->italic = fi->italic;
    fip->bold = fi->bold;
    fip->face = fi->face;
    if (fi->spacing) {		/* fixed spacing */
	fip->spacing = 0;
	fip->size = (*(fi->spacing)) / size;
    } else {			/* proportional spacing */
	fip->spacing = 1;
	fip->size = size;
    }
}

static void hpgl_set_font(char *name, double size)
{
    static FontInfo fi;

    mkFontInfo(name, size, &fi);
    setFont(&fi);
}

static void hpgl_set_color(char *name)
{
    gvcolor_t color;

#ifdef HPDEBUG
    fprintf(stderr, "set color %s\n", name);
#endif
    colorxlate(name, &color, RGBA_BYTE);
    set_color((Color *) color.u.rgba);
}

static void hpgl_set_style(char **s)
{
    char *line;

    while ((line = *s++)) {
	if (streq(line, "solid"))
	    set_line_style(SOLID);
	else if (streq(line, "dashed"))
	    set_line_style(DASHED);
	else if (streq(line, "dotted"))
	    set_line_style(DOTTED);
	else if (streq(line, "invis"))
	    set_line_style(INVIS);
	else if (streq(line, "bold"))
	    set_line_bold(TRUE);
	else if (streq(line, "filled")) {	/* no-op */
	} else if (streq(line, "unfilled")) {	/* no-op */
	} else {
	    agerr(AGERR,
		  "hpgl_set_style: unsupported style %s - ignoring\n",
		  line);
	}
    }
}

static void hpgl_textpara(point p, textpara_t * para)
{
    char buffer[128];

    if (isInvis())
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

    sprintf(buffer, "PA%d,%d%s", CX(p.x), CY(p.y), Sep);
    output(buffer);
    output_text(para->str);

#ifdef HPDEBUG
    fprintf(stderr, "text =%s=\n", para->str);
#endif
}

static int firstSeg;
#define FLATNESS  1.0
static int isFlat(double x0, double y0, double x1, double y1, double x2,
		  double y2, double x3, double y3)
{
    double sa, ca, y, O = y3 - y0, A = x3 - x0, H = sqrt(O * O + A * A);

    if (H == 0)
	return TRUE;

    sa = O / H, ca = A / H;
    y = -sa * (x1 - x0) + ca * (y1 - y0);
    if (y > FLATNESS || y < -FLATNESS)
	return FALSE;
    y = -sa * (x2 - x0) + ca * (y2 - y0);
    return y <= FLATNESS && y >= -FLATNESS;
}

static void Bzier(double x0, double y0, double x1, double y1, double x2,
		  double y2, double x3, double y3)
{
    char buffer[64];
    if (isFlat(x0, y0, x1, y1, x2, y2, x3, y3)) {
	if (firstSeg) {
	    sprintf(buffer, "%d,%d", CX(x3), CY(y3));
	    firstSeg = 0;
	} else {
	    sprintf(buffer, ",%d,%d", CX(x3), CY(y3));
	}
	output(buffer);
	return;
    }
    Bzier(x0, y0,
	  (x0 + x1) / 2, (y0 + y1) / 2,
	  (x0 + x2) / 4 + x1 / 2, (y0 + y2) / 4 + y1 / 2,
	  (x0 + x3) / 8 + 3 * (x1 + x2) / 8,
	  (y0 + y3) / 8 + 3 * (y1 + y2) / 8);
    Bzier((x0 + x3) / 8 + 3 * (x1 + x2) / 8,
	  (y0 + y3) / 8 + 3 * (y1 + y2) / 8, (x1 + x3) / 4 + x2 / 2,
	  (y1 + y3) / 4 + y2 / 2, (x2 + x3) / 2, (y2 + y3) / 2, x3, y3);

}

static void hpgl_bezier(point * A, int n, int arrow_at_start,
			int arrow_at_end, int filled)
{
    char buffer[32];
    int j;

    if (arrow_at_start || arrow_at_end)
	agerr(AGERR, "hpgl_bezier illegal arrow args\n");
    if (isInvis())
	return;
    sprintf(buffer, "PA%d,%d%sPD", CX(A[0].x), CY(A[0].y), Sep);
    output(buffer);
    firstSeg = 1;
    for (j = 1; j < n; j += 3)
	Bzier((double) A[j - 1].x, (double) A[j - 1].y,
	      (double) A[j].x, (double) A[j].y,
	      (double) A[j + 1].x, (double) A[j + 1].y,
	      (double) A[j + 2].x, (double) A[j + 2].y);
    sprintf(buffer, "%sPU%s\n", Sep, Sep);
    output(buffer);
}

static void hpgl_polygon(point * A, int n, int filled)
{
    int j;
    char buffer[64];

    if (isInvis())
	return;
    sprintf(buffer, "PA%d,%d%sPM0%sPD", CX(A[0].x), CY(A[0].y), Sep, Sep);
    output(buffer);
    for (j = 1; j < n - 1; j++) {
	sprintf(buffer, "%d,%d,", CX(A[j].x), CY(A[j].y));
	output(buffer);
    }
    sprintf(buffer, "%d,%d%sPM2%sPU%s", CY(A[n - 1].x), CY(A[n - 1].y),
	    Sep, Sep, Sep);
    output(buffer);
    if (filled) {
#ifdef HPDEBUG
	fprintf(stderr, "fill pen %d\n", CurrentPen);
#endif
	if (CurrentPen == 1) {
	    sprintf(buffer, "FP%sLT%sEP%sLT99%s\n", Sep, Sep, Sep, Sep);
	} else {
	    sprintf(buffer, "FP%sSP1%sLT%sEP%sSP%d%sLT99%s\n",
		    Sep, Sep, Sep, Sep, CurrentPen, Sep, Sep);
	}
    } else {
	sprintf(buffer, "EP%s\n", Sep);
    }
    output(buffer);
}

/***** Arrowheads now centralized in emit.c
static void hpgl_arrowhead(point p,double theta,double scale,int flag)
{
    point   sp,ep;
    double  costh, sinth,arroww2,arrowl;
    char    buffer[128];

    if (isInvis()) return;
    costh = cos(RADIANS(theta));
    sinth = sin(RADIANS(theta));
	arrowl = ARROW_LENGTH * scale;
	arroww2 = ARROW_WIDTH * scale / 2.0;
    sp.x = p.x + arrowl*costh + arroww2*sinth;
    sp.y = p.y + arrowl*sinth - arroww2*costh;
    ep.x = p.x + arrowl*costh - arroww2*sinth;
    ep.y = p.y + arrowl*sinth + arroww2*costh;
	sprintf(buffer,"PA%d,%d%sPM0%sPD%d,%d,%d,%d%sPM2%sPU%sFP%s\n",
      CX(sp.x),CY(sp.y),Sep,Sep,
      CX(p.x),CY(p.y),CX(ep.x),CY(ep.y), Sep, Sep, Sep, Sep);
    output(buffer);
}
**********/

static void hpgl_ellipse(point p, int rx, int ry, int filled)
{
    char buffer[128];

    if (isInvis())
	return;
    sprintf(buffer, "PA%d,%d%s", p.x, p.y, Sep);
    output(buffer);
    hpgl_set_scale(Scale * rx, Scale * ry);
    if (filled) {
	if (CurrentPen == 1)
	    sprintf(buffer, "WG1,0,360%sLT%sEW1,0,360%sLT99%s", Sep, Sep,
		    Sep, Sep);
	else
	    sprintf(buffer, "WG1,0,360%sSP1%sLT%sEW1,0,360%sSP%d%sLT99%s",
		    Sep, Sep, Sep, Sep, CurrentPen, Sep, Sep);
    } else
	sprintf(buffer, "EW1,0,360%s", Sep);
    output(buffer);
    hpgl_set_scale(Scale, Scale);
}

/* Use encoded form */
static void hpgl_polyline(point * A, int n)
{
    int j;
    char buffer[64];

    if (isInvis())
	return;
    sprintf(buffer, "PA%d,%d%sPD", CX(A[0].x), CY(A[0].y), Sep);
    output(buffer);
    for (j = 1; j < n - 1; j++) {
	sprintf(buffer, "%d,%d,", CX(A[j].x), CY(A[j].y));
	output(buffer);
    }
    sprintf(buffer, "%d,%d%sPU%s\n", CX(A[n - 1].x), CY(A[n - 1].y), Sep,
	    Sep);
    output(buffer);
}

static void hpgl_usershape(usershape_t *us, boxf p, point *A, int n, boolean filled)
{
    static boolean onetime = TRUE;
    if (onetime) {
	agerr(AGERR, "custom shapes not available with this driver\n");
	onetime = FALSE;
    }
}

codegen_t HPGL_CodeGen = {
    hpgl_reset,
    hpgl_begin_job, 0,		/* hpgl_end_job */
    hpgl_begin_graph, 0,	/* hpgl_end_graph */
    hpgl_begin_page, hpgl_end_page,
    0, /* hpgl_begin_layer */ 0,	/* hpgl_end_layer */
    0, /* hpgl_begin_cluster */ 0,	/* hpgl_end_cluster */
    0, /* hpgl_begin_nodes */ 0,	/* hpgl_end_nodes */
    0, /* hpgl_begin_edges */ 0,	/* hpgl_end_edges */
    0, /* hpgl_begin_node */ 0,	/* hpgl_end_node */
    0, /* hpgl_begin_edge */ 0,	/* hpgl_end_edge */
    hpgl_begin_context, hpgl_end_context,
    0, /* hpgl_begin_anchor */ 0,	/* hpgl_end_anchor */
    hpgl_set_font, hpgl_textpara,
    hpgl_set_color, hpgl_set_color, hpgl_set_style,
    hpgl_ellipse, hpgl_polygon,
    hpgl_bezier, hpgl_polyline,
    0,				/* bezier_has_arrows */
    0,				/* hpgl_comment */
    hpgl_usershape
};
