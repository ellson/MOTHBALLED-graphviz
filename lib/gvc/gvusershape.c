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

#include <stddef.h>
#include <string.h>

#include "types.h"
#include "graph.h"
#include "agxbuf.h"
#include "utils.h"

static Dict_t *ImageDict;

typedef struct imagerec_s {
    Dtlink_t link;
    usershape_t us;
} imagerec_t;

typedef struct {
    char *template;
    int size, type;
} knowntype_t;

#define HDRLEN 20

#define PNG_MAGIC  "\x89PNG\x0D\x0A\x1A\x0A"
#define PS_MAGIC   "%!PS-Adobe-"
#define BMP_MAGIC  "BM"
#define GIF_MAGIC  "GIF8"
#define JPEG_MAGIC "\xFF\xD8\xFF\xE0"
#define PDF_MAGIC  "%PDF-"
#define EPS_MAGIC  "\xC5\xD0\xD3\xC6"

knowntype_t knowntypes[] = {
    { PNG_MAGIC,  sizeof(PNG_MAGIC)-1,  FT_PNG,  },
    { PS_MAGIC,   sizeof(PS_MAGIC)-1,   FT_PS,   },
    { BMP_MAGIC,  sizeof(BMP_MAGIC)-1,  FT_BMP,  },
    { GIF_MAGIC,  sizeof(GIF_MAGIC)-1,  FT_GIF,  },
    { JPEG_MAGIC, sizeof(JPEG_MAGIC)-1, FT_JPEG, },
    { PDF_MAGIC,  sizeof(PDF_MAGIC)-1,  FT_PDF,  },
    { EPS_MAGIC,  sizeof(EPS_MAGIC)-1,  FT_EPS,  },
};

static int imagetype (usershape_t *us)
{
    char header[HDRLEN];
    int i;

    if (fread(header, 1, HDRLEN, us->f) != HDRLEN)
	return -1;
    for (i = 0; i < sizeof(knowntypes) / sizeof(knowntype_t); i++) {
	if (!memcmp (header, knowntypes[i].template, knowntypes[i].size))
	    return (us->type = knowntypes[i].type);
    }
    return -1;
}
    
static bool get_int_lsb_first (FILE *f, unsigned int sz, unsigned int *val)
{
    int ch, i;

    *val = 0;
    for (i = 0; i < sz; i++) {
	ch = fgetc(f);
	if (feof(f))
	    return false;
	*val |= (ch << 8*i);
    }
    return true;
}
	
static bool get_int_msb_first (FILE *f, unsigned int sz, unsigned int *val)
{
    int ch, i;

    *val = 0;
    for (i = 0; i < sz; i++) {
	ch = fgetc(f);
	if (feof(f))
	    return false;
        *val <<= 8;
	*val |= ch;
    }
    return true;
}

static void png_size (usershape_t *us)
{
    unsigned int w, h;

    us->dpi = DEFAULT_DPI;
    fseek(us->f, 16, SEEK_SET);
    if (get_int_msb_first(us->f, 4, &w) && get_int_msb_first(us->f, 4, &h)) {
        us->w = w;
        us->h = h;
    }
}

static void gif_size (usershape_t *us)
{
    unsigned int w, h;

    us->dpi = DEFAULT_DPI;
    fseek(us->f, 6, SEEK_SET);
    if (get_int_lsb_first(us->f, 2, &w) && get_int_lsb_first(us->f, 2, &h)) {
        us->w = w;
        us->h = h;
    }
}

static void bmp_size (usershape_t *us) {
    unsigned int size_x_msw, size_x_lsw, size_y_msw, size_y_lsw;

    us->dpi = DEFAULT_DPI;
    fseek (us->f, 16, SEEK_SET);
    if ( get_int_lsb_first (us->f, 2, &size_x_msw) &&
         get_int_lsb_first (us->f, 2, &size_x_lsw) &&
         get_int_lsb_first (us->f, 2, &size_y_msw) &&
         get_int_lsb_first (us->f, 2, &size_y_lsw) ) {
        us->w = size_x_msw << 16 | size_x_lsw;
        us->h = size_y_msw << 16 | size_y_lsw;
    }
}

static void jpeg_size (usershape_t *us) {
    unsigned int marker, length, size_x, size_y, junk;

    /* These are the markers that follow 0xff in the file.
     * Other markers implicitly have a 2-byte length field that follows.
     */
    static unsigned char standalone_markers [] = {
        0x01,                       /* Temporary */
        0xd0, 0xd1, 0xd2, 0xd3,     /* Reset */
            0xd4, 0xd5, 0xd6,
            0xd7,
        0xd8,                       /* Start of image */
        0xd9,                       /* End of image */
        0
    };

    us->dpi = DEFAULT_DPI;
    while (true) {
        /* Now we must be at a 0xff or at a series of 0xff's.
         * If that is not the case, or if we're at EOF, then there's
         * a parsing error.
         */
        if (! get_int_msb_first (us->f, 1, &marker))
            return;

        if (marker == 0xff)
            continue;

        /* Ok.. marker now read. If it is not a stand-alone marker,
         * then continue. If it's a Start Of Frame (0xc?), then we're there.
         * If it's another marker with a length field, then skip ahead
         * over that length field.
         */

        /* A stand-alone... */
        if (strchr ((char*)standalone_markers, marker))
            continue;

        /* Incase of a 0xc0 marker: */
        if (marker == 0xc0) {
            /* Skip length and 2 lengths. */
            if ( get_int_msb_first (us->f, 3, &junk)   &&
                 get_int_msb_first (us->f, 2, &size_x) &&
                 get_int_msb_first (us->f, 2, &size_y) ) {

            /* Store length. */
                us->h = size_x;
                us->w = size_y;
            }
	    return;
        }

        /* Incase of a 0xc2 marker: */
        if (marker == 0xc2) {
            /* Skip length and one more byte */
            if (! get_int_msb_first (us->f, 3, &junk))
                return;

            /* Get length and store. */
            if ( get_int_msb_first (us->f, 2, &size_x) &&
                 get_int_msb_first (us->f, 2, &size_y) ) {
                us->h = size_x;
                us->w = size_y;
            }
	    return;
        }

        /* Any other marker is assumed to be followed by 2 bytes length. */
        if (! get_int_msb_first (us->f, 2, &length))
            return;

        fseek (us->f, length - 2, SEEK_CUR);
    }
}

static void ps_size (usershape_t *us)
{
    char line[BUFSIZ];
    bool saw_bb;
    int lx, ly, ux, uy;

    us->dpi = POINTS_PER_INCH;
    fseek(us->f, 0, SEEK_SET);
    saw_bb = false;
    while (fgets(line, sizeof(line), us->f)) {
        if (sscanf (line, "%%%%BoundingBox: %d %d %d %d", &lx, &ly, &ux, &uy) == 4) {
            saw_bb = true;
	    break;
        }
    }
    if (saw_bb) {
        us->w = ux - lx;
        us->h = uy - ly;
    }
}

static void imagerec_close (Dict_t * dict, Void_t * p, Dtdisc_t * disc)
{
    imagerec_t *val = (imagerec_t *)p;

    if (val->us.f)
	fclose(val->us.f);
    if (val->us.data && val->us.datafree)
	val->us.datafree(val->us.data);
    free (val);
}

static Dtdisc_t ImageDictDisc = {
    offsetof(imagerec_t, us.name), /* key */
    -1,                         /* size */
    0,                          /* link offset */
    NIL(Dtmake_f),
    imagerec_close,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

static imagerec_t *imagerec_open (char *name)
{
    imagerec_t probe, *val;
    usershape_t *us;
    char *fn;

    if (!ImageDict)
        ImageDict = dtopen(&ImageDictDisc, Dttree);

    probe.us.name = name;
    val = dtsearch(ImageDict, &probe);
    if (!val) {
        val = malloc(sizeof(imagerec_t));
        if (!val)
	    return NULL;
	us = &(val->us);
        us->name = name;
        us->w = us->h = 0;
	us->data = us->datafree = NULL;

	if ((fn = safefile(name))) {
#ifndef MSWIN32
	    us->f = fopen(fn, "r");
#else
	    us->f = fopen(fn, "rb");
#endif
	}
        if (us->f) {
            switch(imagetype(us)) {
	        case FT_GIF:
	            gif_size(us);
	            break;
	        case FT_PNG:
	            png_size(us);
	            break;
	        case FT_BMP:
	            bmp_size(us);
	            break;
	        case FT_JPEG:
	            jpeg_size(us);
	            break;
	        case FT_PS:
	            ps_size(us);
	            break;
	        case FT_PDF:   /* no pdf_size code available */
	        case FT_EPS:   /* no eps_size code available */
	        default:
	            break;
            }
        }
        dtinsert(ImageDict, val);
    }

    return val;
}

point gvusershape_size(graph_t * g, char *name)
{
    point rv;
    imagerec_t *val;
    double dpi;

    dpi = GD_drawing(g)->dpi;
    if (dpi < 1.0)
        dpi = POINTS_PER_INCH;

    /* no shape file, no shape size */
    if (!name || (*name == '\0')) {
        rv.x = rv.y = -1;
        return rv;
    }
    if (!strncasecmp(name, "http://", 7)) {
        rv.x = rv.y = 0;
        return rv;              /* punt on obvious web addresses */
    }

    if ((val = imagerec_open (name))) {
	rv.x = val->us.w * dpi / val->us.dpi;
	rv.y = val->us.h * dpi / val->us.dpi;
    }
    else {
        rv.x = rv.y = -1;
    }

    return rv;
}

usershape_t *gvusershape_find(char *name)
{
    imagerec_t probe, *val;

    probe.us.name = name;
    val = dtsearch(ImageDict, &probe);
    if (!val)
	return NULL;
    return &(val->us);
}
