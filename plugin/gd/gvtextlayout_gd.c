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

#ifdef HAVE_LIBGD
#include "gd.h"

#if defined(HAVE_LIBFREETYPE) && defined(HAVE_GD_FREETYPE) && defined(HAVE_LIBFONTCONFIG) && defined(HAVE_GD_FONTCONFIG)

/* fontsize at which text is omitted entirely */
#define FONTSIZE_MUCH_TOO_SMALL 0.15
/* fontsize at which text is rendered by a simple line */
#define FONTSIZE_TOO_SMALL 1.5

static void gd_textlayout(textpara_t * para, char **fontpath)
{
    static char *fntpath;
    char *err;
    int brect[8];
    gdFTStringExtra strex;

    strex.flags = gdFTEX_XSHOW
	| gdFTEX_RETURNFONTPATHNAME | gdFTEX_RESOLUTION;
    strex.xshow = NULL;
    strex.hdpi = strex.vdpi = 72;

    if (strstr(para->fontname, "/"))
	strex.flags |= gdFTEX_FONTPATHNAME;
    else
	strex.flags |= gdFTEX_FONTCONFIG;

    para->width = 0.0;
    para->height = 0.0;
    para->xshow = NULL;

    para->layout = NULL;
    para->free_layout = NULL;

    if (para->fontname) {
	if (para->fontsize <= FONTSIZE_MUCH_TOO_SMALL) {
	    /* OK, but ignore text entirely */
	    return;
	} else if (para->fontsize <= FONTSIZE_TOO_SMALL) {
	    /* draw line in place of text */
	    /* fake a finite fontsize so that line length is calculated */
	    para->fontsize = FONTSIZE_TOO_SMALL;
	}
	/* call gdImageStringFT with null *im to get brect and to set font cache */
	err = gdImageStringFTEx(NULL, brect, -1, para->fontname,
				para->fontsize, 0, 0, 0, para->str, &strex);

	if (err) {
	    fprintf(stderr,"%s\n", err);
	    *fontpath = NULL;   /* indicate error */
	    return;
	}

	if (strex.xshow) {
	    /* transfer malloc'ed xshow string to para */
	    para->xshow = strex.xshow;
	    strex.xshow = NULL;
	}

	if (fntpath)
	    free(fntpath);
	*fontpath = fntpath = strex.fontpath;

	if (para->str && para->str[0]) {
	    /* can't use brect on some archtectures if strlen 0 */
	    para->width = (double) (brect[4] - brect[0]);
	    para->height = (double) (brect[1] - brect[5]);
	}
    }
}

static gvtextlayout_engine_t gd_textlayout_engine = {
    gd_textlayout,
};
#endif
#endif

gvplugin_installed_t gvtextlayout_gd_types[] = {
#if defined(HAVE_LIBGD) && defined(HAVE_LIBFREETYPE) && defined(HAVE_GD_FREETYPE) && defined(HAVE_LIBFONTCONFIG) && defined(HAVE_GD_FONTCONFIG)
    {0, "textlayout", 2, &gd_textlayout_engine, NULL},
#endif
    {0, NULL, 0, NULL, NULL}
};
