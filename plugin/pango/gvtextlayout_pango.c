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

static void pango_free_layout (void *layout)
{
    g_object_unref((PangoLayout*)layout);
}

static void pango_textlayout(GVCOMMON_t *common, textpara_t * para, char **fontpath)
{
    static char buf[512];  /* returned in fontpath, only good until next call */
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
    char *text;
    const char *family;
    PangoStyle style;
    PangoWeight weight;
    PangoVariant variant;
    PangoStretch stretch;

    if (!fontmap)
        fontmap = pango_cairo_font_map_get_default();
    pango_cairo_font_map_set_resolution(PANGO_CAIRO_FONT_MAP(fontmap), (double)POINTS_PER_INCH);
    if (!context)
        context = pango_cairo_font_map_create_context (PANGO_CAIRO_FONT_MAP(fontmap));

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
            desc = pango_font_description_from_string(buf);
	}
	else
            desc = pango_font_description_from_string(fontname);
    }
    family = pango_font_description_get_family (desc);
    style = pango_font_description_get_style (desc);
    weight = pango_font_description_get_weight (desc);
    variant = pango_font_description_get_variant (desc);
    stretch = pango_font_description_get_stretch (desc);
    strcpy(buf,family);
    switch (style) {
	case PANGO_STYLE_NORMAL:            /* strcat(buf, ", normal");*/ break;
	case PANGO_STYLE_OBLIQUE:           strcat(buf, ", oblique"); break;
	case PANGO_STYLE_ITALIC:            strcat(buf, ", italic"); break;
    }
    switch (weight) {
	case PANGO_WEIGHT_ULTRALIGHT:       strcat(buf, ", ultralight"); break;
	case PANGO_WEIGHT_LIGHT:            strcat(buf, ", light"); break;
	case PANGO_WEIGHT_NORMAL:           /* strcat(buf, ", normal");*/ break;
	case PANGO_WEIGHT_SEMIBOLD:         strcat(buf, ", semibold"); break;
	case PANGO_WEIGHT_BOLD:             strcat(buf, ", bold"); break;
	case PANGO_WEIGHT_ULTRABOLD:        strcat(buf, ", ultrabold"); break;
	case PANGO_WEIGHT_HEAVY:            strcat(buf, ", heavy"); break;
    }
    switch (variant) {
	case PANGO_VARIANT_NORMAL:          /* strcat(buf, ", normal");*/ break;
	case PANGO_VARIANT_SMALL_CAPS:      strcat(buf, ", smallcaps"); break;
    }
    switch (stretch) {
	case PANGO_STRETCH_ULTRA_CONDENSED: strcat(buf, ", ultracondensed"); break;
	case PANGO_STRETCH_EXTRA_CONDENSED: strcat(buf, ", extracondensed"); break;
	case PANGO_STRETCH_CONDENSED:       strcat(buf, ", condensed"); break;
	case PANGO_STRETCH_SEMI_CONDENSED:  strcat(buf, ", semicondensed"); break;
	case PANGO_STRETCH_NORMAL:          /* strcat(buf, ", normal");*/ break;
	case PANGO_STRETCH_SEMI_EXPANDED:   strcat(buf, ", semiexpanded"); break;
	case PANGO_STRETCH_EXPANDED:        strcat(buf, ", expanded"); break;
	case PANGO_STRETCH_EXTRA_EXPANDED:  strcat(buf, ", extraexpanded"); break;
	case PANGO_STRETCH_ULTRA_EXPANDED:  strcat(buf, ", ultraexpanded"); break;
    }
    *fontpath = buf;

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
