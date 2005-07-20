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

#include                "gvc.h"
#include                "gd.h"

static Dict_t *ImageDict;

static gdImagePtr loadshapeimage(char *name)
{
    gdImagePtr rv = 0;
    char *shapeimagefile, *suffix;
    FILE *in = NULL;

    if ((shapeimagefile = safefile(name))) {
#ifndef MSWIN32
	in = fopen(shapeimagefile, "r");
#else
	in = fopen(shapeimagefile, "rb");
#endif
    }
    if (!in)
	agerr(AGERR, "couldn't open image file %s\n", shapeimagefile);
    else {
	suffix = strrchr(shapeimagefile, '.');
	if (!suffix)
	    suffix = shapeimagefile;
	else
	    suffix++;
	if (!strcasecmp(suffix, "wbmp"))
	    rv = gdImageCreateFromWBMP(in);
#ifdef HAVE_GD_GIF
	else if (!strcasecmp(suffix, "gif"))
	    rv = gdImageCreateFromGif(in);
#endif
#ifdef HAVE_GD_JPEG
	else if (!strcasecmp(suffix, "jpeg") || !strcasecmp(suffix, "jpg"))
	    rv = gdImageCreateFromJpeg(in);
#endif
#ifdef HAVE_GD_PNG
	else if (!strcasecmp(suffix, "png"))
	    rv = gdImageCreateFromPng(in);
#endif
#ifdef HAVE_GD_XPM
	else if (!strcasecmp(suffix, "xbm"))
	    rv = gdImageCreateFromXbm(in);
#endif
	else
	    agerr(AGERR, "image file %s suffix not recognized\n", name);
	fclose(in);
	if (!rv)
	    agerr(AGERR, "image file %s contents were not recognized\n",
		  name);
    }
    return rv;
}

typedef struct imagerec_s {
    Dtlink_t link;
    char *name;
    gdImagePtr im;
} imagerec_t;


static void imagerec_free(Dict_t * dict, Void_t * p, Dtdisc_t * disc)
{
    gdImagePtr im = ((imagerec_t *) p)->im;

    if (im)
	gdImageDestroy(im);
}

static Dtdisc_t ImageDictDisc = {
    offsetof(imagerec_t, name),	/* key */
    -1,				/* size */
    0,				/* link offset */
    NIL(Dtmake_f),
    imagerec_free,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

gdImagePtr gd_getshapeimage(char *name)
{
    imagerec_t probe, *val;
    if (!name)
	return 0;		/* cdt does not like NULL keys */
    if (!ImageDict)
	ImageDict = dtopen(&ImageDictDisc, Dttree);
    probe.name = name;
    val = dtsearch(ImageDict, &probe);
    if (!val) {
	val = GNEW(imagerec_t);
	val->name = name;
	val->im = loadshapeimage(name);
	dtinsert(ImageDict, val);
    }
    return val->im;
}

void gd_freeusershapes(void)
{
    if (ImageDict) {
	dtclose(ImageDict);
	ImageDict = 0;
    }
}

point gd_image_size(graph_t * g, char *shapeimagefile)
{
    point rv;
    gdImagePtr im;
    double dpi;

    dpi = GD_drawing(g)->dpi;
    if (dpi < 1.0)
	dpi = DEFAULT_DPI;
    im = gd_getshapeimage(shapeimagefile);
    if (im) {
	rv.x = im->sx * POINTS_PER_INCH / dpi;
	rv.y = im->sy * POINTS_PER_INCH / dpi;
    } else
	rv.x = rv.y = -1;
    return rv;
}
