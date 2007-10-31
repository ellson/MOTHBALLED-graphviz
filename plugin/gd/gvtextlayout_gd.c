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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gvplugin_textlayout.h"

typedef enum { AGWARN, AGERR, AGMAX, AGPREV } agerrlevel_t;
extern int agerr(agerrlevel_t level, char *fmt, ...);

#ifdef HAVE_LIBGD
#include "gd.h"

#if defined(HAVE_LIBGD) && defined(HAVE_GD_FREETYPE)

/* fontsize at which text is omitted entirely */
#define FONTSIZE_MUCH_TOO_SMALL 0.15
/* fontsize at which text is rendered by a simple line */
#define FONTSIZE_TOO_SMALL 1.5

#ifndef HAVE_GD_FONTCONFIG
/* gd_alternate_fontlist;
 * Sometimes fonts are stored under a different name,
 * especially on Windows. Without fontconfig, we provide
 * here some rudimentary name mapping.
 */
char *gd_alternate_fontlist(char *font)
{
    static char *fontbuf;
    static int fontbufsz;
    char *p, *fontlist;
    int len;

    len = strlen(font) + 1;
    if (len > fontbufsz) {
	fontbufsz = 2 * len;
	if (fontbuf)
	    fontbuf = malloc(fontbufsz);
	else
	    fontbuf = realloc(fontbuf, fontbufsz);
    }

    /* fontbuf to contain font without style descriptions like -Roman or -Italic */
    strcpy(fontbuf, font);
    if ((p = strchr(fontbuf, '-')) || (p = strchr(fontbuf, '_')))
	*p = 0;

    fontlist = fontbuf;
    if ((strcasecmp(font, "times-bold") == 0)
	|| (strcasecmp(fontbuf, "timesbd") == 0)
	|| (strcasecmp(fontbuf, "timesb") == 0))
	fontlist = "timesbd;Timesbd;TIMESBD;timesb;Timesb;TIMESB";

    else if ((strcasecmp(font, "times-italic") == 0)
	     || (strcasecmp(fontbuf, "timesi") == 0))
	fontlist = "timesi;Timesi;TIMESI";

    else if ((strcasecmp(font, "timesnewroman") == 0)
	     || (strcasecmp(font, "timesnew") == 0)
	     || (strcasecmp(font, "timesroman") == 0)
	     || (strcasecmp(fontbuf, "times") == 0))
	fontlist = "times;Times;TIMES";

    else if ((strcasecmp(font, "arial-bold") == 0)
	     || (strcasecmp(fontbuf, "arialb") == 0))
	fontlist = "arialb;Arialb;ARIALB";

    else if ((strcasecmp(font, "arial-italic") == 0)
	     || (strcasecmp(fontbuf, "ariali") == 0))
	fontlist = "ariali;Ariali;ARIALI";

    else if (strcasecmp(fontbuf, "helvetica") == 0)
	fontlist = "helvetica;Helvetica;HELVETICA;arial;Arial;ARIAL";

    else if (strcasecmp(fontbuf, "arial") == 0)
	fontlist = "arial;Arial;ARIAL";

    else if (strcasecmp(fontbuf, "courier") == 0)
	fontlist = "courier;Courier;COURIER;cour";

    return fontlist;
}
#endif				/* HAVE_GD_FONTCONFIG */

static boolean gd_textlayout(textpara_t * para, char **fontpath)
{
    char *err;
    char *fontlist;
    int brect[8];
    gdFTStringExtra strex;
    double fontsize;

    strex.flags = gdFTEX_XSHOW
	| gdFTEX_RETURNFONTPATHNAME | gdFTEX_RESOLUTION;
    strex.xshow = NULL;
    strex.hdpi = strex.vdpi = POINTS_PER_INCH;

    if (strstr(para->fontname, "/"))
	strex.flags |= gdFTEX_FONTPATHNAME;
    else
	strex.flags |= gdFTEX_FONTCONFIG;

    para->width = 0.0;
    para->height = 0.0;
    para->yoffset_layout = 0.0;
    para->xshow = NULL;

    para->layout = NULL;
    para->free_layout = NULL;

    fontsize = para->fontsize;
    para->yoffset_centerline = 0.1 * fontsize;

    if (para->fontname) {
	if (fontsize <= FONTSIZE_MUCH_TOO_SMALL) {
	    return TRUE; /* OK, but ignore text entirely */
	} else if (fontsize <= FONTSIZE_TOO_SMALL) {
	    /* draw line in place of text */
	    /* fake a finite fontsize so that line length is calculated */
	    fontsize = FONTSIZE_TOO_SMALL;
	}
	/* call gdImageStringFT with null *im to get brect and to set font cache */
#ifdef HAVE_GD_FONTCONFIG
	fontlist = para->fontname;
#else
	fontlist = gd_alternate_fontlist(para->fontname);
#endif

	err = gdImageStringFTEx(NULL, brect, -1, fontlist,
				fontsize, 0, 0, 0, para->str, &strex);

	if (err) {
	    agerr(AGERR,"%s\n", err);
	    return FALSE; /* indicate error */
	}

	if (strex.xshow) {
	    /* transfer malloc'ed xshow string to para */
	    para->xshow = strex.xshow;
	    strex.xshow = NULL;
	}

	if (fontpath)
	    *fontpath = strex.fontpath;

	if (para->str && para->str[0]) {
	    /* can't use brect on some archtectures if strlen 0 */
	    para->width = (double) (brect[4] - brect[0]);
	    /* 1.2 specifies how much extra space to leave between lines;
             * see LINESPACING in const.h.
             */
	    para->height = (int)(para->fontsize * 1.2);
	}
    }
    return TRUE;
}

static gvtextlayout_engine_t gd_textlayout_engine = {
    gd_textlayout,
};
#endif
#endif

gvplugin_installed_t gvtextlayout_gd_types[] = {
#if defined(HAVE_LIBGD) && defined(HAVE_GD_FREETYPE)
    {0, "textlayout", 2, &gd_textlayout_engine, NULL},
#endif
    {0, NULL, 0, NULL, NULL}
};
