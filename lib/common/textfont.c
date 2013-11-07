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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdt.h"
#include "render.h"

static double timesFontWidth[] = {
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.3329, 0.4079, 0.5000, 0.5000, 0.8329, 0.7779, 0.3329,	/*  !"#$%&' */
    0.3329, 0.3329, 0.5000, 0.5639, 0.2500, 0.3329, 0.2500, 0.2779,	/* ()*+,-./ */
    0.5000, 0.5000, 0.5000, 0.5000, 0.5000, 0.5000, 0.5000, 0.5000,	/* 01234567 */
    0.5000, 0.5000, 0.2779, 0.2779, 0.5639, 0.5639, 0.5639, 0.4439,	/* 89:;<=>? */
    0.9209, 0.7219, 0.6669, 0.6669, 0.7219, 0.6109, 0.5559, 0.7219,	/* @ABCDEFG */
    0.7219, 0.3329, 0.3889, 0.7219, 0.6109, 0.8889, 0.7219, 0.7219,	/* HIJKLMNO */
    0.5559, 0.7219, 0.6669, 0.5559, 0.6109, 0.7219, 0.7219, 0.9439,	/* PQRSTUVW */
    0.7219, 0.7219, 0.6109, 0.3329, 0.2779, 0.3329, 0.4689, 0.5000,	/* XYZ[\]^_ */
    0.3329, 0.4439, 0.5000, 0.4439, 0.5000, 0.4439, 0.3329, 0.5000,	/* `abcdefg */
    0.5000, 0.2779, 0.2779, 0.5000, 0.2779, 0.7779, 0.5000, 0.5000,	/* hijklmno */
    0.5000, 0.5000, 0.3329, 0.3889, 0.2779, 0.5000, 0.5000, 0.7219,	/* pqrstuvw */
    0.5000, 0.5000, 0.4439, 0.4799, 0.1999, 0.4799, 0.5409, 0.2500,	/* xyz{|}~  */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.3329, 0.5000, 0.5000, 0.1669, 0.5000, 0.5000, 0.5000,	/*  ¡¢£¤¥¦§ */
    0.5000, 0.1799, 0.4439, 0.5000, 0.3329, 0.3329, 0.5559, 0.5559,	/* ¨©ª«¬­®¯ */
    0.2500, 0.5000, 0.5000, 0.5000, 0.2500, 0.2500, 0.4529, 0.3499,	/*  ±²³´ ¶· */
    0.3329, 0.4439, 0.4439, 0.5000, 1.0000, 1.0000, 0.2500, 0.4439,	/* ¸¹º»¼½ ¿ */
    0.2500, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329,	/*  ÁÂÃÄÅÆÇ */
    0.3329, 0.2500, 0.3329, 0.3329, 0.2500, 0.3329, 0.3329, 0.3329,	/* È ÊË ÍÎÏ */
    1.0000, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/* Ð        */
    0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500, 0.2500,	/*          */
    0.2500, 0.8889, 0.2500, 0.2759, 0.2500, 0.2500, 0.2500, 0.2500,	/*  á ã     */
    0.6109, 0.7219, 0.8889, 0.3099, 0.2500, 0.2500, 0.2500, 0.2500,	/* èéêë     */
    0.2500, 0.6669, 0.2500, 0.2500, 0.2500, 0.2779, 0.2500, 0.2500,	/*  ñ   õ   */
    0.2779, 0.5000, 0.7219, 0.5000, 0.2500, 0.2500, 0.2500, 0.2500,	/* øùúû     */
};
static double arialFontWidth[] = {
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.3549, 0.5559, 0.5559, 0.8889, 0.6669, 0.2209,	/*  !"#$%&' */
    0.3329, 0.3329, 0.3889, 0.5839, 0.2779, 0.3329, 0.2779, 0.2779,	/* ()*+,-./ */
    0.5559, 0.5559, 0.5559, 0.5559, 0.5559, 0.5559, 0.5559, 0.5559,	/* 01234567 */
    0.5559, 0.5559, 0.2779, 0.2779, 0.5839, 0.5839, 0.5839, 0.5559,	/* 89:;<=>? */
    1.0149, 0.6669, 0.6669, 0.7219, 0.7219, 0.6669, 0.6109, 0.7779,	/* @ABCDEFG */
    0.7219, 0.2779, 0.5000, 0.6669, 0.5559, 0.8329, 0.7219, 0.7779,	/* HIJKLMNO */
    0.6669, 0.7779, 0.7219, 0.6669, 0.6109, 0.7219, 0.6669, 0.9439,	/* PQRSTUVW */
    0.6669, 0.6669, 0.6109, 0.2779, 0.2779, 0.2779, 0.4689, 0.5559,	/* XYZ[\]^_ */
    0.2219, 0.5559, 0.5559, 0.5000, 0.5559, 0.5559, 0.2779, 0.5559,	/* `abcdefg */
    0.5559, 0.2219, 0.2219, 0.5000, 0.2219, 0.8329, 0.5559, 0.5559,	/* hijklmno */
    0.5559, 0.5559, 0.3329, 0.5000, 0.2779, 0.5559, 0.5000, 0.7219,	/* pqrstuvw */
    0.5000, 0.5000, 0.5000, 0.3339, 0.2599, 0.3339, 0.5839, 0.2779,	/* xyz{|}~  */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 0.3329, 0.5559, 0.5559, 0.1669, 0.5559, 0.5559, 0.5559,	/*  ¡¢£¤¥¦§ */
    0.5559, 0.1909, 0.3329, 0.5559, 0.3329, 0.3329, 0.5000, 0.5000,	/* ¨©ª«¬­®¯ */
    0.2779, 0.5559, 0.5559, 0.5559, 0.2779, 0.2779, 0.5369, 0.3499,	/*  ±²³´ ¶· */
    0.2219, 0.3329, 0.3329, 0.5559, 1.0000, 1.0000, 0.2779, 0.6109,	/* ¸¹º»¼½ ¿ */
    0.2779, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329, 0.3329,	/*  ÁÂÃÄÅÆÇ */
    0.3329, 0.2779, 0.3329, 0.3329, 0.2779, 0.3329, 0.3329, 0.3329,	/* È ÊË ÍÎÏ */
    1.0000, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/* Ð        */
    0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*          */
    0.2779, 1.0000, 0.2779, 0.3699, 0.2779, 0.2779, 0.2779, 0.2779,	/*  á ã     */
    0.5559, 0.7779, 1.0000, 0.3649, 0.2779, 0.2779, 0.2779, 0.2779,	/* èéêë     */
    0.2779, 0.8889, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779, 0.2779,	/*  ñ   õ   */
    0.2219, 0.6109, 0.9439, 0.6109, 0.2779, 0.2779, 0.2779, 0.2779,	/* øùúû     */
};
static double courFontWidth[] = {
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  !"#$%&' */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* ()*+,-./ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* 01234567 */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* 89:;<=>? */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* @ABCDEFG */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* HIJKLMNO */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* PQRSTUVW */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* XYZ[\]^_ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* `abcdefg */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* hijklmno */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* pqrstuvw */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* xyz{|}~  */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  ¡¢£¤¥¦§ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* ¨©ª«¬­®¯ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  ±²³´ ¶· */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* ¸¹º»¼½ ¿ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  ÁÂÃÄÅÆÇ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* È ÊË ÍÎÏ */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* Ð        */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*          */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  á ã     */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* èéêë     */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/*  ñ   õ   */
    0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999, 0.5999,	/* øùúû     */
};

/* estimate_textspan_size:
 * Estimate size of textspan, for given face and size, in points.
 */
static void
estimate_textspan_size(textspan_t * span, char **fontpath)
{
    double *Fontwidth, fontsize;
    char c, *p, *fpp, *fontname;

    fontname = span->font->name;
    fontsize = span->font->size;

    span->size.x = 0.0;
    span->size.y = fontsize * LINESPACING;
    span->yoffset_layout = 0.0;
    span->yoffset_centerline = 0.1 * fontsize;
    span->layout = NULL;
    span->free_layout = NULL;

    if (!strncasecmp(fontname, "cour", 4)) {
	fpp = "[internal courier]";
	Fontwidth = courFontWidth;
    } else if (!strncasecmp(fontname, "arial", 5)
	       || !strncasecmp(fontname, "helvetica", 9)) {
	fpp = "[internal arial]";
	Fontwidth = arialFontWidth;
    } else {
	fpp = "[internal times]";
	Fontwidth = timesFontWidth;
    }
    if (fontpath)
	*fontpath = fpp;
    if ((p = span->str)) {
	while ((c = *p++))
	    span->size.x += Fontwidth[(unsigned char) c];
 	/* NOTE: Tables are based on a font of size 1. Need to multiply by
 	 * fontsize to get appropriate value.
 	 */
	span->size.x *= fontsize;
    }
}

/*
 * This table maps standard Postscript font names to URW Type 1 fonts.
 *
 *   The original source is in ps_font_equiv.txt.  This is sorted 
 *   during make into ps_font_equiv.h to ensure that it is in the right
 *   order for bsearch()
 */
static PostscriptAlias postscript_alias[] = {
#include "ps_font_equiv.h"
};

static int fontcmpf(const void *a, const void *b)
{
    return (strcasecmp(((PostscriptAlias*)a)->name, ((PostscriptAlias*)b)->name));
}

static PostscriptAlias* translate_postscript_fontname(char* fontname)
{
    static PostscriptAlias key;
    static PostscriptAlias *result;

    if (key.name == NULL || strcasecmp(key.name, fontname)) {
        key.name = fontname;
        result = (PostscriptAlias *) bsearch((void *) &key,
			(void *) postscript_alias,
			sizeof(postscript_alias) / sizeof(PostscriptAlias),
			sizeof(PostscriptAlias),
                        fontcmpf);
    }
    return result;
}

pointf textspan_size(GVC_t *gvc, textspan_t * span)
{
    char **fpp = NULL, *fontpath = NULL;
    textfont_t *font;

    assert(span->font);
    font = span->font;

    assert(font->name);
    font->postscript_alias = translate_postscript_fontname(font->name);

    if (Verbose && emit_once(font->name))
	fpp = &fontpath;

    if (! gvtextlayout(gvc, span, fpp))
	estimate_textspan_size(span, fpp);

    if (fpp) {
	if (fontpath)
	    fprintf(stderr, "fontname: \"%s\" resolved to: %s\n",
		    font->name, fontpath);
	else
	    fprintf(stderr, "fontname: unable to resolve \"%s\"\n", font->name);
    }

    return span->size;
}

textfont_t * new_textfont(void)
{
    textfont_t *tf = calloc(1, sizeof(textfont_t));
    assert(tf);
    return tf;
}

void ref_textfont(textfont_t * tf)
{
    tf->cnt++;
}

void unref_textfont(textfont_t * tf)
{
    assert(tf);
    if (tf->cnt) {
	tf->cnt--;
    }
    else {
	if (tf->name)
	    free(tf->name);
	if (tf->color)
	    free(tf->color);
	free(tf);
    }
}

#if 0
typedef struct {
	/* key */
	char *name;
	char *color;
	unsigned int flags;
	/* non key */
	char *postscript_alias;
} font_t;

Void_t* font_makef(Dt_t* dt, Void_t* obj, Dtdisc_t* disc)
{
	font_t *f1 = (font_t*)obj;
	font_t *f2 = calloc(1,sizeof(font_t));
	
	/* key */
	if (f1->name) f2->name = strdup(f1->name);
	if (f1->color) f2->color = strdup(f1->color);
	f2->flags = f1->flags;

	/* non key */
	f2->postscript_alias = f1->postscript_alias;

	return f2;
}

void font_freef(Dt_t* dt, Void_t* obj, Dtdisc_t* disc)
{
	font_t *f = (font_t*)obj;

	if (f->name) free(f->name);
	if (f->color) free(f->color);
	free(f);
}

int font_comparf (Dt_t* dt, Void_t* key1, Void_t* key2, Dtdisc_t* disc)
{
	int rc;
	font_t *f1 = (font_t*)key1, *f2 = (font_t*)key2;

	rc = strcmp(f1->name, f2->name);
	if (rc) return rc;
	rc = strcmp(f1->color, f2->color);
	if (rc) return rc;
	return (f1->flags - f2->flags);
}

Dtdisc_t fontdisc = {
	0,sizeof(font_t),-1,font_makef,font_freef,font_comparf,NULL,NULL,NULL
};

#define TEST 1
#ifdef TEST

font_t font1 = { "Times", "black", 0 };
font_t font2 = { "Arial", "black", 0 };
font_t font3 = { "Arial", "black", 4 };
font_t font4 = { "Arial", "black", 0 };  /* dup of 2 */
font_t font5 = { "Arial", "red", 0 };
font_t font6 = { "Times", "black", 0 };  /* dup of 1 */
	
int main (void) 
{
	Dt_t *fontname_dt;
	font_t *f1 = &font1, *f2 = &font2, *f3 = &font3, *f4 = &font4, *f5 = &font5, *f6 = &font6;

	fprintf(stderr,"%p %p %p %p %p %p\n", f1, f2, f3, f4, f5, f6);
	fprintf(stderr,"%s %s %s %s %s %s\n", f1->name, f2->name, f3->name, f4->name, f5->name, f6->name);

	fontname_dt = dtopen( &fontdisc, Dtoset);

	f1 = dtinsert(fontname_dt, f1);
	f2 = dtinsert(fontname_dt, f2);
	f3 = dtinsert(fontname_dt, f3);
	f4 = dtinsert(fontname_dt, f4);
	f5 = dtinsert(fontname_dt, f5);
	f6 = dtinsert(fontname_dt, f6);

	fprintf(stderr,"%p %p %p %p %p %p\n", f1, f2, f3, f4, f5, f6);
	fprintf(stderr,"%s %s %s %s %s %s\n", f1->name, f2->name, f3->name, f4->name, f5->name, f6->name);

	dtclose(fontname_dt);

	return 0;
}

#endif
#endif
