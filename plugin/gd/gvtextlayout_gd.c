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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gvplugin_textlayout.h"
#include "gd.h"

#ifdef HAVE_GD_FREETYPE

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
	if (fontbuf == NULL)
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

/* gd_psfontResolve:
 *  * Construct alias for postscript fontname.
 *   * NB. Uses a static array - non-reentrant.
 *    */

#define ADD_ATTR(a) \
  if (a) { \
        strcat(buf, comma ? " " : ", "); \
        comma = 1; \
        strcat(buf, a); \
  }

char* gd_psfontResolve (PostscriptAlias* pa)
{
    static char buf[1024];
    int comma=0;
    strcpy(buf, pa->family);

    ADD_ATTR(pa->weight);
    ADD_ATTR(pa->stretch);
    ADD_ATTR(pa->style);
   
    return buf;
}

static boolean gd_textlayout(textspan_t * span, char **fontpath)
{
    char *err, *fontlist, *fontname;
    double fontsize;
    int brect[8];
    gdFTStringExtra strex;
#ifdef HAVE_GD_FONTCONFIG
    PostscriptAlias *pA;
#endif

    fontname = span->font->name;
    fontsize = span->font->size;

    strex.fontpath = NULL;
    strex.flags = gdFTEX_RETURNFONTPATHNAME | gdFTEX_RESOLUTION;
    strex.hdpi = strex.vdpi = POINTS_PER_INCH;

    if (strstr(fontname, "/"))
	strex.flags |= gdFTEX_FONTPATHNAME;
    else
	strex.flags |= gdFTEX_FONTCONFIG;

    span->size.x = 0.0;
    span->size.y = 0.0;
    span->yoffset_layout = 0.0;

    span->layout = NULL;
    span->free_layout = NULL;

    span->yoffset_centerline = 0.1 * fontsize;

    if (fontname) {
	if (fontsize <= FONTSIZE_MUCH_TOO_SMALL) {
	    return TRUE; /* OK, but ignore text entirely */
	} else if (fontsize <= FONTSIZE_TOO_SMALL) {
	    /* draw line in place of text */
	    /* fake a finite fontsize so that line length is calculated */
	    fontsize = FONTSIZE_TOO_SMALL;
	}
	/* call gdImageStringFT with null *im to get brect and to set font cache */
#ifdef HAVE_GD_FONTCONFIG
	gdFTUseFontConfig(1);  /* tell gd that we really want to use fontconfig, 'cos it s not the default */
	pA = span->font->postscript_alias;
	if (pA)
	    fontlist = gd_psfontResolve (pA);
	else
	    fontlist = fontname;
#else
	fontlist = gd_alternate_fontlist(fontname);
#endif

	err = gdImageStringFTEx(NULL, brect, -1, fontlist,
				fontsize, 0, 0, 0, span->str, &strex);

	if (err) {
	    agerr(AGERR,"%s\n", err);
	    return FALSE; /* indicate error */
	}

	if (fontpath)
	    *fontpath = strex.fontpath;
	else
	    free (strex.fontpath); /* strup'ed in libgd */

	if (span->str && span->str[0]) {
	    /* can't use brect on some archtectures if strlen 0 */
	    span->size.x = (double) (brect[4] - brect[0]);
	    /* 1.2 specifies how much extra space to leave between lines;
             * see LINESPACING in const.h.
             */
	    span->size.y = (int)(fontsize * 1.2);
	}
    }
    return TRUE;
}

static gvtextlayout_engine_t gd_textlayout_engine = {
    gd_textlayout,
};
#endif

gvplugin_installed_t gvtextlayout_gd_types[] = {
#ifdef HAVE_GD_FREETYPE
    {0, "textlayout", 2, &gd_textlayout_engine, NULL},
#endif
    {0, NULL, 0, NULL, NULL}
};
