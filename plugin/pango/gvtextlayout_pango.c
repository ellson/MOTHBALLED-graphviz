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

#include <string.h>
#include "gvplugin_textlayout.h"

#ifdef HAVE_PANGOCAIRO
#include <pango/pangocairo.h>

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

static char* find_postscript_font(char* fontname)
{
  int i;

  for (i = 0; i < sizeof(postscript_alias)/sizeof(*postscript_alias); i++) {
    if (strcmp(fontname, postscript_alias[i].name) == 0) {
	return postscript_alias[i].string_desc;
    }
  }
  return NULL;
}

static void pango_free_layout (void *layout)
{
    g_object_unref((PangoLayout*)layout);
}

static void pango_textlayout(GVCOMMON_t *common, textpara_t * para, char **fontpath)
{
    static PangoFontMap *fontmap;
    static PangoContext *context;
    static PangoFontDescription *desc;
    static char *fontname;
    PangoLayout *layout;
    PangoRectangle ink_rect, logical_rect;
#if ENABLE_PANGO_XSHOW
    PangoRectangle char_rect;
    PangoLayoutIter* iter;
#endif
#ifdef ENABLE_PANGO_MARKUP
    PangoAttrList *attrs;
    GError *error = NULL;
#endif
    char *text, *fontreq;
    const char *family;

    if (!fontmap)
        fontmap = pango_cairo_font_map_get_default();
    pango_cairo_font_map_set_resolution(PANGO_CAIRO_FONT_MAP(fontmap), (double)POINTS_PER_INCH);
    if (!context)
        context = pango_cairo_font_map_create_context (PANGO_CAIRO_FONT_MAP(fontmap));

    if (!fontname || strcmp(fontname, para->fontname)) {
	fontname = para->fontname;
        pango_font_description_free (desc);

        /* try to find a match for a PostScript font
	 * - or just get best available match */
        if (! (fontreq = find_postscript_font(fontname)))
	    fontreq = fontname;
        desc = pango_font_description_from_string(fontreq);
    }
    family = pango_font_description_get_family (desc);
    *fontpath = (char *)family;

    pango_font_description_set_size (desc, (gint)(para->fontsize * PANGO_SCALE));

#ifdef ENABLE_PANGO_MARKUP
    if (! pango_parse_markup (para->str, -1, 0, &attrs, &text, NULL, &error))
	die(error->message);
#else
    text = para->str;
#endif

    layout = pango_layout_new (context);
    para->layout = (void *)layout;    /* layout free with textpara - see labels.c */
    para->free_layout = pango_free_layout;    /* function for freeing pango layout */

    pango_layout_set_text (layout, text, -1);
    pango_layout_set_font_description (layout, desc);
#ifdef ENABLE_PANGO_MARKUP
    pango_layout_set_attributes (layout, attrs);
#endif

    pango_layout_get_extents (layout, &ink_rect, &logical_rect);

    /* if pango doesn't like the font then it sets width=0 but height = garbage */
    if (logical_rect.width == 0) {
	*fontpath = NULL;              /* indicate a problem */
	logical_rect.height = 0;
    }

    para->width = (double)logical_rect.width / PANGO_SCALE;
    para->height = (double)logical_rect.height / PANGO_SCALE;

    /* determine position of each character in the layout */
    para->xshow = NULL;
#ifdef ENABLE_PANGO_XSHOW
/* FIXME - unfinished code */
    iter = pango_layout_get_iter (layout);
    do {
        pango_layout_iter_get_char_extents (iter, &char_rect);
	char_rect.x /= PANGO_SCALE;
	char_rect.y /= PANGO_SCALE;
    } while (pango_layout_iter_next_char (iter));
    pango_layout_iter_free (iter);
#endif
}

static gvtextlayout_engine_t pango_textlayout_engine = {
    pango_textlayout,
};
#endif

gvplugin_installed_t gvtextlayout_pango_types[] = {
#ifdef HAVE_PANGOCAIRO
    {0, "textlayout", 10, &pango_textlayout_engine, NULL},
#endif
    {0, NULL, 0, NULL, NULL}
};
