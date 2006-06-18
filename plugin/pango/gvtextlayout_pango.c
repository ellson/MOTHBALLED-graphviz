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

#include "gvplugin_textlayout.h"

#ifdef HAVE_PANGOCAIRO
#include <pango/pangocairo.h>

static void pango_free_layout (void *layout)
{
    g_object_unref((PangoLayout*)layout);
}

static void pango_textlayout(textpara_t * para, char **fontpath)
{
    static PangoFontMap *fontmap;
    static PangoContext *context;
    PangoFontDescription *desc;
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

    if (!fontmap)
        fontmap = pango_cairo_font_map_get_default();
    pango_cairo_font_map_set_resolution(PANGO_CAIRO_FONT_MAP(fontmap), (double)POINTS_PER_INCH);
    if (!context)
        context = pango_cairo_font_map_create_context (PANGO_CAIRO_FONT_MAP(fontmap));

    desc = pango_font_description_new();
    pango_font_description_set_family (desc, para->fontname);
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
    else {
    	*fontpath = "[pango]";
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

    pango_font_description_free (desc);
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
