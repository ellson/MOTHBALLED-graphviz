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
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gvplugin_loadimage.h"

extern void svggen_fputs(GVJ_t * job, char *s);
extern void svggen_printf(GVJ_t * job, const char *format, ...);

extern void epsf_emit_body(usershape_t *us, FILE *of);
extern shape_desc *find_user_shape(char *name);

typedef enum {
    FORMAT_PNG_SVG, FORMAT_GIF_SVG, FORMAT_JPEG_SVG,
    FORMAT_PS_PS,
} format_type;

static void core_loadimage_svg(GVJ_t * job, usershape_t *us, boxf b, bool filled)
{
    if (us->name) {
        svggen_fputs(job, "<image xlink:href=\"");
        svggen_fputs(job, us->name);
        if (job->rotation) {
            svggen_printf (job, "\" width=\"%gpx\" height=\"%gpx\" preserveAspectRatio=\"xMidYMid meet\" x=\"%g\" y=\"%g\"",
                b.UR.y - b.LL.y, b.UR.x - b.LL.x, b.LL.x, b.UR.y);
            svggen_printf (job, " transform=\"rotate(%d %g %g)\"",
                job->rotation, b.LL.x, b.UR.y);
        }
        else {
            svggen_printf (job, "\" width=\"%gpx\" height=\"%gpx\" preserveAspectRatio=\"xMidYMid meet\" x=\"%g\" y=\"%g\"",
                b.UR.x - b.LL.x, b.UR.y - b.LL.y, b.LL.x, b.LL.y);
        }
        svggen_fputs(job, "/>\n");
    }
}

static void ps_freeimage(usershape_t *us)
{
    munmap(us->data, us->datasize);
}

/* usershape described by a postscript function */
static void core_loadimage_ps(GVJ_t * job, usershape_t *us, boxf b, bool filled)
{
    FILE *out = job->output_file;
    int j;
    pointf AF[4];

    if (!us->f) {
        if (find_user_shape(us->name)) {
	    AF[0] = b.LL;
	    AF[2] = b.UR;
	    AF[1].x = AF[0].x;
	    AF[1].y = AF[2].y;
	    AF[3].x = AF[2].x;
	    AF[3].y = AF[0].y;
            if (filled) {
//                ps_begin_context();
//                ps_set_color(S[SP].fillcolor);
                fprintf(out, "[ ");
                for (j = 0; j < 4; j++)
                    fprintf(out, "%g %g ", AF[j].x, AF[j].y);
                fprintf(out, "%g %g ", AF[0].x, AF[0].y);
                fprintf(out, "]  %d true %s\n", 4, us->name);
//                ps_end_context();
            }
            fprintf(out, "[ ");
            for (j = 0; j < 4; j++)
                fprintf(out, "%g %g ", AF[j].x, AF[j].y);
            fprintf(out, "%g %g ", AF[0].x, AF[0].y);
            fprintf(out, "]  %d false %s\n", 4, us->name);
        }
        return;
    }

    if (us->data) {
        if (us->datafree != (void*)ps_freeimage) {
            us->datafree(us);        /* free incompatible cache data */
            us->data = NULL;
            us->datafree = NULL;
            us->datasize = 0;
        }
    }

    if (!us->data) { /* read file into cache */
        int fd = fileno(us->f);
	struct stat statbuf;

        fseek(us->f, 0, SEEK_SET);
        switch (us->type) {
            case FT_PS:
            case FT_EPS:
		fstat(fd, &statbuf);
		us->datasize = statbuf.st_size;
		us->data = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
		us->must_inline = true;
                break;
            default:
                break;
        }
        if (us->data)
            us->datafree = (void*)ps_freeimage;
    }

    if (us->data) {
        fprintf(out, "gsave %g %g translate newpath\n",
		b.LL.x - us->x, b.LL.y - us->y);
        if (us->must_inline)
            epsf_emit_body(us, out);
        else
            fprintf(out, "user_shape_%d\n", us->macro_id);
        fprintf(out, "grestore\n");
    }
}

static gvloadimage_engine_t engine_svg = {
    core_loadimage_svg
};

static gvloadimage_engine_t engine_ps = {
    core_loadimage_ps
};

gvplugin_installed_t gvloadimage_core_types[] = {
    {FORMAT_PNG_SVG, "png2svg", 1, &engine_svg, NULL},
    {FORMAT_GIF_SVG, "gif2svg", 1, &engine_svg, NULL},
    {FORMAT_JPEG_SVG, "jpeg2svg", 1, &engine_svg, NULL},
    {FORMAT_PS_PS, "ps2ps", 1, &engine_ps, NULL},
    {0, NULL, 0, NULL, NULL}
};
