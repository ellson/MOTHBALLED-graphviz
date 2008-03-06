/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*********************************************************
*       This software is part of the graphviz package    *
*	        http://www.graphviz.org/                 *
*	                                                 *
*	    Copyright (c) 1994-2004 AT&T Corp.           *
*	        and is licensed under the                *
*	    Common Public License, Version 1.0           *
*	              by AT&T Corp.                      *
*	                                                 *
*	Information and Software Systems Research        *
*	      AT&T Research, Florham Park NJ             *
**********************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "gvplugin_textlayout.h"

#ifdef HAVE_PANGOCAIRO
#include <pango/pangocairo.h>
#ifdef HAVE_FONTCONFIG
#include <fontconfig/fontconfig.h>
#endif

static void pango_free_layout (void *layout)
{
    g_object_unref((PangoLayout*)layout);
}

#define FONT_DPI 96.

extern char* psfontResolve (PostscriptAlias* pa);

static boolean pango_textlayout(textpara_t * para, char **fontpath)
{
    static char buf[1024];  /* returned in fontpath, only good until next call */
    static PangoFontMap *fontmap;
    static PangoContext *context;
    static PangoFontDescription *desc;
    static char *fontname;
    char *fnt, *psfnt = NULL;
    PangoLayout *layout;
    PangoRectangle logical_rect;
    PangoLayoutIter* iter;
	cairo_font_options_t* options;
/* #define ENABLE_PANGO_XSHOW */
#ifdef ENABLE_PANGO_XSHOW
    PangoRectangle char_rect;
    int xshow_alloc, xshow_pos;
#endif
#ifdef ENABLE_PANGO_MARKUP
    PangoAttrList *attrs;
    GError *error = NULL;
#endif
    char *text;
    double textlayout_scale;

    if (!context) {
	fontmap = pango_cairo_font_map_get_default();
	context = pango_cairo_font_map_create_context (PANGO_CAIRO_FONT_MAP(fontmap));
	options=cairo_font_options_create();
	cairo_font_options_set_antialias(options,CAIRO_ANTIALIAS_GRAY);
	cairo_font_options_set_hint_style(options,CAIRO_HINT_STYLE_FULL);
	cairo_font_options_set_hint_metrics(options,CAIRO_HINT_METRICS_ON);
	cairo_font_options_set_subpixel_order(options,CAIRO_SUBPIXEL_ORDER_BGR);
	pango_cairo_context_set_font_options(context, options);

	g_object_unref(fontmap);
    }

    if (!fontname || strcmp(fontname, para->fontname)) {
	fontname = para->fontname;
	pango_font_description_free (desc);

	if (para->postscript_alias) {
	    psfnt = fnt = psfontResolve (para->postscript_alias);
	}
	else
	    fnt = fontname;

	desc = pango_font_description_from_string(fnt);
        /* all text layout is done at a scale of 96ppi */
        pango_font_description_set_size (desc, (gint)(para->fontsize * PANGO_SCALE));

        if (fontpath) {  /* -v support */
	    PangoFont *font;
    	    PangoFontDescription *tdesc;
	    char *tfont;

            font = pango_font_map_load_font(fontmap, context, desc);
	    tdesc = pango_font_describe(font);
	    tfont = pango_font_description_to_string(tdesc);
	    strcpy(buf, "\"");
	    strcat(buf, tfont);
	    strcat(buf, "\" ");
	    if (psfnt)
		strcat(buf, "(PostScript) ");
	    else
		strcat(buf, "(non-PS    ) ");

#ifdef HAVE_FONTCONFIG
	    FcPattern *pat, *match;
	    FcFontSet *fs;
	    FcResult result;
    
            if (! FcInit())
	        return FALSE;
    
	    pat = FcNameParse((FcChar8 *) tfont);
	    FcConfigSubstitute (0, pat, FcMatchPattern);
	    FcDefaultSubstitute (pat);
	    fs = FcFontSetCreate();
	    match = FcFontMatch (0, pat, &result);
	    if (match)
	        FcFontSetAdd (fs, match);
	    FcPatternDestroy (pat);
	    if (fs) {
	        FcChar8 *family, *style, *file;
    
	        if (FcPatternGetString (fs->fonts[0], FC_FILE, 0, &file) != FcResultMatch)
		    file = (FcChar8 *) "<unknown font filename>";
	        if (FcPatternGetString (fs->fonts[0], FC_FAMILY, 0, &family) != FcResultMatch)
		    family = (FcChar8 *) "<unknown font family>";
	        if (FcPatternGetString (fs->fonts[0], FC_STYLE, 0, &style) != FcResultMatch)
		    style = (FcChar8 *) "<unknown font style>";
		strcat(buf, "\"");
	        strcat(buf, (char *)family);
	        strcat(buf, ", ");
	        strcat(buf, (char *)style);
	        strcat(buf, "\" ");
	        strcat(buf, (char *)file);
	    }
	    FcFontSetDestroy(fs);
#endif
            *fontpath = buf;
	    g_free(tfont);
        }
    }

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

    pango_layout_get_extents (layout, NULL, &logical_rect);

    /* if pango doesn't like the font then it sets width=0 but height = garbage */
    if (logical_rect.width == 0)
	logical_rect.height = 0;

    textlayout_scale = POINTS_PER_INCH / (FONT_DPI * PANGO_SCALE);
    para->width = ROUND(logical_rect.width * textlayout_scale);
    para->height = ROUND(logical_rect.height * textlayout_scale);

    /* The y offset from baseline to 0,0 of the bitmap representation */
    iter = pango_layout_get_iter (layout);
    para->yoffset_layout = pango_layout_iter_get_baseline (iter) * textlayout_scale;

    /* The distance below midline for y centering of text strings */
    para->yoffset_centerline = 0.1 * para->fontsize;

    pango_layout_iter_free (iter);
    if (logical_rect.width == 0)
	return FALSE;
    return TRUE;
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
