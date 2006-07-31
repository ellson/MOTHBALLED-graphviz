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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32
#include <io.h>
#endif

#include "macros.h"
#include "const.h"

#include "gvplugin_render.h"
#include "graph.h"
#include "agxbuf.h"
#include "utils.h"
#include "color.h"

/* Number of points to split splines into */
#define BEZIERSUBDIVISION 6

typedef enum { FORMAT_FIG, } format_type;

static int Depth;

void figgen_fputs(GVJ_t * job, char *s)
{
    int len;

    len = strlen(s);
    fwrite(s, sizeof(char), (unsigned) len, job->output_file);
}

/* figgen_printf:
 * Note that this function is unsafe due to the fixed buffer size.
 * It should only be used when the caller is sure the input will not
 * overflow the buffer. In particular, it should be avoided for
 * input coming from users. Also, if vsnprintf is available, the
 * code should check for return values to use it safely.
 */
void figgen_printf(GVJ_t * job, const char *format, ...)
{
    char buf[BUFSIZ];
    va_list argp;

    va_start(argp, format);
#ifdef HAVE_VSNPRINTF
    (void) vsnprintf(buf, sizeof(buf), format, argp);
#else
    (void) vsprintf(buf, format, argp);
#endif
    va_end(argp);

    figgen_fputs(job, buf);
}

static void figptarray(GVJ_t *job, pointf * A, int n, int close)
{
    int i;
    point p;

    for (i = 0; i < n; i++) {
	PF2P(A[i],p);
        figgen_printf(job, " %d %d", p.x, p.y);
    }
    if (close) {
	PF2P(A[0],p);
        figgen_printf(job, " %d %d", p.x, p.y);
    }
    figgen_fputs(job, "\n");
}

static char *fig_string(char *s)
{
    static char *buf = NULL;
    static int bufsize = 0;
    int pos = 0;
    char *p;
    unsigned char c;

    if (!buf) {
        bufsize = 64;
        buf = malloc(bufsize * sizeof(char));
    }

    p = buf;
    while ((c = *s++)) {
        if (pos > (bufsize - 8)) {
            bufsize *= 2;
            buf = realloc(buf, bufsize * sizeof(char));
            p = buf + pos;
        }
        if (isascii(c)) {
            if (c == '\\') {
                *p++ = '\\';
                pos++;
            }
            *p++ = c;
            pos++;
        } else {
            *p++ = '\\';
            sprintf(p, "%03o", c);
            p += 3;
            pos += 4;
        }
    }
    *p = '\0';
    return buf;
}

static int figColorResolve(int *new, int r, int g, int b)
{
#define maxColors 256
    static int top = 0;
    static short red[maxColors], green[maxColors], blue[maxColors];
    int c;
    int ct = -1;
    long rd, gd, bd, dist;
    long mindist = 3 * 255 * 255;       /* init to max poss dist */

    *new = 0;                   /* in case it is not a new color */
    for (c = 0; c < top; c++) {
        rd = (long) (red[c] - r);
        gd = (long) (green[c] - g);
        bd = (long) (blue[c] - b);
        dist = rd * rd + gd * gd + bd * bd;
        if (dist < mindist) {
            if (dist == 0)
                return c;       /* Return exact match color */
            mindist = dist;
            ct = c;
        }
    }
    /* no exact match.  We now know closest, but first try to allocate exact */
    if (top++ == maxColors)
        return ct;              /* Return closest available color */
    red[c] = r;
    green[c] = g;
    blue[c] = b;
    *new = 1;                   /* flag new color */
    return c;                   /* Return newly allocated color */
}

/* this table is in xfig color index order */
static char *figcolor[] = {
    "black", "blue", "green", "cyan", "red", "magenta", "yellow", "white", (char *) NULL
};

static void figgen_resolve_color(GVJ_t *job, gvcolor_t * color)
{
    int object_code = 0;        /* always 0 for color */
    int i, new;

    switch (color->type) {
	case COLOR_STRING:
	    for (i = 0; figcolor[i]; i++) {
		if (streq(figcolor[i], color->u.string)) {
		    color->u.index = i;
		    break;
		}
	    }
	    break;
	case RGBA_BYTE:
	    i = 32 + figColorResolve(&new,
			color->u.rgba[0],
			color->u.rgba[1],
			color->u.rgba[2]);
	    if (new)
		figgen_printf(job, "%d %d #%02x%02x%02x\n",
			object_code, i,
			color->u.rgba[0],
			color->u.rgba[1],
			color->u.rgba[2]);
	    color->u.index = i;
	    break;
	default:
	    assert(0);	/* internal error */
    }

    color->type = COLOR_INDEX;
}

static void figgen_line_style(gvstyle_t *style, int *line_style, double *style_val)
{
    switch (style->pen) {
	case PEN_DASHED: 
	    *line_style = 1;
	    *style_val = 10.;
	    break;
	case PEN_DOTTED:
	    *line_style = 2;
	    *style_val = 10.;
	    break;
	case PEN_SOLID:
	default:
	    *line_style = 0;
	    *style_val = 0.;
	    break;
    }
}

static void figgen_comment(GVJ_t *job, char *str)
{
    figgen_printf(job, "# %s\n", str);
}

static void figgen_begin_graph(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    figgen_fputs(job, "#FIG 3.2\n");
    figgen_printf(job, "# Generated by %s version %s (%s)\n",
	job->common->info[0], job->common->info[1], job->common->info[2]);
    figgen_printf(job, "# For: %s\n", job->common->user);
    figgen_printf(job, "# Title: %s\n", obj->u.g->name);
    figgen_printf(job, "# Pages: %d\n", job->pagesArraySize.x * job->pagesArraySize.y);
    figgen_fputs(job, "Portrait\n"); /* orientation */
    figgen_fputs(job, "Center\n");   /* justification */
    figgen_fputs(job, "Inches\n");   /* units */
    figgen_fputs(job, "Letter\n");   /* papersize */
    figgen_fputs(job, "100.00\n");   /* magnification % */
    figgen_fputs(job, "Single\n");   /* multiple-page */
    figgen_fputs(job, "-2\n");       /* transparent color (none) */
    figgen_fputs(job, "1200");	     /* resolution */
    figgen_fputs(job, " 2\n");       /* coordinate system (upper left) */
}

static void figgen_end_graph(GVJ_t * job)
{
    figgen_fputs(job, "# end of FIG file\n");
}

static void figgen_begin_page(GVJ_t * job)
{
    Depth = 2;
}

static void figgen_begin_node(GVJ_t * job)
{
    Depth = 1;
}

static void figgen_end_node(GVJ_t * job)
{
    Depth = 2;
}

static void figgen_begin_edge(GVJ_t * job)
{
    Depth = 0;
}

static void figgen_end_edge(GVJ_t * job)
{
    Depth = 2;
}

static void figgen_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    gvstyle_t *style = job->style;

    int object_code = 4;        /* always 4 for text */
    int sub_type = 0;           /* text justification */
    int color = style->pencolor.u.index;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int font = -1;		/* init to xfig's default font */
    double font_size = para->fontsize * job->zoom;
    double angle = job->rotation ? (PI / 2.0) : 0.0;
    int font_flags = 4;		/* PostScript font */
    double height = 0.0;
    double length = 0.0;

    if (para->postscript_alias) /* if it is a standard postscript font */
	font = para->postscript_alias->xfig_code; 

    switch (para->just) {
    case 'l':
        sub_type = 0;
        break;
    case 'r':
        sub_type = 2;
        break;
    default:
    case 'n':
        sub_type = 1;
        break;
    }

    figgen_printf(job,
            "%d %d %d %d %d %d %.1f %.4f %d %.1f %.1f %d %d %s\\001\n",
            object_code, sub_type, color, depth, pen_style, font,
            font_size, angle, font_flags, height, length, ROUND(p.x), ROUND(p.y),
            fig_string(para->str));
}

static void figgen_ellipse(GVJ_t * job, pointf * A, int filled)
{
    gvstyle_t *style = job->style;

    int object_code = 1;        /* always 1 for ellipse */
    int sub_type = 1;           /* ellipse defined by radii */
    int line_style;		/* solid, dotted, dashed */
    int thickness = style->penwidth;
    int pen_color = style->pencolor.u.index;
    int fill_color = style->fillcolor.u.index;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int area_fill = filled ? 20 : -1;
    double style_val;
    int direction = 0;
    double angle = 0.0;
    int center_x, center_y, radius_x, radius_y;
    int start_x, start_y, end_x, end_y;

    figgen_line_style(style, &line_style, &style_val);

    start_x = center_x = ROUND(A[0].x);
    start_y = center_y = ROUND(A[0].y);
    radius_x = ROUND(A[1].x - A[0].x);
    radius_y = ROUND(A[1].y - A[0].y);
    end_x = ROUND(A[1].x);
    end_y = ROUND(A[1].y);

    figgen_printf(job,
            "%d %d %d %d %d %d %d %d %d %.3f %d %.4f %d %d %d %d %d %d %d %d\n",
            object_code, sub_type, line_style, thickness, pen_color,
            fill_color, depth, pen_style, area_fill, style_val, direction,
            angle, center_x, center_y, radius_x, radius_y, start_x,
            start_y, end_x, end_y);
}

static void
figgen_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
	      int arrow_at_end, int filled)
{
    gvstyle_t *style = job->style;

    int object_code = 3;        /* always 3 for spline */
    int sub_type;
    int line_style;		/* solid, dotted, dashed */
    int thickness = style->penwidth;
    int pen_color = style->pencolor.u.index;
    int fill_color = style->fillcolor.u.index;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int area_fill;
    double style_val;
    int cap_style = 0;
    int forward_arrow = 0;
    int backward_arrow = 0;
    int npoints = n;
    int i;

    pointf pf, V[4];
    point p;
    int j, step;
    int count = 0;
    int size;

    char *buffer;
    char *buf;
    buffer =
        malloc((npoints + 1) * (BEZIERSUBDIVISION +
                                1) * 20 * sizeof(char));
    buf = buffer;

    figgen_line_style(style, &line_style, &style_val);

    if (filled) {
        sub_type = 5;     /* closed X-spline */
        area_fill = 20;   /* fully saturated color */
        fill_color = job->style->fillcolor.u.index;
    }
    else {
        sub_type = 4;     /* opened X-spline */
        area_fill = -1;
        fill_color = 0;
    }
    V[3].x = A[0].x;
    V[3].y = A[0].y;
    /* Write first point in line */
    count++;
    PF2P(A[0], p);
    size = sprintf(buf, " %d %d", p.x, p.y);
    buf += size;
    /* write subsequent points */
    for (i = 0; i + 3 < n; i += 3) {
        V[0] = V[3];
        for (j = 1; j <= 3; j++) {
            V[j].x = A[i + j].x;
            V[j].y = A[i + j].y;
        }
        for (step = 1; step <= BEZIERSUBDIVISION; step++) {
            count++;
            pf = Bezier (V, 3, (double) step / BEZIERSUBDIVISION, NULL, NULL);
	    PF2P(pf, p);
            size = sprintf(buf, " %d %d", p.x, p.y);
            buf += size;
        }
    }

    figgen_printf(job, "%d %d %d %d %d %d %d %d %d %.1f %d %d %d %d\n",
            object_code,
            sub_type,
            line_style,
            thickness,
            pen_color,
            fill_color,
            depth,
            pen_style,
            area_fill,
            style_val, cap_style, forward_arrow, backward_arrow, count);

    figgen_printf(job, " %s\n", buffer);      /* print points */
    free(buffer);
    for (i = 0; i < count; i++) {
        figgen_printf(job, " %d", i % (count - 1) ? 1 : 0);   /* -1 on all */
    }
    figgen_fputs(job, "\n");
}

static void figgen_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    gvstyle_t *style = job->style;

    int object_code = 2;        /* always 2 for polyline */
    int sub_type = 3;           /* always 3 for polygon */
    int line_style;		/* solid, dotted, dashed */
    int thickness = style->penwidth;
    int pen_color = style->pencolor.u.index;
    int fill_color = style->fillcolor.u.index;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int area_fill = filled ? 20 : -1;
    double style_val;
    int join_style = 0;
    int cap_style = 0;
    int radius = 0;
    int forward_arrow = 0;
    int backward_arrow = 0;
    int npoints = n + 1;

    figgen_line_style(style, &line_style, &style_val);

    figgen_printf(job,
            "%d %d %d %d %d %d %d %d %d %.1f %d %d %d %d %d %d\n",
            object_code, sub_type, line_style, thickness, pen_color,
            fill_color, depth, pen_style, area_fill, style_val, join_style,
            cap_style, radius, forward_arrow, backward_arrow, npoints);
    figptarray(job, A, n, 1);        /* closed shape */
}

static void figgen_polyline(GVJ_t * job, pointf * A, int n)
{
    gvstyle_t *style = job->style;

    int object_code = 2;        /* always 2 for polyline */
    int sub_type = 1;           /* always 1 for polyline */
    int line_style;		/* solid, dotted, dashed */
    int thickness = style->penwidth;
    int pen_color = style->pencolor.u.index;
    int fill_color = 0;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int area_fill = 0;
    double style_val;
    int join_style = 0;
    int cap_style = 0;
    int radius = 0;
    int forward_arrow = 0;
    int backward_arrow = 0;
    int npoints = n;

    figgen_line_style(style, &line_style, &style_val);

    figgen_printf(job,
            "%d %d %d %d %d %d %d %d %d %.1f %d %d %d %d %d %d\n",
            object_code, sub_type, line_style, thickness, pen_color,
            fill_color, depth, pen_style, area_fill, style_val, join_style,
            cap_style, radius, forward_arrow, backward_arrow, npoints);
    figptarray(job, A, n, 0);        /* open shape */
}

gvrender_engine_t figgen_engine = {
    0,				/* figgen_begin_job */
    0,				/* figgen_end_job */
    figgen_begin_graph,
    figgen_end_graph,
    0,				/* figgen_begin_layer */
    0,				/* figgen_end_layer */
    figgen_begin_page,
    0,				/* figgen_end_page */
    0,				/* figgen_begin_cluster */
    0,				/* figgen_end_cluster */
    0,				/* figgen_begin_nodes */
    0,				/* figgen_end_nodes */
    0,				/* figgen_begin_edges */
    0,				/* figgen_end_edges */
    figgen_begin_node,
    figgen_end_node,
    figgen_begin_edge,
    figgen_end_edge,
    0,				/* figgen_begin_anchor */
    0,				/* figgen_end_anchor */
    figgen_textpara,
    figgen_resolve_color,
    figgen_ellipse,
    figgen_polygon,
    figgen_bezier,
    figgen_polyline,
    figgen_comment
};


/* NB.  List must be LANG_C sorted */
static char *figgen_knowncolors[] = {
    "black", "blue", "cyan", "green", "magenta", "red", "white", "yellow",
};


gvrender_features_t figgen_features = {
    EMIT_COLORS
	| GVRENDER_Y_GOES_DOWN,	/* flags */
    DEFAULT_EMBED_MARGIN,	/* default margin - points */
    {1440.,1440.},		/* default dpi */
   	 /* FIXME - this default dpi is a very strange number!!!
	  * It was picked to make .png usershapes the right size on my screen.
	  * It happens to be 1.2 * 1200, but I can't explain the 1.2.
	  * (I was expecting 1.3333 which is 96/72, but thats too big.)
    	  * Also 1200 is hardcoded in figgen_begin_graph() instead of using job->dpi 
          */
    figgen_knowncolors,		/* knowncolors */
    sizeof(figgen_knowncolors) / sizeof(char *), /* sizeof knowncolors */
    RGBA_BYTE,			/* color_type */
    NULL,                       /* device */
    "fig",                      /* gvloadimage target for usershapes */
};

gvplugin_installed_t gvrender_core_fig_types[] = {
    {FORMAT_FIG, "fig", 1, &figgen_engine, &figgen_features},
    {0, NULL, 0, NULL, NULL}
};
