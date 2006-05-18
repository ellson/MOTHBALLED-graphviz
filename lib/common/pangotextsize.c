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

#ifdef HAVE_PANGOCAIRO
#include <pango/pangocairo.h>
#endif

char * pango_textsize(textline_t * textline, char *fontname, double fontsize, char *fontpath)
{
#ifdef HAVE_PANGOCAIRO
    static PangoFontMap *fontmap;
    static PangoContext *context;
    PangoFontDescription *desc;
    PangoLayout *layout;
    PangoRectangle ink_rect, logical_rect, char_rect;
#if 0
    PangoLayoutIter* iter;
#endif
#ifdef ENABLE_PANGO_MARKUP
    PangoAttrList *attrs;
    GError *error = NULL;
#endif
    char *text;
    double scale = (double)PANGO_SCALE * DEFAULT_DPI / POINTS_PER_INCH;

    if (!fontmap)
        fontmap = pango_cairo_font_map_get_default();
    if (!context)
        context = pango_cairo_font_map_create_context (PANGO_CAIRO_FONT_MAP(fontmap));

    desc = pango_font_description_new();
    pango_font_description_set_family (desc, fontname);
    pango_font_description_set_size (desc, (gint)(fontsize * PANGO_SCALE));

#ifdef ENABLE_PANGO_MARKUP
    if (! pango_parse_markup (textline->str, -1, 0, &attrs, &text, NULL, &error))
	die(error->message);
#else
    text = textline->str;
#endif

    layout = pango_layout_new (context);
    textline->layout = (void *)layout;    /* layout free with textline - see labels.c */

    pango_layout_set_text (layout, text, -1);
    pango_layout_set_font_description (layout, desc);
#ifdef ENABLE_PANGO_MARKUP
    pango_layout_set_attributes (layout, attrs);
#endif

    pango_layout_get_extents (layout, &ink_rect, &logical_rect);
    textline->width = logical_rect.width / scale;
    textline->height = logical_rect.height / scale;

    /* determine position of each character in the layout */
    textline->xshow = NULL;
#if 0
    iter = pango_layout_get_iter (layout);
    do {
        pango_layout_iter_get_char_extents (iter, &char_rect);
	char_rect.x /= PANGO_SCALE; char_rect.y /= PANGO_SCALE;
    } while (pango_layout_iter_next_char (iter));
    pango_layout_iter_free (iter);
#endif

    pango_font_description_free (desc);

    fontpath = "[pango]";
#else
    return "No Pango support available";
#endif
    return NULL;
}
