/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#include "config.h"

#include <stdlib.h>

#include "gvplugin_loadimage.h"
#include "gvio.h"

#ifdef HAVE_PANGOCAIRO
#include <cairo.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkcairo.h>

#ifdef WIN32 //*dependencies
    #pragma comment( lib, "gvc.lib" )
    #pragma comment( lib, "glib-2.0.lib" )
    #pragma comment( lib, "cairo.lib" )
    #pragma comment( lib, "gobject-2.0.lib" )
    #pragma comment( lib, "graph.lib" )
    #pragma comment( lib, "gdk-pixbuf.lib" )
#endif

typedef enum {
    FORMAT_BMP_CAIRO,
    FORMAT_JPEG_CAIRO,
    FORMAT_PNG_CAIRO,
    FORMAT_ICO_CAIRO,
    FORMAT_TIFF_CAIRO,
} format_type;

#if 0
// FIXME - should be using a stream reader
static cairo_status_t
reader (void *closure, unsigned char *data, unsigned int length)
{
    if (length == fread(data, 1, length, (FILE *)closure)
     || feof((FILE *)closure))
        return CAIRO_STATUS_SUCCESS;
    return CAIRO_STATUS_READ_ERROR;
}
#endif

static void gdk_freeimage(usershape_t *us)
{
    g_object_unref((GdkPixbuf*)(us->data));
}

static GdkPixbuf* gdk_loadimage(GVJ_t * job, usershape_t *us)
{
    GdkPixbuf *image = NULL;

    assert(job);
    assert(us);
    assert(us->name);

    if (us->data) {
        if (us->datafree == gdk_freeimage)
             image = (GdkPixbuf*)(us->data); /* use cached data */
        else {
             us->datafree(us);        /* free incompatible cache data */
             us->datafree = NULL;
             us->data = NULL;
        }
    }
    if (!image) { /* read file into cache */
	if (!gvusershape_file_access(us))
	    return NULL;
        switch (us->type) {
            case FT_PNG:
            case FT_JPEG:
            case FT_BMP:
            case FT_ICO:
            case FT_TIFF:
                // FIXME - should be using a stream reader
                image = gdk_pixbuf_new_from_file(us->name, NULL);
                break;
            default:
                image = NULL;
        }
        if (image) {
            us->data = (void*)image;
            us->datafree = gdk_freeimage;
        }
	gvusershape_file_release(us);
    }
    return image;
}

static void gdk_loadimage_cairo(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
    cairo_t *cr = (cairo_t *) job->context; /* target context */
    GdkPixbuf *image;

    image = gdk_loadimage(job, us);
    if (image) {
        cairo_save(cr);
	cairo_translate(cr, b.LL.x, -b.UR.y);
	cairo_scale(cr, (b.UR.x - b.LL.x)/(us->w), (b.UR.y - b.LL.y)/(us->h)); 
        gdk_cairo_set_source_pixbuf (cr, image, 0, 0);
        cairo_paint (cr);
        cairo_restore(cr);
    }
}

static gvloadimage_engine_t engine_gdk = {
    gdk_loadimage_cairo
};

#endif

gvplugin_installed_t gvloadimage_gdk_types[] = {
#ifdef HAVE_PANGOCAIRO
    {FORMAT_BMP_CAIRO,  "bmp:cairo", 1, &engine_gdk, NULL},
    {FORMAT_JPEG_CAIRO, "jpe:cairo", 2, &engine_gdk, NULL},
    {FORMAT_JPEG_CAIRO, "jpg:cairo", 2, &engine_gdk, NULL},
    {FORMAT_JPEG_CAIRO, "jpeg:cairo", 2, &engine_gdk, NULL},
    {FORMAT_PNG_CAIRO,  "png:cairo", -1, &engine_gdk, NULL},
    {FORMAT_ICO_CAIRO,  "ico:cairo", 1, &engine_gdk, NULL},
//    {FORMAT_TIFF_CAIRO, "tif:cairo", 1, &engine_gdk, NULL},
//    {FORMAT_TIFF_CAIRO, "tiff`:cairo", 1, &engine_gdk, NULL},
#endif
    {0, NULL, 0, NULL, NULL}
};
