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

#include                "render.h"
#include                "gd.h"
#include                "utils.h"
#include                <fcntl.h>
#ifdef MSWIN32
#include <io.h>
#endif

extern gdImagePtr gd_getshapeimage(char *name);
extern void gd_freeusershapes(void);

static gdImagePtr im;

#ifdef _UWIN
#ifndef DEFAULT_FONTPATH
#define		DEFAULT_FONTPATH	"/win/fonts"
#endif				/* DEFAULT_FONTPATH */
#else
#ifndef MSWIN32
#ifndef DEFAULT_FONTPATH
#define		DEFAULT_FONTPATH	"/usr/share/ttf:/usr/local/share/ttf:/usr/share/fonts/ttf:/usr/local/share/fonts/ttf:/usr/lib/fonts:/usr/local/lib/fonts:/usr/lib/fonts/ttf:/usr/local/lib/fonts/ttf:/usr/common/graphviz/lib/fonts/ttf:/windows/fonts:/dos/windows/fonts:/usr/add-on/share/ttf:."
#endif				/* DEFAULT_FONTPATH */
#else
#ifndef DEFAULT_FONTPATH
#define		DEFAULT_FONTPATH	"%WINDIR%/FONTS;C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
#endif				/* DEFAULT_FONTPATH */
#endif				/* MSWIN32 */
#endif				/* _UWIN */

#define BEZIERSUBDIVISION 10

/* font modifiers */
#define REGULAR		0
#define BOLD		1
#define ITALIC		2

/* patterns */
#define P_SOLID		0
#define P_DOTTED	4
#define P_DASHED	11
#define P_NONE		15

/* bold line constant */
#define WIDTH_NORMAL	1
#define WIDTH_BOLD	3

/* static int	N_pages; */
/* static point	Pages; */
static double Dpi;
static double DevScale;
static double CompScale;
static int Rot;

static point Viewport;
static pointf GraphFocus;
static double Zoom;

typedef struct context_t {
    int pencolor, fillcolor;
    char *fontfam, fontopt, font_was_set, pen, fill, penwidth;
    double fontsz;
} context_t;

#define MAXNEST	4
static context_t cstk[MAXNEST];
static int SP;
static node_t *Curnode;

static int white, black, transparent;

static int gd_resolve_color(char *name)
{
    color_t color;

    if (!(strcmp(name, "transparent"))) {
	/* special case for "transparent" color */
	return transparent;
    } else {
	colorxlate(name, &color, RGBA_BYTE);
	/* seems gd alpha is "transparency" rather than the usual "opacity" */
	return gdImageColorResolveAlpha(im,
					color.u.rgba[0],
					color.u.rgba[1],
					color.u.rgba[2],
					(255 -
					 color.u.rgba[3]) * gdAlphaMax /
					255);
    }
}

static pointf gdpt(pointf p)
{
    pointf rv;

    if (Rot == 0) {
	rv.x = (p.x - GraphFocus.x) * CompScale + Viewport.x / 2.;
	rv.y = -(p.y - GraphFocus.y) * CompScale + Viewport.y / 2.;
    } else {
	rv.x = -(p.y - GraphFocus.y) * CompScale + Viewport.x / 2.;
	rv.y = -(p.x - GraphFocus.x) * CompScale + Viewport.y / 2.;
    }
    return rv;
}

static void gd_begin_job(FILE * ofp, graph_t * g, char **lib, char *user,
			 char *info[], point pages)
{
/*	Pages = pages; */
/*	N_pages = pages.x * pages.y; */
#ifdef MYTRACE
    fprintf(stderr, "gd_begin_job\n");
#endif
}

static void gd_end_job(void)
{
#ifdef MYTRACE
    fprintf(stderr, "gd_end_job\n");
#endif
}

static void init1_gd(GVC_t * gvc, graph_t * g, box bb, point pb)
{
    Dpi = GD_drawing(g)->dpi;
    if (Dpi < 1.0)
	Dpi = DEFAULT_DPI;
    DevScale = Dpi / POINTS_PER_INCH;

    Viewport.x = gvc->job->width;
    Viewport.y = gvc->job->height;
#if 0
    if (Viewport.x) {
	Zoom = gvc->job->zoom;
	GraphFocus = gvc->job->focus;
    } else {
	Viewport.x =
	    (bb.UR.x - bb.LL.x + 2 * GD_drawing(g)->margin.x) * DevScale + 2;
	Viewport.y =
	    (bb.UR.y - bb.LL.y + 2 * GD_drawing(g)->margin.y) * DevScale + 2;
	GraphFocus.x = (GD_bb(g).UR.x - GD_bb(g).LL.x) / 2.;
	GraphFocus.y = (GD_bb(g).UR.y - GD_bb(g).LL.y) / 2.;
	Zoom = 1.0;
    }
#else
    Zoom = gvc->job->zoom;
    GraphFocus = gvc->job->focus;
#endif
    CompScale = Zoom * DevScale;
}

static void init2_gd(gdImagePtr im)
{
    SP = 0;

    white = gdImageColorResolveAlpha(im,
				     gdRedMax, gdGreenMax, gdBlueMax,
				     gdAlphaOpaque);
    black = gdImageColorResolveAlpha(im, 0, 0, 0, gdAlphaOpaque);
    transparent = gdImageColorResolveAlpha(im,
					   gdRedMax - 1, gdGreenMax,
					   gdBlueMax, gdAlphaTransparent);
    gdImageColorTransparent(im, transparent);

    cstk[0].pencolor = black;	/* set pen black */
    cstk[0].fontfam = "times";	/* font family name */
    cstk[0].fontopt = REGULAR;	/* modifier: REGULAR, BOLD or ITALIC */
    cstk[0].pen = P_SOLID;	/* pen pattern style, default is solid */
    cstk[0].fill = P_NONE;
    cstk[0].penwidth = WIDTH_NORMAL;
}

static boolean is_format_truecolor_capable(int Output_lang)
{
    boolean rv = FALSE;

    if (Output_lang == GD) {
	rv = TRUE;
#ifdef HAVE_LIBZ
    } else if (Output_lang == GD2) {
	rv = TRUE;
#endif
#ifdef HAVE_GD_GIF
    } else if (Output_lang == GIF) {
	rv = FALSE;
#endif
#ifdef HAVE_GD_JPEG
    } else if (Output_lang == JPEG) {
	rv = TRUE;
#endif
#ifdef HAVE_GD_PNG
    } else if (Output_lang == PNG) {
	rv = TRUE;
#endif
    } else if (Output_lang == WBMP) {
	rv = FALSE;
#ifdef HAVE_GD_XPM
    } else if (Output_lang == XBM) {
	rv = FALSE;
#endif
    }
    return rv;
}

static void gd_begin_graph_to_file(GVC_t * gvc, graph_t * g, box bb,
				   point pb)
{
    char *bgcolor_str, *truecolor_str;
    boolean truecolor_p = FALSE;	/* try to use cheaper paletted mode */
    boolean bg_transparent_p = FALSE;
    int bgcolor;

    init1_gd(gvc, g, bb, pb);

    truecolor_str = agget(g, "truecolor");	/* allow user to force truecolor */
    bgcolor_str = agget(g, "bgcolor");

    if (truecolor_str && truecolor_str[0])
	truecolor_p = mapbool(truecolor_str);

    if (bgcolor_str && strcmp(bgcolor_str, "transparent") == 0) {
	bg_transparent_p = TRUE;
	if (is_format_truecolor_capable(Output_lang))
	    truecolor_p = TRUE;	/* force truecolor */
    }

    if (GD_has_images(g))
	truecolor_p = TRUE;	/* force truecolor */

    if (truecolor_p) {
	if (Verbose)
	    fprintf(stderr, "%s: allocating a %dK TrueColor GD image\n",
		    CmdName, ROUND(Viewport.x * Viewport.y * 4 / 1024.));
	im = gdImageCreateTrueColor(Viewport.x, Viewport.y);
    } else {
	if (Verbose)
	    fprintf(stderr, "%s: allocating a %dK PaletteColor GD image\n",
		    CmdName, ROUND(Viewport.x * Viewport.y / 1024.));
	im = gdImageCreate(Viewport.x, Viewport.y);
    }
    if (!im) {
	agerr(AGERR, "gdImageCreate returned NULL. Malloc problem?\n");
	return;
    }

    init2_gd(im);

    if (bgcolor_str && bgcolor_str[0])
	if (bg_transparent_p)
	    bgcolor = transparent;
	else
	    bgcolor = gd_resolve_color(bgcolor_str);
    else
	bgcolor = white;

    cstk[0].fillcolor = bgcolor;

    /* Blending must be off to lay a transparent bgcolor.
       Nothing to blend with anyway. */
    gdImageAlphaBlending(im, FALSE);

    gdImageFill(im, im->sx / 2, im->sy / 2, bgcolor);

    /* Blend everything else together,
       especially fonts over non-transparent backgrounds */
    gdImageAlphaBlending(im, TRUE);


#ifdef MYTRACE
    fprintf(stderr, "gd_begin_graph_to_file\n");
#endif
}

static void gd_begin_graph_to_memory(GVC_t * gvc, graph_t * g, box bb,
				     point pb)
{
    if (Verbose)
	fprintf(stderr, "%s: using existing GD image\n", CmdName);

    init1_gd(gvc, g, bb, pb);

    im = *(gdImagePtr *) Output_file;

    init2_gd(im);

#ifdef MYTRACE
    fprintf(stderr, "gd_begin_graph_to_memory\n");
#endif
}

static void gd_end_graph_to_file(void)
{
    if (!im)
	return;

/*
 * Windows will do \n -> \r\n  translations on stdout unless told otherwise.
 */
#ifdef HAVE_SETMODE
#ifdef O_BINARY
    setmode(fileno(Output_file), O_BINARY);
#endif
#endif

    /* Only save the alpha channel in outputs that support it if
       the base color was transparent.   Otherwise everything
       was blended so there is no useful alpha info */
    if (im->trueColor) {
	if (is_format_truecolor_capable(Output_lang))
	    gdImageSaveAlpha(im, TRUE);
	else
	    gdImageTrueColorToPalette(im, 0, 256);
    }
    if (Output_lang == GD) {
	gdImageGd(im, Output_file);
#ifdef HAVE_LIBZ
    } else if (Output_lang == GD2) {
#define GD2_CHUNKSIZE 128
#define GD2_RAW 1
#define GD2_COMPRESSED 2
	gdImageGd2(im, Output_file, GD2_CHUNKSIZE, GD2_COMPRESSED);
#endif
#ifdef HAVE_GD_GIF
    } else if (Output_lang == GIF) {
	gdImageGif(im, Output_file);
#endif
#ifdef HAVE_GD_JPEG
    } else if (Output_lang == JPEG) {
	/*
	 * Write IM to OUTFILE as a JFIF-formatted JPEG image, using
	 * quality JPEG_QUALITY.  If JPEG_QUALITY is in the range
	 * 0-100, increasing values represent higher quality but
	 * also larger image size.  If JPEG_QUALITY is negative, the
	 * IJG JPEG library's default quality is used (which should
	 * be near optimal for many applications).  See the IJG JPEG
	 * library documentation for more details.
	 */
#define JPEG_QUALITY -1
	gdImageJpeg(im, Output_file, JPEG_QUALITY);
#endif
#ifdef HAVE_GD_PNG
    } else if (Output_lang == PNG) {
	gdImagePng(im, Output_file);
#endif
    } else if (Output_lang == WBMP) {
	/* Use black for the foreground color for the B&W wbmp image. */
	gdImageWBMP(im, black, Output_file);
#ifdef HAVE_GD_XPM
    } else if (Output_lang == XBM) {
	gdImageXbm(im, Output_file);
#endif
    }
    gd_freeusershapes();
    gdImageDestroy(im);
#ifdef MYTRACE
    fprintf(stderr, "gd_end_graph_to_file\n");
#endif
}

static void gd_end_graph_to_memory(void)
{
/* leave image in memory to be handled by Gdtclft output routines */
#ifdef MYTRACE
    fprintf(stderr, "gd_end_graph_to_memory\n");
#endif
}

static void
gd_begin_page(graph_t * g, point page, double scale, int rot, point offset)
{
    if (!im)
	return;

/*	int		page_number; */
/*	point		sz; */

    Rot = rot;

/*	page_number = page.x + page.y * Pages.x + 1; */
#ifdef MYTRACE
    fprintf(stderr, "gd_begin_page\n");
    fprintf(stderr, " page=%d,%d offset=%d,%d\n", page.x, page.y, offset.x,
	    offset.y);
    fprintf(stderr, " page_number=%d\n", page_number);
#endif
}

void gd_end_page(void)
{
#ifdef MYTRACE
    fprintf(stderr, "gd_end_page\n");
#endif
}

static void gd_begin_node(node_t * n)
{
    Curnode = n;
}

static void gd_end_node(void)
{
    Curnode = NULL;
}

static void gd_begin_context(void)
{
    assert(SP + 1 < MAXNEST);
    cstk[SP + 1] = cstk[SP];
    SP++;
}

static void gd_end_context(void)
{
    int psp = SP - 1;
    assert(SP > 0);
    SP = psp;
}

static void gd_set_font(char *fontname, double fontsize)
{
    context_t *cp;
#if 0
    char *p;
    char *q;
#endif

    cp = &(cstk[SP]);
    cp->fontsz = fontsize;
#if 0
    p = strdup(fontname);
    if ((q = strchr(p, '-'))) {
	*q++ = 0;
	if (strcasecmp(q, "italic") == 0)
	    cp->fontopt = ITALIC;
	else if (strcasecmp(q, "bold") == 0)
	    cp->fontopt = BOLD;
    }
#endif
    cp->fontfam = fontname;
}

static void gd_set_pencolor(char *name)
{
    if (!im)
	return;

    cstk[SP].pencolor = gd_resolve_color(name);
}

static void gd_set_fillcolor(char *name)
{
    if (!im)
	return;

    cstk[SP].fillcolor = gd_resolve_color(name);
}

static void gd_set_style(char **s)
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
	else
	    agerr(AGWARN,
		  "gd_set_style: unsupported style %s - ignoring\n", line);
    }
}

void gd_missingfont(char *err, char *fontreq)
{
    static char *lastmissing = 0;
    static int n_errors = 0;

    if (n_errors >= 20)
	return;
    if ((lastmissing == 0) || (strcmp(lastmissing, fontreq))) {
#if HAVE_GD_FONTCONFIG
	agerr(AGERR, "%s : %s\n", err, fontreq);
#else
	char *p = getenv("GDFONTPATH");
	if (!p)
	    p = DEFAULT_FONTPATH;
	agerr(AGERR, "%s : %s in %s\n", err, fontreq, p);
#endif
	if (lastmissing)
	    free(lastmissing);
	lastmissing = strdup(fontreq);
	n_errors++;
	if (n_errors >= 20)
	    agerr(AGWARN, "(font errors suppressed)\n");
    }
}

extern gdFontPtr gdFontTiny, gdFontSmall, gdFontMediumBold, gdFontLarge,
    gdFontGiant;

#if !defined(DISABLE_CODEGENS) && !defined(HAVE_GD_FREETYPE)
/* builtinFont:
 * Map fontsz in pixels to builtin font.
 */
static gdFontPtr builtinFont(double fsize)
{
    if (fsize <= 8.5) {
	return gdFontTiny;
    } else if (fsize <= 9.5) {
	return gdFontSmall;
    } else if (fsize <= 10.5) {
	return gdFontMediumBold;
    } else if (fsize <= 11.5) {
	return gdFontLarge;
    } else {
	return gdFontGiant;
    }
}

int builtinFontHt(double fontsz)
{
    gdFontPtr fp = builtinFont(fontsz);
    return fp->h;
}
int builtinFontWd(double fontsz)
{
    gdFontPtr fp = builtinFont(fontsz);
    return fp->w;
}
#endif

static void gd_textline(point p, textline_t * line)
{
    char *fontlist, *err;
    pointf mp, ep;
    int brect[8];
    char *str = line->str;
    double fontsz = cstk[SP].fontsz;
    gdFTStringExtra strex;
    int pencolor;

    if (!im)
	return;

    strex.flags = gdFTEX_RESOLUTION;
    strex.hdpi = strex.vdpi = Dpi * Zoom;

    if (cstk[SP].pen == P_NONE)
	return;

    pencolor = cstk[SP].pencolor;
    if (!im->trueColor && cstk[SP].fillcolor == transparent)
	pencolor = -pencolor;	/* disable antialiasing */

    if (strstr(cstk[SP].fontfam, "/"))
	strex.flags |= gdFTEX_FONTPATHNAME;
    else
	strex.flags |= gdFTEX_FONTCONFIG;

    fontlist = gd_alternate_fontlist(cstk[SP].fontfam);

    switch (line->just) {
    case 'l':
	mp.x = p.x;
	break;
    case 'r':
	mp.x = p.x - line->width;
	break;
    default:
    case 'n':
	mp.x = p.x - line->width / 2;
	break;
    }
    ep.y = mp.y = p.y;
    ep.x = mp.x + line->width;

    mp = gdpt(mp);
    if (fontsz * Zoom <= FONTSIZE_MUCH_TOO_SMALL) {
	/* ignore entirely */
    } else if (fontsz * Zoom <= FONTSIZE_TOO_SMALL) {
	/* draw line in place of text */
	ep = gdpt(ep);
	gdImageLine(im, ROUND(mp.x), ROUND(mp.y),
		    ROUND(ep.x), ROUND(ep.y), cstk[SP].pencolor);
    } else {
#ifdef HAVE_GD_FREETYPE
	err = gdImageStringFTEx(im, brect, pencolor,
				fontlist, fontsz, Rot ? (PI / 2) : 0,
				ROUND(mp.x), ROUND(mp.y), str, &strex);
#if 0
	gdImagePolygon(im, (gdPointPtr) brect, 4, cstk[SP].pencolor);
	fprintf(stderr,
		"textline: font=%s size=%g width=%g dpi=%d width/dpi=%g\n",
		fontlist, fontsz, (double) (brect[4] - brect[0]),
		strex.hdpi,
		(((double) (brect[4] - brect[0])) / strex.hdpi));
#endif
	if (err) {
	    /* revert to builtin fonts */
	    gd_missingfont(err, cstk[SP].fontfam);
#endif
	    fontsz = (fontsz * Dpi) / POINTS_PER_INCH;
	    mp.y += 2;
	    if (fontsz <= 8.5) {
		gdImageString(im, gdFontTiny,
			      ROUND(mp.x), ROUND(mp.y - 9.),
			      (unsigned char *) str, cstk[SP].pencolor);
	    } else if (fontsz <= 9.5) {
		gdImageString(im, gdFontSmall,
			      ROUND(mp.x), ROUND(mp.y - 12.),
			      (unsigned char *) str, cstk[SP].pencolor);
	    } else if (fontsz <= 10.5) {
		gdImageString(im, gdFontMediumBold,
			      ROUND(mp.x), ROUND(mp.y - 13.),
			      (unsigned char *) str, cstk[SP].pencolor);
	    } else if (fontsz <= 11.5) {
		gdImageString(im, gdFontLarge,
			      ROUND(mp.x), ROUND(mp.y - 14.),
			      (unsigned char *) str, cstk[SP].pencolor);
	    } else {
		gdImageString(im, gdFontGiant,
			      ROUND(mp.x), ROUND(mp.y - 15.),
			      (unsigned char *) str, cstk[SP].pencolor);
	    }
#ifdef HAVE_GD_FREETYPE
	}
#endif
    }
}

static void
gd_bezier(point * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    pointf p, p0, p1, V[4];
    int i, j, step;
    int style[20];
    int pen, width;
    gdImagePtr brush = NULL;
    gdPoint F[4];

    if (!im)
	return;

    if (cstk[SP].pen != P_NONE) {
	if (cstk[SP].pen == P_DASHED) {
	    for (i = 0; i < 10; i++)
		style[i] = cstk[SP].pencolor;
	    for (; i < 20; i++)
		style[i] = transparent;
	    gdImageSetStyle(im, style, 20);
	    pen = gdStyled;
	} else if (cstk[SP].pen == P_DOTTED) {
	    for (i = 0; i < 2; i++)
		style[i] = cstk[SP].pencolor;
	    for (; i < 12; i++)
		style[i] = transparent;
	    gdImageSetStyle(im, style, 12);
	    pen = gdStyled;
	} else {
	    pen = cstk[SP].pencolor;
	}
#if 0
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    width = cstk[SP].penwidth;
	    brush = gdImageCreate(width, width);
	    gdImagePaletteCopy(brush, im);
	    gdImageFilledRectangle(brush,
				   0, 0, width - 1, width - 1,
				   cstk[SP].pencolor);
	    gdImageSetBrush(im, brush);
	    if (pen == gdStyled)
		pen = gdStyledBrushed;
	    else
		pen = gdBrushed;
	}
#else
	width = cstk[SP].penwidth;
	gdImageSetThickness(im, width);
#endif
	p.x = A[0].x;
	p.y = A[0].y;
	p = gdpt(p);
	F[0].x = ROUND(p.x);
	F[0].y = ROUND(p.y);
	p.x = A[n-1].x;
	p.y = A[n-1].y;
	p = gdpt(p);
	F[3].x = ROUND(p.x);
	F[3].y = ROUND(p.y);
	V[3].x = A[0].x;
	V[3].y = A[0].y;
	for (i = 0; i + 3 < n; i += 3) {
	    V[0] = V[3];
	    for (j = 1; j <= 3; j++) {
		V[j].x = A[i + j].x;
		V[j].y = A[i + j].y;
	    }
	    p0 = gdpt(V[0]);
	    for (step = 1; step <= BEZIERSUBDIVISION; step++) {
		p1 = gdpt(Bezier
			  (V, 3, (double) step / BEZIERSUBDIVISION, NULL,
			   NULL));
		gdImageLine(im, ROUND(p0.x), ROUND(p0.y), ROUND(p1.x),
			    ROUND(p1.y), pen);
		if (filled) {
    		    F[1].x = ROUND(p0.x);
    		    F[1].y = ROUND(p0.y);
    		    F[2].x = ROUND(p1.x);
    		    F[2].y = ROUND(p1.y);
		    gdImageFilledPolygon(im, F, 4, cstk[SP].fillcolor);
		}
		p0 = p1;
	    }
	}
	if (brush)
	    gdImageDestroy(brush);
    }
}

static void gd_polygon(point * A, int n, int filled)
{
    pointf p;
    int i;
    gdPoint *points;
    int style[20];
    int pen, width;
    gdImagePtr brush = NULL;

    if (!im)
	return;

    if (cstk[SP].pen != P_NONE) {
	if (cstk[SP].pen == P_DASHED) {
	    for (i = 0; i < 10; i++)
		style[i] = cstk[SP].pencolor;
	    for (; i < 20; i++)
		style[i] = transparent;
	    gdImageSetStyle(im, style, 20);
	    pen = gdStyled;
	} else if (cstk[SP].pen == P_DOTTED) {
	    for (i = 0; i < 2; i++)
		style[i] = cstk[SP].pencolor;
	    for (; i < 12; i++)
		style[i] = transparent;
	    gdImageSetStyle(im, style, 12);
	    pen = gdStyled;
	} else {
	    pen = cstk[SP].pencolor;
	}
#if 1
	/* use brush instead of Thickness to improve end butts */
	gdImageSetThickness(im, WIDTH_NORMAL);
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    width = cstk[SP].penwidth * CompScale;
	    brush = gdImageCreate(width, width);
	    gdImagePaletteCopy(brush, im);
	    gdImageFilledRectangle(brush,
				   0, 0, width - 1, width - 1,
				   cstk[SP].pencolor);
	    gdImageSetBrush(im, brush);
	    if (pen == gdStyled)
		pen = gdStyledBrushed;
	    else
		pen = gdBrushed;
	}
#else
	width = cstk[SP].penwidth;
	gdImageSetThickness(im, width);
#endif
	points = N_GNEW(n, gdPoint);
	for (i = 0; i < n; i++) {
	    p.x = A[i].x;
	    p.y = A[i].y;
	    p = gdpt(p);
	    points[i].x = ROUND(p.x);
	    points[i].y = ROUND(p.y);
	}
	if (filled) {
	    gdImageFilledPolygon(im, points, n, cstk[SP].fillcolor);
	}

	gdImagePolygon(im, points, n, pen);
	free(points);
	if (brush)
	    gdImageDestroy(brush);
    }
}

static void gd_ellipse(point p, int rx, int ry, int filled)
{
    pointf mp;
    int i;
    int style[40];		/* need 2* size for arcs, I don't know why */
    int pen, width;
    gdImagePtr brush = NULL;

    if (!im)
	return;

    if (cstk[SP].pen != P_NONE) {
	if (cstk[SP].pen == P_DASHED) {
	    for (i = 0; i < 20; i++)
		style[i] = cstk[SP].pencolor;
	    for (; i < 40; i++)
		style[i] = transparent;
	    gdImageSetStyle(im, style, 40);
	    pen = gdStyled;
	} else if (cstk[SP].pen == P_DOTTED) {
	    for (i = 0; i < 2; i++)
		style[i] = cstk[SP].pencolor;
	    for (; i < 24; i++)
		style[i] = transparent;
	    gdImageSetStyle(im, style, 24);
	    pen = gdStyled;
	} else {
	    pen = cstk[SP].pencolor;
	}
#if 1
	/* use brush instead of Thickness to improve outline appearance */
	gdImageSetThickness(im, WIDTH_NORMAL);
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    width = cstk[SP].penwidth;
	    brush = gdImageCreate(width, width);
	    gdImagePaletteCopy(brush, im);
	    gdImageFilledRectangle(brush,
				   0, 0, width - 1, width - 1,
				   cstk[SP].pencolor);
	    gdImageSetBrush(im, brush);
	    if (pen == gdStyled)
		pen = gdStyledBrushed;
	    else
		pen = gdBrushed;
	}
#else
	width = cstk[SP].penwidth;
	gdImageSetThickness(im, width);
#endif
	if (Rot) {
	    int t;
	    t = rx;
	    rx = ry;
	    ry = t;
	}
	mp.x = p.x;
	mp.y = p.y;
	mp = gdpt(mp);
	if (filled) {
	    gdImageFilledEllipse(im, ROUND(mp.x), ROUND(mp.y),
				 ROUND(CompScale * (rx + rx)),
				 ROUND(CompScale * (ry + ry)),
				 cstk[SP].fillcolor);
	}
	gdImageArc(im, ROUND(mp.x), ROUND(mp.y),
		   ROUND(CompScale * (rx + rx)),
		   ROUND(CompScale * (ry + ry)), 0, 360, pen);
	if (brush)
	    gdImageDestroy(brush);
    }
}

static void gd_polyline(point * A, int n)
{
    pointf p, p1;
    int i;
    int style[20];
    int pen, width;
    gdImagePtr brush = NULL;

    if (!im)
	return;

    if (cstk[SP].pen != P_NONE) {
	if (cstk[SP].pen == P_DASHED) {
	    for (i = 0; i < 10; i++)
		style[i] = cstk[SP].pencolor;
	    for (; i < 20; i++)
		style[i] = transparent;
	    gdImageSetStyle(im, style, 20);
	    pen = gdStyled;
	} else if (cstk[SP].pen == P_DOTTED) {
	    for (i = 0; i < 2; i++)
		style[i] = cstk[SP].pencolor;
	    for (; i < 12; i++)
		style[i] = transparent;
	    gdImageSetStyle(im, style, 12);
	    pen = gdStyled;
	} else {
	    pen = cstk[SP].pencolor;
	}
#if 0
	if (cstk[SP].penwidth != WIDTH_NORMAL) {
	    width = cstk[SP].penwidth;
	    brush = gdImageCreate(width, width);
	    gdImagePaletteCopy(brush, im);
	    gdImageFilledRectangle(brush,
				   0, 0, width - 1, width - 1,
				   cstk[SP].pencolor);
	    gdImageSetBrush(im, brush);
	    if (pen == gdStyled)
		pen = gdStyledBrushed;
	    else
		pen = gdBrushed;
	}
#else
	width = cstk[SP].penwidth;
	gdImageSetThickness(im, width);
#endif
	p.x = A[0].x;
	p.y = A[0].y;
	p = gdpt(p);
	for (i = 1; i < n; i++) {
	    p1.x = A[i].x;
	    p1.y = A[i].y;
	    p1 = gdpt(p1);
	    gdImageLine(im, ROUND(p.x), ROUND(p.y),
			ROUND(p1.x), ROUND(p1.y), pen);
	    p.x = p1.x;
	    p.y = p1.y;
	}
	if (brush)
	    gdImageDestroy(brush);
    }
}

static void gd_user_shape(char *name, point * A, int n, int filled)
{
    gdImagePtr im2 = 0;
    pointf destul, destlr;
    pointf ul, lr;		/* upper left, lower right */
    double sx, sy;		/* target size */
    double scalex, scaley;	/* scale factors */
    int i;
    char *shapeimagefile;

    if (streq(name, "custom"))
	shapeimagefile = agget(Curnode, "shapefile");
    else
	shapeimagefile = name;
    im2 = gd_getshapeimage(shapeimagefile);
    if (im2) {
	pointf delta;
	/* compute dest origin and size */
	ul.x = lr.x = A[0].x;
	ul.y = lr.y = A[0].y;
	for (i = 1; i < n; i++) {
	    if (ul.x > A[i].x)
		ul.x = A[i].x;
	    if (ul.y < A[i].y)
		ul.y = A[i].y;
	    if (lr.y > A[i].y)
		lr.y = A[i].y;
	    if (lr.x < A[i].x)
		lr.x = A[i].x;
	}
	destul = gdpt(ul);
	destlr = gdpt(lr);
	delta.x = destlr.x - destul.x;
	delta.y = destlr.y - destul.y;
	scalex = delta.x / (double) (im2->sx);
	scaley = delta.y / (double) (im2->sy);
	/* keep aspect ratio fixed by just using the smaller scale */
	if (scalex < scaley) {
	    sx = im2->sx * scalex;
	    sy = im2->sy * scalex;
	} else {
	    sx = im2->sx * scaley;
	    sy = im2->sy * scaley;
	}
	if (sx < delta.x)
	    destul.x += (delta.x - sx) / 2.0;
	if (sy < delta.y)
	    destul.y += (delta.y - sy) / 2.0;
	sx = ROUND(sx);
	sy = ROUND(sy);
	gdImageCopyResized(im, im2, ROUND(destul.x), ROUND(destul.y), 0, 0,
			   sx, sy, im2->sx, im2->sy);
    }
}

codegen_t GD_CodeGen = {
    0,				/* gd_reset */
    gd_begin_job, gd_end_job,
    gd_begin_graph_to_file, gd_end_graph_to_file,
    gd_begin_page, gd_end_page,
    0, /* gd_begin_layer */ 0,	/* gd_end_layer */
    0, /* gd_begin_cluster */ 0,	/* gd_end_cluster */
    0, /* gd_begin_nodes */ 0,	/* gd_end_nodes */
    0, /* gd_begin_edges */ 0,	/* gd_end_edges */
    gd_begin_node, gd_end_node,
    0, /* gd_begin_edge */ 0,	/* gd_end_edge */
    gd_begin_context, gd_end_context,
    0, /* gd_begin_anchor */ 0,	/* gd_end_anchor */
    gd_set_font, gd_textline,
    gd_set_pencolor, gd_set_fillcolor, gd_set_style,
    gd_ellipse, gd_polygon,
    gd_bezier, gd_polyline,
    0,				/* bezier_has_arrows */
    0,				/* gd_comment */
    0,				/* gd_textsize */
    gd_user_shape,
    0				/* gd_user_shape_size */
};

codegen_t memGD_CodeGen = {	/* see tcldot */
    0,				/* gd_reset */
    gd_begin_job, gd_end_job,
    gd_begin_graph_to_memory, gd_end_graph_to_memory,
    gd_begin_page, gd_end_page,
    0, /* gd_begin_layer */ 0,	/* gd_end_layer */
    0, /* gd_begin_cluster */ 0,	/* gd_end_cluster */
    0, /* gd_begin_nodes */ 0,	/* gd_end_nodes */
    0, /* gd_begin_edges */ 0,	/* gd_end_edges */
    0, /* gd_begin_node */ 0,	/* gd_end_node */
    0, /* gd_begin_edge */ 0,	/* gd_end_edge */
    gd_begin_context, gd_end_context,
    0, /* gd_begin_anchor */ 0,	/* gd_end_anchor */
    gd_set_font, gd_textline,
    gd_set_pencolor, gd_set_fillcolor, gd_set_style,
    gd_ellipse, gd_polygon,
    gd_bezier, gd_polyline,
    0,				/* bezier_has_arrows */
    0,				/* gd_comment */
    0,				/* gd_textsize */
    gd_user_shape,
    0				/* gd_user_shape_size */
};
