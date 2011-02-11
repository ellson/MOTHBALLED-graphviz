#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <pango/pangocairo.h>

#define WIDTH 300
#define HEIGHT 300
#define DEFAULT_FONT_FAMILY "Helvetica"
#define DEFAULT_FONT_SIZE 16.
#define DEFAULT_TEXT "ABCgjpqyXYZ"
#define FONT_DPI 96

static void draw_text(cairo_t *cr, char *text, char *font_family, double font_size){
 	static PangoFontMap *fontmap;
	static PangoContext *context;
	PangoLayout *layout;
	PangoFontDescription *desc;
	PangoRectangle logical_rect, ink_rect;
	PangoLayoutIter *iter;
	PangoFont *font;
	PangoFontMetrics *fontmetrics;
	cairo_font_options_t *options;
	const char *fontclass, *fontdesc;
	int baseline, ascent, descent;

	if (!context) {
		fontmap = pango_cairo_font_map_new();
		pango_cairo_font_map_set_resolution(PANGO_CAIRO_FONT_MAP(fontmap),FONT_DPI);
		context = pango_font_map_create_context (fontmap);
		options=cairo_font_options_create();
		cairo_font_options_set_antialias(options,CAIRO_ANTIALIAS_GRAY);
		cairo_font_options_set_hint_style(options,CAIRO_HINT_STYLE_FULL);
		cairo_font_options_set_hint_metrics(options,CAIRO_HINT_METRICS_ON);
		cairo_font_options_set_subpixel_order(options,CAIRO_SUBPIXEL_ORDER_BGR);
		pango_cairo_context_set_font_options(context, options);
//		pango_cairo_context_set_resolution(context, FONT_DPI);
		cairo_font_options_destroy(options);
		g_object_unref(fontmap);
	}
	
	layout = pango_layout_new(context);
	pango_layout_set_text(layout,text,-1);
	desc = pango_font_description_from_string(font_family);
	pango_font_description_set_size (desc, (gint)(font_size * PANGO_SCALE));

	font = pango_font_map_load_font(fontmap, context, desc);
	fontclass = G_OBJECT_CLASS_NAME(G_OBJECT_GET_CLASS(font));
        fontdesc = pango_font_description_to_string(pango_font_describe(font));
	fprintf(stderr,"%s: %s %s\n", font_family, fontclass, fontdesc);
        g_free((gpointer)fontdesc);

	pango_layout_set_font_description(layout,desc);
	pango_font_description_free(desc);

	/* draw text  - black */
	cairo_set_source_rgb(cr,0.0,0.0,0.0);
	pango_cairo_show_layout(cr,layout);
	
	/* draw logical_rect - purple */
	pango_layout_get_extents (layout, &ink_rect, &logical_rect);

	cairo_set_source_rgb(cr,1.0,0.0,1.0);
	cairo_rectangle(cr,
		logical_rect.x/PANGO_SCALE, logical_rect.y/PANGO_SCALE,
		logical_rect.width/PANGO_SCALE, logical_rect.height/PANGO_SCALE);
	cairo_stroke(cr);

	/* draw ink_rect - green */
	cairo_set_source_rgb(cr,0.0,1.0,0.0);
	cairo_rectangle(cr,
		ink_rect.x/PANGO_SCALE, ink_rect.y/PANGO_SCALE,
		ink_rect.width/PANGO_SCALE, ink_rect.height/PANGO_SCALE);
	cairo_stroke(cr);

	/* draw baseline - red */
	cairo_set_source_rgb(cr,1.0,0.0,0.0);
	iter = pango_layout_get_iter(layout);
	baseline = pango_layout_iter_get_baseline (iter);

	cairo_move_to(cr,logical_rect.y/PANGO_SCALE, baseline / PANGO_SCALE);
	cairo_rel_line_to(cr,logical_rect.width/PANGO_SCALE, 0);
	cairo_stroke(cr);

	/* draw ascent - yellow */
	cairo_set_source_rgb(cr,1.0,1.0,0.0);
	fontmetrics = pango_context_get_metrics(context, NULL, NULL);

	ascent = pango_font_metrics_get_ascent(fontmetrics);
#if 0  /* I don't understand this value */
	cairo_move_to(cr,logical_rect.y/PANGO_SCALE, (baseline - ascent) / PANGO_SCALE);
	cairo_rel_line_to(cr,logical_rect.width/PANGO_SCALE, 0);
	cairo_stroke(cr);
#endif

	/* draw descent - yellow */
	descent = pango_font_metrics_get_descent(fontmetrics);
#if 0  /* I don't understand this value */
	cairo_move_to(cr,logical_rect.y/PANGO_SCALE, (baseline + descent) / PANGO_SCALE);
	cairo_rel_line_to(cr,logical_rect.width/PANGO_SCALE, 0);
	cairo_stroke(cr);
#endif

	pango_font_metrics_unref(fontmetrics);

	g_object_unref(layout);
}

static cairo_status_t
writer (void *closure, const unsigned char *data, unsigned int length)
{
    if (length == fwrite((const char*)data, 1, length, (FILE*)closure))
	return CAIRO_STATUS_SUCCESS;
    return CAIRO_STATUS_WRITE_ERROR;
}

int
main (int argc, char *argv[])
{
	cairo_t *cr;
	cairo_status_t status;
	cairo_surface_t *surface;

	if ( argc > 1 && argv[1][0] == '-' ) {
		fprintf(stderr, "Usage: %s [text [font_family [font_size]]]\n", argv[0]);
		fprintf(stderr, "Result: a PNG image to stdout\n");
		return 0;
	}

	char *text = DEFAULT_TEXT;
	char *font_family = DEFAULT_FONT_FAMILY;
	double font_size = DEFAULT_FONT_SIZE;

	if (argc > 1) text = argv[1];
	if (argc > 2) font_family = argv[2];
	if (argc > 3) font_size = atof(argv[3]);

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,WIDTH,HEIGHT);
	cr = cairo_create(surface);

	/* draw axis - turquoise */
	cairo_set_source_rgb(cr,0.0,1.0,1.0);
	cairo_move_to(cr,WIDTH/10,0);
	cairo_rel_line_to(cr,0,HEIGHT);
	cairo_stroke(cr);

	cairo_move_to(cr,0,HEIGHT/10);
	cairo_rel_line_to(cr,WIDTH,0);
	cairo_stroke(cr);

	cairo_save(cr);
	/* position at 0,0 */
	cairo_translate(cr, WIDTH/10, HEIGHT/10);
	draw_text(cr, text, font_family, font_size);
	cairo_restore(cr);

	cairo_destroy(cr);

	status = cairo_surface_write_to_png_stream(surface, writer, stdout);
	cairo_surface_destroy(surface);
	return status;
}
