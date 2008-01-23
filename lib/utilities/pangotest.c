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

/* cc `pkg-config --cflags --libs pangocairo libpng12` pangotest.c -o pangotest */

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <pango/pangocairo.h>

#define DEFAULT_FONT_FAMILY "Arial"
#define DEFAULT_FONT_SIZE 32
#define ANTIALIAS



static void
die (const char *gasp)
{
    fprintf (stderr, "Error: %s\n", gasp);
    exit (1);
}



PangoLayout *  get_pango_layout (cairo_t	*cr,
		  char	*markup_text,
		  double	*width,
		  double	*height)
{
    PangoFontDescription *desc;
	PangoFontMap* fontmap;
	PangoContext *context;
    PangoLayout *layout;
    int pango_width, pango_height;
    char *text;
    PangoAttrList *attr_list;
	cairo_font_options_t* options;
	fontmap = pango_cairo_font_map_get_default();
	context = pango_cairo_font_map_create_context(PANGO_CAIRO_FONT_MAP(fontmap));
	options=cairo_font_options_create();
#ifdef ANTIALIAS
//	cairo_font_options_set_antialias(options,CAIRO_ANTIALIAS_GRAY);
#endif
	cairo_font_options_set_hint_style(options,CAIRO_HINT_STYLE_FULL);
	cairo_font_options_set_hint_metrics(options,CAIRO_HINT_METRICS_ON);
	cairo_font_options_set_subpixel_order(options,CAIRO_SUBPIXEL_ORDER_BGR);
//	pango_cairo_context_set_font_options(context, options);

	desc = pango_font_description_new();
	pango_font_description_set_family(desc, "CENTAUR.TTF");
    pango_font_description_set_size (desc, DEFAULT_FONT_SIZE * PANGO_SCALE);
//	pango_font_description_set_style    (desc,PANGO_STYLE_ITALIC);

    if (! pango_parse_markup (markup_text, -1, '\0', &attr_list, &text, NULL, NULL))
		die("pango_parse_markup");
	layout = pango_layout_new (context);
	pango_layout_set_text (layout, text, -1);
	pango_layout_set_font_description (layout, desc);
    pango_layout_set_attributes (layout, attr_list);
    pango_font_description_free (desc);

    if (width || height)
	pango_layout_get_size (layout, &pango_width, &pango_height);

    if (width)
	*width = (double) pango_width / PANGO_SCALE;

    if (height)
	*height = (double) pango_height / PANGO_SCALE;

    return layout;
}

cairo_status_t
writer (void *closure, const unsigned char *data, unsigned int length)
{
	int a;
	if (length == fwrite(data, 1, length, (FILE *)closure))
	{
		return CAIRO_STATUS_SUCCESS;

	}

	return CAIRO_STATUS_WRITE_ERROR;
}

int
pango_main(char* filename)
{
	cairo_t *cr;
    cairo_surface_t *surface;
    PangoLayout *layout;
    double width, height;
	FILE* output_file;
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,1400,60);
	cr = cairo_create(surface);
    cairo_rectangle(cr, 0, 0, 1400, 60);
   cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_fill(cr);
   cairo_set_source_rgb(cr, 0, 0, 0);


	layout = get_pango_layout (cr,"ABCDEFGHIJKLMNOPRSTUVYZ.abcdefghijklmnoprstuvsd", &width, &height);
	pango_cairo_show_layout(cr, layout);
	output_file = fopen(filename, "wb+");	
	cairo_surface_write_to_png_stream(surface, writer, output_file);
	g_object_unref (layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
	fclose(output_file);
	return 0;
}
