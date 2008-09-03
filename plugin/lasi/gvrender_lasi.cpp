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

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <LASi.h>


#include "gvplugin_render.h"
#include "ps.h"
#include "agxbuf.h"
#include "utils.h"

using namespace LASi;
using namespace std;

typedef enum { FORMAT_PS, FORMAT_PS2, FORMAT_EPS } format_type;

PostscriptDocument doc;

/* cat_libfile:
 * Write library files onto the given file pointer.
 * arglib is an NULL-terminated array of char*
 * Each non-trivial entry should be the name of a file to be included.
 * stdlib is an NULL-terminated array of char*
 * Each of these is a line of a standard library to be included.
 * If any item in arglib is the empty string, the stdlib is not used.
 * The stdlib is printed first, if used, followed by the user libraries.
 * We check that for web-safe file usage.
 */
static void cat_libfile(GVJ_t *job, const char **arglib, const char **stdlib)
{
    FILE *fp;
    const char **s, *bp, *p;
    int i;
    boolean use_stdlib = TRUE;

    /* check for empty string to turn off stdlib */
    if (arglib) {
	for (i = 0; use_stdlib && ((p = arglib[i])); i++) {
	    if (*p == '\0')
		use_stdlib = FALSE;
	}
    }
    if (use_stdlib)
	for (s = stdlib; *s; s++) {
	    doc.osBody() << *s << endl;
	}
    if (arglib) {
	for (i = 0; (p = arglib[i]) != 0; i++) {
	    if (*p == '\0')
		continue;	/* ignore empty string */
	    p = safefile(p);	/* make sure filename is okay */
	    if ((fp = fopen(p, "r"))) {
		while ((bp = Fgets(fp)))
		    doc.osBody() << bp;
		doc.osBody() << endl; /* append a newline just in case */
	    } else
		job->common->errorfn("can't open library file %s\n", p);
	}
    }
}

static void lasi_printpointf(GVJ_t * job, pointf p)
{
    doc.osBody() << p.x << ' ' << p.y << ' ' ;
}

static void lasi_printpointflist(GVJ_t * job, pointf A[], int n)
{
    for (int i = 0; i < n; i++) {
	lasi_printpointf(job, A[i]);
    }
}

static void lasi_begin_job(GVJ_t * job)
{
//    gvdevice_fputs(job, "%!PS-Adobe-3.0 EPSF-3.0\n");
//    gvdevice_printf(job, "%%%%Creator: %s version %s (%s)\n",
//	    job->common->info[0], job->common->info[1], job->common->info[2]);
//    gvdevice_printf(job, "%%%%For: %s\n", job->common->user);
}

static void lasi_end_job(GVJ_t * job)
{
//    gvdevice_fputs(job, "%%Trailer\n");
//    if (job->render.id != FORMAT_EPS)
//        gvdevice_printf(job, "%%%%Pages: %d\n", job->common->viewNum);
//    if (job->common->show_boxes == NULL)
//        if (job->render.id != FORMAT_EPS)
//	    gvdevice_printf(job, "%%%%BoundingBox: %d %d %d %d\n",
//	        job->boundingBox.LL.x, job->boundingBox.LL.y,
//	        job->boundingBox.UR.x, job->boundingBox.UR.y);
//    gvdevice_fputs(job, "end\nrestore\n");
//    gvdevice_fputs(job, "%%EOF\n");

doc.write(cout);

}

static void lasi_begin_graph(GVJ_t * job)
{
    for (int i = 0; ps_txt[i]; i++) {
        doc.osBody() << ps_txt[i] << endl;
    }
}

#if 0
static void lasi_begin_graph(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    setupLatin1 = FALSE;

    if (job->common->viewNum == 0) {
        gvdevice_printf(job, "%%%%Title: %s\n", obj->u.g->name);
    	if (job->render.id != FORMAT_EPS)
            gvdevice_fputs(job, "%%Pages: (atend)\n");
	else
	    gvdevice_fputs(job, "%%Pages: 1\n");
        if (job->common->show_boxes == NULL) {
    	    if (job->render.id != FORMAT_EPS)
                gvdevice_fputs(job, "%%BoundingBox: (atend)\n");
	    else
	        gvdevice_printf(job, "%%%%BoundingBox: %d %d %d %d\n",
	            job->pageBoundingBox.LL.x, job->pageBoundingBox.LL.y,
	            job->pageBoundingBox.UR.x, job->pageBoundingBox.UR.y);
	}
        gvdevice_fputs(job, "%%EndComments\nsave\n");
        /* include shape library */
        cat_preamble(job, job->common->lib);
	/* include epsf */
        epsf_define(job->output_file);
        if (job->common->show_boxes) {
            char* args[2];
            args[0] = job->common->show_boxes[0];
            args[1] = NULL;
            cat_libfile(job, NULL, args);
        }
    }
    isLatin1 = (GD_charset(obj->u.g) == CHAR_LATIN1);
    /* We always setup Latin1. The charset info is always output,
     * and installing it is cheap. With it installed, we can then
     * rely on ps_string to convert UTF-8 characters whose encoding
     * is in the range of Latin-1 into the Latin-1 equivalent and
     * get the expected PostScript output.
     */
    if (!setupLatin1) {
	gvdevice_fputs(job, "setupLatin1\n");	/* as defined in ps header */
	setupLatin1 = TRUE;
    }
    /*  Set base URL for relative links (for Distiller >= 3.0)  */
    if (obj->url)
	gvdevice_printf(job, "[ {Catalog} << /URI << /Base (%s) >> >>\n"
		"/PUT pdfmark\n", obj->url);
}

static void lasi_begin_layer(GVJ_t * job, char *layername, int layerNum, int numLayers)
{
    gvdevice_printf(job, "%d %d setlayer\n", layerNum, numLayers);
}

static void lasi_begin_page(GVJ_t * job)
{
    box pbr = job->pageBoundingBox;

    gvdevice_printf(job, "%%%%Page: %d %d\n",
	    job->common->viewNum + 1, job->common->viewNum + 1);
    if (job->common->show_boxes == NULL)
        gvdevice_printf(job, "%%%%PageBoundingBox: %d %d %d %d\n",
	    pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    gvdevice_printf(job, "%%%%PageOrientation: %s\n",
	    (job->rotation ? "Landscape" : "Portrait"));
    if (job->render.id == FORMAT_PS2)
        gvdevice_printf(job, "<< /PageSize [%d %d] >> setpagedevice\n",
            pbr.UR.x, pbr.UR.y);
    gvdevice_printf(job, "%d %d %d beginpage\n",
	    job->pagesArrayElem.x, job->pagesArrayElem.y, job->numPages);
    if (job->common->show_boxes == NULL)
        gvdevice_printf(job, "gsave\n%d %d %d %d boxprim clip newpath\n",
	    pbr.LL.x, pbr.LL.y, pbr.UR.x-pbr.LL.x, pbr.UR.y-pbr.LL.y);
    gvdevice_printf(job, "%g %g set_scale %d rotate %g %g translate\n",
	    job->scale.x, job->scale.y,
	    job->rotation,
	    job->translation.x, job->translation.y);

    /*  Define the size of the PS canvas  */
    if (job->render.id == FORMAT_PS2) {
	if (pbr.UR.x >= PDFMAX || pbr.UR.y >= PDFMAX)
	    job->common->errorfn("canvas size (%d,%d) exceeds PDF limit (%d)\n"
		  "\t(suggest setting a bounding box size, see dot(1))\n",
		  pbr.UR.x, pbr.UR.y, PDFMAX);
	gvdevice_printf(job, "[ /CropBox [%d %d %d %d] /PAGES pdfmark\n",
		pbr.LL.x, pbr.LL.y, pbr.UR.x, pbr.UR.y);
    }
}

static void lasi_end_page(GVJ_t * job)
{
    if (job->common->show_boxes) {
	gvdevice_fputs(job, "0 0 0 edgecolor\n");
	cat_libfile(job, NULL, job->common->show_boxes + 1);
    }
    /* the showpage is really a no-op, but at least one PS processor
     * out there needs to see this literal token.  endpage does the real work.
     */
    gvdevice_fputs(job, "endpage\nshowpage\ngrestore\n");
    gvdevice_fputs(job, "%%PageTrailer\n");
    gvdevice_printf(job, "%%%%EndPage: %d\n", job->common->viewNum);
}

static void lasi_begin_cluster(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    gvdevice_printf(job, "%% %s\n", obj->u.sg->name);

    gvdevice_fputs(job, "gsave\n");
}

static void lasi_end_cluster(GVJ_t * job)
{
    gvdevice_fputs(job, "grestore\n");
}

static void lasi_begin_node(GVJ_t * job)
{
    gvdevice_fputs(job, "gsave\n");
}

static void lasi_end_node(GVJ_t * job)
{
    gvdevice_fputs(job, "grestore\n");
}

static void
lasi_begin_edge(GVJ_t * job)
{
    gvdevice_fputs(job, "gsave\n");
}

static void lasi_end_edge(GVJ_t * job)
{
    gvdevice_fputs(job, "grestore\n");
}

static void lasi_begin_anchor(GVJ_t *job, char *url, char *tooltip, char *target)
{
    obj_state_t *obj = job->obj;

    if (url && obj->url_map_p) {
        gvdevice_fputs(job, "[ /Rect [ ");
	gvdevice_printpointflist(job, obj->url_map_p, 2);
        gvdevice_fputs(job, " ]\n");
        gvdevice_printf(job, "  /Border [ 0 0 0 ]\n"
		"  /Action << /Subtype /URI /URI %s >>\n"
		"  /Subtype /Link\n"
		"/ANN pdfmark\n",
		ps_string(url, isLatin1));
    }
}

static void
ps_set_pen_style(GVJ_t *job)
{
    double penwidth = job->obj->penwidth;
    char *p, *line, **s = job->obj->rawstyle;

    gvdevice_printnum(job, penwidth);
    gvdevice_fputs(job," setlinewidth\n");

    while (s && (p = line = *s++)) {
	if (strcmp(line, "setlinewidth") == 0)
	    continue;
	while (*p)
	    p++;
	p++;
	while (*p) {
            gvdevice_printf(job,"%s ", p);
	    while (*p)
		p++;
	    p++;
	}
	if (strcmp(line, "invis") == 0)
	    job->obj->penwidth = 0;
	gvdevice_printf(job, "%s\n", line);
    }
}
#endif

static void ps_set_color(GVJ_t *job, gvcolor_t *color)
{
    const char *objtype;

    if (color) {
	switch (job->obj->type) {
	    case ROOTGRAPH_OBJTYPE:
	    case CLUSTER_OBJTYPE:
		objtype = "graph";
		break;
	    case NODE_OBJTYPE:
		objtype = "node";
		break;
	    case EDGE_OBJTYPE:
		objtype = "edge";
		break;
	    default:
		objtype = "sethsb";
		break;
	}
//	gvdevice_printf(job, "%.3f %.3f %.3f %scolor\n",
//	    color->u.HSVA[0], color->u.HSVA[1], color->u.HSVA[2], objtype);
	doc.osBody() << color->u.HSVA[0] << ' ' << color->u.HSVA[1] << ' ' << color->u.HSVA[2] << ' ' << objtype << "color" << endl;
    }
}

static void lasi_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    char *str;

    if (job->obj->pencolor.u.HSVA[3] < .5)
	return;  /* skip transparent text */

    ps_set_color(job, &(job->obj->pencolor));
//    gvdevice_printnum(job, para->fontsize);
//    gvdevice_printf(job, " /%s set_font\n", para->fontname);
    doc.osBody() << setFont(para->fontname) << setFontSize(para->fontsize) << endl;
//    str = ps_string(para->str,isLatin1);
    switch (para->just) {
    case 'r':
        p.x -= para->width;
        break;
    case 'l':
        p.x -= 0.0;
        break;
    case 'n':
    default:
        p.x -= para->width / 2.0;
        break;
    }
    p.y += para->yoffset_centerline;
//    gvdevice_printpointf(job, p);
//    gvdevice_fputs(job, " moveto ");
    lasi_printpointf(job, p);
    doc.osBody() << "moveto" << endl;
//    gvdevice_printnum(job, para->width);
//    gvdevice_printf(job, " %s alignedtext\n", str);
    doc.osBody() << show(para->str) << endl;

}

static void lasi_ellipse(GVJ_t * job, pointf * A, int filled)
{
    /* A[] contains 2 points: the center and corner. */
    pointf AA[2];

    AA[0] = A[0];
    AA[1].x = A[1].x - A[0].x;
    AA[1].y = A[1].y - A[0].y;

    if (filled && job->obj->fillcolor.u.HSVA[3] > .5) {
	ps_set_color(job, &(job->obj->fillcolor));
//	gvdevice_printpointflist(job, AA, 2);
//	gvdevice_fputs(job, " ellipse_path fill\n");
        lasi_printpointflist(job, AA, 2);
        doc.osBody() << "ellipse_path fill" << endl;
    }
    if (job->obj->pencolor.u.HSVA[3] > .5) {
//	ps_set_pen_style(job);
	ps_set_color(job, &(job->obj->pencolor));
//	gvdevice_printpointflist(job, AA, 2);
//	gvdevice_fputs(job, " ellipse_path stroke\n");
        lasi_printpointflist(job, AA, 2);
        doc.osBody() << "ellipse_path stroke" << endl;
    }
}

static void
lasi_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
	     int arrow_at_end, int filled)
{
    int j;

    if (filled && job->obj->fillcolor.u.HSVA[3] > .5) {
	ps_set_color(job, &(job->obj->fillcolor));
//	gvdevice_fputs(job, "newpath ");
//	gvdevice_printpointf(job, A[0]);
//	gvdevice_fputs(job, " moveto\n");
	doc.osBody() << "newpath ";
	lasi_printpointf(job, A[0]);
	doc.osBody() << "moveto" << endl;
	for (j = 1; j < n; j += 3) {
//	    gvdevice_printpointflist(job, &A[j], 3);
//	    gvdevice_fputs(job, " curveto\n");
	    lasi_printpointflist(job, &A[j], 3);
	    doc.osBody() << "curveto" << endl;
	}
//	gvdevice_fputs(job, "closepath fill\n");
	doc.osBody() << "closepath fill" << endl;
    }
    if (job->obj->pencolor.u.HSVA[3] > .5) {
//	ps_set_pen_style(job);
	ps_set_color(job, &(job->obj->pencolor));
//	gvdevice_fputs(job, "newpath ");
//	gvdevice_printpointf(job, A[0]);
//	gvdevice_fputs(job, " moveto\n");
	doc.osBody() << "newpath ";
	lasi_printpointf(job, A[0]);
	doc.osBody() << "moveto" << endl;
	for (j = 1; j < n; j += 3) {
//	    gvdevice_printpointflist(job, &A[j], 3);
//	    gvdevice_fputs(job, " curveto\n");
	    lasi_printpointflist(job, &A[j], 3);
	    doc.osBody() << "curveto" << endl;
	}
//	gvdevice_fputs(job, "stroke\n");
	doc.osBody() << "stroke" << endl;
    }
}

static void lasi_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    int j;

    if (filled && job->obj->fillcolor.u.HSVA[3] > .5) {
	ps_set_color(job, &(job->obj->fillcolor));
//	gvdevice_fputs(job, "newpath ");
//	gvdevice_printpointf(job, A[0]);
//	gvdevice_fputs(job, " moveto\n");
	doc.osBody() << "newpath ";
	lasi_printpointf(job, A[0]);
	doc.osBody() << "moveto" << endl;
	for (j = 1; j < n; j++) {
//	    gvdevice_printpointf(job, A[j]);
//	    gvdevice_fputs(job, " lineto\n");
	    lasi_printpointf(job, A[j]);
	    doc.osBody() << "lineto" << endl;
        }
//	gvdevice_fputs(job, "closepath fill\n");
	doc.osBody() << "closepath fill" << endl;
    }
    if (job->obj->pencolor.u.HSVA[3] > .5) {
//	ps_set_pen_style(job);
	ps_set_color(job, &(job->obj->pencolor));
//	gvdevice_fputs(job, "newpath ");
//	gvdevice_printpointf(job, A[0]);
//	gvdevice_fputs(job, " moveto\n");
	doc.osBody() << "newpath ";
	lasi_printpointf(job, A[0]);
	doc.osBody() << "moveto" << endl;
        for (j = 1; j < n; j++) {
//	    gvdevice_printpointf(job, A[j]);
//	    gvdevice_fputs(job, " lineto\n");
	    lasi_printpointf(job, A[j]);
	    doc.osBody() << "lineto" << endl;
	}
//	gvdevice_fputs(job, "closepath stroke\n");
	doc.osBody() << "closepath stroke" << endl;
    }
}

static void lasi_polyline(GVJ_t * job, pointf * A, int n)
{
    int j;

    if (job->obj->pencolor.u.HSVA[3] > .5) {
//	ps_set_pen_style(job);
	ps_set_color(job, &(job->obj->pencolor));
//	gvdevice_fputs(job, "newpath ");
//	gvdevice_printpointf(job, A[0]);
//	gvdevice_fputs(job, " moveto\n");
	doc.osBody() << "newpath ";
	lasi_printpointf(job, A[0]);
	doc.osBody() << "moveto" << endl;
        for (j = 1; j < n; j++) {
//	    gvdevice_printpointf(job, A[j]);
//	    gvdevice_fputs(job, " lineto\n");
	    lasi_printpointf(job, A[j]);
	    doc.osBody() << "lineto" << endl;
	}
//	gvdevice_fputs(job, "stroke\n");
	doc.osBody() << "stroke" << endl;
    }
}

static void lasi_comment(GVJ_t * job, char *str)
{
//    gvdevice_fputs(job, "% ");
//    gvdevice_fputs(job, str);
//    gvdevice_fputs(job, "\n");
    doc.osBody() << "% " << str << endl;
}

#if 0
static void lasi_library_shape(GVJ_t * job, char *name, pointf * A, int n, int filled)
{
    if (filled && job->obj->fillcolor.u.HSVA[3] > .5) {
	ps_set_color(job, &(job->obj->fillcolor));
	gvdevice_fputs(job, "[ ");
        gvdevice_printpointflist(job, A, n);
        gvdevice_fputs(job, " ");
        gvdevice_printpointf(job, A[0]);
	gvdevice_printf(job, " ]  %d true %s\n", n, name);
    }
    if (job->obj->pencolor.u.HSVA[3] > .5) {
        ps_set_pen_style(job);
        ps_set_color(job, &(job->obj->pencolor));
        gvdevice_fputs(job, "[ ");
        gvdevice_printpointflist(job, A, n);
        gvdevice_fputs(job, " ");
        gvdevice_printpointf(job, A[0]);
        gvdevice_printf(job, " ]  %d false %s\n", n, name);
    }
}
#endif

static gvrender_engine_t lasi_engine = {
    lasi_begin_job,
    lasi_end_job,
    lasi_begin_graph,
    0,				/* lasi_end_graph */
0,//    lasi_begin_layer,
    0,				/* lasi_end_layer */
0,//    lasi_begin_page,
0,//    lasi_end_page,
0,//    lasi_begin_cluster,
0,//    lasi_end_cluster,
    0,				/* lasi_begin_nodes */
    0,				/* lasi_end_nodes */
    0,				/* lasi_begin_edges */
    0,				/* lasi_end_edges */
0,//    lasi_begin_node,
0,//    lasi_end_node,
0,//    lasi_begin_edge,
0,//    lasi_end_edge,
0,//    lasi_begin_anchor,
    0,				/* lasi_end_anchor */
    lasi_textpara,
    0,				/* lasi_resolve_color */
    lasi_ellipse,
    lasi_polygon,
    lasi_bezier,
    lasi_polyline,
    lasi_comment,
0,//    lasi_library_shape,
};

static gvrender_features_t render_features_lasi = {
    GVRENDER_DOES_TRANSFORM
	| GVRENDER_DOES_MAPS
	| GVRENDER_NO_BG
	| GVRENDER_DOES_MAP_RECTANGLE,
    4.,                         /* default pad - graph units */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    HSVA_DOUBLE,		/* color_type */
};

static gvdevice_features_t device_features_ps = {
    GVDEVICE_DOES_PAGES
	| GVDEVICE_DOES_LAYERS,	/* flags */
    {36.,36.},			/* default margin - points */
    {612.,792.},                /* default page width, height - points */
    {72.,72.},			/* default dpi */
};

static gvdevice_features_t device_features_eps = {
    0,				/* flags */
    {36.,36.},			/* default margin - points */
    {612.,792.},                /* default page width, height - points */
    {72.,72.},			/* default dpi */
};

gvplugin_installed_t gvrender_lasi_types[] = {
    {FORMAT_PS, "lasi", 1, &lasi_engine, &render_features_lasi},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_lasi_types[] = {
    {FORMAT_PS, "ps:lasi", -5, NULL, &device_features_ps},
    {FORMAT_PS2, "ps2:lasi", -5, NULL, &device_features_ps},
    {FORMAT_EPS, "eps:lasi", -5, NULL, &device_features_eps},
    {0, NULL, 0, NULL, NULL}
};
