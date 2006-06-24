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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>

#include "gvplugin_render.h"


#ifdef HAVE_LIBGD
#include "gd.h"

typedef enum { FORMAT_GD, FORMAT_GD2, FORMAT_GIF, FORMAT_JPEG, FORMAT_PNG,
	FORMAT_WBMP, FORMAT_XBM, } format_type;

extern int mapbool(char *);
extern char *safefile(char *shapefilename);
extern pointf Bezier(pointf * V, int degree, double t, pointf * Left, pointf * Right);

#define BEZIERSUBDIVISION 10

/* fontsize at which text is omitted entirely */
#define FONTSIZE_MUCH_TOO_SMALL 0.15
/* fontsize at which text is rendered by a simple line */
#define FONTSIZE_TOO_SMALL 1.5

static void gdgen_resolve_color(GVJ_t * job, gvcolor_t * color)
{
    gdImagePtr im = (gdImagePtr) job->surface;

    if (!im)
	return;

    /* seems gd alpha is "transparency" rather than the usual "opacity" */
    color->u.index = gdImageColorResolveAlpha(im,
			  color->u.rgba[0],
			  color->u.rgba[1],
			  color->u.rgba[2],
			  (255 - color->u.rgba[3]) * gdAlphaMax / 255);
    color->type = COLOR_INDEX;
}

static int white, black, transparent, basecolor;

static void gdgen_begin_page(GVJ_t * job)
{
    char *bgcolor_str = NULL, *truecolor_str = NULL;
    bool truecolor_p = FALSE;	/* try to use cheaper paletted mode */
    bool bg_transparent_p = FALSE;
    int bgcolor = 0;
    gdImagePtr im = NULL;


/* FIXME - ... */
//    truecolor_str = agget(job->gvc->g, "truecolor");	/* allow user to force truecolor */
//    bgcolor_str = agget(job->gvc->g, "bgcolor");

    if (truecolor_str && truecolor_str[0])
	truecolor_p = mapbool(truecolor_str);

    if (bgcolor_str && strcmp(bgcolor_str, "transparent") == 0) {
	bg_transparent_p = TRUE;
	if (job->render.features->flags & GVRENDER_DOES_TRUECOLOR)
	    truecolor_p = TRUE;	/* force truecolor */
    }

//    if (GD_has_images(job->gvc->g))
//	truecolor_p = TRUE;	/* force truecolor */

    if (job->external_surface) {
	if (job->common->verbose)
	    fprintf(stderr, "%s: using existing GD image\n", job->common->cmdname);
	im = (gdImagePtr) (job->output_file);
    } else {
	if (truecolor_p) {
	    if (job->common->verbose)
		fprintf(stderr,
			"%s: allocating a %dK TrueColor GD image\n",
			job->common->cmdname,
			ROUND(job->width * job->height * 4 / 1024.));
	    im = gdImageCreateTrueColor(job->width, job->height);
	} else {
	    if (job->common->verbose)
		fprintf(stderr,
			"%s: allocating a %dK PaletteColor GD image\n",
			job->common->cmdname, ROUND(job->width * job->height / 1024.));
	    im = gdImageCreate(job->width, job->height);
	}
    }
    job->surface = (void *) im;

    if (!im) {
#if 0
/* FIXME - error function */
	agerr(AGERR, "gdImageCreate returned NULL. Malloc problem?\n");
#endif
	return;
    }

    white = gdImageColorResolveAlpha(im,
				     gdRedMax, gdGreenMax, gdBlueMax,
				     gdAlphaOpaque);
    black = gdImageColorResolveAlpha(im, 0, 0, 0, gdAlphaOpaque);
    transparent = gdImageColorResolveAlpha(im,
					   gdRedMax - 1, gdGreenMax,
					   gdBlueMax, gdAlphaTransparent);
    gdImageColorTransparent(im, transparent);

    if (bgcolor_str && bgcolor_str[0])
	if (bg_transparent_p) {
	    bgcolor = transparent;
	} else {
#if 0
/* FIXME - colorxlation only available to gvrender.c ... */
	    bgcolor = gd_resolve_color(bgcolor_str);
#endif
    } else {
	bgcolor = white;
    }

#if 0
/* FIXME - ??? */
    cstk[0].fillcolor = bgcolor;
#endif

    /* Blending must be off to lay a transparent basecolor.
       Nothing to blend with anyway. */
    gdImageAlphaBlending(im, FALSE);
    gdImageFill(im, im->sx / 2, im->sy / 2, bgcolor);
    /* Blend everything else together,
       especially fonts over non-transparent backgrounds */
    gdImageAlphaBlending(im, TRUE);
}

static void gdgen_end_page(GVJ_t * job)
{
    gdImagePtr im = (gdImagePtr) job->surface;

    if (!im)
	return;
    if (job->external_surface) {
	/* leave image in memory to be handled by Gdtclft output routines */
#ifdef MYTRACE
	fprintf(stderr, "gdgen_end_graph (to memory)\n");
#endif
    } else {
#ifdef HAVE_SETMODE
#ifdef O_BINARY
	/*
	 * Windows will do \n -> \r\n  translations on stdout
	 * unless told otherwise.  */
	setmode(fileno(job->output_file), O_BINARY);
#endif
#endif

	/* Only save the alpha channel in outputs that support it if
	   the base color was transparent.   Otherwise everything
	   was blended so there is no useful alpha info */
	gdImageSaveAlpha(im, (basecolor == transparent));
	switch (job->render.id) {
#if 0
	case FORMAT_GD:
	    gdImageGd(im, job->output_file);
	    break;
	case FORMAT_GD2:
#define GD2_CHUNKSIZE 128
#define GD2_RAW 1
#define GD2_COMPRESSED 2
	    gdImageGd2(im, job->output_file, GD2_CHUNKSIZE,
		       GD2_COMPRESSED);
	    break;
#endif
	case FORMAT_GIF:
#ifdef HAVE_GD_GIF
	    gdImageTrueColorToPalette(im, 0, 256);
	    gdImageGif(im, job->output_file);
#endif
	    break;
	case FORMAT_JPEG:
#ifdef HAVE_GD_JPEG
	    /*
	     * Write IM to OUTFILE as a JFIF-formatted JPEG image, using
	     * quality JPEG_QUALITY.  If JPEG_QUALITY is in the range
	     * 0-100, increasing values represent higher quality but also
	     * larger image size.  If JPEG_QUALITY is negative, the
	     * IJG JPEG library's default quality is used (which should
	     * be near optimal for many applications).  See the IJG JPEG
	     * library documentation for more details.  */
#define JPEG_QUALITY -1
	    gdImageJpeg(im, job->output_file, JPEG_QUALITY);
#endif
	    break;
	case FORMAT_PNG:
#ifdef HAVE_GD_PNG
	    gdImagePng(im, job->output_file);
#endif
	    break;
	case FORMAT_WBMP:
	    /* Use black for the foreground color for the B&W wbmp image. */
	    gdImageWBMP(im, black, job->output_file);
	    break;
	case FORMAT_XBM:
#if 0
#ifdef HAVE_GD_XPM
	    gdImageXbm(im, job->output_file);
#endif
#endif
	    break;
	}
	gdImageDestroy(im);
#ifdef MYTRACE
	fprintf(stderr, "gdgen_end_graph (to file)\n");
#endif
    }
}

void gdgen_missingfont(char *err, char *fontreq)
{
    static char *lastmissing = 0;
    static int n_errors = 0;

    if (n_errors >= 20)
	return;
    if ((lastmissing == 0) || (strcmp(lastmissing, fontreq))) {
#if defined(HAVE_LIBFONTCONFIG) && defined(HAVE_GD_FONTCONFIG)
#if 0
/* FIXME - error function */
	agerr(AGERR, "%s : %s\n", err, fontreq);
#endif
#else
	char *p = getenv("GDFONTPATH");
	if (!p)
	    p = DEFAULT_FONTPATH;
#if 0
/* FIXME - error function */
	agerr(AGERR, "%s : %s in %s\n", err, fontreq, p);
#endif
#endif
	if (lastmissing)
	    free(lastmissing);
	lastmissing = strdup(fontreq);
	n_errors++;
#if 0
/* FIXME - error function */
	if (n_errors >= 20)
	    agerr(AGWARN, "(font errors suppressed)\n");
#endif
    }
}

extern gdFontPtr gdFontTiny, gdFontSmall, gdFontMediumBold, gdFontLarge, gdFontGiant;

static void gdgen_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    gdImagePtr im = (gdImagePtr) job->surface;
    pointf mp, ep;
    double parawidth = para->width * job->scale.x;
    gdFTStringExtra strex;
#if defined(HAVE_LIBFREETYPE) && defined(HAVE_GD_FREETYPE)
    char *err;
    int brect[8];
#endif

    if (!im)
	return;

    strex.flags = gdFTEX_RESOLUTION;
    strex.hdpi = strex.vdpi = POINTS_PER_INCH * job->scale.x;

    if (strstr(para->fontname, "/"))
	strex.flags |= gdFTEX_FONTPATHNAME;
    else
	strex.flags |= gdFTEX_FONTCONFIG;

    switch (para->just) {
    case 'l':
	mp.x = 0.0;
	break;
    case 'r':
	mp.x = -parawidth;
	break;
    default:
    case 'n':
	mp.x = -parawidth / 2;
	break;
    }
    ep.x = mp.x + parawidth;

    if (job->rotation) {
	mp.y = -mp.x + p.y;
	ep.y = ep.x + p.y;
	ep.x = mp.x = p.x;
    }
    else {
	mp.x += p.x;
	ep.x += p.x;
	ep.y = mp.y = p.y;
    }

    if (para->fontsize <= FONTSIZE_MUCH_TOO_SMALL) {
	/* ignore entirely */
    } else if (para->fontsize <= FONTSIZE_TOO_SMALL) {
	/* draw para in place of text */
	gdImageLine(im, ROUND(mp.x), ROUND(mp.y),
		    ROUND(ep.x), ROUND(ep.y),
		    job->style->pencolor.u.index);
    } else {
#if defined(HAVE_LIBFREETYPE) && defined(HAVE_GD_FREETYPE)
	err = gdImageStringFTEx(im, brect, job->style->pencolor.u.index,
				para->fontname, para->fontsize, job->rotation ? (PI / 2) : 0,
				ROUND(mp.x), ROUND(mp.y), (char *)(para->str), &strex);
#if 0
	gdImagePolygon(im, (gdPointPtr) brect, 4,
		       job->style->pencolor.u.index);
#endif
#if 0
	fprintf(stderr,
		"textpara: font=%s size=%g pos=%g,%g width=%g dpi=%d width/dpi=%g\n",
		para->fontname, para->fontsize, mp.x, mp.y, (double) (brect[4] - brect[0]),
		strex.hdpi,
		(((double) (brect[4] - brect[0])) / strex.hdpi));
#endif
	if (err) {
	    /* revert to builtin fonts */
	    gdgen_missingfont(err, para->fontname);
#endif
	    mp.y += 2;
	    if (para->fontsize <= 8.5) {
		gdImageString(im, gdFontTiny,
			      ROUND(mp.x), ROUND(mp.y - 9.),
			      (unsigned char *)para->str,
			      job->style->pencolor.u.index);
	    } else if (para->fontsize <= 9.5) {
		gdImageString(im, gdFontSmall,
			      ROUND(mp.x), ROUND(mp.y - 12.),
			      (unsigned char *)para->str,
			      job->style->pencolor.u.index);
	    } else if (para->fontsize <= 10.5) {
		gdImageString(im, gdFontMediumBold,
			      ROUND(mp.x), ROUND(mp.y - 13.),
			      (unsigned char *)para->str,
			      job->style->pencolor.u.index);
	    } else if (para->fontsize <= 11.5) {
		gdImageString(im, gdFontLarge,
			      ROUND(mp.x), ROUND(mp.y - 14.),
			      (unsigned char *)para->str,
			      job->style->pencolor.u.index);
	    } else {
		gdImageString(im, gdFontGiant,
			      ROUND(mp.x), ROUND(mp.y - 15.),
			      (unsigned char *)para->str,
			      job->style->pencolor.u.index);
	    }
#if defined(HAVE_LIBFREETYPE) && defined(HAVE_GD_FREETYPE)
	}
#endif
    }
}

static int gdgen_set_penstyle(GVJ_t * job, gdImagePtr im, gdImagePtr brush)
{
    gvstyle_t *style = job->style;
    int i, pen, width, dashstyle[40];

    if (style->pen == PEN_DASHED) {
	for (i = 0; i < 20; i++)
	    dashstyle[i] = style->pencolor.u.index;
	for (; i < 40; i++)
	    dashstyle[i] = transparent;
	gdImageSetStyle(im, dashstyle, 20);
	pen = gdStyled;
    } else if (style->pen == PEN_DOTTED) {
	for (i = 0; i < 2; i++)
	    dashstyle[i] = style->pencolor.u.index;
	for (; i < 24; i++)
	    dashstyle[i] = transparent;
	gdImageSetStyle(im, dashstyle, 24);
	pen = gdStyled;
    } else {
	pen = style->pencolor.u.index;
    }

    width = style->penwidth * job->scale.x;
    if (width < PENWIDTH_NORMAL)
	width = PENWIDTH_NORMAL;  /* gd can't do thin lines */
    gdImageSetThickness(im, width);
    /* use brush instead of Thickness to improve end butts */
    if (width != PENWIDTH_NORMAL) {
	brush = gdImageCreate(width, width);
	gdImagePaletteCopy(brush, im);
	gdImageFilledRectangle(brush, 0, 0, width - 1, width - 1,
			       style->pencolor.u.index);
	gdImageSetBrush(im, brush);
	if (pen == gdStyled)
	    pen = gdStyledBrushed;
	else
	    pen = gdBrushed;
    }

    return pen;
}

static void
gdgen_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
	     int arrow_at_end, int filled)
{
    gvstyle_t *style = job->style;
    gdImagePtr im = (gdImagePtr) job->surface;
    pointf p0, p1, V[4];
    int i, j, step, pen;
    gdImagePtr brush = NULL;
    gdPoint F[4];

    if (!im)
	return;

    pen = gdgen_set_penstyle(job, im, brush);

    V[3] = A[0];
    PF2P(A[0], F[0]);
    PF2P(A[n-1], F[3]);
    for (i = 0; i + 3 < n; i += 3) {
	V[0] = V[3];
	for (j = 1; j <= 3; j++)
	    V[j] = A[i + j];
	p0 = V[0];
	for (step = 1; step <= BEZIERSUBDIVISION; step++) {
	    p1 = Bezier(V, 3, (double) step / BEZIERSUBDIVISION, NULL, NULL);
	    PF2P(p0, F[1]);
	    PF2P(p1, F[2]);
	    gdImageLine(im, F[1].x, F[1].y, F[2].x, F[2].y, pen);
	    if (filled)
		gdImageFilledPolygon(im, F, 4, style->fillcolor.u.index);
	    p0 = p1;
	}
    }
    if (brush)
	gdImageDestroy(brush);
}

static void gdgen_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    gvstyle_t *style = job->style;
    gdImagePtr im = (gdImagePtr) job->surface;
    gdImagePtr brush = NULL;
    int i;
    gdPoint *points;
    int pen;

    if (!im)
	return;

    pen = gdgen_set_penstyle(job, im, brush);

    points = malloc(n * sizeof(gdPoint));
    for (i = 0; i < n; i++) {
	points[i].x = ROUND(A[i].x);
	points[i].y = ROUND(A[i].y);
    }
    if (filled)
	gdImageFilledPolygon(im, points, n, style->fillcolor.u.index);

    gdImagePolygon(im, points, n, pen);
    free(points);
    if (brush)
	gdImageDestroy(brush);
}

static void gdgen_ellipse(GVJ_t * job, pointf * A, int filled)
{
    gvstyle_t *style = job->style;
    gdImagePtr im = (gdImagePtr) job->surface;
    double dx, dy;
    int pen;
    gdImagePtr brush = NULL;

    if (!im)
	return;

    pen = gdgen_set_penstyle(job, im, brush);

    dx = 2 * (A[1].x - A[0].x);
    dy = 2 * (A[1].y - A[0].y);

    if (filled)
	gdImageFilledEllipse(im, ROUND(A[0].x), ROUND(A[0].y),
			     ROUND(dx), ROUND(dy),
			     style->fillcolor.u.index);
    gdImageArc(im, ROUND(A[0].x), ROUND(A[0].y), ROUND(dx), ROUND(dy),
	       0, 360, pen);
    if (brush)
	gdImageDestroy(brush);
}

static void gdgen_polyline(GVJ_t * job, pointf * A, int n)
{
    gvstyle_t *style = job->style;
    gdImagePtr im = (gdImagePtr) job->surface;
    pointf p, p1;
    int i;
    int dashstyle[20];
    int pen, width;
    gdImagePtr brush = NULL;

    if (!im)
	return;

    if (style->pen == PEN_DASHED) {
	for (i = 0; i < 10; i++)
	    dashstyle[i] = style->pencolor.u.index;
	for (; i < 20; i++)
	    dashstyle[i] = transparent;
	gdImageSetStyle(im, dashstyle, 20);
	pen = gdStyled;
    } else if (style->pen == PEN_DOTTED) {
	for (i = 0; i < 2; i++)
	    dashstyle[i] = style->pencolor.u.index;
	for (; i < 12; i++)
	    dashstyle[i] = transparent;
	gdImageSetStyle(im, dashstyle, 12);
	pen = gdStyled;
    } else {
	pen = style->pencolor.u.index;
    }
    width = style->penwidth * job->scale.x;
    if (width < PENWIDTH_NORMAL)
	width = PENWIDTH_NORMAL;  /* gd can't do thin lines */
    gdImageSetThickness(im, width);
    if (style->penwidth != PENWIDTH_NORMAL) {
	brush = gdImageCreate(width, width);
	gdImagePaletteCopy(brush, im);
	gdImageFilledRectangle(brush, 0, 0, width - 1, width - 1,
			       style->pencolor.u.index);
	gdImageSetBrush(im, brush);
	if (pen == gdStyled)
	    pen = gdStyledBrushed;
	else
	    pen = gdBrushed;
    }
    p = A[0];
    for (i = 1; i < n; i++) {
	p1 = A[i];
	gdImageLine(im, ROUND(p.x), ROUND(p.y),
		    ROUND(p1.x), ROUND(p1.y), pen);
	p = p1;
    }
    if (brush)
	gdImageDestroy(brush);
}

static gvrender_engine_t gdgen_engine = {
    0,				/* gdgen_begin_job */
    0,				/* gdgen_end_job */
    0,				/* gdgen_begin_graph */
    0,				/* gdgen_end_graph */
    0,				/* gdgen_begin_layer */
    0,				/* gdgen_end_layer */
    gdgen_begin_page,
    gdgen_end_page,
    0,				/* gdgen_begin_cluster */
    0,				/* gdgen_end_cluster */
    0,				/* gdgen_begin_nodes */
    0,				/* gdgen_end_nodes */
    0,				/* gdgen_begin_edges */
    0,				/* gdgen_end_edges */
    0,				/* gdgen_begin_node */
    0,				/* gdgen_end_node */
    0,				/* gdgen_begin_edge */
    0,				/* gdgen_end_edge */
    0,				/* gdgen_begin_anchor */
    0,				/* gdgen_end_anchor */
    gdgen_textpara,
    gdgen_resolve_color,
    gdgen_ellipse,
    gdgen_polygon,
    gdgen_bezier,
    gdgen_polyline,
    0,				/* gdgen_comment */
};

static gvrender_features_t gdgen_features_tc = {
    GVRENDER_DOES_TRUECOLOR
	| GVRENDER_Y_GOES_DOWN,	/* flags */
    0,				/* default margin - points */
    {96.,96.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_BYTE,			/* color_type */
    NULL,			/* device */
    "gd",			/* gvloadimage target for usershapes */
};

static gvrender_features_t gdgen_features = {
    GVRENDER_Y_GOES_DOWN,	/* flags */
    0,				/* default margin - points */
    {96.,96.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    RGBA_BYTE,			/* color_type */
    NULL,			/* device */
    "gd",			/* gvloadimage target for usershapes */
};

#endif

gvplugin_installed_t gvrender_gd_types[] = {
#ifdef HAVE_LIBGD
#if 0
    {FORMAT_GD, "gd", 1, &gdgen_engine, &gdgen_features_tc},
    {FORMAT_GD2, "gd2", 1, &gdgen_engine, &gdgen_features_tc},
#endif
#ifdef HAVE_GD_GIF
    {FORMAT_GIF, "gif", 1, &gdgen_engine, &gdgen_features},
#endif
#ifdef HAVE_GD_JPEG
    {FORMAT_JPEG, "jpg", 1, &gdgen_engine, &gdgen_features_tc},
    {FORMAT_JPEG, "jpeg", 1, &gdgen_engine, &gdgen_features_tc},
#endif
#ifdef HAVE_GD_PNG
    {FORMAT_PNG, "png", 1, &gdgen_engine, &gdgen_features_tc},
#endif
    {FORMAT_WBMP, "wbmp", 1, &gdgen_engine, &gdgen_features},
#ifdef HAVE_GD_XPM
    {FORMAT_XBM, "xbm", 1, &gdgen_engine, &gdgen_features},
#endif
#endif
    {0, NULL, 0, NULL, NULL}
};
