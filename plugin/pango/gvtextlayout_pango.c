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

static boolean pango_textlayout(textpara_t * para, char **fontpath)
{
    static char buf[1024];  /* returned in fontpath, only good until next call */
    static PangoFontMap *fontmap;
    static PangoContext *context;
    static PangoFontDescription *desc;
    static char *fontname;
    char *fnt;
    PangoLayout *layout;
    PangoRectangle logical_rect;
    PangoLayoutIter* iter;
#if ENABLE_PANGO_XSHOW
    PangoRectangle char_rect;
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
	g_object_unref(fontmap);
    }

    if (!fontname || strcmp(fontname, para->fontname)) {
	fontname = para->fontname;
	pango_font_description_free (desc);

	if (para->postscript_alias) {
	    int comma=0;
	    strcpy(buf, para->postscript_alias->family);
	    if (para->postscript_alias->weight) {
		strcat(buf, comma ? " " : ", ");
		comma = 1;
		strcat(buf, para->postscript_alias->weight);
	    }
	    if (para->postscript_alias->stretch) {
		strcat(buf, comma ? " " : ", ");
		comma = 1;
		strcat(buf, para->postscript_alias->stretch);
	    }
	    if (para->postscript_alias->style) {
		strcat(buf, comma ? " " : ", ");
		comma = 1;
		strcat(buf, para->postscript_alias->style);
	    }
	    fnt = buf;
	}
	else
	    fnt = fontname;

	desc = pango_font_description_from_string(fnt);

        if (fontpath) {  /* -v support */
#ifdef HAVE_FONTCONFIG
	    FcPattern *pat, *match;
	    FcFontSet *fs;
	    FcResult result;
    
            if (! FcInit())
	        return FALSE;
    
	    pat = FcNameParse((FcChar8 *) fnt);
	    FcConfigSubstitute (0, pat, FcMatchPattern);
	    FcDefaultSubstitute (pat);
	    fs = FcFontSetCreate();
	    match = FcFontMatch (0, pat, &result);
	    if (match)
	        FcFontSetAdd (fs, match);
	    FcPatternDestroy (pat);
	    strcpy(buf,"");
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
            *fontpath = buf;
	    FcFontSetDestroy(fs);
#else
	    *fontpath = fnt;
#endif
        }
    }
    /* all text layout is done at a scale of 96ppi */
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

    pango_layout_get_extents (layout, NULL, &logical_rect);

    /* if pango doesn't like the font then it sets width=0 but height = garbage */
    if (logical_rect.width == 0)
	logical_rect.height = 0;

    textlayout_scale = POINTS_PER_INCH / (FONT_DPI * PANGO_SCALE);
    para->width = logical_rect.width * textlayout_scale;
    para->height = logical_rect.height * textlayout_scale;

    iter = pango_layout_get_iter (layout);
    para->yoffset = pango_layout_iter_get_baseline (iter) * textlayout_scale
	+.1 * para->fontsize;  /* In labels.c y is already midline 
			 * 0.15 is the distance below midline for y centering */

    /* determine position of each character in the layout */
    para->xshow = NULL;
#ifdef ENABLE_PANGO_XSHOW
/* FIXME - unfinished code */
    do {
	pango_layout_iter_get_char_extents (iter, &char_rect);
	char_rect.x *= fontlayout_scale;
	char_rect.y *= fontlayout_scale;
    } while (pango_layout_iter_next_char (iter));
    pango_layout_iter_free (iter);
#endif
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
