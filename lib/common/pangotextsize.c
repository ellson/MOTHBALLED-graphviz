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
#ifdef ENABLE_PANGO_MARKUP
    PangoAttrList *attrs;
    GError *error = NULL;
#endif
    char *text;
    int width;

    if (!fontmap)
        fontmap = pango_cairo_font_map_get_default();
    if (!context)
        context = pango_cairo_font_map_create_context (PANGO_CAIRO_FONT_MAP(fontmap));

    desc = pango_font_description_new();
    pango_font_description_set_family (desc, fontname);
    pango_font_description_set_size (desc, fontsize);

#ifdef ENABLE_PANGO_MARKUP
    if (! pango_parse_markup (textline->str, -1, 0, &attrs, &text, NULL, &error))
	die(error->message);
#else
    text = textline->str;
#endif

    layout = pango_layout_new (context);
    pango_layout_set_text (layout, textline->str, -1);
    pango_layout_set_font_description (layout, desc);
#ifdef ENABLE_PANGO_MARKUP
    pango_layout_set_attributes (layout, attrs);
#endif

    pango_font_description_free (desc);

    pango_layout_get_size (layout, NULL, &width);

    g_object_unref (layout);

    textline->width = width;
    textline->xshow = NULL;
    fontpath = "[pango]";

#else
    return "No Pango support available";
#endif
    return NULL;
}
