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

#include	"render.h"
#include	"gd.h"

/* sometimes fonts are stored under a different name */
char *gd_alternate_fontlist(char *font)
{
#ifdef HAVE_GD_FONTCONFIG
    return font;
#else
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
	fontlist = "arialb;Alialb;ARIALB";

    else if ((strcasecmp(font, "arial-italic") == 0)
	     || (strcasecmp(fontbuf, "ariali") == 0))
	fontlist = "ariali;Aliali;ARIALI";

    else if (strcasecmp(fontbuf, "helvetica") == 0)
	fontlist = "helvetica;Helvetica;HELVETICA;arial;Arial;ARIAL";

    else if (strcasecmp(fontbuf, "arial") == 0)
	fontlist = "arial;Arial;ARIAL";

    else if (strcasecmp(fontbuf, "courier") == 0)
	fontlist = "courier;Courier;COURIER;cour";

    return fontlist;
#endif				/* HAVE_GD_FONTCONFIG */
}

/* return size in points (1/72 in) for the string in font at fontsz in points */
char *gd_textsize(textline_t * textline, char *fontname, double fontsz,
		  char **fontpath)
{
    char *fontlist, *err;
    int brect[8];
    gdFTStringExtra strex;

    strex.flags = gdFTEX_XSHOW
	| gdFTEX_RETURNFONTPATHNAME | gdFTEX_RESOLUTION;
    strex.xshow = NULL;
    strex.hdpi = strex.vdpi = 72;

    if (strstr(fontname, "/"))
	strex.flags |= gdFTEX_FONTPATHNAME;
    else
	strex.flags |= gdFTEX_FONTCONFIG;

    textline->width = 0.0;
    textline->xshow = NULL;

    fontlist = gd_alternate_fontlist(fontname);
    if (fontlist) {
	if (fontsz <= FONTSIZE_MUCH_TOO_SMALL) {
	    /* OK, but ignore text entirely */
	    return NULL;
	} else if (fontsz <= FONTSIZE_TOO_SMALL) {
	    /* draw line in place of text */
	    /* fake a finite fontsize so that line length is calculated */
	    fontsz = FONTSIZE_TOO_SMALL;
	}
#ifdef HAVE_GD_FREETYPE
	/* call gdImageStringFT with null *im to get brect and to set font cache */
	err = gdImageStringFTEx(NULL, brect, -1, fontlist,
				fontsz, 0, 0, 0, textline->str, &strex);

	if (err)
	    return err;

	if (strex.xshow) {
	    /* transfer malloc'ed xshow string to textline */
	    textline->xshow = strex.xshow;
	    strex.xshow = NULL;
	}

	*fontpath = strex.fontpath;

	if (textline->str && textline->str[0]) {
	    /* can't use brect on some archtectures if strlen 0 */
	    textline->width = (double) (brect[4] - brect[0]);
	}
#else
	return "No Freetype support available";
#endif
    }
    return NULL;
}
