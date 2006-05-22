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

#include "gd.h"

/* FIXME - shouldn't need this */
#include "types.h"

#include "gvplugin_textlayout.h"

/* fontsize at which text is omitted entirely */
#define FONTSIZE_MUCH_TOO_SMALL 0.15
/* fontsize at which text is rendered by a simple line */
#define FONTSIZE_TOO_SMALL 1.5

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

void textlayout(textline_t * textline, char *fontname, double fontsize, char **fontpath)
{
    static char *fntpath;
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

    textline->dimen.x = 0.0;
    textline->dimen.y = 0.0;
    textline->xshow = NULL;

    fontlist = gd_alternate_fontlist(fontname);
    textline->layout = (void*)fontlist;
    textline->free_layout = NULL; /* no need to free fontlist (??) */


    if (fontlist) {
	if (fontsize <= FONTSIZE_MUCH_TOO_SMALL) {
	    /* OK, but ignore text entirely */
	    return;
	} else if (fontsize <= FONTSIZE_TOO_SMALL) {
	    /* draw line in place of text */
	    /* fake a finite fontsize so that line length is calculated */
	    fontsize = FONTSIZE_TOO_SMALL;
	}
	/* call gdImageStringFT with null *im to get brect and to set font cache */
	err = gdImageStringFTEx(NULL, brect, -1, fontlist,
				fontsize, 0, 0, 0, textline->str, &strex);

	if (err)
	    return;

	if (strex.xshow) {
	    /* transfer malloc'ed xshow string to textline */
	    textline->xshow = strex.xshow;
	    strex.xshow = NULL;
	}

	if (fntpath)
	    free(fntpath);
	*fontpath = fntpath = strex.fontpath;

	if (textline->str && textline->str[0]) {
	    /* can't use brect on some archtectures if strlen 0 */
	    textline->dimen.x = (double) (brect[4] - brect[0]);
	    textline->dimen.y = (double) (brect[5] - brect[1]);
	}
    }
}

gvtextlayout_engine_t textlayout_engine = {
    textlayout,
};

gvplugin_installed_t gvtextlayout_gd_types[] = {
    {0, "textlayout", 2, &textlayout_engine, NULL},
    {0, NULL, 0, NULL, NULL}
};
