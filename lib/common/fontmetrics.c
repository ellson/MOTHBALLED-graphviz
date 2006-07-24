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

#if defined(WITH_CODEGENS) && !defined(HAVE_GD_FREETYPE) && defined(HAVE_LIBGD)
extern codegen_t *Output_codegen;
extern codegen_t GD_CodeGen;
#endif

/* estimate_textsize:
 * Estimate width and height of text, for given face and size, in points.
 * Value is stored para->width.
 * NOTE: Tables are based on a font of size 1. Need to multiply by
 * fontsize to get appropriate value.
 */
static void
estimate_textlayout(graph_t *g, textpara_t * para, char **fontpath)
{
    double *Fontwidth;
    char c, *p;

    para->width = 0.0;
    para->height = para->fontsize;
    para->xshow = NULL;
    para->layout = para->fontname;
    para->free_layout = NULL;

    if (!strncasecmp(para->fontname, "cour", 4)) {
	*fontpath = "[internal courier]";
	Fontwidth = courFontWidth;
    } else if (!strncasecmp(para->fontname, "arial", 5)
	       || !strncasecmp(para->fontname, "helvetica", 9)) {
	*fontpath = "[internal arial]";
	Fontwidth = arialFontWidth;
    } else {
	*fontpath = "[internal times]";
	Fontwidth = timesFontWidth;
    }
    if ((p = para->str)) {
	while ((c = *p++))
	    para->width += Fontwidth[(unsigned char) c];
	para->width *= para->fontsize;
    }
}

typedef struct _PostscriptAlias {
    char* name;
    char* string_desc;
} PostscriptAlias;

/* This table maps standard Postscript font names to URW Type 1 fonts */
static PostscriptAlias postscript_alias[] = {
    { "AvantGarde-Book", "URW Gothic L, Book" },
    { "AvantGarde-BookOblique", "URW Gothic L, Book, Oblique" },
    { "AvantGarde-Demi", "URW Gothic L, Demi" },
    { "AvantGarde-DemiOblique", "URW Gothic L, Demi, Oblique" },

    { "Bookman-Demi", "URW Bookman L, Demi, Bold" },
    { "Bookman-DemiItalic", "URW Bookman L, Demi, Bold, Italic" },
    { "Bookman-Light", "URW Bookman L, Light" },
    { "Bookman-LightItalic", "URW Bookman L, Light, Italic" },

    { "Courier", "Nimbus Mono L, Regular" },
    { "Courier-Oblique", "Nimbus Mono L, Regular, Oblique" },
    { "Courier-Bold", "Nimbus Mono L, Bold" },
    { "Courier-BoldOblique", "Nimbus Mono L, Bold, Oblique" },

    { "Helvetica", "Nimbus Sans L, Regular" },
    { "Helvetica-Oblique", "Nimbus Sans L, Regular, Italic" },
    { "Helvetica-Bold", "Nimbus Sans L, Bold" },
    { "Helvetica-BoldOblique", "Nimbus Sans L, Bold, Italic" },

    { "Helvetica-Narrow", "Nimbus Sans L, Regular, Condensed" },
    { "Helvetica-Narrow-Oblique", "Nimbus Sans L, Regular, Condensed, Italic" },
    { "Helvetica-Narrow-Bold", "Nimbus Sans L, Bold, Condensed" },
    { "Helvetica-Narrow-BoldOblique", "Nimbus Sans L, Bold, Condensed, Italic" },

    { "NewCenturySchlbk-Roman", "Century Schoolbook L, Roman" },
    { "NewCenturySchlbk-Italic", "Century Schoolbook L, Italic" },
    { "NewCenturySchlbk-Bold", "Century Schoolbook L, Bold" },
    { "NewCenturySchlbk-BoldItalic", "Century Schoolbook L, Bold, Italic" },

    { "Palatino-Roman", "URW Palladio L, Roman" },
    { "Palatino-Italic", "URW Palladio L, Italic" },
    { "Palatino-Bold", "URW Palladio L, Bold" },
    { "Palatino-BoldItalic", "URW Palladio L, Bold, Italic" },

    { "Symbol", "Standard Symbols L, Regular" },

    { "Times-Roman", "Nimbus Roman No9 L, Regular" },
    { "Times-Italic", "Nimbus Roman No9 L, Regular, Italic" },
    { "Times-Bold", "Nimbus Roman No9 L, Medium" },
    { "Times-BoldItalic", "Nimbus Roman No9 L, Medium, Italic" },

    { "ZapfChancery-MediumItalic", "URW Chancery L, Medium, Italic" },
    { "ZapfDingbats", "Dingbats" },
};

static char* translate_postscript_fontname(char* fontname)
{
  int i;

  for (i = 0; i < sizeof(postscript_alias)/sizeof(*postscript_alias); i++) {
    if (strcmp(fontname, postscript_alias[i].name) == 0) {
        return postscript_alias[i].string_desc;
    }
  }
  return NULL;
}

pointf textsize(graph_t *g, textpara_t * para, char *fontname, double fontsize)
{
    char *fontpath = NULL;
    pointf size;

    para->fontname = fontname;
    para->fontsize = fontsize;

    para->translated_fontname = translate_postscript_fontname(fontname);

    if (! gvtextlayout(GD_gvc(g), para, &fontpath) || !fontpath)
	estimate_textlayout(g, para, &fontpath);

    if (Verbose) {
	if (emit_once(para->fontname)) {
	    fprintf(stderr, "%s: fontname \"%s\" resolved to \"%s\"\n",
		    GD_gvc(g)->common.cmdname, para->fontname, fontpath);
	}
    }
    size.x = para->width;
    size.y = para->height;
    return size;
}
