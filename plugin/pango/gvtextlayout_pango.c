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

/******************************************/
/* Based on snippet from fonts.c in pango sources */

typedef struct
{
  int value;
  const char str[16];
} FieldMap;

static const FieldMap style_map[] = {
  { PANGO_STYLE_NORMAL, "" },
  { PANGO_STYLE_OBLIQUE, "Oblique" },
  { PANGO_STYLE_ITALIC, "Italic" }
};

static const FieldMap variant_map[] = {
  { PANGO_VARIANT_NORMAL, "" },
  { PANGO_VARIANT_SMALL_CAPS, "Small-Caps" }
};

static const FieldMap stretch_map[] = {
  { PANGO_STRETCH_ULTRA_CONDENSED, "Ultra-Condensed" },
  { PANGO_STRETCH_EXTRA_CONDENSED, "Extra-Condensed" },
  { PANGO_STRETCH_CONDENSED,       "Condensed" },
  { PANGO_STRETCH_SEMI_CONDENSED,  "Semi-Condensed" },
  { PANGO_STRETCH_NORMAL,          "" },
  { PANGO_STRETCH_SEMI_EXPANDED,   "Semi-Expanded" },
  { PANGO_STRETCH_EXPANDED,        "Expanded" },
  { PANGO_STRETCH_EXTRA_EXPANDED,  "Extra-Expanded" },
  { PANGO_STRETCH_ULTRA_EXPANDED,  "Ultra-Expanded" }
};

static const FieldMap gravity_map[] = {
  { PANGO_GRAVITY_SOUTH, "Not-Rotated" },
  { PANGO_GRAVITY_SOUTH, "South" },
  { PANGO_GRAVITY_SOUTH, "Upside-Down" },
  { PANGO_GRAVITY_NORTH, "North" },
  { PANGO_GRAVITY_EAST,  "Rotated-Left" },
  { PANGO_GRAVITY_EAST,  "East" },
  { PANGO_GRAVITY_WEST,  "Rotated-Right" },
  { PANGO_GRAVITY_WEST,  "West" }
};

static const char*
string_field (const FieldMap *map, int n_elements, int val)
{
  int i;
  for (i=0; i<n_elements; i++)
      if (map[i].value == val)
	  return map[i].str;;
  return "";
}
/* Borrowed from pango/pangofc-fontmap.c */

static int
pango_fc_convert_weight_to_fc (PangoWeight pango_weight)
{
#ifdef FC_WEIGHT_ULTRABOLD
  /* fontconfig 2.1 only had light/medium/demibold/bold/black */
  if (pango_weight < (PANGO_WEIGHT_ULTRALIGHT + PANGO_WEIGHT_LIGHT) / 2)
    return FC_WEIGHT_ULTRALIGHT;
  else if (pango_weight < (PANGO_WEIGHT_LIGHT + PANGO_WEIGHT_NORMAL) / 2)
    return FC_WEIGHT_LIGHT;
  else if (pango_weight < (PANGO_WEIGHT_NORMAL + 500 /* PANGO_WEIGHT_MEDIUM */) / 2)
    return FC_WEIGHT_NORMAL;
  else if (pango_weight < (500 /* PANGO_WEIGHT_MEDIUM */ + PANGO_WEIGHT_SEMIBOLD) / 2)
    return FC_WEIGHT_MEDIUM;
  else if (pango_weight < (PANGO_WEIGHT_SEMIBOLD + PANGO_WEIGHT_BOLD) / 2)
    return FC_WEIGHT_DEMIBOLD;
  else if (pango_weight < (PANGO_WEIGHT_BOLD + PANGO_WEIGHT_ULTRABOLD) / 2)
    return FC_WEIGHT_BOLD;
  else if (pango_weight < (PANGO_WEIGHT_ULTRABOLD + PANGO_WEIGHT_HEAVY) / 2)
    return FC_WEIGHT_ULTRABOLD;
  else
    return FC_WEIGHT_BLACK;
#else  /* fontconfig < 2.2 */
  if (pango_weight < (PANGO_WEIGHT_LIGHT + PANGO_WEIGHT_NORMAL) / 2)
    return FC_WEIGHT_LIGHT;
  else if (pango_weight < (500 /* PANGO_WEIGHT_MEDIUM */ + PANGO_WEIGHT_SEMIBOLD) / 2)
    return FC_WEIGHT_MEDIUM;
  else if (pango_weight < (PANGO_WEIGHT_SEMIBOLD + PANGO_WEIGHT_BOLD) / 2)
    return FC_WEIGHT_DEMIBOLD;
  else if (pango_weight < (PANGO_WEIGHT_BOLD + PANGO_WEIGHT_ULTRABOLD) / 2)
    return FC_WEIGHT_BOLD;
  else
    return FC_WEIGHT_BLACK;
#endif
}

/******************************************/

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
	    buf[0] = '\0';
	    if (psfnt)
		strcat(buf, "(ps) ");
	    strcat(buf, "\"");
	    strcat(buf, tfont);
	    strcat(buf, "\" ");

#ifdef HAVE_FONTCONFIG
	    FcPattern *pat, *match;
	    FcFontSet *fs;
	    FcResult result;
    
            if (! FcInit())
	        return FALSE;

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0])) 

	    pat = FcPatternBuild (0,
		FC_FAMILY, FcTypeString,
		    pango_font_description_get_family(tdesc),
		FC_STYLE, FcTypeString,
		    string_field( style_map, ARRAY_SIZE(style_map),
			pango_font_description_get_style(tdesc) ),
		FC_WEIGHT, FcTypeInteger,
		    pango_fc_convert_weight_to_fc(
			pango_font_description_get_weight(tdesc) ),
		(char *) 0);

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
