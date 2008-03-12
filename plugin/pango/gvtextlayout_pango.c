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
#include <pango/pangofc-font.h>
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
    static double fontsize;
    char *fnt, *psfnt = NULL;
    PangoLayout *layout;
    PangoRectangle logical_rect;
    PangoLayoutIter* iter;
	cairo_font_options_t* options;
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

    if (!fontname || strcmp(fontname, para->fontname) != 0 || fontsize != para->fontsize) {
	fontname = para->fontname;
	fontsize = para->fontsize;
	pango_font_description_free (desc);

	if (para->postscript_alias) {
	    psfnt = fnt = psfontResolve (para->postscript_alias);
	}
	else
	    fnt = fontname;

	desc = pango_font_description_from_string(fnt);
        /* all text layout is done at a scale of 96ppi */
        pango_font_description_set_size (desc, (gint)(fontsize * PANGO_SCALE));

        if (fontpath) {  /* -v support */
	    PangoFont *font;

            font = pango_font_map_load_font(fontmap, context, desc);

	    buf[0] = '\0';
	    if (psfnt)
		strcat(buf, "(ps) ");
#ifdef PANGO_TYPE_FC_FONT
	    {
	        FT_Face face;
	        PangoFcFont *fcfont;
	        FT_Stream stream;
	        FT_StreamDesc streamdesc;
	        fcfont = PANGO_FC_FONT(font);
	        if (fcfont) {
	            face = pango_fc_font_lock_face(fcfont);
	            if (face) {
		        strcat(buf, "\"");
		        strcat(buf, face->family_name);
		        strcat(buf, ", ");
		        strcat(buf, face->style_name);
		        strcat(buf, "\" ");
    
		        stream = face->stream;
		        if (stream) {
			    streamdesc = stream->pathname;
			    if (streamdesc.pointer)
			        strcat(buf, (char*)streamdesc.pointer);
		            else
			        strcat(buf, "*no pathname available*");
		        }
		        else
			    strcat(buf, "*no stream available*");
		    }
	            pango_fc_font_unlock_face(fcfont);
	        }
	        else
	            strcat(buf, "*not using fontconfig*");
	    }
#else
	    {
    		PangoFontDescription *tdesc;
		char *tfont;
		
	        tdesc = pango_font_describe(font);
	        tfont = pango_font_description_to_string(tdesc);
	        strcat(buf, "\"");
	        strcat(buf, tfont);
	        strcat(buf, "\" ");
	        g_free(tfont);
	    }
#endif
            *fontpath = buf;
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
