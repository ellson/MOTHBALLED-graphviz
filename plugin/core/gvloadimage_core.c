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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#if HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#include "gvplugin_loadimage.h"

/* for n->name */
#include "graph.h"

extern void core_loadimage_xdot(GVJ_t*, usershape_t*, boxf, boolean);
extern void epsf_emit_body(usershape_t *us, FILE *of);
extern shape_desc *find_user_shape(char *name);

typedef enum {
    FORMAT_PNG_SVG, FORMAT_GIF_SVG, FORMAT_JPEG_SVG,
    FORMAT_PNG_XDOT, FORMAT_GIF_XDOT, FORMAT_JPEG_XDOT, FORMAT_PS_XDOT, FORMAT_SVG_XDOT,
    FORMAT_PNG_FIG, FORMAT_GIF_FIG, FORMAT_JPEG_FIG,
    FORMAT_PNG_VRML, FORMAT_GIF_VRML, FORMAT_JPEG_VRML,
    FORMAT_PS_PS, FORMAT_PSLIB_PS, FORMAT_SVG_SVG,
    FORMAT_PNG_DOT, FORMAT_GIF_DOT, FORMAT_JPEG_DOT, FORMAT_PS_DOT, FORMAT_SVG_DOT,
} format_type;

static void core_loadimage_svg(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
    assert(job);
    assert(us);
    assert(us->name);

    gvdevice_fputs(job, "<image xlink:href=\"");
    gvdevice_fputs(job, us->name);
    if (job->rotation) {
        gvdevice_printf (job, "\" width=\"%gpx\" height=\"%gpx\" preserveAspectRatio=\"xMidYMid meet\" x=\"%g\" y=\"%g\"",
            b.UR.y - b.LL.y, b.UR.x - b.LL.x, b.LL.x, b.UR.y);
        gvdevice_printf (job, " transform=\"rotate(%d %g %g)\"",
            job->rotation, b.LL.x, b.UR.y);
    }
    else {
        gvdevice_printf (job, "\" width=\"%gpx\" height=\"%gpx\" preserveAspectRatio=\"xMinYMin meet\" x=\"%g\" y=\"%g\"",
            b.UR.x - b.LL.x, b.UR.y - b.LL.y, b.LL.x, -b.UR.y);
    }
    gvdevice_fputs(job, "/>\n");
}

static void core_loadimage_fig(GVJ_t * job, usershape_t *us, boxf bf, boolean filled)
{
    int object_code = 2;        /* always 2 for polyline */
    int sub_type = 5;           /* always 5 for image */
    int line_style = 0;		/* solid, dotted, dashed */
    int thickness = 0;
    int pen_color = 0;
    int fill_color = -1;
    int depth = 1;
    int pen_style = -1;         /* not used */
    int area_fill = 0;
    double style_val = 0.0;
    int join_style = 0;
    int cap_style = 0;
    int radius = 0;
    int forward_arrow = 0;
    int backward_arrow = 0;
    int npoints = 5;
    int flipped = 0;

    box b;

    assert(job);
    assert(us);
    assert(us->name);

    BF2B(bf, b);

    gvdevice_printf(job, "%d %d %d %d %d %d %d %d %d %.1f %d %d %d %d %d %d\n %d %s\n",
            object_code, sub_type, line_style, thickness, pen_color,
            fill_color, depth, pen_style, area_fill, style_val, join_style,
            cap_style, radius, forward_arrow, backward_arrow, npoints,
            flipped, us->name);
    gvdevice_printf(job," %d %d %d %d %d %d %d %d %d %d\n",
	    b.LL.x, b.LL.y,
	    b.LL.x, b.UR.y,
	    b.UR.x, b.UR.y,
	    b.UR.x, b.LL.y,
	    b.LL.x, b.LL.y);
}

static void core_loadimage_vrml(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
    FILE *out;
    obj_state_t *obj;
    node_t *n;

    assert(job);

    out = job->output_file;
    obj = job->obj;
    assert(out);
    assert(obj);

    assert(us);
    assert(us->name);

    n = job->obj->u.n;
    assert(n);

    fprintf(out, "Shape {\n");
    fprintf(out, "  appearance Appearance {\n");
    fprintf(out, "    material Material {\n");
    fprintf(out, "      ambientIntensity 0.33\n");
    fprintf(out, "        diffuseColor 1 1 1\n");
    fprintf(out, "    }\n");
    fprintf(out, "    texture ImageTexture { url \"%s\" }\n", us->name);
    fprintf(out, "  }\n");
    fprintf(out, "}\n");
}

static void ps_freeimage(usershape_t *us)
{
#if HAVE_SYS_MMAN_H
    munmap(us->data, us->datasize);
#else
    free(us->data);
#endif
}

/* usershape described by a postscript file */
static void core_loadimage_ps(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
    FILE *out;

    assert(job);
    assert(us);
    assert(us->name);

    out = job->output_file;
    assert(out);

    if (us->data) {
        if (us->datafree != ps_freeimage) {
            us->datafree(us);        /* free incompatible cache data */
            us->data = NULL;
            us->datafree = NULL;
            us->datasize = 0;
        }
    }

    if (!us->data) { /* read file into cache */
        int fd;
	struct stat statbuf;

	if (!gvusershape_file_access(us))
	    return;
	fd = fileno(us->f);
        switch (us->type) {
            case FT_PS:
            case FT_EPS:
		fstat(fd, &statbuf);
		us->datasize = statbuf.st_size;
#if HAVE_SYS_MMAN_H
		us->data = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
#else
		us->data = malloc(statbuf.st_size);
		read(fd, us->data, statbuf.st_size);
#endif
		us->must_inline = TRUE;
                break;
            default:
                break;
        }
        if (us->data)
            us->datafree = ps_freeimage;
	gvusershape_file_release(us);
    }

    if (us->data) {
        fprintf(out, "gsave %g %g translate newpath\n",
		b.LL.x - (double)(us->x), b.LL.y - (double)(us->y));
        if (us->must_inline)
            epsf_emit_body(us, out);
        else
            fprintf(out, "user_shape_%d\n", us->macro_id);
        fprintf(out, "grestore\n");
    }
}

/* usershape described by a member of a postscript library */
static void core_loadimage_pslib(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
    int i;
    pointf AF[4];
    FILE *out;
    shape_desc *shape;

    assert(job);
    assert(us);
    assert(us->name);

    out = job->output_file;
    assert(out);

    if ((shape = (shape_desc*)us->data)) {
	AF[0] = b.LL;
	AF[2] = b.UR;
	AF[1].x = AF[0].x;
	AF[1].y = AF[2].y;
	AF[3].x = AF[2].x;
	AF[3].y = AF[0].y;
        if (filled) {
//            ps_begin_context();
//            ps_set_color(S[SP].fillcolor);
            fprintf(out, "[ ");
            for (i = 0; i < 4; i++)
                fprintf(out, "%g %g ", AF[i].x, AF[i].y);
            fprintf(out, "%g %g ", AF[0].x, AF[0].y);
            fprintf(out, "]  %d true %s\n", 4, us->name);
//            ps_end_context();
        }
        fprintf(out, "[ ");
        for (i = 0; i < 4; i++)
            fprintf(out, "%g %g ", AF[i].x, AF[i].y);
        fprintf(out, "%g %g ", AF[0].x, AF[0].y);
        fprintf(out, "]  %d false %s\n", 4, us->name);
    }
}

void core_loadimage_dot(GVJ_t *gvc, usershape_t *us, boxf b, boolean filled)
{
    /* null function - basically suppress the missing loader message */
}

static gvloadimage_engine_t engine_svg = {
    core_loadimage_svg
};

static gvloadimage_engine_t engine_fig = {
    core_loadimage_fig
};

static gvloadimage_engine_t engine_vrml = {
    core_loadimage_vrml
};

static gvloadimage_engine_t engine_ps = {
    core_loadimage_ps
};

static gvloadimage_engine_t engine_pslib = {
    core_loadimage_pslib
};

static gvloadimage_engine_t engine_dot = {
    core_loadimage_dot
};

static gvloadimage_engine_t engine_xdot = {
    core_loadimage_xdot
};

gvplugin_installed_t gvloadimage_core_types[] = {
    {FORMAT_PNG_SVG, "png:svg", 1, &engine_svg, NULL},
    {FORMAT_GIF_SVG, "gif:svg", 1, &engine_svg, NULL},
    {FORMAT_JPEG_SVG, "jpeg:svg", 1, &engine_svg, NULL},
    {FORMAT_JPEG_SVG, "jpe:svg", 1, &engine_svg, NULL},
    {FORMAT_JPEG_SVG, "jpg:svg", 1, &engine_svg, NULL},

    {FORMAT_PNG_FIG, "png:fig", 1, &engine_fig, NULL},
    {FORMAT_GIF_FIG, "gif:fig", 1, &engine_fig, NULL},
    {FORMAT_JPEG_FIG, "jpeg:fig", 1, &engine_fig, NULL},
    {FORMAT_JPEG_FIG, "jpe:fig", 1, &engine_fig, NULL},
    {FORMAT_JPEG_FIG, "jpg:fig", 1, &engine_fig, NULL},

    {FORMAT_PNG_VRML, "png:vrml", 1, &engine_vrml, NULL},
    {FORMAT_GIF_VRML, "gif:vrml", 1, &engine_vrml, NULL},
    {FORMAT_JPEG_VRML, "jpeg:vrml", 1, &engine_vrml, NULL},
    {FORMAT_JPEG_VRML, "jpe:vrml", 1, &engine_vrml, NULL},
    {FORMAT_JPEG_VRML, "jpg:vrml", 1, &engine_vrml, NULL},

    {FORMAT_PS_PS, "ps:ps", 1, &engine_ps, NULL},
    {FORMAT_PSLIB_PS, "(lib):ps", 1, &engine_pslib, NULL},  /* for pslib */

    {FORMAT_PNG_DOT, "png:dot", 1, &engine_dot, NULL},
    {FORMAT_GIF_DOT, "gif:dot", 1, &engine_dot, NULL},
    {FORMAT_JPEG_DOT, "jpeg:dot", 1, &engine_dot, NULL},
    {FORMAT_JPEG_DOT, "jpe:dot", 1, &engine_dot, NULL},
    {FORMAT_JPEG_DOT, "jpg:dot", 1, &engine_dot, NULL},
    {FORMAT_PS_DOT, "ps:dot", 1, &engine_dot, NULL},
    {FORMAT_SVG_DOT, "svg:dot", 1, &engine_dot, NULL},

    {FORMAT_PNG_XDOT, "png:xdot", 1, &engine_xdot, NULL},
    {FORMAT_GIF_XDOT, "gif:xdot", 1, &engine_xdot, NULL},
    {FORMAT_JPEG_XDOT, "jpeg:xdot", 1, &engine_xdot, NULL},
    {FORMAT_JPEG_XDOT, "jpe:xdot", 1, &engine_xdot, NULL},
    {FORMAT_JPEG_XDOT, "jpg:xdot", 1, &engine_xdot, NULL},
    {FORMAT_PS_XDOT, "ps:xdot", 1, &engine_xdot, NULL},
    {FORMAT_SVG_XDOT, "svg:xdot", 1, &engine_xdot, NULL},

    {FORMAT_SVG_SVG, "svg:svg", 1, &engine_svg, NULL},
    {0, NULL, 0, NULL, NULL}
};
