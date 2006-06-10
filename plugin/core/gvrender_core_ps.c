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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#ifdef HAVE_LIBZ
#include <zlib.h>
#ifdef MSWIN32
#include <io.h>
#endif
#endif

#undef HAVE_LIBGD

#include "textpara.h"
#include "gvplugin_render.h"
#include "gv_ps.h"
#include "const.h"
#include "macros.h"
#ifdef HAVE_LIBGD
#include "gd.h"
#endif

extern void cat_libfile(FILE * ofp, char **arglib, char **stdlib);
extern void epsf_define(FILE * of);
extern char *ps_string(char *ins, int latin);

typedef enum { FORMAT_PS, FORMAT_PS2, } format_type;

static box DBB;
static int isLatin1;
static char setupLatin1;

static char *last_fontname;
static double last_fontsize;
static gvcolor_t last_color;

static void psgen_begin_job(GVJ_t * job)
{
    last_fontname = NULL;
    last_fontsize = 0.0;
    last_color.u.HSV[0] = 0;
    last_color.u.HSV[1] = 0;
    last_color.u.HSV[2] = 0;

    fprintf(job->output_file, "%%!PS-Adobe-2.0\n");
    fprintf(job->output_file, "%%%%Creator: %s version %s (%s)\n",
	    job->common->info[0], job->common->info[1], job->common->info[2]);
    fprintf(job->output_file, "%%%%For: %s\n", job->common->user);
}

static void psgen_end_job(GVJ_t * job)
{
    fprintf(job->output_file, "%%%%Trailer\n");
    fprintf(job->output_file, "%%%%Pages: %d\n", job->common->viewNum);
    if (job->common->show_boxes == NULL)
	fprintf(job->output_file, "%%%%BoundingBox: %d %d %d %d\n",
	    DBB.LL.x, DBB.LL.y, DBB.UR.x, DBB.UR.y);
    fprintf(job->output_file, "end\nrestore\n");
    fprintf(job->output_file, "%%%%EOF\n");
}

static void psgen_begin_graph(GVJ_t * job, char *graphname)
{
    setupLatin1 = FALSE;

    if (job->common->viewNum == 0) {
        fprintf(job->output_file, "%%%%Title: %s\n", graphname);
        fprintf(job->output_file, "%%%%Pages: (atend)\n");
        if (job->common->show_boxes == NULL)
            fprintf(job->output_file, "%%%%BoundingBox: (atend)\n");
        fprintf(job->output_file, "%%%%EndComments\nsave\n");
        cat_libfile(job->output_file, job->common->lib, gv_ps_txt);
        epsf_define(job->output_file);
    }
#ifdef FIXME
    isLatin1 = (GD_charset(g) == CHAR_LATIN1);
    if (isLatin1 && !setupLatin1) {
	fprintf(job->output_file, "setupLatin1\n");	/* as defined in ps header */
	setupLatin1 = TRUE;
    }
#endif
#ifdef FIXME
    /*  Set base URL for relative links (for Distiller >= 3.0)  */
    if (((s = agget(g, "href")) && s[0])
	|| ((s = agget(g, "URL")) && s[0])) {
	    fprintf(job->output_file, "[ {Catalog} << /URI << /Base (%s) >> >>\n"
		    "/PUT pdfmark\n", s);
    }
#endif
}

static void psgen_end_graph(GVJ_t * job)
{
#if 0
    if (EPSF_contents) {
	dtclose(EPSF_contents);
	EPSF_contents = 0;
	N_EPSF_files = 0;
    }
    onetime = FALSE;
#endif
}

static void psgen_begin_layer(GVJ_t * job, char *layername, int layerNum, int numLayers)
{
    fprintf(job->output_file, "%d %d setlayer\n", layerNum, numLayers);
}

static point sub_points(point p0, point p1)
{
    p0.x -= p1.x;
    p0.y -= p1.y;
    return p0;
}

static void psgen_begin_page(GVJ_t * job)
{
    point sz;
    box PB, pbr;

    BF2B(job->boundingBox, PB);

    sz = sub_points(PB.UR, PB.LL);
    if (job->rotation) {
	pbr.LL.x = PB.LL.y;
	pbr.LL.y = PB.LL.x;
	pbr.UR.x = PB.UR.y;
	pbr.UR.y = PB.UR.x;
    }
    else {
	pbr = PB;
    }

    if (job->common->viewNum == 0)
	DBB = pbr;
    else
	EXPANDBB(DBB, pbr);

    fprintf(job->output_file, "%%%%Page: %d %d\n",
	    job->common->viewNum + 1, job->common->viewNum + 1);
    if (job->common->show_boxes == NULL)
        fprintf(job->output_file, "%%%%PageBoundingBox: %d %d %d %d\n",
	    pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    fprintf(job->output_file, "%%%%PageOrientation: %s\n",
	    (job->rotation ? "Landscape" : "Portrait"));
    if (job->common->show_boxes == NULL)
        fprintf(job->output_file, "gsave\n%d %d %d %d boxprim clip newpath\n",
	    pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    fprintf(job->output_file, "gsave %g set_scale\n", job->zoom);
    if (job->rotation) {
	fprintf(job->output_file, "%d rotate\n", job->rotation);
	fprintf(job->output_file, "%g %g translate\n",
		   -job->pageBox.LL.x + pbr.LL.y / job->zoom,
		   -job->pageBox.UR.y - pbr.LL.x / job->zoom);
    }
    else
	fprintf(job->output_file, "%g %g translate\n",
		    -job->pageBox.LL.x + pbr.LL.x / job->zoom,
		    -job->pageBox.LL.y + pbr.LL.y / job->zoom);

#if 0
    /*  Define the size of the PS canvas  */
    if (Output_lang == PDF) {
	if (PB.UR.x >= PDFMAX || PB.UR.y >= PDFMAX)
	    agerr(AGWARN,
		  "canvas size (%d,%d) exceeds PDF limit (%d)\n"
		  "\t(suggest setting a bounding box size, see dot(1))\n",
		  PB.UR.x, PB.UR.y, PDFMAX);
	fprintf(job->output_file, "[ /CropBox [%d %d %d %d] /PAGES pdfmark\n",
		PB.LL.x, PB.LL.y, PB.UR.x, PB.UR.y);
    }
#endif
}

static void psgen_end_page(GVJ_t * job)
{
    if (job->common->show_boxes)
	cat_libfile(job->output_file, NULL, job->common->show_boxes + 1);
    /* the showpage is really a no-op, but at least one PS processor
     * out there needs to see this literal token.  endpage does the real work.
     */
    fprintf(job->output_file, "endpage\nshowpage\ngrestore\n");
    fprintf(job->output_file, "%%%%PageTrailer\n");
    fprintf(job->output_file, "%%%%EndPage: %d\n", job->common->viewNum);
}

static void psgen_begin_cluster(GVJ_t * job, char *clustername, long id)
{
    fprintf(job->output_file, "%% %s\n", clustername);

#if 0
    /*  Embed information for Distiller to generate hyperlinked PDF  */
    map_begin_cluster(g);
#endif
}

static void psgen_begin_node(GVJ_t * job, char *nodename, long id)
{
#if 0
    /*  Embed information for Distiller to generate hyperlinked PDF  */
    map_begin_node(n);
#endif
}

static void
psgen_begin_edge(GVJ_t * job, char *tailname, bool directed,
		 char *headname, long id)
{
#if 0
    /* Embed information for Distiller, so it can generate hyperactive PDF  */
    map_begin_edge(e);
#endif
}

static void
psgen_begin_anchor(GVJ_t * job, char *href, char *tooltip, char *target)
{
}

static void psgen_end_anchor(GVJ_t * job)
{
}

static void
ps_set_pen_style(GVJ_t *job)
{
#if 0
    double penwidth = job->style->penwidth * job->zoom;
    char *p, *line, **s = job->rawstyle;

    fprintf(stderr,"%g setlinewidth\n", penwidth);

    while (s && (p = line = *s++)) {
	if (streq(line, "setlinewidth"))
	    continue;
	while (*p)
	    p++;
	p++;
	while (*p) {
            fprintf(stderr,"%s ", p);
	    while (*p)
		p++;
	    p++;
	}
//	if (streq(line, "invis"))
//	    job->style->width = 0;
	fprintf(stderr, "%s\n", line);
    }
#endif
}

static void ps_set_color(GVJ_t *job, gvcolor_t *color)
{
    if (color) {
	if ( last_color.u.HSV[0] != color->u.HSV[0]
	  || last_color.u.HSV[1] != color->u.HSV[1]
	  || last_color.u.HSV[2] != color->u.HSV[2]) {
	    fprintf(job->output_file, "%.3f %.3f %.3f %scolor\n",
		color->u.HSV[0], color->u.HSV[1], color->u.HSV[2], job->objname);
	    last_color.u.HSV[0] = color->u.HSV[0];
	    last_color.u.HSV[1] = color->u.HSV[1];
	    last_color.u.HSV[2] = color->u.HSV[2];
	}
    }
}

static void psgen_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    double adj, sz;
    char *str;

    ps_set_color(job, &(job->style->pencolor));
    if (para->fontname) {
	sz = para->fontsize;
        if (sz != last_fontsize
          || last_fontname == NULL
	  || strcmp(para->fontname, last_fontname) != 0) {
	    fprintf(job->output_file, "%.2f /%s set_font\n", sz, para->fontname);
	    last_fontsize = sz;
	    last_fontname = para->fontname;
	}
    }

    str = ps_string(para->str,isLatin1);
    if (para->xshow) {
        switch (para->just) {
        case 'l':
            break;
        case 'r':
            p.x -= para->width;
            break;
        default:
        case 'n':
            p.x -= para->width / 2;
            break;
        }
        fprintf(job->output_file, "%g %g moveto\n%s\n[%s]\nxshow\n",
                p.x, p.y, str, para->xshow);
    } else {
        switch (para->just) {
        case 'l':
            adj = 0.0;
            break;
        case 'r':
            adj = -1.0;
            break;
        default:
        case 'n':
            adj = -0.5;
            break;
        }
        fprintf(job->output_file, "%g %g moveto %1f %.1f %s alignedtext\n",
                p.x, p.y, para->width, adj, str);
    }
}

static void psgen_ellipse(GVJ_t * job, pointf * A, int filled)
{
    /* A[] contains 2 points: the center and corner. */

    if (filled) {
	ps_set_color(job, &(job->style->fillcolor));
	fprintf(job->output_file, "%g %g %g %g ellipse_path fill\n",
	    A[0].x, A[0].y, fabs(A[1].x - A[0].x), fabs(A[1].y - A[0].y));
    }
    ps_set_pen_style(job);
    ps_set_color(job, &(job->style->pencolor));
    fprintf(job->output_file, "%g %g %g %g ellipse_path stroke\n",
	A[0].x, A[0].y, fabs(A[1].x - A[0].x), fabs(A[1].y - A[0].y));
}

static void
psgen_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
	     int arrow_at_end, int filled)
{
    int j;

    if (filled) {
	ps_set_color(job, &(job->style->fillcolor));
	fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
	for (j = 1; j < n; j += 3)
	    fprintf(job->output_file, "%g %g %g %g %g %g curveto\n",
		A[j].x, A[j].y, A[j + 1].x, A[j + 1].y, A[j + 2].x,
		A[j + 2].y);
	fprintf(job->output_file, "closepath fill\n");
    }
    ps_set_pen_style(job);
    ps_set_color(job, &(job->style->pencolor));
    fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
    for (j = 1; j < n; j += 3)
	fprintf(job->output_file, "%g %g %g %g %g %g curveto\n",
		A[j].x, A[j].y, A[j + 1].x, A[j + 1].y, A[j + 2].x,
		A[j + 2].y);
    fprintf(job->output_file, "stroke\n");
}

static void psgen_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    int j;

    if (filled) {
	ps_set_color(job, &(job->style->fillcolor));
	fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
	for (j = 1; j < n; j++)
	    fprintf(job->output_file, "%g %g lineto\n", A[j].x, A[j].y);
	fprintf(job->output_file, "closepath fill\n");
    }
    ps_set_pen_style(job);
    ps_set_color(job, &(job->style->pencolor));
    fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
    for (j = 1; j < n; j++)
	fprintf(job->output_file, "%g %g lineto\n", A[j].x, A[j].y);
    fprintf(job->output_file, "closepath stroke\n");
}

static void psgen_polyline(GVJ_t * job, pointf * A, int n)
{
    int j;

    ps_set_pen_style(job);
    ps_set_color(job, &(job->style->pencolor));
    fprintf(job->output_file, "newpath %g %g moveto\n", A[0].x, A[0].y);
    for (j = 1; j < n; j++)
	fprintf(job->output_file, "%g %g lineto\n", A[j].x, A[j].y);
    fprintf(job->output_file, "stroke\n");
}

static void psgen_comment(GVJ_t * job, char *str)
{
    fprintf(job->output_file, "%% %s\n", str);
}

static void ps_freeimage_gd (void *data)
{
#ifdef HAVE_LIBGD
    gdImageDestroy((gdImagePtr)data);
#endif
}

static void ps_freeimage_ps (void *data)
{
#if 0
    free (data);
#endif
}

#ifdef HAVE_LIBGD
static void writePSBitmap (GVJ_t *job, gdImagePtr im, boxf b)
{
    int x, y, px;

    fprintf(job->output_file, "gsave\n");

    /* this sets the position of the image */
    fprintf(job->output_file, "%g %g translate %% lower-left coordinate\n", b.LL.x, b.LL.y);

    /* this sets the rendered size to fit the box */
    fprintf(job->output_file,"%g %g scale\n", b.UR.x - b.LL.x, b.UR.y - b.LL.y);

    /* xsize ysize bits-per-sample [matrix] */
    fprintf(job->output_file, "%d %d 8 [%d 0 0 %d 0 %d]\n", im->sx, im->sy,
                        im->sx, -(im->sy), im->sy);

    fprintf(job->output_file, "{<\n");
    for (y = 0; y < im->sy; y++) {
        for (x = 0; x < im->sx; x++) {
            if (im->trueColor) {
                px = gdImageTrueColorPixel(im, x, y);
                fprintf(job->output_file, "%02x%02x%02x",
                    gdTrueColorGetRed(px),
                    gdTrueColorGetGreen(px),
                    gdTrueColorGetBlue(px));
            }
            else {
                px = gdImagePalettePixel(im, x, y);
                fprintf(job->output_file, "%02x%02x%02x",
                    im->red[px],
                    im->green[px],
                    im->blue[px]);
            }
        }
        fprintf(job->output_file, "\n");
    }

    fprintf(job->output_file, ">}\n");
    fprintf(job->output_file, "false 3 colorimage\n");

    fprintf(job->output_file, "grestore\n");

}
#endif


/* ps_usershape:
 * Images for postscript are complicated by the old epsf shape, as
 * well as user-defined shapes using postscript code.
 * If the name is custom, we look for the image stored in the
 * current node's shapefile attribute.
 * Else we see if name is a user-defined postscript function
 * Else we assume name is the name of the image. This occurs when
 * the image is part of an html label.
 */
static void
psgen_usershape(GVJ_t * job, usershape_t *us, boxf b, bool filled)
{
    int j;
#ifdef HAVE_LIBGD
    gdImagePtr gd_img = NULL;
#endif
#ifdef XXX_PS
    ps_image_t *ps_img = NULL;
#endif
    point offset;

    if (!us->f) {
#ifdef XXX_PS
        if (find_user_shape(us->name)) {
            if (filled) {
                ps_begin_context();
                ps_set_color(S[SP].fillcolor);
                fprintf(job->output_file, "[ ");
                for (j = 0; j < n; j++)
                    fprintf(job->output_file, "%d %d ", A[j].x, A[j].y);
                fprintf(job->output_file, "%d %d ", A[0].x, A[0].y);
                fprintf(job->output_file, "]  %d true %s\n", n, us->name);
                ps_end_context();
            }
            fprintf(job->output_file, "[ ");
            for (j = 0; j < n; j++)
                fprintf(job->output_file, "%d %d ", A[j].x, A[j].y);
            fprintf(job->output_file, "%d %d ", A[0].x, A[0].y);
            fprintf(job->output_file, "]  %d false %s\n", n, us->name);
        }
        else {   /* name not find by find_ser_shape */  }
#endif
        return;
    }

    if (us->data) {
        if (us->datafree == ps_freeimage_gd) {
#ifdef HAVE_LIBGD
            gd_img = (gdImagePtr)(us->data);  /* use cached data */
#endif
        }
        else if (us->datafree == ps_freeimage_ps) {
#ifdef XXX_PS
            ps_img = (ps_image_t *)(us->data);  /* use cached data */
#endif
        }
        else {
            us->datafree(us->data);        /* free incompatible cache data */
            us->data = NULL;
        }
    }

#ifdef HAVE_LIBGD
#ifdef XXX_PS
    if (!ps_img && !gd_img) { /* read file into cache */
#else
    if (!gd_img) { /* read file into cache */
#endif
#else
#ifdef XXX_PS
    if (!ps_img) { /* read file into cache */
#else
    if (false) { /* nothing to do */
#endif
#endif
        fseek(us->f, 0, SEEK_SET);
        switch (us->type) {
#ifdef HAVE_LIBGD
#ifdef HAVE_GD_PNG
            case FT_PNG:
                gd_img = gdImageCreateFromPng(us->f);
                break;
#endif
#ifdef HAVE_GD_GIF
            case FT_GIF:
                gd_img = gdImageCreateFromGif(us->f);
                break;
#endif
#ifdef HAVE_GD_JPEG
            case FT_JPEG:
                gd_img = gdImageCreateFromJpeg(us->f);
                break;
#endif
#endif
#ifdef XXX_PS
            case FT_PS:
            case FT_EPS:
                ps_img = ps_usershape_to_image(us->name);
                break;
#endif
            default:
                break;
        }
#ifdef HAVE_LIBGD
        if (gd_img) {
            us->data = (void*)gd_img;
            us->datafree = ps_freeimage_gd;
        }
#endif
#ifdef XXX_PS
        if (ps_img) {
            us->data = (void*)ps_img;
            us->datafree = ps_freeimage_ps;
        }
#endif
    }

#ifdef XXX_PS
    if (ps_img) {
        ps_begin_context();
        offset.x = -ps_img->origin.x - (ps_img->size.x) / 2;
        offset.y = -ps_img->origin.y - (ps_img->size.y) / 2;
        fprintf(job->output_file, "%d %d translate newpath\n",
            ND_coord_i(Curnode).x + offset.x,
            ND_coord_i(Curnode).y + offset.y);
        if (ps_img->must_inline)
            epsf_emit_body(ps_img, job->output_file);
        else
            fprintf(job->output_file, "user_shape_%d\n", ps_img->macro_id);
        ps_end_context();
        return;
    }
#endif

#ifdef HAVE_LIBGD
    if (gd_img) {
        writePSBitmap (job, gd_img, b);
        return;
    }
#endif

#if 0
/* FIXME */
    /* some other type of image */
    agerr(AGERR, "usershape %s is not supported  in PostScript output\n", us->name);
#endif
}

static gvrender_engine_t psgen_engine = {
    psgen_begin_job,
    psgen_end_job,
    psgen_begin_graph,
    psgen_end_graph,
    psgen_begin_layer,
    0,				/* psgen_end_layer */
    psgen_begin_page,
    psgen_end_page,
    psgen_begin_cluster,
    0,				/* psgen_end_cluster */
    0,				/* psgen_begin_nodes */
    0,				/* psgen_end_nodes */
    0,				/* psgen_begin_edges */
    0,				/* psgen_end_edges */
    psgen_begin_node,
    0,				/* psgen_end_node */
    psgen_begin_edge,
    0,				/* psgen_end_edge */
    psgen_begin_anchor,
    psgen_end_anchor,
    psgen_textpara,
    0,				/* psgen_resolve_color */
    psgen_ellipse,
    psgen_polygon,
    psgen_bezier,
    psgen_polyline,
    psgen_comment,
    psgen_usershape
};

static gvrender_features_t psgen_features = {
    GVRENDER_DOES_MULTIGRAPH_OUTPUT_FILES
	| GVRENDER_DOES_TRANSFORM,
    DEFAULT_PRINT_MARGIN,	/* default margin - points */
    {72.,72.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    HSV_DOUBLE,			/* color_type */
};

gvplugin_installed_t gvrender_core_ps_types[] = {
    {FORMAT_PS, "ps", -2, &psgen_engine, &psgen_features},
    {FORMAT_PS2, "ps2", -2, &psgen_engine, &psgen_features},
    {0, NULL, 0, NULL, NULL}
};
